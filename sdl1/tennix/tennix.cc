
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
#include <time.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#endif

#define MAX(a, b)  (((a) > (b)) ? (a) : (b)) 
#define MIN(a, b)  (((a) < (b)) ? (a) : (b)) 

#include "tennix.h"
#include "archive.hh"
#include "game.h"
#include "graphics.h"
#include "sound.h"
#include "input.h"
#include "network.h"
#include "util.h"
#include "animation.h"

#include "locations.h"

SDL_Surface *screen;

#ifdef WIN32

/* IDs from the resource file */
#define START_BUTTON 1
#define CHECKBOX_FULLSCREEN 2
#define QUIT_BUTTON 3

BOOL CALLBACK ConfigDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static int checkbox_is_checked;

    switch (uMsg) {
        case WM_CLOSE:
            EndDialog(hwndDlg, IDCANCEL);
            break;
        case WM_COMMAND:
            switch (wParam) {
                case START_BUTTON:
                    EndDialog(hwndDlg, (checkbox_is_checked)?(IDYES):(IDNO));
                    break;
                case QUIT_BUTTON:
                    EndDialog(hwndDlg, IDCANCEL);
                    break;
                case CHECKBOX_FULLSCREEN:
                    checkbox_is_checked ^= 1;
                    break;
            }
            break;
        default:
            return FALSE;
    }
    return TRUE;
}
#endif

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, int nCmdShow) {
#else
int main( int argc, char** argv) {
#endif
    int i, slide, slide_direction;
    unsigned int x;
    Uint32 ticks;
    int mx, my;
    unsigned int worldmap_xpos, worldmap_ypos;
    Uint8 *keys;
    Uint8 mb;
    SDL_Event e;
    int sdl_flags = SDL_SWSURFACE;
    int btn_hovering = 0, btn_hovering_old = 0;
    int slide_start;
    bool mouse_pressed = false;
    bool quit = false;
    bool benchmark = false;
    unsigned int night_start, night_end;
    GameState *current_game = NULL, *prepared_game = NULL;
    InputDevice* input_devices;
    unsigned int input_device_count, input_device_id;
    Animation *intro;
    AnimationState *intro_playback;
    float wiggle;
    TennixNet* connection = NULL;
    char* net_host = NULL;
    bool net_master = false;
    TennixArchive* tnxar = NULL;

    MenuButton btn_back = {
        NULL, /* not needed for image-based button */
        MENU_OPTIONS_BORDER,
        HEIGHT-MENU_OPTIONS_BORDER,
        0, -1, /* width and height will be set by menu_button_init */
        255, 0, 0,
        GR_BACK
    };
    MenuButton btn_start = {
        NULL, /* not needed for image-based button */
        WIDTH-MENU_OPTIONS_BORDER,
        HEIGHT-MENU_OPTIONS_BORDER,
        -1, -1, /* width and height will be set by menu_button_init */
        0, 255, 0,
        GR_FORWARD
    };
    MenuButton btn_player1 = {
        NULL,
        CONTROLLER_SETUP_BORDER,
        HEIGHT/2 + CONTROLLER_SETUP_SIZE/2 + 10,
        CONTROLLER_SETUP_SIZE, MENU_OPTIONS_BUTTON_HEIGHT,
        50, 50, 255,
        GR_COUNT
    };
    MenuButton btn_player2 = {
        NULL,
        WIDTH-CONTROLLER_SETUP_SIZE-CONTROLLER_SETUP_BORDER,
        HEIGHT/2 + CONTROLLER_SETUP_SIZE/2 + 10,
        CONTROLLER_SETUP_SIZE, MENU_OPTIONS_BUTTON_HEIGHT,
        255, 50, 50,
        GR_COUNT
    };

    int highlight_location = -1;
    float highlight_location_distance = 0.0;
    float new_location_distance = 0.0;
    bool location_info_visible = false;
    unsigned int location_info_xpos = 0, location_info_ypos = 0;

    const SDL_VideoInfo* vi = NULL;

    bool do_help = false;

    int state = MENU_STATE_STARTED;

#ifdef ENABLE_FPS_LIMIT
    Uint32 ft, frames; /* frame timer and frames */
#endif

#if defined(MAEMO) || defined(MACOSX)
    sdl_flags |= SDL_FULLSCREEN;
#endif

#ifdef WIN32
    int mb_result;
    mb_result = DialogBoxA(hInstance, "CONFIG", 0, (DLGPROC)ConfigDialogProc);

    switch (mb_result) {
        case IDYES:
            sdl_flags |= SDL_FULLSCREEN;
            break;
        case IDCANCEL:
            return 0;
            break;
        default:
            break;
    }
#else
    fprintf(stderr, "Tennix Classic Championship Tour 2011 (v" VERSION ")\n" COPYRIGHT "\n" URL "\n\n");

    i = 1;
    while (i < argc) {
        /* A poor/lazy man's getopt */
        #define OPTION_SET(longopt,shortopt) \
                (strcmp(argv[i], longopt)==0 || strcmp(argv[i], shortopt)==0)
        #define OPTION_VALUE \
                ((i+1 < argc)?(argv[i+1]):(NULL))
        #define OPTION_VALUE_PROCESSED \
                (i++)
        if (OPTION_SET("--fullscreen", "-f")) {
            sdl_flags |= SDL_FULLSCREEN;
        }
        else if (OPTION_SET("--help", "-h")) {
            do_help = true;
        }
        else if (OPTION_SET("--benchmark", "-b")) {
            benchmark = true;
        }
        else if (OPTION_SET("--slave", "-s")) {
            net_host = OPTION_VALUE;
            if (OPTION_VALUE != NULL) {
                OPTION_VALUE_PROCESSED;
                net_master = false;
            } else {
                fprintf(stderr, "Missing option parameter.\n");
                do_help = true;
                break;
            }
        }
        else if (OPTION_SET("--master", "-m")) {
            net_host = OPTION_VALUE;
            if (OPTION_VALUE != NULL) {
                OPTION_VALUE_PROCESSED;
                net_master = true;
            } else {
                fprintf(stderr, "Missing option parameter.\n");
                do_help = true;
                break;
            }
        }
        else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            do_help = true;
        }
        i++;
    }

    if (do_help == true) {
        fprintf(stderr, "Usage: %s [OPTIONS]\n\n"
        " Where [OPTIONS] are zero or more of the following:\n\n"
        "    [--fullscreen|-f]           Fullscreen mode\n"
        "    [--benchmark|-b]            Run in benchmark/attract mode\n"
        "    [--master|-m <IP-of-slave>] Network play as master\n"
        "    [--slave|-s <IP-of-master>] Network play as slave\n"
        "    [--help|-h]                 Show help information\n\n"
        " See tennix(6) for details.\n", argv[0]);
        return 0;
    }
#endif

    if (benchmark) {
        srand(100);
    } else {
        srand((unsigned)time(NULL));
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1) {
        fprintf( stderr, "Can't init SDL:  %s\n", SDL_GetError());
        exit( 1);
    }

    vi = SDL_GetVideoInfo();
    if( (screen = SDL_SetVideoMode( WIDTH, HEIGHT, vi->vfmt->BitsPerPixel, sdl_flags)) == NULL) {
        fprintf( stderr, "Can't set video mode: %s\n", SDL_GetError());
        exit( 1);
    }   

    SDL_WM_SetCaption( "Tennix Classic Championship Tour 2011", "Tennix 2011");
    SDL_ShowCursor( SDL_DISABLE);
    SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, 1);

    tnxar = new TennixArchive(ARCHIVE_FILE, ARCHIVE_FILE_INSTALLED);
    init_graphics(*tnxar);
    init_sound(*tnxar);
    init_input(*tnxar);
    init_network();
    delete tnxar;

    if (net_host != NULL) {
        connection = network_connect(net_host, net_master);
    }

    menu_button_init(&btn_back);
    menu_button_init(&btn_start);
    menu_button_init(&btn_player1);
    menu_button_init(&btn_player2);

    input_devices = find_input_devices(&input_device_count);

    current_game = gamestate_load(GAMESTATE_FILE);
    if (current_game != NULL) {
        /* restore pointer to location */
        current_game->location = &(locations[current_game->current_location]);
        /* */
        for (i=1; i<=MAXPLAYERS; i++) {
            if (PLAYER(current_game, i).type == PLAYER_TYPE_HUMAN) {
                input_device_id = PLAYER(current_game, i).input_device_index;
                if (input_device_id < input_device_count) {
                    /* ok, we still have that device around */
                    PLAYER(current_game, i).input = &(input_devices[input_device_id]);
                } else {
                    /* the device vanished - set to AI (FIXME: select new device) */
                    PLAYER(current_game, i).type = PLAYER_TYPE_AI;
                    PLAYER(current_game, i).input = NULL;
                }
            }
        }
    }

