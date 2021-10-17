
# Experimental implementation of a Python-based Tennix AI Bot

import random
import time
import threading
import math
import sys

import tennix

DEBUG = False

class BasicBot(object):
    name = 'Unnamed Bot'

    def say(self, *args):
        # FIXME: Expose debugging flag through "tennix" module
        global DEBUG
        if DEBUG:
            print self.name, 'says:', (' '.join(str(x) for x in args))

class DumbPythonBot(BasicBot):
    name = 'John McInpyre'

    GAME_Y_MID = 242
    DESIRED_POWER = { tennix.INPUT_KEY_HIT: 90, tennix.INPUT_KEY_TOPSPIN: 70, tennix.INPUT_KEY_SMASH: 70 }
    DEFAULT_APPROACH_DISTANCE = 40.0

    def __init__(self, gamestate, player_id):
        self.gamestate = gamestate
        self.player_id = player_id

        self.pressed_key = -1

        self.ball_approaching = False
        self.have_to_serve = False

        self.old_ball_x, self.old_ball_y = tennix.get_ball_pos(self.gamestate)
        self.ball_x, self.ball_y = self.old_ball_x, self.old_ball_y
        self.player_x, self.player_y = tennix.get_position(self.gamestate, self.player_id)
        self.power = tennix.get_power(self.gamestate, self.player_id)

        self.desired_y_position = self.GAME_Y_MID
        self.approach_distance = self.DEFAULT_APPROACH_DISTANCE
        self.think_about_y_position()

        self.cancelled = False
        self.t = threading.Thread(target=self.think)
        self.t.start()
        self.say(self.name, 'has been loaded')

    def distance(self, a, b):
        return math.sqrt((a[0]-b[0])**2 + (a[1]-b[1])**2)

    def look_at_power_bar(self):
        self.power = tennix.get_power(self.gamestate, self.player_id)

    def look_at_player(self):
        self.player_x, self.player_y = tennix.get_position(self.gamestate, self.player_id)

    def look_at_ball(self):
        self.old_ball_x, self.old_ball_y = self.ball_x, self.ball_y
        self.ball_x, self.ball_y = tennix.get_ball_pos(self.gamestate)
        old_distance = self.distance((self.player_x, self.player_y), (self.old_ball_x, self.old_ball_y))
        new_distance = self.distance((self.player_x, self.player_y), (self.ball_x, self.ball_y))

        self.ball_approaching = (old_distance > new_distance)
        self.have_to_serve = ((old_distance == new_distance) and abs(self.player_x - self.ball_x) < 50)

    def have_sufficient_power(self):
        return self.power > 90

    def think_about_y_position(self):
        if not self.ball_approaching and not self.have_to_serve:
            #self.desired_y_position = self.GAME_Y_MID
            return

        ball_x_speed = self.old_ball_x - self.ball_x

        ball_y_speed = self.old_ball_y - self.ball_y

        #self.say('My ball x speed is', ball_x_speed)
        #self.say('My ball y speed is', ball_y_speed)

        x_diff = self.player_x - self.ball_x
        if ball_x_speed != 0.0:
            self.approach_distance = abs(ball_x_speed*.6)
            y_diff = ball_y_speed * x_diff / ball_x_speed
        else:
            self.approach_distance = self.DEFAULT_APPROACH_DISTANCE
            y_diff = 0.0

        self.say('I have to move', y_diff, 'to catch the ball now', x_diff, 'away')

        self.desired_y_position = self.ball_y + y_diff + random.randint(-5,5)

    def am_positioned(self):
        return abs(self.desired_y_position - self.player_y) < 30

    def think(self):
        i = 0
        while not self.cancelled:
            time.sleep(.2)

            self.look_at_power_bar()
            self.look_at_player()
            self.look_at_ball()

            self.think_about_y_position()

            if self.pressed_key == -1:
                if self.ball_approaching or self.have_to_serve:
                    self.pressed_key = tennix.INPUT_KEY_HIT
            else:
                self.say('holding key with distance', abs(self.player_x - self.ball_x))
                self.say('my approach distance is', self.approach_distance)
                if not self.ball_approaching and not self.have_to_serve:
                    self.pressed_key = -1
                if abs(self.player_x - self.ball_x) < self.approach_distance and self.have_sufficient_power() and self.am_positioned():
                    self.pressed_key = -1

            #self.current_tactic = random.randint(tennix.INPUT_KEY_HIT, tennix.INPUT_KEY_SMASH)
            #self.desired_power = self.DESIRED_POWER[self.current_tactic]*(1.2-0.4*random.random())
            #print 'determined new tactic:', self.current_tactic
            i += 1
        self.say('Thread for', self.name, 'finished')

#        if math.sqrt((self.player_x - self.ball_x)**2 + (self.player_y - self.ball_y)**2) > 150:
#            return False
#
#        if self.power > self.desired_power and random.randint(0, 3)==0:
#            self.power_locked = True
#            return False
#        if key_id == self.current_tactic and not self.power_locked:
#            #if abs(self.player_x - self.ball_x) < 10 and self.power > self.desired_power:
#            return abs(self.player_x - self.ball_x) > 5 and self.power < self.desired_power + 0.3*random.random()
#        else:
#            return False

    def finish(self):
        self.say('I am going to be unloaded')
        self.cancelled = True
        if self.t is not None:
            self.t.join()

    def get_key(self, key_id):
        return self.pressed_key == key_id

    def get_axis(self, axis_id):
        self.look_at_player()
        if axis_id == tennix.INPUT_AXIS_Y:
            return max(-1.0, min(1.0, (self.desired_y_position - self.player_y)/4.))

        return 0.0


tennix.register_bot(DumbPythonBot)

