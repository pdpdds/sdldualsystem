
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "tennix.h"
#include "game.h"
#include "graphics.h"
#include "input.h"
#include "sound.h"
#include "util.h"
#include "network.h"


GameState *gamestate_new() {
    GameState *s;

    GameState gs_template = {
        NULL,
        -1,
        { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, -1, false },
        {
            { NULL, -1, GAME_X_MIN-RACKET_X_MID*2, GAME_Y_MID, 0.0, true, 0, DESIRE_NORMAL, PLAYER_TYPE_AI, 0, {0}, PLAYER_ACCEL_DEFAULT },
            { NULL, -1, GAME_X_MAX+RACKET_X_MID*2, GAME_Y_MID, 0.0, true, 0, DESIRE_NORMAL, PLAYER_TYPE_AI, 0, {0}, PLAYER_ACCEL_DEFAULT },
        },
        1,
        REFEREE_NORMAL,
        0,
        WINNER_NONE,
        SOUND_EVENT_NONE, /* sound events */
        SCORE_UNDECIDED,
        0,
        EVENTCOUNTER_GAMESTATE_START,
        EVENTCOUNTER_GAMESTATE_START,
        STATUSMSG_WELCOME,
    };

    s = (GameState*)malloc(sizeof(GameState));
    if (s == NULL) abort();

    memcpy(s, &gs_template, sizeof(GameState));

    game_setup_serve(s);

    return s;
}


int gamestate_save(GameState* s, const char* filename)
{
    const Location *location;
    InputDevice* input_devices[MAXPLAYERS];
    int i, result = 0;
    FILE* fp = NULL;
#ifndef WIN32
    char tmp[MAXPATHLEN];

    assert(getcwd(tmp, MAXPATHLEN) == tmp);
    assert(chdir(getenv("HOME")) == 0);
#endif

    /**
     * Process-specific data (pointers to data
     * structures only of interest to this process)
     * has to be "removed" before saving.
     *
     * It can later be restored (in gamestate_load).
     **/

    fp = fopen(filename, "w");
    if (fp == NULL) {
        return -1;
    }

    /* Save data for later recovery + clear */
    location = s->location;
    s->location = NULL;
    for (i=1; i<=MAXPLAYERS; i++) {
        input_devices[i-1] = PLAYER(s, i).input;
        PLAYER(s, i).input = NULL;
    }

    if (fwrite(s, sizeof(GameState), 1, fp) != 1) {
        result = -2;
    }

    /* Restore process-specific data */
    s->location = location;
    for (i=1; i<=MAXPLAYERS; i++) {
        PLAYER(s, i).input = input_devices[i-1];
    }

    fclose(fp);

#ifndef WIN32
    assert(chdir(tmp) == 0);
#endif

    return result;
}


GameState* gamestate_load(const char* filename)
{
    FILE* fp;
    GameState* s = NULL;
#ifndef WIN32
    char tmp[MAXPATHLEN];

    assert(getcwd(tmp, MAXPATHLEN) == tmp);
    assert(chdir(getenv("HOME")) == 0);
#endif

    fp = fopen(filename, "r");

    if (fp != NULL) {
        s = (GameState*)malloc(sizeof(GameState));
        if (s != NULL) {
            if (fread(s, sizeof(GameState), 1, fp) == 1) {
                /* Restore/create process-specific data */
                /* FIXME: s->location, players' "input" */
            } else {
                free(s);
                s = NULL;
            }
        }
        fclose(fp);
    }

#ifndef WIN32
    assert(chdir(tmp) == 0);
#endif

    return s;
}