#ifdef ENABLE_FPS_LIMIT
    frames = 0;
    ft = SDL_GetTicks();
#endif

    if (benchmark) {
        GameState* g = gamestate_new();
        PLAYER(g, 1).type = PLAYER_TYPE_AI;
        PLAYER(g, 2).type = PLAYER_TYPE_AI;
        g->location = &(locations[0]);
        gameloop(g, connection);
        free(g);
        exit(0);
    }

    /*intro = create_intro();
    intro_playback = animation_state_new(intro);
    animation_state_run(intro_playback, 1);
    animation_state_free(intro_playback);
    animation_free(intro);
    start_fade();*/

    worldmap_xpos = (WIDTH-get_image_width(GR_WORLDMAP))/2;
    worldmap_ypos = (HEIGHT-get_image_height(GR_WORLDMAP))/2;

    i = 0;
    /* Sliding initialization */
    ticks = SDL_GetTicks();
    slide = slide_start = get_image_width(GR_SIDEBAR);
    slide_direction = 0;
    while(!quit) {
        /* State transitions */
        switch (state) {
            case MENU_STATE_STARTED:
                state = MENU_STATE_SLIDE_TO_MAINMENU;
                break;
            case MENU_STATE_SLIDE_TO_MAINMENU:
                clear_screen();
                //rectangle(0, 0, WIDTH, HEIGHT, 80, 80, 80);
                store_screen();
                slide = slide_start;
                slide_direction = -1;
                state = MENU_STATE_SLIDE_TO_MAINMENU_IN_PROGRESS;
                break;
            case MENU_STATE_SLIDE_TO_MAINMENU_IN_PROGRESS:
                if (slide == 0) {
                    slide_direction = 0;
                    state = MENU_STATE_MAINMENU;
                }
                break;
            case MENU_STATE_MAINMENU:
                free(prepared_game);
                prepared_game = NULL;
                break;
            case MENU_STATE_SLIDE_TO_LOCATION:
                slide = 1;
                slide_direction = 3;
                state = MENU_STATE_SLIDE_TO_LOCATION_IN_PROGRESS;
                break;
            case MENU_STATE_SLIDE_TO_LOCATION_IN_PROGRESS:
                if (slide == slide_start) {
                    state = MENU_STATE_FADE_TO_LOCATION;
                }
                break;
            case MENU_STATE_FADE_TO_LOCATION:
                start_fade();
                state = MENU_STATE_LOCATION;
                clear_screen();

                //rectangle(0, 0, WIDTH, HEIGHT, 80, 80, 80);
                /* Draw and store the worldmap with day/night times */
                show_image(GR_WORLDMAP, WIDTH/2-get_image_width(GR_WORLDMAP)/2, HEIGHT/2-get_image_height(GR_WORLDMAP)/2, 255);
                day_night(get_image_width(GR_WORLDMAP), &night_start, &night_end);
                if (night_start > night_end) {
                    rectangle_alpha(worldmap_xpos, worldmap_ypos, night_end, get_image_height(GR_WORLDMAP), 0, 0, 0, 150);
                    rectangle_alpha(worldmap_xpos+night_start, worldmap_ypos, get_image_width(GR_WORLDMAP)-night_start, get_image_height(GR_WORLDMAP), 0, 0, 0, 150);
                } else {
                    rectangle_alpha(worldmap_xpos+night_start, worldmap_ypos, night_end-night_start, get_image_height(GR_WORLDMAP), 0, 0, 0, 150);
                }

                /* add misc items to screen */
                font_draw_string(FONT_XLARGE, "Pick a location", (WIDTH-font_get_string_width(FONT_XLARGE, "Pick a location"))/2, 20);

                store_screen();
                break;
            case MENU_STATE_FADE_TO_OPTIONS:
                start_fade();
                clear_screen();
                //rectangle(0, 0, WIDTH, HEIGHT, 80, 80, 80);
                rectangle(CONTROLLER_SETUP_BORDER, HEIGHT/2-CONTROLLER_SETUP_SIZE/2, CONTROLLER_SETUP_SIZE, CONTROLLER_SETUP_SIZE, 150, 150, 150);
                rectangle(WIDTH-CONTROLLER_SETUP_BORDER-CONTROLLER_SETUP_SIZE, (HEIGHT-CONTROLLER_SETUP_SIZE)/2, CONTROLLER_SETUP_SIZE, CONTROLLER_SETUP_SIZE, 150, 150, 150);
                font_draw_string(FONT_XLARGE, "Controller setup", (WIDTH-font_get_string_width(FONT_XLARGE, "Controller setup"))/2, 20);
                font_draw_string(FONT_MEDIUM, "Player 1", 130 - font_get_string_width(FONT_MEDIUM, "Player 1")/2, (HEIGHT-CONTROLLER_SETUP_SIZE)/2 - 10 - font_get_height(FONT_MEDIUM));
                font_draw_string(FONT_MEDIUM, "Player 1", 130 - font_get_string_width(FONT_MEDIUM, "Player 1")/2, (HEIGHT-CONTROLLER_SETUP_SIZE)/2 - 10 - font_get_height(FONT_MEDIUM));
                font_draw_string(FONT_MEDIUM, "Player 2", WIDTH - CONTROLLER_SETUP_SIZE/2 - CONTROLLER_SETUP_BORDER - font_get_string_width(FONT_MEDIUM, "Player 2")/2, (HEIGHT-CONTROLLER_SETUP_SIZE)/2 - 10 - font_get_height(FONT_MEDIUM));
                store_screen();
                start_fade();
                state = MENU_STATE_OPTIONS;
                break;
            case MENU_STATE_LOCATION:
            case MENU_STATE_OPTIONS:
                /* Prepare a new game */
                if (prepared_game == NULL) {
                    prepared_game = gamestate_new();
                    prepared_game->location = NULL;
                    /* FIXME - this should not be written here, but taken from the input devices */
                    btn_player1.text = "Keyboard (arrows)";
                    btn_player2.text = "Carl Van Court";
#ifdef MAEMO
                    PLAYER(prepared_game, 1).input_device_index = 0;
#else
                    PLAYER(prepared_game, 1).input_device_index = 2;
#endif
                    PLAYER(prepared_game, 1).type = PLAYER_TYPE_HUMAN;
                    PLAYER(prepared_game, 1).input = &(input_devices[PLAYER(prepared_game, 1).input_device_index]);
                    location_info_visible = false;
                    prepared_game->current_location = -1;
                    highlight_location = -1;
                }
                break;
            case MENU_STATE_SLIDE_TO_GAME:
                /*slide = 1;
                slide_direction = 2;
                state = MENU_STATE_SLIDE_TO_GAME_IN_PROGRESS;
                break;
            case MENU_STATE_SLIDE_TO_GAME_IN_PROGRESS:
                if (slide == slide_start) {
                    state = MENU_STATE_GAME;
                }*/
                state = MENU_STATE_GAME;
                break;
            case MENU_STATE_SLIDE_TO_RESUME:
                slide = 1;
                slide_direction = 2;
                state = MENU_STATE_SLIDE_TO_RESUME_IN_PROGRESS;
                break;
            case MENU_STATE_SLIDE_TO_RESUME_IN_PROGRESS:
                if (slide == slide_start) {
                    state = MENU_STATE_RESUME;
                }
                break;
            case MENU_STATE_GAME:
                if (prepared_game == NULL) {
                    fprintf(stderr, "Game not yet prepared!\n");
                    exit(EXIT_FAILURE);
                }

                /* Cancel a possibly started game */
                free(current_game);
                current_game = prepared_game;
                prepared_game = NULL;
                /* no break - we are continuing with "resume" */
            case MENU_STATE_RESUME:
                if (current_game == NULL) {
                    fprintf(stderr, "Cannot resume game!\n");
                    exit(EXIT_FAILURE);
                }
                start_fade();
                gameloop(current_game, connection);
                SDL_Delay(150);
                while(SDL_PollEvent(&e));
#ifdef ENABLE_FPS_LIMIT
                frames = 0;
                ft = SDL_GetTicks();
#endif
                start_fade();
                state = MENU_STATE_SLIDE_TO_MAINMENU;
                break;
            case MENU_STATE_SLIDE_TO_QUIT:
                slide = 1;
                slide_direction = 3;
                state = MENU_STATE_SLIDE_TO_QUIT_IN_PROGRESS;
                break;
            case MENU_STATE_SLIDE_TO_QUIT_IN_PROGRESS:
                if (slide == slide_start) {
                    state = MENU_STATE_QUIT;
                }
                break;
            case MENU_STATE_QUIT:
                quit = true;
                break;
            default:
                fprintf(stderr, "State error: %d\n", state);
                exit(EXIT_FAILURE);
        }

        /* Sliding */
        if (SDL_GetTicks() > ticks + 20) {
            if (slide >= 1 && slide <= slide_start) {
                slide += slide_direction+(slide_direction*slide/(sqrt(2*slide)));
                slide = MAX(0, MIN(slide_start, slide));
            } else if (slide_direction != 0) {
                slide_direction = 0;
            }
            ticks = SDL_GetTicks();
        }

        /* Graphics */
#ifdef DEBUG
        if (state != MENU_STATE_OPTIONS) {
            fill_image_offset(GR_FOG, 0, 0, WIDTH, HEIGHT, -i, 0);
        }
#endif
        show_image(GR_SIDEBAR, WIDTH-get_image_width(GR_SIDEBAR)+slide, 0, 255);
        show_image(GR_TENNIXLOGO, WIDTH-get_image_width(GR_SIDEBAR)-10, 20-slide, 255);
        if (state != MENU_STATE_OPTIONS && state != MENU_STATE_LOCATION) {
            /* Main Menu */
            show_image(GR_BTN_PLAY, WIDTH-get_image_width(GR_BTN_PLAY)+slide+(slide/7)+3-(3*(btn_hovering==MENU_START)), 150, 255);
            if (current_game != NULL) {
                show_image(GR_BTN_RESUME, WIDTH-get_image_width(GR_BTN_RESUME)+slide+(slide/7)+3-(3*(btn_hovering==MENU_RESUME)), 230, 255);
                font_draw_string(FONT_SMALL, "match paused", 10, 10);
            } else {
                font_draw_string(FONT_MEDIUM, "Tennix Classic Championship Tour 2011", 10, 10);
            }
            //font_draw_string_color(FONT_MEDIUM, URL, 10-1, HEIGHT-10-1-font_get_height(FONT_MEDIUM), 130, 130, 130);
            font_draw_string_color(FONT_MEDIUM, URL, 10, HEIGHT-10-font_get_height(FONT_MEDIUM), 100, 100, 100);
            show_image(GR_BTN_QUIT, WIDTH-get_image_width(GR_BTN_QUIT)+slide+(slide/7)+3-(3*(btn_hovering==MENU_QUIT)), 350, 255);
        } else if (state == MENU_STATE_OPTIONS) {
            /* Options screen */
            draw_button_object(&btn_back, mx, my);
            draw_button_object(&btn_start, mx, my);
            draw_button_object(&btn_player1, mx, my);
            draw_button_object(&btn_player2, mx, my);
            wiggle = 15*sinf((float)ticks/300.);
            if (PLAYER(prepared_game, 1).input_device_index > -1) {
                show_image_rotozoom(input_devices[PLAYER(prepared_game, 1).input_device_index].icon, CONTROLLER_SETUP_BORDER + CONTROLLER_SETUP_SIZE/2, HEIGHT/2, wiggle, 1.0);
            } else {
                show_image_rotozoom(GR_INPUT_AI, CONTROLLER_SETUP_BORDER + CONTROLLER_SETUP_SIZE/2, HEIGHT/2, wiggle, 1.0);
            }
            if (PLAYER(prepared_game, 2).input_device_index > -1) {
                show_image_rotozoom(input_devices[PLAYER(prepared_game, 2).input_device_index].icon, WIDTH-CONTROLLER_SETUP_BORDER-CONTROLLER_SETUP_SIZE/2, HEIGHT/2, -wiggle, 1.0);
            } else {
                show_image_rotozoom(GR_INPUT_AI, WIDTH-CONTROLLER_SETUP_BORDER-CONTROLLER_SETUP_SIZE/2, HEIGHT/2, -wiggle, 1.0);
            }
        } else if (state == MENU_STATE_LOCATION) {
            /* Location selection screen */
            for (x=0; x<location_count(); x++) {
                new_location_distance = SQUARE_DISTANCE(mx-worldmap_xpos-locations[x].worldmap_x,
                                                        my-worldmap_ypos-locations[x].worldmap_y);
                if (highlight_location == -1) {
                    if (new_location_distance < 20*20) {
                        highlight_location = x;
                    }
                } else {
                    highlight_location_distance = SQUARE_DISTANCE(mx-worldmap_xpos-locations[highlight_location].worldmap_x,
                                                                  my-worldmap_ypos-locations[highlight_location].worldmap_y);
                    if (highlight_location_distance > 20*20) {
                        highlight_location = -1;
                    }
                    if (highlight_location_distance > new_location_distance && new_location_distance < 20*20) {
                        highlight_location = x;
                    }
                }
            }
            if (prepared_game != NULL) {
                if (!location_info_visible) {
                    for (x=0; x<location_count(); x++) {
                        /* draw rectangle for location at "x"*/
                        if (highlight_location != -1 && (unsigned int)highlight_location == x) {
                            rectangle(worldmap_xpos + locations[x].worldmap_x-3, worldmap_ypos + locations[x].worldmap_y-3, 7, 7, 255*((i/10)%2), 0, 255*((i/10)%2));
                        }

                        rectangle(worldmap_xpos + locations[x].worldmap_x-2, worldmap_ypos + locations[x].worldmap_y-2, 5, 5, 255, 255*(prepared_game->current_location != -1 && x==(unsigned int)(prepared_game->current_location)), 0);
                    }
                } else {
                    rectangle_alpha(location_info_xpos-5, location_info_ypos-5, 10+get_image_width(prepared_game->location->photo), get_image_height(prepared_game->location->photo)+100, 30, 30, 30, 200);
                    show_sprite(prepared_game->location->photo, (i/1000)%(prepared_game->location->photo_frames), prepared_game->location->photo_frames, location_info_xpos, location_info_ypos, 255);
                font_draw_string(FONT_SMALL, prepared_game->location->name, location_info_xpos+MENU_OPTIONS_BORDER, location_info_ypos+MENU_OPTIONS_BORDER+200);
                font_draw_string(FONT_SMALL, prepared_game->location->area, location_info_xpos+MENU_OPTIONS_BORDER, location_info_ypos+MENU_OPTIONS_BORDER+200+font_get_height(FONT_SMALL));
                font_draw_string(FONT_SMALL, prepared_game->location->city, location_info_xpos+MENU_OPTIONS_BORDER, location_info_ypos+MENU_OPTIONS_BORDER+200+2*font_get_height(FONT_SMALL));
                font_draw_string(FONT_SMALL, prepared_game->location->court_type_name, location_info_xpos+MENU_OPTIONS_BORDER, location_info_ypos+MENU_OPTIONS_BORDER+200+3*font_get_height(FONT_SMALL));
                }
                if (prepared_game->location != NULL) {
                    draw_button_object(&btn_start, mx, my);
                }
            }
            draw_button_object(&btn_back, mx, my);
        }

        SDL_PollEvent( &e);
        if( e.type == SDL_QUIT) {
            state = MENU_STATE_SLIDE_TO_QUIT;
            /*break;*/
        }

        keys = SDL_GetKeyState( NULL);
        mb = SDL_GetMouseState( &mx, &my);

        btn_hovering_old = btn_hovering;
        if (state == MENU_STATE_MAINMENU) {
            btn_hovering = M_POS_DECODE(mx, my);
            if (current_game == NULL) {
                btn_hovering &= ~MENU_RESUME;
            }
        } else if (state == MENU_STATE_LOCATION) {
            if (M_POS_BUTTON(btn_back, mx, my)) {
                btn_hovering = MENU_QUIT;
            } else if (M_POS_BUTTON(btn_start, mx, my)) {
                btn_hovering = MENU_START;
            } else {
                btn_hovering = 0;
            }
        } else if (state == MENU_STATE_OPTIONS) {
            if (M_POS_BUTTON(btn_back, mx, my)) {
                btn_hovering = MENU_QUIT;
            } else if (M_POS_BUTTON(btn_start, mx, my)) {
                btn_hovering = MENU_START;
            } else if (M_POS_BUTTON(btn_player1, mx, my)) {
                btn_hovering = MENU_PLAYER1;
            } else if (M_POS_BUTTON(btn_player2, mx, my)) {
                btn_hovering = MENU_PLAYER2;
            } else {
                btn_hovering = 0;
            }
        } else {
            /* No menu screen - no hovering. */
            btn_hovering = 0;
        }
#ifndef MAEMO /* On Maemo, we cannot really "hover" (touchscreen!) */
        if (btn_hovering_old != btn_hovering && btn_hovering != 0) {
#ifdef HAVE_VOICE_FILES
            if (btn_hovering == MENU_QUIT) {
                play_sample(VOICE_QUIT_IT);
            } else if (btn_hovering == MENU_START) {
                play_sample(VOICE_NEW_GAME);
            } else {
                play_sample(SOUND_MOUSEOVER);
            }
#else
            /*play_sample(SOUND_MOUSEOVER);*/
#endif
        }
#endif
   
        if( keys[SDLK_ESCAPE] || keys['q']) {
            /* FIXME: do the state thingie! */
            break;
        }

        if( keys['f']) {
            SDL_WM_ToggleFullScreen( screen);
        }
   
#ifndef MAEMO /* No mouse cursor on Maemo (we have a touchscreen) */
        if (state == MENU_STATE_MAINMENU || state == MENU_STATE_OPTIONS || state == MENU_STATE_LOCATION) {
            show_image(GR_CURSOR, mx-2, my-1, 255);
        }
#endif

        /* Draw the "real" mouse coordinates */
        /*rectangle(mx-1, my-1, 2, 2, 255, 255, 255);*/

        /* Store the screen, because we are fading after this screen update */
        /*if (!(mb & SDL_BUTTON(SDL_BUTTON_LEFT)) && btn_hovering != MENU_NONE && mouse_pressed == true) store_screen();*/

        updatescr();

        if( mb & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            if (!mouse_pressed) {
                play_sample(SOUND_MOUSEOVER);
            }
            mouse_pressed = true;
        } else if (mouse_pressed == true) {
            /* Mouse button released */
            if (state == MENU_STATE_MAINMENU || state == MENU_STATE_OPTIONS || state == MENU_STATE_LOCATION) {
#ifdef HAVE_VOICE_FILES
                if (btn_hovering == MENU_START) {
                    play_sample(VOICE_LETS_GO);
                } else {
                    play_sample(SOUND_MOUSECLICK);
                }
#else
                /*play_sample(SOUND_MOUSEOVER);*/
#endif
            }
            if (state == MENU_STATE_MAINMENU) {
                switch (btn_hovering) {
                    case MENU_START:
                        state = MENU_STATE_SLIDE_TO_LOCATION;
                        break;
                    case MENU_RESUME:
                        state = MENU_STATE_SLIDE_TO_RESUME;
                        break;
                    case MENU_QUIT:
                        state = MENU_STATE_SLIDE_TO_QUIT;
                        break;
                }
            } else if (state == MENU_STATE_LOCATION) {
                switch (btn_hovering) {
                    case MENU_START:
                        if (prepared_game->location != NULL) {
                            state = MENU_STATE_FADE_TO_OPTIONS;
                        }
                        break;
                    case MENU_QUIT:
                        state = MENU_STATE_SLIDE_TO_MAINMENU;
                        break;
                    default:
                        if (!location_info_visible && highlight_location != -1) {
                            prepared_game->current_location = highlight_location;
                            /* Set the day/night status */
                            if (night_start < night_end) {
                                locations[prepared_game->current_location].night = (locations[prepared_game->current_location].worldmap_x > night_start && locations[prepared_game->current_location].worldmap_x < night_end);
                            } else {
                                locations[prepared_game->current_location].night = (locations[prepared_game->current_location].worldmap_x < night_end || locations[prepared_game->current_location].worldmap_x > night_start);
                            }
                            prepared_game->location = &(locations[prepared_game->current_location]);
                            location_info_xpos = MAX(0, MIN(WIDTH-320-50, mx-320/2));
                            location_info_ypos = MAX(0, MIN(HEIGHT-200-160, my-200/2));
                            location_info_visible = true;
                        } else {
                            location_info_visible = false;
                            highlight_location = -1;
                            prepared_game->current_location = -1;
                            prepared_game->location = NULL;
                        }
                        break;
                }
            } else if (state == MENU_STATE_OPTIONS) {
                switch (btn_hovering) {
                    case MENU_START:
                        state = MENU_STATE_SLIDE_TO_GAME;
                        break;
                    case MENU_QUIT:
                        state = MENU_STATE_FADE_TO_LOCATION;
                        break;
                    case MENU_PLAYER1:
                        /* advance the input device index */
                        PLAYER(prepared_game, 1).input_device_index++;
                        if (PLAYER(prepared_game, 1).input_device_index == (signed int)input_device_count) {
                            PLAYER(prepared_game, 1).input_device_index = -1;
                        }

                        if (input_devices[PLAYER(prepared_game, 1).input_device_index].exclusive_to_player == 2) {
                            PLAYER(prepared_game, 1).input_device_index++;
                        }

                        /* determine the selected input device */
                        if (PLAYER(prepared_game, 1).input_device_index == -1) {
                            PLAYER(prepared_game, 1).type = PLAYER_TYPE_AI;
                            PLAYER(prepared_game, 1).input = NULL;
                            btn_player1.text = "Carl Van Court";
                        } else {
                            PLAYER(prepared_game, 1).type = PLAYER_TYPE_HUMAN;
                            PLAYER(prepared_game, 1).input = &(input_devices[PLAYER(prepared_game, 1).input_device_index]);
                            btn_player1.text = input_device_get_name(PLAYER(prepared_game, 1).input);
                        }
                        break;
                    case MENU_PLAYER2:
                        /* advance the input device index */
                        PLAYER(prepared_game, 2).input_device_index++;

                        if (input_devices[PLAYER(prepared_game, 2).input_device_index].exclusive_to_player == 1) {
                            PLAYER(prepared_game, 2).input_device_index++;
                        }

                        if (PLAYER(prepared_game, 2).input_device_index == (signed int)input_device_count) {
                            PLAYER(prepared_game, 2).input_device_index = -1;
                        }

                        /* determine the selected input device */
                        if (PLAYER(prepared_game, 2).input_device_index == -1) {
                            PLAYER(prepared_game, 2).type = PLAYER_TYPE_AI;
                            PLAYER(prepared_game, 2).input = NULL;
                            btn_player2.text = "Carl Van Court";
                        } else {
                            PLAYER(prepared_game, 2).type = PLAYER_TYPE_HUMAN;
                            PLAYER(prepared_game, 2).input = &(input_devices[PLAYER(prepared_game, 2).input_device_index]);
                            btn_player2.text = input_device_get_name(PLAYER(prepared_game, 2).input);
                        }
                        break;
                }
            }
            mouse_pressed = false;
        }
        i++;
#ifdef ENABLE_FPS_LIMIT
        while (frames*1000.0/((float)(SDL_GetTicks()-ft+1))>(float)(DEFAULT_FPS)) {
            SDL_Delay(10);
        }
        frames++;
#endif
    }

    if (current_game != NULL) {
        if (gamestate_save(current_game, GAMESTATE_FILE) != 0) {
            fprintf(stderr, "Warning: cannot save gamestate to %s\n", GAMESTATE_FILE);
        }
    }

    /* Play the credits */
    intro = create_credits();
    intro_playback = animation_state_new(intro);
    animation_state_run(intro_playback, 1);
    animation_state_free(intro_playback);
    animation_free(intro);

    uninit_graphics();
    uninit_input();
    uninit_network();

    SDL_Quit();
    return 0;
}


void menu_button_init(MenuButton* b)
{
    int w, h;

    if (b->image_id != GR_COUNT) {
        /**
         * If the button is an image, the "w" and "h" attributes of the
         * MenuButton struct are simply factors with which the real width
         * and height of the image (=button) should be multiplied and added
         * to the "x" and "y" position and the "w" and "h" are replaced with
         * the real size of the image for the collision detection
         **/
        w = b->w;
        h = b->h;

        b->w = get_image_width(b->image_id);
        b->h = get_image_height(b->image_id);
        b->x += w*b->w;
        b->y += h*b->h;
    }
}


