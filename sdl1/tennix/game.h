
/**
 *
 * Tennix! SDL Port
 * Copyright (C) 2003, 2007, 2008, 2009 Thomas Perl <thp@thpinfo.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA  02110-1301, USA.
 *
 **/

#ifndef __GAME_H
#define __GAME_H

#include <math.h>
#include "tennix.h"
#include "sound.h"
#include "graphics.h"
#include "input.h"


#define SETS_TO_WIN 3

#define MAXPLAYERS 2

#ifdef DELUXE_EDITION
#  define BALL_STATES 16
#else
#  define BALL_STATES 4
#endif

typedef unsigned char referee_t;
enum {
    REFEREE_NORMAL,
    REFEREE_OUT,
    REFEREE_PLAYER1,
    REFEREE_PLAYER2,
    REFEREE_COUNT
};

typedef unsigned char winner_t;
enum {
    WINNER_NONE,
    WINNER_PLAYER1,
    WINNER_PLAYER2
};

typedef unsigned char soundevent_t;
enum {
    SOUND_EVENT_NONE     = 0<<0,
    SOUND_EVENT_GROUND   = 1<<0,
    SOUND_EVENT_OUT      = 1<<1,
    SOUND_EVENT_APPLAUSE = 1<<2,
    SOUND_EVENT_RACKET   = 1<<3
};
#define SOUND_EVENT_COUNT 4

typedef struct {
    const char* name;
    const char* area;
    const char* city;
    image_id court_type;
    unsigned int max_visitors;
    const char* court_type_name;
    image_id photo;
    unsigned int photo_frames;
    unsigned int worldmap_x;
    unsigned int worldmap_y;
    bool has_referee;
    bool night;
    unsigned int rain;
    unsigned int fog;
} Location;

#define BALL_RESTITUTION .6

typedef struct {
    float x;
    float y;
    float z;
    float move_x;
    float move_y;
    float move_z;
    bool ground_hit;
    char last_hit_by;
    bool inhibit_gravity;
} Ball;

typedef struct {
    InputDevice* input;
    char input_device_index;
    float x;
    float y;
    float power;
    bool use_power;
    unsigned char score;
    unsigned char desire;
    bool type; /* is this player ai-controlled or human? */
    char game; /* score for the current game */
    unsigned char sets[SETS_TO_WIN*2]; /* score for each set */
    float accelerate; /* a value [0..1] how fast the user accelerates */
} Player;

enum {
    PLAYER_TYPE_HUMAN,
    PLAYER_TYPE_AI
};

enum {
    DESIRE_NORMAL,
    DESIRE_TOPSPIN,
    DESIRE_SMASH,
    DESIRE_MAX
};

/* wait 2 seconds before we score the game */
#define SCORING_DELAY 1000

typedef unsigned char scoreevent_t;
enum {
    SCORE_UNDECIDED,
    SCORE_EVENT_NET,
    SCORE_EVENT_OUT,
    SCORE_EVENT_GROUND_INVALID,
    SCORE_EVENT_GROUND_VALID,
    SCORE_EVENT_OFFSCREEN,
    SCORE_EVENT_MAX
};

typedef unsigned char eventcounter_t;
enum {
    EVENTCOUNTER_RENDERSTATE_START = 0,
    EVENTCOUNTER_GAMESTATE_START = 1
};

typedef unsigned char statusmessage_t;
enum {
    STATUSMSG_NONE,
    STATUSMSG_WELCOME,
    STATUSMSG_DEFAULT,
    STATUSMSG_NET,
    STATUSMSG_OUT,
    STATUSMSG_FAULT,
    STATUSMSG_P1SCORES,
    STATUSMSG_P2SCORES,
    STATUSMSG_VOLLEY,
    STATUSMSG_DIDNTCATCH
};

typedef struct {
    const Location* location;
    char current_location; /* index of loc. in global location table */
    Ball ball;
    Player players[MAXPLAYERS];
    unsigned char serving_player;
    referee_t referee;
    unsigned char current_set;
    winner_t winner;
    soundevent_t sound_events;
    scoreevent_t score_event;
    unsigned char score_time;
    eventcounter_t ec_game;
    eventcounter_t ec_sets;
    statusmessage_t status_message;
} GameState;

typedef struct {
    soundevent_t sound_events;
    referee_t referee;
    eventcounter_t ec_game;
    eventcounter_t ec_sets;
    statusmessage_t status_message;
    const char* text_status;
    const char* text_game;
    const char* text_sets;
} RenderState;

#define PI 3.1415

#define GAME_TICKS 15

#define GRAVITY -.03

#define RACKET_X_MID 15
#define RACKET_Y_MID 24

#define BALL_X_MID 9
#define BALL_Y_MID 9

#define GAME_X_MIN 41.0*2
#define GAME_X_MAX 270.0*2
#define GAME_X_MID ((GAME_X_MIN+GAME_X_MAX)/2)