void gameloop(GameState *s, TennixNet* c) {
    Uint32 ot = SDL_GetTicks();
    Uint32 nt;
    Uint32 dt = GAME_TICKS;
    Uint32 diff;
    Uint32 accumulator = 0;
    bool quit = false;
    int p;
    int i=0;
    RenderState r = {
        SOUND_EVENT_NONE,
        REFEREE_COUNT,
        EVENTCOUNTER_RENDERSTATE_START,
        EVENTCOUNTER_RENDERSTATE_START,
        STATUSMSG_NONE,
        NULL,
        NULL,
        NULL,
    };

    /* Catch-up with existing sound events */
    r.sound_events = s->sound_events;

#ifdef ENABLE_FPS_LIMIT
    Uint32 ft, frames; /* frame timer and frames */
#endif

    if (s->location->rain > 0) {
        play_sample_background(SOUND_RAIN);
    }
    if (s->location->max_visitors > 100) {
        play_sample_loop(SOUND_AUDIENCE);
    }

    for (p=1; p<=MAXPLAYERS; p++) {
        input_device_join_game(PLAYER(s, p).input, s, p);
    }

#ifdef ENABLE_FPS_LIMIT
    frames = 0;
    ft = SDL_GetTicks();
#endif
    while( !quit) {
        nt = SDL_GetTicks();
        diff = nt-ot;
        if( diff > 2000) {
            diff = 0;
        }

        accumulator += diff;
        ot = nt;

        while( accumulator >= dt) {
            step(s);

            network_receive(c);
            network_get_gamestate(c, s);

            if ((i++) % 10 == 0) {
                network_send_state(c, s);
            }

            quit = handle_input(s, c);
            accumulator -= dt;
        }

#ifdef ENABLE_FPS_LIMIT
        while (frames*1000.0/((float)(SDL_GetTicks()-ft+1))>(float)(DEFAULT_FPS)) {
            SDL_Delay(10);
        }
        frames++;
#endif

        render(s, &r);
    }

    for (p=1; p<=MAXPLAYERS; p++) {
        input_device_part_game(PLAYER(s, p).input);
    }

    clear_screen();
    //rectangle(0, 0, WIDTH, HEIGHT, 80, 80, 80);
    store_screen();

    stop_sample(SOUND_AUDIENCE);
    stop_sample(SOUND_RAIN);
}

