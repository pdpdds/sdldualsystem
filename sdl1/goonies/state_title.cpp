#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include "windows.h"
#endif

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_mixer.h"

#include "List.h"
#include "Symbol.h"
#include "2DCMC.h"
#include "auxiliar.h"
#include "GLTile.h"
#include "2DCMC.h"
#include "keyboardstate.h"
#include "randomc.h"

#include "GLTManager.h"
#include "SoundManager.h"
#include "SFXManager.h"
#include "MusicManager.h"
#include "GObject.h"
#include "GO_enemy.h"
#include "GO_fratelli.h"
#include "GMap.h"
#include "TheGoonies.h"
#include "TheGooniesApp.h"

#include "font_extractor.h"

#define TITLE_INTRO_TIME 200 

extern bool fullscreen;
extern bool screen_shake;
extern bool water_reflection;
extern bool ambient_light; 

extern int difficulty;
extern SDL_Surface *screen_sfc;
extern SDL_Surface *toogle_video_mode(bool fullscreen);


int menu_noptions[6]={5,0,8,8,3,5};
char *menu0_options[5] = {"play", "password", "how to play", "settings", "quit"};
char *menu2_options[8] = {"difficulty: normal", "customize controls", "audio settings", "video settings", "level pack: original", "editor", "credits", "back"};
char *menu3_options[8] = {"up/jump", "right", "down", "left", "punch", "pause", "quit", "back"};
char *menu4_options[4] = {"sound effects: maximum", "ambience: maximum", "music: maximum", "back"};
char *menu5_options[5] = {"display: fullscreen", "screen shake: on", "water reflections: on", "ambient lights: on", "back"};