#define GAME_Y_MIN 155.0
#define GAME_Y_MAX 330.0
#define GAME_Y_MID ((GAME_Y_MIN+GAME_Y_MAX)/2)

/* net size and collision detection */
#define NET_X 320
#define NET_Y 107
#define NET_WIDTH 5
#define NET_HEIGHT 273
#define NET_TALLNESS 5
#define IN_NET_X(x) (x+BALL_X_MID >= NET_X && x-BALL_X_MID <= NET_X+NET_WIDTH)
#define IN_NET_Y(y) (y >= NET_Y && y <= NET_Y+NET_HEIGHT)
#define NET_COLLISION(x, y, z) (z<=NET_TALLNESS && IN_NET_X(x) && IN_NET_Y(y))
#define NET_COLLISION_BALL(b) (NET_COLLISION(b.x, b.y, b.z))

/* "in field" coordinates (left and right part of court) */
#define FIELD_LEFT_X 119
#define FIELD_LEFT_WIDTH 202
#define FIELD_LEFT_IS_VALID(x) (x >= FIELD_LEFT_X && x <= FIELD_LEFT_X+FIELD_LEFT_WIDTH)
#define FIELD_RIGHT_X 326
#define FIELD_RIGHT_WIDTH 202
#define FIELD_RIGHT_IS_VALID(x) (x >= FIELD_RIGHT_X && x <= FIELD_RIGHT_X+FIELD_RIGHT_WIDTH)

/* coordinates for one-vs-one game */
#define FIELD_SINGLE_Y 154
#define FIELD_SINGLE_HEIGHT 178
#define FIELD_SINGLE_IS_VALID(y) (y >= FIELD_SINGLE_Y && y <= FIELD_SINGLE_Y+FIELD_SINGLE_HEIGHT)

/* coordinates for two-vs-two game */
#define FIELD_DOUBLE_Y 115
#define FIELD_DOUBLE_HEIGHT 256
#define FIELD_DOUBLE_IS_VALID(y) (y >= FIELD_DOUBLE_Y && y <= FIELD_DOUBLE_Y+FIELD_DOUBLE_HEIGHT)

/* player = player that last hit the ball, */
#define GROUND_IS_VALID(player, x, y) (FIELD_SINGLE_IS_VALID(y) && ((player==1)?(FIELD_RIGHT_IS_VALID(x)):(FIELD_LEFT_IS_VALID(x))))
#define IS_OUT(x, y) (!(FIELD_SINGLE_IS_VALID(y) && (FIELD_RIGHT_IS_VALID(x) || FIELD_LEFT_IS_VALID(x))))

#define PLAYER_Y_MIN 0
#define PLAYER_Y_MAX HEIGHT

#define IS_OFFSCREEN_Y(y) (((y)<0-BALL_Y_MID*2) || ((y)>HEIGHT+BALL_Y_MID*2))
#define IS_OFFSCREEN_X(x) (((x)<0-BALL_X_MID*2) || ((x)>WIDTH+BALL_X_MID*2))
#define IS_OFFSCREEN(x,y) ((IS_OFFSCREEN_X(x)) || (IS_OFFSCREEN_Y(y)))

#define PLAYER_AREA_Y RACKET_Y_MID
#define PLAYER_AREA_X RACKET_X_MID
#define IS_NEAR_Y(py,by) (fabsf((py)-(by))<(float)PLAYER_AREA_Y)
#define IS_NEAR_Y_AI(py,by) (fabsf((py)-(by))<(float)PLAYER_AREA_Y/3.5)
#define IS_NEAR_X(px,bx) (fabsf((px)-(bx))<(float)PLAYER_AREA_X)
#define IS_NEAR_X_AI(px,bx) (fabsf((px)-(bx))<(float)PLAYER_AREA_X*2.)

#define PLAYER_MOVE_Y 5.0
#define PLAYER_ACCEL_DEFAULT 0.2
#define PLAYER_ACCEL_INCREASE 1.2

#define POWER_UP_FACTOR 1.1
#define POWER_DOWN_FACTOR 0.9
#define PLAYER_POWER_MAX 100

#define PLAYER(s, num) (s->players[num-1])

/* Comment out the following #define to enable mouse control */
#define ENABLE_MOUSE

/* GameState handling*/
GameState *gamestate_new();

int gamestate_save(GameState* s, const char* filename);
GameState* gamestate_load(const char* filename);

/* Game module functions */
void gameloop(GameState*, TennixNet*);
void step(GameState*);
bool handle_input(GameState*, TennixNet*);
void render(const GameState*, RenderState*);
float get_move_y( GameState*, unsigned char);
void input_human(GameState*, int);
void input_ai(GameState*, int);
void game_setup_serve( GameState*);

int score_game(GameState*);
winner_t game_get_winner(const GameState*);

/* Helper functions for the renderer */
const char* format_sets(const GameState*);
const char* format_game(const GameState*);
const char* format_status(const GameState*);

#endif