void step(GameState* s) {
    bool ground_event = false;
    int p;

    s->ball.z += s->ball.move_z;
    if (!s->ball.inhibit_gravity) {
        s->ball.move_z += GRAVITY;
    }

    s->ball.x += s->ball.move_x;
    s->ball.y += s->ball.move_y;

    for (p=1; p<=MAXPLAYERS; p++) {
        if (PLAYER(s, p).use_power) {
            PLAYER(s, p).power = fmaxf(0.0, fminf(
                        PLAYER(s, p).power*POWER_DOWN_FACTOR,
                        PLAYER_POWER_MAX));
        }
    }

    if (s->ball.z < 0) {
        /* ground event */
        ground_event = true;

        s->referee = REFEREE_NORMAL;

        /* bounce from the ground */
        if (fabsf(s->ball.move_z) > 0.3) {
            s->sound_events ^= SOUND_EVENT_GROUND;
            s->ball.move_z *= -BALL_RESTITUTION;
        } else {
            s->ball.move_z = 0;
        }
        s->ball.z = 0;
    }

    if (NET_COLLISION_BALL(s->ball)) {
        /* the net blocks movement of the ball */
        while (NET_COLLISION_BALL(s->ball)) {
            /* make sure the ball appears OUTSIDE of the net */
            if (s->ball.move_x < 0) {
                s->ball.x += 1;
            } else {
                s->ball.x -= 1;
            }
        }
        s->ball.move_x = 0;
        s->ball.move_y = 0;
    }

    /* see if we have something to score */
    if (s->score_event == SCORE_UNDECIDED) {
        if (NET_COLLISION_BALL(s->ball)) {
            /* the ball "fell" into the net  */
            s->score_event = SCORE_EVENT_NET;
            s->sound_events ^= SOUND_EVENT_OUT;
            s->status_message = STATUSMSG_NET;
        } else if (IS_OFFSCREEN(s->ball.x, s->ball.y)) {
            /* ball flew offscreen */
            s->score_event = SCORE_EVENT_OFFSCREEN;
            s->sound_events ^= SOUND_EVENT_OUT;
            s->status_message = STATUSMSG_OUT;
        } else if (ground_event) {
            /* the ball hit the ground on the screen */
            if (IS_OUT(s->ball.x, s->ball.y)) {
                /* the ball bounced in the OUT area */
                s->score_event = SCORE_EVENT_OUT;
                s->sound_events ^= SOUND_EVENT_OUT;
                s->status_message = STATUSMSG_OUT;
            } else if (GROUND_IS_VALID(s->ball.last_hit_by, s->ball.x, s->ball.y)) {
                if (s->ball.ground_hit) {
                    s->score_event = SCORE_EVENT_GROUND_VALID;
                    s->sound_events ^= SOUND_EVENT_OUT;
                    s->status_message = STATUSMSG_DIDNTCATCH;
                } else {
                    /* first ground hit in valid area */
                    s->ball.ground_hit = true;
                }
            } else {
                /* ball hit somewhere invalid */
                s->score_event = SCORE_EVENT_GROUND_INVALID;
                s->sound_events ^= SOUND_EVENT_OUT;
                s->status_message = STATUSMSG_FAULT;
            }
        }
    }

    if (s->score_event != SCORE_UNDECIDED) {
        /* we have some scoring to do */
        if (s->score_time < SCORING_DELAY/GAME_TICKS) {
            /* schedule scoring in the future */
            s->score_time++;
            s->referee = REFEREE_OUT;
        } else if (s->score_time >= SCORING_DELAY/GAME_TICKS) {
            /* time has ran out - score now */
            switch (score_game(s)) {
                case WINNER_PLAYER1:
                    s->status_message = STATUSMSG_P1SCORES;
                    s->referee = REFEREE_PLAYER1;
                    break;
                case WINNER_PLAYER2:
                    s->status_message = STATUSMSG_P2SCORES;
                    s->referee = REFEREE_PLAYER2;
                    break;
                default:
                    assert(0);
                    break;
            }
            s->score_time = 0;

            game_setup_serve(s);
            if (s->location->max_visitors > 100) {
                s->sound_events ^= SOUND_EVENT_APPLAUSE;
            }
        }
    } else {
        /* score is still undecided - do the racket swing thing */
        for (p=1; p<=2; p++) {
            if (IS_NEAR_X(PLAYER(s, p).x, s->ball.x) && IS_NEAR_Y(PLAYER(s, p).y, s->ball.y-s->ball.z) && PLAYER(s, p).use_power && PLAYER(s, p).power > 30.0 && s->ball.last_hit_by != p) {
                /* RACKET HIT */
                if (!s->ball.ground_hit && s->ball.move_x != 0.0) {
                    s->status_message = STATUSMSG_VOLLEY;
                } else {
                    s->status_message = STATUSMSG_DEFAULT;
                }
                switch (PLAYER(s, p).desire) {
                    case DESIRE_NORMAL:
                        /* normal swing */
                        s->ball.move_x = 2.7 + 2.0*PLAYER(s, p).power/PLAYER_POWER_MAX;
                        s->ball.move_z = 1.2*PLAYER(s, p).power/PLAYER_POWER_MAX;
                        break;
                    case DESIRE_TOPSPIN:
                        /* top spin */
                        s->ball.move_x = 1.1 + 2.2*PLAYER(s, p).power/PLAYER_POWER_MAX;
                        s->ball.move_z = 2.5*PLAYER(s, p).power/PLAYER_POWER_MAX;
                        break;
                    case DESIRE_SMASH:
                        /* smash */
                        s->ball.move_x = 4.0 + 3.0*PLAYER(s, p).power/PLAYER_POWER_MAX;
                        s->ball.move_z = 1.1*PLAYER(s, p).power/PLAYER_POWER_MAX;
                        break;
                }
                s->ball.move_y = get_move_y( s, p);
                s->sound_events ^= SOUND_EVENT_RACKET;
                s->ball.ground_hit = false;
                s->ball.inhibit_gravity = false;
                s->ball.last_hit_by = p;
                if (p==2) {
                    s->ball.move_x *= -1;
                }
            }
        }
    }
}

bool handle_input(GameState* s, TennixNet* c) {
    static NetworkGameState tmp;
    static NetworkInputData net_input;
    Uint8* keys = NULL;
    int p;

    SDL_PumpEvents();
    keys = SDL_GetKeyState(NULL);

    if (keys['1']) {
        net_serialize_gamestate(s, &tmp);
    } else if (keys['2']) {
        net_unserialize_gamestate(&tmp, s);
    }

    network_get_input(c, &net_input);
    if (s->winner == WINNER_NONE) {
        for (p=1; p<=MAXPLAYERS; p++) {
            if( PLAYER(s, p).type == PLAYER_TYPE_HUMAN) {
                if (PLAYER(s, p).input->type == INPUT_TYPE_NETWORK) {
                    memcpy(&(PLAYER(s, p).input->net), &net_input,
                            sizeof(NetworkInputData));
                }
                input_human(s, p);
                if (PLAYER(s, p).input->type != INPUT_TYPE_NETWORK) {
                    network_send_input(c, &(PLAYER(s, p).input->net));
                }
            } else {
                input_ai(s, p);
            }

            /* Make sure player coordinates are valid */
            if (PLAYER(s, p).y < PLAYER_Y_MIN) {
                PLAYER(s, p).y = PLAYER_Y_MIN;
            } else if (PLAYER(s, p).y > PLAYER_Y_MAX) {
                PLAYER(s, p).y = PLAYER_Y_MAX;
            }
        }
    }
    
    /* Maemo: The "F4" button is the "Open menu" button */
    return (keys[SDLK_ESCAPE] || keys['q']);
}