int TheGooniesApp::title_cycle(KEYBOARDSTATE *k)
{
	int old_mouse_x = m_mouse_x;
	int old_mouse_y = m_mouse_y;

	SDL_GetMouseState(&m_mouse_x,&m_mouse_y);
	if (!m_mouse_click_x.EmptyP()) {
		m_mouse_click_x.Delete();
		m_mouse_click_y.Delete();
		m_mouse_button=1;
	} else {
		m_mouse_button=0;
	} // if

	if (m_mouse_x==old_mouse_x && m_mouse_y==old_mouse_y) {
		m_mouse_movement_timmer++;
		
		if (m_mouse_movement_timmer>150 && m_draw_mouse_status==2) {		
			m_draw_mouse_status=3;
			m_draw_mouse=1.0f;
		} // if
	} else {
		m_mouse_movement_timmer=0;

		if (m_draw_mouse_status==0) {
			m_draw_mouse_status=1;
			m_draw_mouse=0.0f;
		} // if
	} // if

	switch(m_draw_mouse_status) {
	case 0:
			break;
	case 1: m_draw_mouse+=0.05f;
			if (m_draw_mouse>=1.0f) {
				m_draw_mouse=1.0f;
				m_draw_mouse_status=2;
			} // if
			break;
	case 2:
			break;
	case 3: m_draw_mouse-=0.05f;
			if (m_draw_mouse<=0.0f) {
				m_draw_mouse=0.0f;
				m_draw_mouse_status=0;
			} // if 			
			break;
	} // switch

	if (m_game!=0) {
		delete m_game;
		m_game=0;
		m_SFXM->SFX_stop_continuous();
	} // if

    if (m_title_state == 0 && m_state_cycle==0) {
		m_GLTM->cache();
		m_MusicM->music_play(MUSIC_ID,"title",m_music_volume,-1);
    }
	
    if (m_title_state == 0 && m_state_cycle > TITLE_INTRO_TIME) {
        m_time_since_last_key = 0;
        m_title_state = 1;
        m_state_cycle = 0;
    }

    if (m_title_state == 2 && m_state_cycle >= 25) {
        m_time_since_last_key = 0;
        if (m_title_next_menu == -1) {}
        else if (m_title_next_menu == -2) {
            // go to intro animation:
            m_titleanimation_state = 0;
			m_titleanimation_speed = 0;
			m_titleanimation_shift = 0;
            return THEGOONIES_STATE_TITLEANIMATION;
        } else if (m_title_next_menu == -2) {
            // go to password:
            m_title_next_menu = 1;
        } else if (m_title_next_menu == -3) {
            // go to how to play:
            m_howtoplay_state = 0;
            m_howtoplay_cycle = 0;
            return THEGOONIES_STATE_HOWTOPLAY;
        } else if (m_title_next_menu == -4) {
            // go to credits:
            m_credits_state = 0;
            m_credits_cycle = 0;
            return THEGOONIES_STATE_CREDITS;
        } else if (m_title_next_menu == -5) {
            // go to editor:
            m_credits_state = 0;
            m_credits_cycle = 0;
            return THEGOONIES_STATE_EDITOR;
        } else {
            m_title_current_menu = m_title_next_menu;
            m_title_next_menu = -1;
            m_title_state = 1;
            m_state_cycle = 0;
            m_title_option_selected = 0;
        }
    }

    if (m_title_state == 2 && m_state_cycle >= 50) {
        switch (m_title_current_menu) {
            case 0:
                if (m_title_next_menu == -1) {
                    m_title_password[0] = 0;
                    if (m_title_option_selected == 0) {
                        return THEGOONIES_STATE_GAMESTART;
                    }
                    if (m_title_option_selected == 4) {
                        return THEGOONIES_STATE_NONE;
                    }
                } else {
				}
                break;
            case 1:
                if (m_title_next_menu == -1) {
					m_MusicM->music_stop(MUSIC_ID);
                    return THEGOONIES_STATE_GAMESTART;
                } else {
				}
                break;
        }
    }

    switch (m_title_state) {
        case 0:
            if ((k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]) ||
                    (k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
					(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)]))
			{
                m_title_state = 1;
                m_state_cycle = 0;
            }
            break;
        case 1:

			// Chance options with mouse:
			if (m_mouse_x!=old_mouse_x ||
				m_mouse_y!=old_mouse_y) {
				int *x1,*x2,*y1,*y2;

				m_title_options_x1.Rewind();
				m_title_options_x2.Rewind();
				m_title_options_y1.Rewind();
				m_title_options_y2.Rewind();
				while(m_title_options_x1.Iterate(x1) &&
					  m_title_options_x2.Iterate(x2) &&
					  m_title_options_y1.Iterate(y1) &&
					  m_title_options_y2.Iterate(y2)) {
					if (!m_title_waiting_keystroke &&
						m_mouse_x>=*x1 && m_mouse_x<*x2 &&
						m_mouse_y>=*y1 && m_mouse_y<*y2) {
						int v = m_title_options_x1.PositionRef(x1);
						if (m_title_option_selected != v) m_SFXM->SFX_play("sfx/cursor_move", m_sfx_volume);
						m_title_option_selected = v;						
					} // if
				} // while
			
			} // if

            switch (m_title_current_menu) {
                case 0:  // main menu:
                    {						
                        if ((k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]))
                        {
                            m_title_option_selected = 4;
                            m_title_state = 2;
                            m_state_cycle = 0;
                            m_title_next_menu = -1;
                            m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
                            m_time_since_last_key = 0;
							m_MusicM->music_fadeout(MUSIC_ID,50);
                        }
						
                        if ((k->keyboard[m_keys_configuration[GKEY_UP]] && !k->old_keyboard[m_keys_configuration[GKEY_UP]]) ||
							(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 2)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 2)]))
                        {
                            m_title_option_selected--;
                            if (m_title_option_selected < 0)
                                m_title_option_selected = 4;
                            m_SFXM->SFX_play("sfx/cursor_move", m_sfx_volume);
                            m_time_since_last_key = 0;
                        }
						
                        if ((k->keyboard[m_keys_configuration[GKEY_DOWN]] && !k->old_keyboard[m_keys_configuration[GKEY_DOWN]]) ||
							(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 3)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 3)]))
                        {
                            m_title_option_selected++;
                            if (m_title_option_selected > 4)
                                m_title_option_selected = 0;
                            m_SFXM->SFX_play("sfx/cursor_move", m_sfx_volume);
                            m_time_since_last_key = 0;
                        }

                        if (m_mouse_button == 1 ||
							(k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
                            (k->keyboard[SDLK_RETURN] && !k->old_keyboard[SDLK_RETURN]) ||
							(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)]))
                        {
                            if (m_title_option_selected == 0 ||
                                m_title_option_selected == 4) {
                                m_title_state = 2;
                                m_state_cycle = 0;
                                m_title_next_menu = -1;
                                m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
								if (m_title_option_selected == 4) m_MusicM->music_fadeout(MUSIC_ID,50);
                            }
                            if (m_title_option_selected == 1) {
                                m_title_state = 2;
                                m_state_cycle = 0;
                                m_title_next_menu = 1;
								m_title_password[0]=0;
                                m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
                            }
                            if (m_title_option_selected == 2) {
                                // HOW TO PLAY
                                m_title_state = 2;
                                m_state_cycle = 0;
                                m_title_next_menu = -3;
                                m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
                            }
                            if (m_title_option_selected == 3) {
                                // OPTIONS
                                m_title_state = 2;
                                m_state_cycle = 0;
                                m_title_next_menu = 2;
                                m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
                            } 
                            m_time_since_last_key = 0;
                        }

                    }
                    m_time_since_last_key++;
                    if (m_time_since_last_key > 1000) {
                        // Show intro animation:
                        m_title_state = 2;
                        m_state_cycle = 0;
                        m_title_next_menu = -2;
                    } 
                    break;

                case 1:  // entering password:
                    {
                        int i;
                        int letters[37] = {SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e,
                                           SDLK_f, SDLK_g, SDLK_h, SDLK_i, SDLK_j,
                                           SDLK_k, SDLK_l, SDLK_m, SDLK_n, SDLK_o,
                                           SDLK_p, SDLK_q, SDLK_r, SDLK_s, SDLK_t,
                                           SDLK_u, SDLK_v, SDLK_w, SDLK_x, SDLK_y,
                                           SDLK_z, SDLK_SPACE,
                                           SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4,
                                           SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9};
                        char characters[37] = {'a', 'b', 'c', 'd', 'e',
                                               'f', 'g', 'h', 'i', 'j',
                                               'k', 'l', 'm', 'n', 'o',
                                               'p', 'q', 'r', 's', 't',
                                               'u', 'v', 'w', 'x', 'y',
                                               'z', ' ',
                                               '0', '1', '2', '3', '4',
                                               '5', '6', '7', '8', '9'};


                        if ((k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]))
                        {
                            m_title_state = 2;
                            m_state_cycle = 0;
                            m_title_next_menu = 0;
                            m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
                        }

                        if ((k->keyboard[SDLK_RETURN] && !k->old_keyboard[SDLK_RETURN]))
                        {
                            m_title_state = 2;
                            m_state_cycle = 0;
                            m_title_next_menu = -1;
                            m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
                        }

                        if ((k->keyboard[SDLK_BACKSPACE] && !k->old_keyboard[SDLK_BACKSPACE]))
                        {
                            int l = strlen(m_title_password);
                            if (l > 0) {
                                l--;
                                m_title_password[l] = 0;
                                m_SFXM->SFX_play("sfx/cursor_move", m_sfx_volume);
                            }
                        }

                        for (i = 0;i < 37;i++)
                        {
                            if ((k->keyboard[letters[i]] && !k->old_keyboard[letters[i]])) {
                                int l = strlen(m_title_password);
                                if (l == 15)
                                    l--;

                                m_title_password[l++] = characters[i];
                                m_title_password[l] = 0;
                                m_SFXM->SFX_play("sfx/cursor_move", m_sfx_volume);
                            }
                        }

                    }
                    break;

                case 2:  // OPTIONS
                    {
                        if ((k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]))
                        {
                            m_title_option_selected = 6;
                            m_title_state = 2;
                            m_state_cycle = 0;
                            m_title_next_menu = 0;
                            m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
                        }

                        if ((k->keyboard[m_keys_configuration[GKEY_UP]] && !k->old_keyboard[m_keys_configuration[GKEY_UP]]) ||
							(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 2)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 2)]))
							
                        {
                            m_title_option_selected--;
                            if (m_title_option_selected < 0)
                                m_title_option_selected = 7;
                            m_SFXM->SFX_play("sfx/cursor_move", m_sfx_volume);
                        }

                        if ((k->keyboard[m_keys_configuration[GKEY_DOWN]] && !k->old_keyboard[m_keys_configuration[GKEY_DOWN]]) ||
							(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 3)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 3)]))
							
                        {
                            m_title_option_selected++;
                            if (m_title_option_selected > 7)
                                m_title_option_selected = 0;
                            m_SFXM->SFX_play("sfx/cursor_move", m_sfx_volume);
                        }

                        if (m_mouse_button == 1 ||
							(k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
                                (k->keyboard[SDLK_RETURN] && !k->old_keyboard[SDLK_RETURN]) ||
							(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)]))
							
                        {
							if (m_title_option_selected == 0) {
								m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
								switch (difficulty) {
									case 25:
										difficulty = 50;
										break;
									case 50:
										difficulty = 100;
										break;
									case 100:
										difficulty = 150;
										break;
									case 150:
										difficulty = 25;
										break;
									default:
										difficulty = 100;
								}
							}
							// controls
							if (m_title_option_selected == 1) {
								m_title_state = 2;
								m_state_cycle = 0;
								m_title_next_menu = 3;
								m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
							}
							// to audio menu
                            if (m_title_option_selected == 2) {
								m_title_state = 2;
								m_state_cycle = 0;
								m_title_next_menu = 4;
								m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
                            }
							// to video menu
                            if (m_title_option_selected == 3) {
								m_title_state = 2;
								m_state_cycle = 0;
								m_title_next_menu = 5;
								m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
                            }
							// Change level pack:
							if (m_title_option_selected == 4) {

								// ...

								m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
							}
							// Editor:
							if (m_title_option_selected == 5) {
								m_title_state = 2;
								m_state_cycle = 0;
								m_title_next_menu = -5;
								m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
							}
							// credits
							if (m_title_option_selected == 6) {
								m_title_state = 2;
								m_state_cycle = 0;
								m_title_next_menu = -4;
								m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
							}
							if (m_title_option_selected == 7) {
								m_title_state = 2;
								m_state_cycle = 0;
								m_title_next_menu = 0;
								m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
							}

							save_configuration();
                        }
                    }

                    break;

                case 3:  // KEYBOARD
                    {
                        if (m_title_waiting_keystroke)
                        {
                            if (!k->keyevents.EmptyP()) {
                                m_keys_configuration[m_title_option_selected] = k->keyevents[0]->sym;
                                m_title_waiting_keystroke = false;

                                save_configuration();
                            }
                        } else
                        {
                            if ((k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]])) {
                                m_title_option_selected = 7;
                                m_title_state = 2;
                                m_state_cycle = 0;
                                m_title_next_menu = 2;
                                m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
                            }

                            if ((k->keyboard[m_keys_configuration[GKEY_UP]] && !k->old_keyboard[m_keys_configuration[GKEY_UP]]) || 
								(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 2)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 2)])) {
                                m_title_option_selected--;
                                if (m_title_option_selected < 0)
                                    m_title_option_selected = 7;
                                m_SFXM->SFX_play("sfx/cursor_move", m_sfx_volume);
                            }
							
                            if ((k->keyboard[m_keys_configuration[GKEY_DOWN]] && !k->old_keyboard[m_keys_configuration[GKEY_DOWN]]) ||
								(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 3)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 3)])) {
								
                                m_title_option_selected++;
                                if (m_title_option_selected > 7)
                                    m_title_option_selected = 0;
                                m_SFXM->SFX_play("sfx/cursor_move", m_sfx_volume);
                            }

                            if (m_mouse_button == 1 ||
								(k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
                                (k->keyboard[SDLK_RETURN] && !k->old_keyboard[SDLK_RETURN]) ||
								(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)])) {
								
                                if (m_title_option_selected == 7) {
                                    m_title_state = 2;
                                    m_state_cycle = 0;
                                    m_title_next_menu = 2;
                                    m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
                                } else {
                                    m_title_waiting_keystroke = true;
                                }
                            }
                        }
                    }
					break;
                case 4:  // AUDIO
				{
					if ((k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]))
					{
						m_title_option_selected = 3;
						m_title_state = 2;
						m_state_cycle = 0;
						m_title_next_menu = 2;
						m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
					}
					
					if ((k->keyboard[m_keys_configuration[GKEY_UP]] && !k->old_keyboard[m_keys_configuration[GKEY_UP]]) ||
						(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 2)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 2)]))
					{
						m_title_option_selected--;
						if (m_title_option_selected < 0)
							m_title_option_selected = 3;
						m_SFXM->SFX_play("sfx/cursor_move", m_sfx_volume);
					}
					
					if ((k->keyboard[m_keys_configuration[GKEY_DOWN]] && !k->old_keyboard[m_keys_configuration[GKEY_DOWN]]) ||
						(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 3)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 3)]))						{
						m_title_option_selected++;
						if (m_title_option_selected > 3)
							m_title_option_selected = 0;
						m_SFXM->SFX_play("sfx/cursor_move", m_sfx_volume);
					}
					
					if (m_mouse_button == 1 ||
						(k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
						(k->keyboard[SDLK_RETURN] && !k->old_keyboard[SDLK_RETURN]) ||
						(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)]))
					{
						if (m_title_option_selected == 0) {
							switch (m_sfx_volume) {
								case 0:
									m_sfx_volume = 32;
									break;
								case 32:
									m_sfx_volume = 64;
									break;
								case 64:
									m_sfx_volume = 96;
									break;
								case 96:
									m_sfx_volume = MIX_MAX_VOLUME;
									break;
								case MIX_MAX_VOLUME:
									m_sfx_volume = 0;
									break;
								default:
									m_sfx_volume = 0;
							}
							m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
						}
						if (m_title_option_selected == 1) {
							switch (m_ambient_volume) {
								case 0:
									m_ambient_volume = 32;
									break;
								case 32:
									m_ambient_volume = 64;
									break;
								case 64:
									m_ambient_volume = 96;
									break;
								case 96:
									m_ambient_volume = MIX_MAX_VOLUME;
									break;
								case MIX_MAX_VOLUME:
									m_ambient_volume = 0;
									break;
								default:
									m_ambient_volume = 0;
							}
							m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
						}
						if (m_title_option_selected == 2) {
							switch (m_music_volume) {
								case 0:
									m_music_volume = 32;
									break;
								case 32:
									m_music_volume = 64;
									break;
								case 64:
									m_music_volume = 96;
									break;
								case 96:
									m_music_volume = MIX_MAX_VOLUME;
									break;
								case MIX_MAX_VOLUME:
									m_music_volume = 0;
									break;
								default:
									m_music_volume = 0;
							}
							m_MusicM->music_volume(MUSIC_ID,m_music_volume);
							m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
						}
						if (m_title_option_selected == 3) {
							m_title_state = 2;
							m_state_cycle = 0;
							m_title_next_menu = 2;
							m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
						}
						
						save_configuration();
					}
				}
				break;
                case 5:  // VIDEO
				{
					if ((k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]))
					{
						m_title_option_selected = 2;
						m_title_state = 2;
						m_state_cycle = 0;
						m_title_next_menu = 2;
						m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
					}
					
					if ((k->keyboard[m_keys_configuration[GKEY_UP]] && !k->old_keyboard[m_keys_configuration[GKEY_UP]]) ||
						(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 2)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 2)]))
					{
						m_title_option_selected--;
						if (m_title_option_selected < 0)
							m_title_option_selected = 4;
						m_SFXM->SFX_play("sfx/cursor_move", m_sfx_volume);
					}
					
					if ((k->keyboard[m_keys_configuration[GKEY_DOWN]] && !k->old_keyboard[m_keys_configuration[GKEY_DOWN]]) ||
						(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 3)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 3)]))
					{
						m_title_option_selected++;
						if (m_title_option_selected > 4)
							m_title_option_selected = 0;
						m_SFXM->SFX_play("sfx/cursor_move", m_sfx_volume);
					}
					
					if (m_mouse_button == 1 ||
						(k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
						(k->keyboard[SDLK_RETURN] && !k->old_keyboard[SDLK_RETURN]) ||
						(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)]))
					{
						if (m_title_option_selected == 0) {
							fullscreen = (fullscreen ? false : true);
							screen_sfc = toogle_video_mode(fullscreen);
							m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
						}
						if (m_title_option_selected == 1) {
							screen_shake = (screen_shake ? false : true);
							m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
						}
						if (m_title_option_selected == 2) {
							water_reflection = (water_reflection ? false : true);
							m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
						}
						if (m_title_option_selected == 3) {
							ambient_light = (ambient_light ? false : true);
							m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
						}
						if (m_title_option_selected == 4) {
							m_title_state = 2;
							m_state_cycle = 0;
							m_title_next_menu = 2;
							m_SFXM->SFX_play("sfx/cursor_select", m_sfx_volume);
						}
						
						save_configuration();
					}
				}
				break;												
            }
            break;
        case 2:
            break;
    }
    return THEGOONIES_STATE_TITLE;
}