void render(const GameState* s, RenderState* r) {
    int x, y, b;
    unsigned int i;
    float zoom;
    float rotate;
    int t=1000;
    soundevent_t sounds;
    Uint32 time = SDL_GetTicks();

    /* The bits in sound_events flip when the sound should play */
    if ((sounds = (r->sound_events ^ s->sound_events)) != 0) {
        if (sounds & SOUND_EVENT_GROUND) {
            sample_volume_group(SOUND_GROUND_FIRST, SOUND_GROUND_LAST,
                    fmaxf(0.0, fminf(1.0, fabsf(s->ball.move_z)/2)));
            pan_sample_group(SOUND_GROUND_FIRST, SOUND_GROUND_LAST,
                    fmaxf(0.0, fminf(1.0, (s->ball.x)/WIDTH)));
            play_sample(SOUND_GROUND);
        }
        if (sounds & SOUND_EVENT_OUT) {
            play_sample(SOUND_OUT);
        }
        if (sounds & SOUND_EVENT_APPLAUSE) {
            play_sample(SOUND_APPLAUSE);
        }
        if (sounds & SOUND_EVENT_RACKET) {
            if (((s->ball.x)/WIDTH) < 0.5) {
                pan_sample_group(SOUND_RACKET_FIRST, SOUND_RACKET_LAST, 0.3);
            } else {
                pan_sample_group(SOUND_RACKET_FIRST, SOUND_RACKET_LAST, 0.7);
            }
            play_sample(SOUND_RACKET);
        }
        r->sound_events = s->sound_events;
    }

    if( s->winner != WINNER_NONE) {
        clear_screen();
        //rectangle(0, 0, WIDTH, HEIGHT, 80, 80, 80);
        store_screen();
        show_sprite( GR_RACKET, 2*(s->winner-1), 4, WIDTH/2 - get_image_width( GR_RACKET)/8, HEIGHT/2 - get_image_height( GR_RACKET), 255);
        /*sprintf( s->game_score_str, "player %d wins the match with %s", s->winner, format_sets( s));
        font_draw_string(FONT_LARGE, s->game_score_str, (WIDTH-font_get_string_width(FONT_LARGE, s->game_score_str))/2, HEIGHT/2 + 30);*/
        updatescr();
        return;
    }
    if ((r->referee != s->referee) ||
            (r->ec_game != s->ec_game) ||
            (r->ec_sets != s->ec_sets) ||
            (r->status_message != s->status_message)) {
        /* Update status message text */
        if (r->status_message != s->status_message) {
            r->text_status = format_status(s);
            r->status_message = s->status_message;
        }
        /* Update game status text */
        if (r->ec_game != s->ec_game) {
            r->text_game = format_game(s);
            r->ec_game = s->ec_game;
        }
        /* Update set status text */
        if (r->ec_sets != s->ec_sets) {
            r->text_sets = format_sets(s);
            if (r->status_message == STATUSMSG_DEFAULT) {
                r->text_status = format_status(s);
            }
            r->ec_sets = s->ec_sets;
        }
        clear_screen();
        //rectangle(0, 0, WIDTH, HEIGHT, 80, 80, 80);
        fill_image(s->location->court_type, 120, 120, 400, 250);
        show_image(GR_COURT, 0, 0, 255);
        font_draw_string(FONT_XLARGE, r->text_game, 14, 14);
        font_draw_string(FONT_XLARGE, r->text_sets,
                (WIDTH-font_get_string_width(FONT_XLARGE, r->text_sets))-14, 14);
        if (s->location->has_referee) {
            switch (s->referee) {
                case REFEREE_NORMAL:
                    t = 1000;
                    break;
                case REFEREE_OUT:
                    t = 200;
                    break;
                case REFEREE_PLAYER1:
                case REFEREE_PLAYER2:
                    t = 400;
                    break;
            }
            t = (time/t)%4;
            switch (t) {
                case 0:
                    t=0;
                    break;
                case 1:
                    t=1;
                    break;
                case 2:
                    t=0;
                    break;
                case 3:
                    t=2;
                    break;
            }
            show_sprite( GR_REFEREE, s->referee*3+t, 12, 250, 10, 255);
            if (voice_finished_flag == 0) {
                show_sprite(GR_TALK, (time/150)%2, 2, 280, 45, 255);
            }
        }
        r->referee = s->referee;
        store_screen();
    }
    
   /* show_sprite( GR_RACKET, (!PLAYER(s, 1).state), 4, PLAYER(s, 1).x-RACKET_X_MID, PLAYER(s, 1).y-RACKET_Y_MID, 255);
    show_sprite( GR_RACKET, (!PLAYER(s, 2).state)+2, 4, PLAYER(s, 2).x-RACKET_X_MID, PLAYER(s, 2).y-RACKET_Y_MID, 255);*/
    show_sprite( GR_RACKET, !PLAYER(s, 1).use_power, 4, PLAYER(s, 1).x-RACKET_X_MID, PLAYER(s, 1).y-RACKET_Y_MID, 255);
    show_sprite( GR_RACKET, !PLAYER(s, 2).use_power + 2, 4, PLAYER(s, 2).x-RACKET_X_MID, PLAYER(s, 2).y-RACKET_Y_MID, 255);
    
    rectangle(10, HEIGHT-30, PLAYER_POWER_MAX, 10, 50, 50, 50);
    rectangle(WIDTH-PLAYER_POWER_MAX-10, HEIGHT-30, PLAYER_POWER_MAX, 10, 50, 50, 50);

    rectangle(10, HEIGHT-30, (int)(PLAYER(s, 1).power), 10, 200, 200, 200);
    rectangle(WIDTH-PLAYER_POWER_MAX-10, HEIGHT-30, (int)(PLAYER(s, 2).power), 10, 200, 200, 200);
    
    if( s->ball.move_x > 0) {
        b = (time/100)%BALL_STATES;
    } else if( s->ball.move_x < 0) {
        b = BALL_STATES-1-(time/100)%BALL_STATES;
    } else {
        b = 0;
    }

    rotate = 0.0;
    zoom = fmaxf(0.5, fminf(1.0, (float)(30.-(s->ball.z))/10.));
    show_image_rotozoom(GR_SHADOW, s->ball.x, s->ball.y+3, rotate, zoom);

    rotate = (-s->ball.move_x * (float)((float)time/10.));
    zoom = 1.0 + fmaxf(0.0, (s->ball.z-10.)/100.);
    show_image_rotozoom(GR_BALL, s->ball.x, s->ball.y - s->ball.z, rotate, zoom);

    /* Player 1's mouse rectangle */
    if (PLAYER(s, 1).input != NULL && PLAYER(s, 1).input->type == INPUT_TYPE_MOUSE) {
        rectangle(PLAYER(s, 1).x-2+5, PLAYER(s, 1).input->my-2, 4, 4, 255, 255, 255);
        rectangle(PLAYER(s, 1).x-1+5, PLAYER(s, 1).input->my-1, 2, 2, 0, 0, 0);
    }

    /* Player 2's mouse rectangle */
    if (PLAYER(s, 2).input != NULL && PLAYER(s, 2).input->type == INPUT_TYPE_MOUSE) {
        rectangle(PLAYER(s, 2).x-2+5, PLAYER(s, 2).input->my-2, 4, 4, 255, 255, 255);
        rectangle(PLAYER(s, 2).x-1+5, PLAYER(s, 2).input->my-1, 2, 2, 0, 0, 0);
    }

    font_draw_string(FONT_MEDIUM, r->text_status,
            (WIDTH-font_get_string_width(FONT_MEDIUM, r->text_status))/2, HEIGHT-50);

    for (i=0; i<s->location->rain; i++) {
        x = rand()%WIDTH;
        y = rand()%HEIGHT;
        draw_line_faded(x, y, x+10, y+30, 0, 0, 255, 100, 200, 255);
    }
    if (s->location->rain) {
        /**
         * Cheap-ish update of the whole screen. This can
         * probably be optimized.
         **/
        update_rect(0, 0, WIDTH, HEIGHT);
    }

#ifdef DEBUG
    line_horiz( PLAYER(s, 1).y, 255, 0, 0);
    line_horiz( PLAYER(s, 2).y, 0, 0, 255);
    line_horiz( s->ball.y, 0, 255, 0);

    line_vert( PLAYER(s, 1).x, 255, 0, 0);
    line_vert( PLAYER(s, 2).x, 0, 0, 255);
    line_vert( s->ball.x, 0, 255, 0);

    line_horiz( GAME_Y_MIN, 100, 100, 100);
    line_horiz( GAME_Y_MAX, 100, 100, 100);
#endif
    switch (s->location->fog) {
        default:
        case 4:
            fill_image_offset(GR_FOG2, 0, 0, WIDTH, HEIGHT, -time/150, 0);
        case 3:
            fill_image_offset(GR_FOG, 0, 0, WIDTH, HEIGHT, -time/100, 20);
        case 2:
            fill_image_offset(GR_FOG2, 0, 0, WIDTH, HEIGHT, -time/180, 80);
        case 1:
            fill_image_offset(GR_FOG, 0, 0, WIDTH, HEIGHT, time/200, 0);
        case 0:
            break;
    }
    if (s->location->night) {
        show_image(GR_NIGHT, 0, 0, 255);
    }

    updatescr();
}

float get_move_y( GameState* s, unsigned char player) {
    float pct, dest, x_len, y_len;
    float py, by, pa, move_x;

    py = (player==1)?(PLAYER(s, 1).y):(PLAYER(s, 2).y);
    by = s->ball.y - s->ball.z;
    pa = RACKET_Y_MID*2;
    move_x = s->ball.move_x;

    /* -1.0 .. 1.0 for racket hit position */
    pct = fmaxf(-1.0, fminf(1.0, (by-py)/(pa/2)));

    /* Y destination for ball */
    dest = GAME_Y_MID + pct*(GAME_Y_MAX-GAME_Y_MIN);

    /* lengths for the ball's journey */
    if( player == 1) {
        x_len = GAME_X_MAX - s->ball.x;
    } else {
        x_len = s->ball.x - GAME_X_MIN;
    }
    y_len = dest - by;

    /* return the should-be value for move_y */
    return (y_len*move_x)/(x_len);
}

void input_human(GameState* s, int player) {
    bool hit, topspin, smash;
    float move_y;

    /* For mouse input, hand the player coordinates to the InputDevice */
    if (PLAYER(s, player).input->type == INPUT_TYPE_MOUSE) {
        PLAYER(s, player).input->player_x = (int)(PLAYER(s, player).x);
        PLAYER(s, player).input->player_y = (int)(PLAYER(s, player).y);
    }

    move_y = PLAYER_MOVE_Y*input_device_get_axis(PLAYER(s, player).input, INPUT_AXIS_Y);

    hit = input_device_get_key(PLAYER(s, player).input, INPUT_KEY_HIT);
    topspin = input_device_get_key(PLAYER(s, player).input, INPUT_KEY_TOPSPIN);
    smash = input_device_get_key(PLAYER(s, player).input, INPUT_KEY_SMASH);

    if (move_y != 0) {
        if (fabsf(move_y) > fabsf(move_y*PLAYER(s, player).accelerate)) {
            move_y *= PLAYER(s, player).accelerate;
        }
        PLAYER(s, player).y += move_y;
        PLAYER(s, player).accelerate *= PLAYER_ACCEL_INCREASE;
        if (PLAYER(s, player).accelerate > 190) {
            PLAYER(s, player).accelerate = 190;
        }
    } else {
        PLAYER(s, player).accelerate = PLAYER_ACCEL_DEFAULT;
    }

    if(hit || topspin || smash) {
        PLAYER(s, player).desire = (topspin)?(DESIRE_TOPSPIN):(DESIRE_NORMAL);
        PLAYER(s, player).desire = (smash)?(DESIRE_SMASH):(PLAYER(s, player).desire);

        PLAYER(s, player).power = fmaxf(10.0, fminf(PLAYER(s, player).power*POWER_UP_FACTOR, PLAYER_POWER_MAX));
        PLAYER(s, player).use_power = false;
    } else {
        PLAYER(s, player).use_power = true;
    }
}