void TheGooniesApp::title_draw(void)
{
    int menu_y = 255;

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

	m_title_options_x1.Delete();
	m_title_options_x2.Delete();
	m_title_options_y1.Delete();
	m_title_options_y2.Delete();

    switch (m_title_state) {
        case 0: {
                m_GLTM->get
                ("bg_copyright"); // this line is just to reload the background
                if (m_state_cycle > (TITLE_INTRO_TIME - 100)) {
                    float f = float(m_state_cycle - (TITLE_INTRO_TIME - 100)) / 100;
                    if (f >= 1)
                        f = 1;
                    m_GLTM->get
                    ("bg_copyright")->draw(1, 1, 1, f);
                }

                {
                    float f = float(m_state_cycle) / TITLE_INTRO_TIME;
                    if (f >= 1)
                        f = 1;
                    f = float(sqrt(f));
                    m_GLTM->get_smooth("title_logo")->draw(( -5600)*(1 - f) + 46*f, ( -850)*(1 - f) + 17*f, 0, 0, 16*(1 - f) + 1*f);
                }

                if (m_state_cycle < 100) {
                    glEnable(GL_COLOR_MATERIAL);

                    {
                        float f = 0;
                        f = abs(int(100 - m_state_cycle)) / 100.0F;
                        glColor4f(0, 0, 0, f);
                    }
                    glNormal3f(0.0, 0.0, 1.0);

                    glBegin(GL_QUADS);
                    glVertex3f(0, 0, 0);
                    glVertex3f(0, 480, 0);
                    glVertex3f(640, 480, 0);
                    glVertex3f(640, 0, 0);
                    glEnd();
                }
            }
            break;

        case 1:
        case 2: {
                int i;
                float f = m_state_cycle / 25.0f;
                m_GLTM->get
                ("bg_copyright")->draw(0, 0, 0, 0, 1);
                m_GLTM->get_smooth("title_logo")->draw(46, 17, 0, 0, 1);

                if (m_title_state == 2)
                    f = 1 - f;
                if (f < 0)
                    f = 0;
                if (f > 1)
                    f = 1;

                switch (m_title_current_menu) {
                    case 0:  // MAIN MENU
                        for (i = 0;i < 5;i++) {
                            int x=0, y=0, dx=0, dy=0;

                            if (m_title_option_selected == i) {
                                font_print_c(320, menu_y + i*38, 0, 0, f, "font_hl", menu0_options[i], -4);
                                font_box_c("font_hl", menu0_options[i], &x, &y, &dx, &dy, -4);
                                m_GLTM->get
                                ("skull_cursor")->set_hotspot(m_GLTM->get
                                                              ("skull_cursor")->get_dx() / 2,
                                                              m_GLTM->get
                                                              ("skull_cursor")->get_dy() / 2);
                                m_GLTM->get
                                ("skull_cursor")->draw(float(320 + (x - 24)), float(menu_y + i*38 + 4), 0, m_state_cycle*4.0f, f);
                                m_GLTM->get
                                ("skull_cursor")->draw(float(320 + (x + dx + 24)), float(menu_y + i*38 + 4), 0, -m_state_cycle*4.0f, f);
                            } else {
                                font_box_c("font", menu0_options[i], &x, &y, &dx, &dy, -2);
                                font_print_c(320, menu_y + i*38, 0, 0, f, "font", menu0_options[i], -2);
                            }

							{
								m_title_options_x1.Add(new int(320 + (x - 24)));
								m_title_options_x2.Add(new int(320 + (x + dx + 24)));
								m_title_options_y1.Add(new int(menu_y + i*38 - 16));
								m_title_options_y2.Add(new int(menu_y + i*38 + 16));
							}
                        }
                        break;
                    case 1:  // ENTER PASSWORD
                        {
                            int x, y, dx, dy;

                            font_print_c(320, menu_y + 1*38, 0, 0, f, "font", "enter password:", -2);

                            font_print_c(320, menu_y + 3*38, 0, 0, f, "font_hl", m_title_password, -4);
                            font_box_c("font_hl", m_title_password, &x, &y, &dx, &dy, -4);
                            m_GLTM->get
                            ("skull_cursor")->set_hotspot(m_GLTM->get
                                                          ("skull_cursor")->get_dx() / 2,
                                                          m_GLTM->get
                                                          ("skull_cursor")->get_dy() / 2);
                            m_GLTM->get
                            ("skull_cursor")->draw(float(320 + (x + dx + 24)), float(menu_y + 3*38 + 4), 0, -m_state_cycle*4.0f, f);
                        }
                        break;
                    case 2:  // OPTIONS
                        {
                            char tmp[80];
                            float factor = 0.66f;

                            for (i = 0;i < 8;i++)
                            {
	                            int x=0, y=0, dx=0, dy=0;

                                sprintf(tmp, menu2_options[i]);
                                if (i == 0) {
                                    if (difficulty == 0)
                                        sprintf(tmp, "difficulty: god mode/no damage");
                                    if (difficulty == 25)
                                        sprintf(tmp, "difficulty: wimp/baby");
                                    if (difficulty == 50)
                                        sprintf(tmp, "difficulty: chunk/easy");
                                    if (difficulty == 100)
                                        sprintf(tmp, "difficulty: mikey/normal");
                                    if (difficulty == 150)
                                        sprintf(tmp, "difficulty: goonie/hard");
                                }
                                if (m_title_option_selected == i) {
                                    font_print_c(320, (int)(menu_y + i*38*factor), 0, 0, f*factor, "font_hl", tmp, -4);
                                    font_box_c("font_hl", tmp, &x, &y, &dx, &dy, -4);
                                    m_GLTM->get
                                    ("skull_cursor")->set_hotspot(m_GLTM->get
                                                                  ("skull_cursor")->get_dx() / 2,
                                                                  m_GLTM->get
                                                                  ("skull_cursor")->get_dy() / 2);
                                    m_GLTM->get
                                    ("skull_cursor")->draw(320 + (x - 24)*factor, menu_y + i*38*factor + 4, 0, m_state_cycle*4.0f, f*factor);
                                    m_GLTM->get
                                    ("skull_cursor")->draw(320 + (x + dx + 24)*factor, menu_y + i*38*factor + 4, 0, -m_state_cycle*4.0f, f*factor);
                                } else {
                                    font_box_c("font", tmp, &x, &y, &dx, &dy, -2);
                                    font_print_c(320, (int)(menu_y + i*38*factor), 0, 0, f*factor, "font", tmp, -2);
                                }
								{
									m_title_options_x1.Add(new int(int(320 + (x - 24)*factor)));
									m_title_options_x2.Add(new int(int(320 + (x + dx + 24)*factor)));
									m_title_options_y1.Add(new int(int(menu_y + (i*38 - 16)*factor)));
									m_title_options_y2.Add(new int(int(menu_y + (i*38 + 16)*factor)));
								}
                            }
                        }
                        break;
                    case 3:  // KEYBOARD CONFIGURATION:
                        {
                            float factor = 0.66f;
                            char tmp[256];

                            for (i = 0;i < 8;i++)
                            {
								int x=0, y=0, dx=0, dy=0;
                                if (i < 7) {
                                    if (m_title_waiting_keystroke && i == m_title_option_selected) {
                                        sprintf(tmp, "%s        -", menu3_options[i]);
                                    } else {
                                        sprintf(tmp, "%s        [%s]", menu3_options[i], SDL_GetKeyName(SDLKey(m_keys_configuration[i])));
                                    }
                                } else {
                                    sprintf(tmp, menu3_options[i]);
                                }
                                if (m_title_option_selected == i) {
                                    font_print_c(320, (int)(menu_y + i*38*factor), 0, 0, f*factor, "font_hl", tmp, -4);
                                    font_box_c("font_hl", tmp, &x, &y, &dx, &dy, -4);
                                    m_GLTM->get
                                    ("skull_cursor")->set_hotspot(m_GLTM->get
                                                                  ("skull_cursor")->get_dx() / 2,
                                                                  m_GLTM->get
                                                                  ("skull_cursor")->get_dy() / 2);
                                    m_GLTM->get
                                    ("skull_cursor")->draw(320 + (x - 24)*factor, menu_y + i*38*factor + 4, 0, m_state_cycle*4.0f, f*factor);
                                    m_GLTM->get
                                    ("skull_cursor")->draw(320 + (x + dx + 24)*factor, menu_y + i*38*factor + 4, 0, -m_state_cycle*4.0f, f*factor);
                                } else {
									font_box_c("font", tmp, &x, &y, &dx, &dy, -2);
                                    font_print_c(320, (int)(menu_y + i*38*factor), 0, 0, f*factor, "font", tmp, -2);
                                }
								{
									m_title_options_x1.Add(new int(int(320 + (x - 24)*factor)));
									m_title_options_x2.Add(new int(int(320 + (x + dx + 24)*factor)));
									m_title_options_y1.Add(new int(int(menu_y + (i*38 - 16)*factor)));
									m_title_options_y2.Add(new int(int(menu_y + (i*38 + 16)*factor)));
								}
                            }
                        }
                        break;
                    case 4:  // AUDIO
					{
						char tmp[80];
						for (i = 0;i < 4;i++)
						{
							int x=0, y=0, dx=0, dy=0;
							sprintf(tmp, menu4_options[i]);
							if (i == 0) {
								if (m_sfx_volume == 0)
									sprintf(tmp, "sound effects: off");
								if (m_sfx_volume == 32)
									sprintf(tmp, "sound effects: low");
								if (m_sfx_volume == 64)
									sprintf(tmp, "sound effects: medium");
								if (m_sfx_volume == 96)
									sprintf(tmp, "sound effects: high");
								if (m_sfx_volume == MIX_MAX_VOLUME)
									sprintf(tmp, "sound effects: maximum");
							}
							if (i == 1) {
								if (m_ambient_volume == 0)
									sprintf(tmp, "ambience: off");
								if (m_ambient_volume == 32)
									sprintf(tmp, "ambience: low");
								if (m_ambient_volume == 64)
									sprintf(tmp, "ambience: medium");
								if (m_ambient_volume == 96)
									sprintf(tmp, "ambience: high");
								if (m_ambient_volume == MIX_MAX_VOLUME)
									sprintf(tmp, "ambience: maximum");
							}
							if (i == 2) {
								if (m_music_volume == 0)
									sprintf(tmp, "music: off");
								if (m_music_volume == 32)
									sprintf(tmp, "music: low");
								if (m_music_volume == 64)
									sprintf(tmp, "music: medium");
								if (m_music_volume == 96)
									sprintf(tmp, "music: high");
								if (m_music_volume == MIX_MAX_VOLUME)
									sprintf(tmp, "music: maximum");
							}
							if (m_title_option_selected == i) {
								font_print_c(320, (int)(menu_y + i*38), 0, 0, f, "font_hl", tmp, -4);
								font_box_c("font_hl", tmp, &x, &y, &dx, &dy, -4);
								m_GLTM->get
                                    ("skull_cursor")->set_hotspot(m_GLTM->get
                                                                  ("skull_cursor")->get_dx() / 2,
                                                                  m_GLTM->get
                                                                  ("skull_cursor")->get_dy() / 2);
								m_GLTM->get
                                    ("skull_cursor")->draw(float(320 + (x - 24)), float(menu_y + i*38) + 4, 0, m_state_cycle*4.0f, f);
								m_GLTM->get
                                    ("skull_cursor")->draw(float(320 + (x + dx + 24)), float(menu_y + i*38) + 4, 0, -m_state_cycle*4.0f, f);
							} else {
								font_box_c("font", tmp, &x, &y, &dx, &dy, -2);
								font_print_c(320, (int)(menu_y + i*38), 0, 0, f, "font", tmp, -2);
							}
							{
								m_title_options_x1.Add(new int(320 + (x - 24)));
								m_title_options_x2.Add(new int(320 + (x + dx + 24)));
								m_title_options_y1.Add(new int(menu_y + i*38 - 16));
								m_title_options_y2.Add(new int(menu_y + i*38 + 16));
							}
						}
					}
                    break;
                    case 5:  // VIDEO
					{
						char tmp[80];
						for (i = 0;i < 5;i++)
						{
							int x=0, y=0, dx=0, dy=0;
							sprintf(tmp, menu5_options[i]);
							if (i == 0) {
								sprintf(tmp, "display: %s", (fullscreen ? "fullscreen" : "windowed"));
							}
							if (i == 1) {
								sprintf(tmp, "screen shake: %s", (screen_shake ? "on" : "off"));
							}
							if (i == 2) {
								sprintf(tmp, "water reflections: %s", (water_reflection ? "on" : "off"));
							}
							if (i == 3) {
								sprintf(tmp, "ambient lights: %s", (ambient_light ? "on" : "off"));
							}
							if (m_title_option_selected == i) {
								font_print_c(320, (int)(menu_y + i*38), 0, 0, f, "font_hl", tmp, -4);
								font_box_c("font_hl", tmp, &x, &y, &dx, &dy, -4);
								m_GLTM->get
                                    ("skull_cursor")->set_hotspot(m_GLTM->get
                                                                  ("skull_cursor")->get_dx() / 2,
                                                                  m_GLTM->get
                                                                  ("skull_cursor")->get_dy() / 2);
								m_GLTM->get
                                    ("skull_cursor")->draw(float(320 + (x - 24)), float(menu_y + i*38) + 4, 0, m_state_cycle*4.0f, f);
								m_GLTM->get
                                    ("skull_cursor")->draw(float(320 + (x + dx + 24)), float(menu_y + i*38) + 4, 0, -m_state_cycle*4.0f, f);
							} else {
								font_box_c("font", tmp, &x, &y, &dx, &dy, -2);
								font_print_c(320, (int)(menu_y + i*38), 0, 0, f, "font", tmp, -2);
							}
							{
								m_title_options_x1.Add(new int(320 + (x - 24)));
								m_title_options_x2.Add(new int(320 + (x + dx + 24)));
								m_title_options_y1.Add(new int(menu_y + i*38 - 16));
								m_title_options_y2.Add(new int(menu_y + i*38 + 16));
							}
						}
					}
					break;											
                }
			}

			if (m_draw_mouse>0) {
				m_GLTM->get("interface_pointer_glow")->draw(1,1,1,float(sin(m_state_cycle/(4*M_PI))*m_draw_mouse),float(m_mouse_x),float(m_mouse_y),0,0,1);
				m_GLTM->get("interface_pointer")->draw(1,1,1,m_draw_mouse,float(m_mouse_x),float(m_mouse_y),0,0,1);
			}

            if (m_title_state == 2 && m_title_next_menu == -1) {

                glEnable(GL_COLOR_MATERIAL);
                {
                    float f = 0;
                    f = abs(int(m_state_cycle)) / 50.0F;
                    if (f >= 1)
                        f = 1;
                    glColor4f(0, 0, 0, f);
                }
                glNormal3f(0.0, 0.0, 1.0);

                glBegin(GL_QUADS);
                glVertex3f(0, 0, 0);
                glVertex3f(0, 480, 0);
                glVertex3f(640, 480, 0);
                glVertex3f(640, 0, 0);
                glEnd();
            }
            break;
    }
}