void input_ai(GameState* s, int player) {
    float fact = 1.7;
    int ball_approaching = 0;

    if ((PLAYER(s, player).x < GAME_X_MID && s->ball.move_x <= 0) ||
        (PLAYER(s, player).x > GAME_X_MID && s->ball.move_x >= 0)) {
        ball_approaching = 1;
    }

    /* FIXME - this is broken since the new physics model has been introduced */

    if (fabsf(PLAYER(s, player).y - (s->ball.y-s->ball.z)) > RACKET_Y_MID*5) {
        fact = 3.5;
    }

    if(1) {
        if( PLAYER(s, player).desire == DESIRE_NORMAL && !IS_NEAR_Y_AI(PLAYER(s, player).y, (s->ball.y-s->ball.z)) && ball_approaching) {
            if( PLAYER(s, player).y < (s->ball.y-s->ball.z)) {
                PLAYER(s, player).y += fminf(2*fact, (s->ball.y-s->ball.z) - PLAYER(s, player).y);
            } else if( PLAYER(s, player).y > (s->ball.y-s->ball.z)) {
                PLAYER(s, player).y -= fminf(2*fact, PLAYER(s, player).y - (s->ball.y-s->ball.z));
            }
        }

        if (IS_NEAR_Y(PLAYER(s, player).y, (s->ball.y-s->ball.z)) && IS_NEAR_X_AI(PLAYER(s, player).x, s->ball.x) && PLAYER(s, player).power > 90.) {
            PLAYER(s, player).use_power = true;
        } else if (ball_approaching) {
            PLAYER(s, player).power = fmaxf(10., fminf(PLAYER(s, player).power*POWER_UP_FACTOR, PLAYER_POWER_MAX));
            PLAYER(s, player).use_power = false;
        }
    }
}

void game_setup_serve( GameState* s) {
    s->ball.z = 10.0;
    s->ball.y = GAME_Y_MID;
    s->ball.move_x = 0.0;
    s->ball.move_y = 0.0;
    s->ball.move_z = 0.0;
    s->ball.last_hit_by = -1;
    s->ball.inhibit_gravity = true;

    if( s->serving_player == 1) {
        s->ball.x = GAME_X_MIN-RACKET_X_MID*1.5;
    } else {
        s->ball.x = GAME_X_MAX+RACKET_X_MID*1.5;
    }
}

int score_game(GameState* s) {
    Player *last_hit_by = NULL;
    Player *other = NULL;

    Player* winner = NULL;
    Player* loser = NULL;

    /* determine "last hit by" and "other" */
    if (s->ball.last_hit_by == 1) {
        last_hit_by = &(PLAYER(s, 1));
        other = &(PLAYER(s, 2));
    } else {
        last_hit_by = &(PLAYER(s, 2));
        other = &(PLAYER(s, 1));
    }

    switch (s->score_event) {
        case SCORE_EVENT_NET:
            winner = other;
            break;
        case SCORE_EVENT_OUT:
        case SCORE_EVENT_GROUND_INVALID:
        case SCORE_EVENT_OFFSCREEN:
        case SCORE_EVENT_GROUND_VALID:
            if (s->ball.ground_hit) {
                winner = last_hit_by;
            } else {
                winner = other;
            }
            break;
        default:
            break;
    }

    /* determine loser based on winner */
    if (winner == last_hit_by) {
        loser = other;
    } else {
        loser = last_hit_by;
    }

    /* we cannot be in an "impossibly high" set */
    assert(s->current_set < SETS_TO_WIN*2-1);

    winner->game++;
    s->ec_game++;
    if( loser->game < winner->game-1) {
        if( winner->game >= 4) {
            winner->game = loser->game = 0;
            winner->sets[s->current_set]++;
            s->ec_sets++;

            /* serving is changed when the "game" is over */
            s->serving_player = (s->serving_player==1)?(2):(1);

#ifdef HAVE_VOICE_FILES
            /* speak the current score */
            voice_say_list(4, VOICE_ZERO_IN + (PLAYER(s, 1).sets[s->current_set])*2, VOICE_TO, VOICE_ZERO_OUT + (PLAYER(s, 2).sets[s->current_set])*2, VOICE_IN_THE_FIRST_SET+s->current_set);
#endif

            /* scoring the set.. */
            if( (winner->sets[s->current_set] == 6 && loser->sets[s->current_set] < 5) ||
                winner->sets[s->current_set] == 7) {
                s->current_set++;
                s->winner = game_get_winner( s);
            }
        }
    }

    /* forget this event - we've handled it */
    s->score_event = SCORE_UNDECIDED;

    if (winner == &PLAYER(s, 1)) {
        return WINNER_PLAYER1;
    } else {
        return WINNER_PLAYER2;
    }
}

const char* format_sets(const GameState* s) {
    static char sets[100];
    static char tmp[100];
    int i, max = s->current_set;

    sets[0] = '\0';

    if( s->winner != WINNER_NONE) {
        max--;
    }
    for( i=0; i<=max; i++) {
        sprintf( tmp, "%d:%d, ", PLAYER(s, 1).sets[i], PLAYER(s, 2).sets[i]);
        strcat( sets, tmp);
    }

    sets[strlen(sets)-2] = '\0';

    return sets;
}

const char* format_game(const GameState* s) {
    static char game[100];
    static const int game_scoring[] = { 0, 15, 30, 40 };

    if( PLAYER(s, 1).game < 4 && PLAYER(s, 2).game < 4) {
#ifdef HAVE_VOICE_FILES
        if (PLAYER(s, 1).game > 0 || PLAYER(s, 2).game > 0) {
            if (PLAYER(s, 1).game == PLAYER(s, 2).game) {
                voice_say_list(2, VOICE_LOVE_IN + 2*(PLAYER(s, 1).game), VOICE_ALL);
            } else {
                voice_say_list(2, VOICE_LOVE_IN + 2*(PLAYER(s, 1).game), VOICE_LOVE_OUT + 2*(PLAYER(s, 2).game));
            }
        }
#endif
        sprintf( game, "%d - %d", game_scoring[PLAYER(s, 1).game], game_scoring[PLAYER(s, 2).game]);
    } else if( PLAYER(s, 1).game > PLAYER(s, 2).game) {
#ifdef HAVE_VOICE_FILES
        voice_say_list(1, VOICE_ADVANTAGE_PLAYER_ONE);
#endif
        strcpy( game, "advantage player 1");
    } else if( PLAYER(s, 1).game < PLAYER(s, 2).game) {
#ifdef HAVE_VOICE_FILES
        voice_say_list(1, VOICE_ADVANTAGE_PLAYER_TWO);
#endif
        strcpy( game, "advantage player 2");
    } else {
#ifdef HAVE_VOICE_FILES
        voice_say_list(1, VOICE_DEUCE);
#endif
        strcpy( game, "deuce");
    }

    return game;
}

const char* format_status(const GameState* s) {
    static char status[100];
    static const char* set_names[] = { "first", "second", "third", "fourth", "fifth" };

    switch (s->status_message) {
        case STATUSMSG_NONE:
            return "";
        case STATUSMSG_WELCOME:
            return "welcome to tennix 2";
        case STATUSMSG_DEFAULT:
            sprintf(status, "%d:%d in %s set",
                    PLAYER(s, 1).sets[s->current_set],
                    PLAYER(s, 2).sets[s->current_set],
                    set_names[s->current_set]);
            return status;
        case STATUSMSG_NET:
            return "net!";
        case STATUSMSG_OUT:
            return "out!";
        case STATUSMSG_FAULT:
            return "fault!";
        case STATUSMSG_P1SCORES:
            return "player 1 scores.";
        case STATUSMSG_P2SCORES:
            return "player 2 scores.";
        case STATUSMSG_VOLLEY:
            return "volley!";
        case STATUSMSG_DIDNTCATCH:
            return "did not catch the ball.";
        default:
            return "";
    }
}

winner_t game_get_winner(const GameState* s) {
    unsigned int i;
    int sets[2] = {0};

    for( i=0; i<s->current_set; i++) {
        if( PLAYER(s, 1).sets[i] > PLAYER(s, 2).sets[i]) {
            sets[0]++;
        } else {
            sets[1]++;
        }
    }

    if( sets[0] == SETS_TO_WIN) return WINNER_PLAYER1;
    if( sets[1] == SETS_TO_WIN) return WINNER_PLAYER2;

    return WINNER_NONE;
}

