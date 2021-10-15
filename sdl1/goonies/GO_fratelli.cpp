#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include "windows.h"
#endif

#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include "List.h"

#include "auxiliar.h"
#include "2DCMC.h"
#include "Symbol.h"
#include "GLTile.h"
#include "keyboardstate.h"
#include "VirtualController.h"

#include "GLTManager.h"
#include "SoundManager.h"
#include "SFXManager.h"
#include "GObject.h"
#include "GO_character.h"
#include "GO_enemy.h"
#include "GO_fratelli.h"
#include "GO_musicalnote.h"
#include "GObjectCreator.h"
#include "GO_bridge.h"

#include "GMap.h"

#include "TheGooniesCtnt.h"

GO_fratelli::GO_fratelli(int x, int y, int sfx_volume, int color) : GO_enemy(x, y, sfx_volume)
{
    m_class = new Symbol(fratelli_symbol);

    m_state = CSTATE_STANDING_LEFT;
    m_state_cycle = 0;
    m_layer = 2;

    m_color = color;

    m_walking_channel = -1;
    m_climbing_channel = -1;

    m_hit_counter = 0;
	m_turning_counter = 0;

    switch (m_color) {
        case 0:
            m_walking_speed = YELLOW_FRATELLI_SPEED;
            break;
        case 1:
            m_walking_speed = BLUE_FRATELLI_SPEED;
            break;
        case 2:
            m_walking_speed = GREEN_FRATELLI_SPEED;
            break;
        case 3:
            m_walking_speed = RED_FRATELLI_SPEED;
            break;
        case 4:
            m_walking_speed = WHITE_FRATELLI_SPEED;
            break;
        case 5:
            m_walking_speed = GREY_FRATELLI_SPEED;
            break;
    }

    m_AI_state = 0;
    m_AI_state_cycle = 0;

    m_time_since_last_bullet = 0;

    m_vc = new VirtualController();
}

GO_fratelli::~GO_fratelli()
{
    delete m_vc;
}


bool GO_fratelli::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{

#ifdef __DEBUG_MESSAGES
    output_debug_message("Fratelli: (%f,%f)\n", m_x, m_y);
#endif

    m_vc->new_cycle();

    m_vc->m_joystick[VC_LEFT] = false;
    m_vc->m_joystick[VC_RIGHT] = false;
    m_vc->m_joystick[VC_UP] = false;
    m_vc->m_joystick[VC_DOWN] = false;
    m_vc->m_button[0] = false;

    // Artificial Intelligence:
    switch (m_AI_state) {
        case 0:  // stand
            if (m_AI_state_cycle == 30)
                m_vc->m_joystick[VC_RIGHT] = true;
            if (m_AI_state_cycle == 60)
                m_vc->m_joystick[VC_LEFT] = true;
            if (m_AI_state_cycle == 90)
                m_vc->m_joystick[VC_RIGHT] = true;

            // decide where the player is and go to that:
            if (m_AI_state_cycle == 120) {
                GO_character *c = (GO_character *)map->get_object(character_symbol);
                if (c != 0) {
                    if (c->get_x() < m_x)
                        m_AI_state = 1;
                    else
                        m_AI_state = 2;
                } else {
                    m_AI_state = 1 + (rand() % 2);
                }
                m_AI_state_cycle = 0;
            }
            break;
        case 1:  // left
            m_vc->m_joystick[VC_LEFT] = true;

			if ((m_AI_state_cycle%2)==0) {
				if (map->collision_with_object(this, GLTM, rope_symbol) && (rand() % 25) == 0) {
					m_AI_state = 3;
					m_AI_state_cycle = 0;
				}

				m_y += 50;
				if (map->collision_with_object(this, GLTM, rope_symbol) && (rand() % 25) == 0) {
					m_AI_state = 4;
					m_AI_state_cycle = 0;
				}
				m_y -= 50;

				if (!map->collision_with_background(this, -10, 10, GLTM)) {
					GObject *go = map->collision_with_object(this, -10,10, GLTM, bridge_symbol);
					if (go==0) m_vc->m_joystick[VC_UP] = true;
				}
				if (m_state == CSTATE_WALKING_LEFT && map->collision_with_background(this, -10, -2, GLTM))
					m_AI_state = 2;

				if (m_AI_state_cycle > 25 && m_time_since_last_bullet > 150) {
					GO_character *c = (GO_character *)map->get_object(character_symbol);
					if (c != 0) {
						if (c->get_x() < m_x && c->get_y() > m_y - 32 && c->get_y() < m_y + 32) {
							m_AI_state = 5;
							m_AI_state_cycle = 0;
						}
					}
				}
			} // if
            break;
        case 2:  // right
            m_vc->m_joystick[VC_RIGHT] = true;

			if ((m_AI_state_cycle%2)==0) {
				if (map->collision_with_object(this, GLTM, rope_symbol) && (rand() % 25) == 0) {
					m_AI_state = 3;
					m_AI_state_cycle = 0;
				}

				m_y += 50;
				if (map->collision_with_object(this, GLTM, rope_symbol) && (rand() % 25) == 0) {
					m_AI_state = 4;
					m_AI_state_cycle = 0;
				}
				m_y -= 50;

				if (!map->collision_with_background(this, 10, 10, GLTM)) {
					GObject *go = map->collision_with_object(this, -10,10, GLTM, bridge_symbol);
					if (go==0) m_vc->m_joystick[VC_UP] = true;
				}
				if (m_state == CSTATE_WALKING_RIGHT && map->collision_with_background(this, 10, -2, GLTM))
					m_AI_state = 1;

				if (m_AI_state_cycle > 25 && m_time_since_last_bullet > 150) {
					GO_character *c = (GO_character *)map->get_object(character_symbol);
					if (c != 0) {
						if (c->get_x() > m_x && c->get_y() > m_y - 32 && c->get_y() < m_y + 32) {
							m_AI_state = 6;
							m_AI_state_cycle = 0;
						}
					}
				}
			} // if

            break;
        case 3:  // climb up
            m_vc->m_joystick[VC_UP] = true;
            if (m_AI_state_cycle > 5 &&
                    m_state != CSTATE_CLIMBING_UP &&
                    m_state != CSTATE_CLIMBING &&
                    m_state != CSTATE_CLIMBING_DOWN) {
                m_AI_state = 0;
                m_AI_state_cycle = 0;
            }
            break;
        case 4:  // climb down
            m_vc->m_joystick[VC_DOWN] = true;
            if (m_AI_state_cycle > 5 &&
                    m_state != CSTATE_CLIMBING_UP &&
                    m_state != CSTATE_CLIMBING &&
                    m_state != CSTATE_CLIMBING_DOWN) {
                m_AI_state = 0;
                m_AI_state_cycle = 0;
            }
            break;
        case 5:  // shoot left
            if (m_AI_state_cycle < 5)
                m_vc->m_button[0] = true;
            if (m_AI_state_cycle == 50) {
                m_AI_state = 1;
                m_AI_state_cycle = 0;
                m_time_since_last_bullet = 0;
            }
            break;
        case 6:  // shoot right
            if (m_AI_state_cycle < 5)
                m_vc->m_button[0] = true;
            if (m_AI_state_cycle == 50) {
                m_AI_state = 2;
                m_AI_state_cycle = 0;
                m_time_since_last_bullet = 0;
            }
            break;
    }

    m_AI_state_cycle++;
    m_time_since_last_bullet++;

    if (m_state == CSTATE_TAKENDOWN) {
        m_AI_state = 0;
        m_AI_state_cycle = 0;
    }


    return internal_cycle(m_vc, map, layer, GLTM, SFXM);
}

bool GO_fratelli::internal_cycle(VirtualController *v, GMap *map, int layer, GLTManager *GLTM, SFXManager *SFXM)
{
    m_previous_state = m_state;

    if (m_last_tile_used == 0)
        m_last_tile_used = GLTM->get
                           ("ob_yellowguy-l1");

    GO_character *c = (GO_character *)map->get_object(character_symbol);
    bool has_redbadbook = false;
    bool has_purplebook = false;
    if (c != 0 && c->player_has("GO_redbadbook") != 0)
        has_redbadbook = true;
    if (c != 0 && c->player_has("GO_purpledbook") != 0)
        has_purplebook = true;

    if (has_purplebook)
        v->m_button[0] = false;

    switch (m_state) {
        case CSTATE_STANDING_LEFT:
        case CSTATE_STANDING_RIGHT:
            assert(m_layer == 2);
            if (v->m_joystick[VC_UP] && !v->m_old_joystick[VC_UP]) {
                GObject *o;
                o = map->collision_with_object(this, GLTM, rope_symbol);
                if (o != 0) {
                    // climb a rope!
                    m_state = CSTATE_CLIMBING_UP;
                    m_x = o->get_x() + 10;
                } else {
                    if (SFXM != 0) {
                        switch (m_color) {
                            case 0:
                                SFXM->SFX_play("sfx/fratelli_jump_ma", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 1:
                                SFXM->SFX_play("sfx/fratelli_jump_francis", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 2:
                                SFXM->SFX_play("sfx/fratelli_jump_ma", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 3:
                                SFXM->SFX_play("sfx/fratelli_jump_francis", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 4:
                                SFXM->SFX_play("sfx/fratelli_jump_jake", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 5:
                                SFXM->SFX_play("sfx/fratelli_jump_jake", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                        }

                    }
                    if (m_state == CSTATE_STANDING_LEFT) {
                        m_state = CSTATE_JUMPING_LEFT;
                        m_state_cycle = 0;
                    } else {
                        m_state = CSTATE_JUMPING_RIGHT;
                        m_state_cycle = 0;
                    }
                }

            } else {
                if (v->m_joystick[VC_LEFT]) {
                    m_state_cycle = 0;
                    m_state = CSTATE_WALKING_LEFT;
                }
                if (v->m_joystick[VC_RIGHT]) {
                    m_state_cycle = 0;
                    m_state = CSTATE_WALKING_RIGHT;
                }

                // down an rope:
                if (v->m_joystick[VC_DOWN] && !v->m_old_joystick[VC_DOWN]) {
                    m_y += 50;
                    GObject *o;
                    o = map->collision_with_object(this, GLTM, rope_symbol);
                    if (o != 0) {
                        set_layer(3, map);
                        m_state = CSTATE_CLIMBING_DOWN;
                        m_x = o->get_x() + 10;
                        m_y -= 28;
                    } else {
                        m_y -= 50;
                    }
                }

                // shoot:
                if (v->m_button[0] && !v->m_old_button[0]) {
                    if (m_state == CSTATE_STANDING_LEFT || m_state == CSTATE_WALKING_LEFT) {
                        m_state = CSTATE_SHOOT_LEFT;
                        m_state_cycle = 0;
                    } else {
                        m_state = CSTATE_SHOOT_RIGHT;
                        m_state_cycle = 0;
                    }
                }
            }

            // test for fall:
            if (m_state != CSTATE_ENTERING_DOOR) {
                m_y++;
                if (!map->collision_with_background(this, GLTM)) {
                   GObject *go = map->collision_with_object(this, GLTM, bridge_symbol);
                   if (go == 0) {
                      if (m_state == CSTATE_STANDING_LEFT)
                          m_state = CSTATE_FALLING_LEFT;
                      else
                          m_state = CSTATE_FALLING_RIGHT;
                      m_state_cycle = 0;
                   }

                }
                m_y--;
            }
			if (map->collision_with_object(this, GLTM, bigrock_symbol)) {
				m_state = CSTATE_TAKENDOWN;
				m_state_cycle = 0;
			}			
								
            break;
        case CSTATE_WALKING_LEFT:
            assert(m_layer == 2);
            m_state_cycle++;
            m_x -= (has_redbadbook ? m_walking_speed * 1.25f : m_walking_speed);
            if (v->m_joystick[VC_UP] && !v->m_old_joystick[VC_UP]) {
                GObject *o;
                o = map->collision_with_object(this, GLTM, rope_symbol);
                if (o != 0) {
                    // climb a rope!
                    m_state = CSTATE_CLIMBING_UP;
                    m_x = o->get_x() + 10;
                } else {
                    if (SFXM != 0) {
                        switch (m_color) {
                            case 0:
                                SFXM->SFX_play("sfx/fratelli_jump_ma", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 1:
                                SFXM->SFX_play("sfx/fratelli_jump_francis", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 2:
                                SFXM->SFX_play("sfx/fratelli_jump_ma", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 3:
                                SFXM->SFX_play("sfx/fratelli_jump_francis", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 4:
                                SFXM->SFX_play("sfx/fratelli_jump_jake", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 5:
                                SFXM->SFX_play("sfx/fratelli_jump_jake", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                        }
                    }
                    m_state = CSTATE_JUMPING_LEFT_LEFT;
                    m_state_cycle = 0;
                }
            } else {
                if (!v->m_joystick[VC_LEFT]) {
                    m_state_cycle = 0;
                    m_state = CSTATE_STANDING_LEFT;
                }
                if (v->m_joystick[VC_RIGHT] && !v->m_old_joystick[VC_RIGHT]) {
                    m_state_cycle = 0;
                    m_state = CSTATE_WALKING_RIGHT;
                }

                // down an rope:
                if (v->m_joystick[VC_DOWN] && !v->m_old_joystick[VC_DOWN]) {
                    m_y += 50;
                    GObject *o;
                    o = map->collision_with_object(this, GLTM, rope_symbol);
                    if (o != 0) {
                        set_layer(3, map);
                        m_state = CSTATE_CLIMBING_DOWN;
                        m_x = o->get_x() + 10;
                        m_y -= 28;
                    } else {
                        m_y -= 50;
                    }
                }

                // shoot:
                if (v->m_button[0] && !v->m_old_button[0]) {
                    if (m_state == CSTATE_STANDING_LEFT || m_state == CSTATE_WALKING_LEFT) {
                        m_state = CSTATE_SHOOT_LEFT;
                        m_state_cycle = 0;
                    } else {
                        m_state = CSTATE_SHOOT_RIGHT;
                        m_state_cycle = 0;
                    }
                }
            }

            // test for fall:
            if (m_state != CSTATE_ENTERING_DOOR) {
                m_y++;
                if (!map->collision_with_background(this, GLTM)) {
                   GObject *go = map->collision_with_object(this, GLTM, bridge_symbol);
                   if (go == 0) {
                      m_state = CSTATE_FALLING_LEFT;
                      m_state_cycle = 0;
                   }
                }
                m_y--;
            }

            break;
        case CSTATE_WALKING_RIGHT:
            assert(m_layer == 2);
            m_state_cycle++;
            m_x += (has_redbadbook ? m_walking_speed * 1.25f : m_walking_speed);
            if (v->m_joystick[VC_UP] && !v->m_old_joystick[VC_UP]) {
                GObject *o;
                o = map->collision_with_object(this, GLTM, rope_symbol);
                if (o != 0) {
                    // climb a rope!
                    m_state = CSTATE_CLIMBING_UP;
                    m_x = o->get_x() + 10;
                } else {
                    if (SFXM != 0) {
                        switch (m_color) {
                            case 0:
                                SFXM->SFX_play("sfx/fratelli_jump_ma", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 1:
                                SFXM->SFX_play("sfx/fratelli_jump_francis", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 2:
                                SFXM->SFX_play("sfx/fratelli_jump_ma", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 3:
                                SFXM->SFX_play("sfx/fratelli_jump_francis", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 4:
                                SFXM->SFX_play("sfx/fratelli_jump_jake", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                            case 5:
                                SFXM->SFX_play("sfx/fratelli_jump_jake", m_sfx_volume, get_angle(map), get_distance(map));
                                break;
                        }
                    }
                    m_state = CSTATE_JUMPING_RIGHT_RIGHT;
                    m_state_cycle = 0;
                }
            } else {
                if (v->m_joystick[VC_LEFT] && !v->m_old_joystick[VC_LEFT]) {
                    m_state_cycle = 0;
                    m_state = CSTATE_WALKING_LEFT;
                }
                if (!v->m_joystick[VC_RIGHT]) {
                    m_state_cycle = 0;
                    m_state = CSTATE_STANDING_RIGHT;
                }

                // down an rope:
                if (v->m_joystick[VC_DOWN] && !v->m_old_joystick[VC_DOWN]) {
                    m_y += 50;
                    GObject *o;
                    o = map->collision_with_object(this, GLTM, rope_symbol);
                    if (o != 0) {
                        set_layer(3, map);
                        m_state = CSTATE_CLIMBING_DOWN;
                        m_x = o->get_x() + 10;
                        m_y -= 28;
                    } else {
                        m_y -= 50;
                    }
                }

                // shoot:
                if (v->m_button[0] && !v->m_old_button[0]) {
                    if (m_state == CSTATE_STANDING_LEFT || m_state == CSTATE_WALKING_LEFT) {
                        m_state = CSTATE_SHOOT_LEFT;
                        m_state_cycle = 0;
                    } else {
                        m_state = CSTATE_SHOOT_RIGHT;
                        m_state_cycle = 0;
                    }
                }
            }

            // test for fall:
            if (m_state != CSTATE_ENTERING_DOOR) {
                m_y++;
                if (!map->collision_with_background(this, GLTM)) {
                   GObject *go = map->collision_with_object(this, GLTM, bridge_symbol);
                   if (go == 0) {
                      m_state = CSTATE_FALLING_RIGHT;
                      m_state_cycle = 0;
                   }

                }
                m_y--;
            }

            break;
        case CSTATE_FALLING_LEFT: {
                int i, j = 1;
                if (m_state_cycle > 4)
                    j++;
                if (m_state_cycle > 16)
                    j++;

                for (i = 0;i < j;i++) {
                    m_y++;
                    if (map->collision_with_background(this, GLTM)) {
                        m_state = CSTATE_STANDING_LEFT;
                        m_state_cycle = 0;
                        m_y--;
                        if (SFXM != 0)
                            SFXM->SFX_play("sfx/player_land", m_sfx_volume, get_angle(map), get_distance(map));
                    }
                    else  {
                          GObject *go = map->collision_with_object(this, GLTM, bridge_symbol);
                          if (go != 0) {
                            m_state = CSTATE_STANDING_LEFT;
                            m_state_cycle = 0;
                            m_y--;
                          }
                    }
                }

                m_state_cycle++;
            }
            break;
        case CSTATE_FALLING_RIGHT: {
                int i, j = 1;
                if (m_state_cycle > 4)
                    j++;
                if (m_state_cycle > 16)
                    j++;

                for (i = 0;i < j;i++) {
                    m_y++;
                    if (map->collision_with_background(this, GLTM)) {
                        m_state = CSTATE_STANDING_RIGHT;
                        m_state_cycle = 0;
                        m_y--;
                        if (SFXM != 0)
                            SFXM->SFX_play("sfx/player_land", m_sfx_volume, get_angle(map), get_distance(map));
                    }
                    else  {
                          GObject *go = map->collision_with_object(this, GLTM, bridge_symbol);
                          if (go != 0) {
                             m_state = CSTATE_STANDING_RIGHT;
                             m_state_cycle = 0;
                             m_y--;
                          }
                    }

                }

                m_state_cycle++;
            }
            break;

        case CSTATE_JUMPING_LEFT:
        case CSTATE_JUMPING_RIGHT:
        case CSTATE_JUMPING_LEFT_LEFT:
        case CSTATE_JUMPING_RIGHT_RIGHT: {
                int i, y_move = 3;
                if (m_state_cycle < 39)
                    y_move = 2;
                if (m_state_cycle < 33)
                    y_move = 1;
                if (m_state_cycle < 27)
                    y_move = 0;
                if (m_state_cycle < 21)
                    y_move = -1;
                if (m_state_cycle < 15)
                    y_move = -2;
                if (m_state_cycle < 9)
                    y_move = -3;
                if (m_state_cycle < 4)
                    y_move = -4;

                if (y_move > 0) {
                    for (i = 0;i < y_move;i++) {
                        m_y++;
                        if (map->collision_with_background(this, 0, 1, GLTM)) {
                            if (SFXM != 0)
                                SFXM->SFX_play("sfx/player_land", m_sfx_volume, get_angle(map), get_distance(map));
                            if (m_state == CSTATE_JUMPING_LEFT ||
                                    m_state == CSTATE_JUMPING_LEFT_LEFT) {
                                m_state = CSTATE_STANDING_LEFT;
                                y_move = 0;
                            }
                            if (m_state == CSTATE_JUMPING_RIGHT ||
                                    m_state == CSTATE_JUMPING_RIGHT_RIGHT) {
                                m_state = CSTATE_STANDING_RIGHT;
                                y_move = 0;
                            }
                            m_state_cycle = 0;
                        }
                        else  {
                          if (map->collision_with_object(this, GLTM, bridge_symbol) != 0)  {
                             // SFXM->SFX_play("sfx/player_walk_bridge1", m_sfx_volume, get_player_angle(), get_player_distance());
                             if (m_state == CSTATE_JUMPING_LEFT ||
                                  m_state == CSTATE_JUMPING_LEFT_LEFT) {
                                  m_state = CSTATE_STANDING_LEFT;
                                  y_move = 0;
                            }
                            if (m_state == CSTATE_JUMPING_RIGHT ||
                                  m_state == CSTATE_JUMPING_RIGHT_RIGHT) {
                                  m_state = CSTATE_STANDING_RIGHT;
                                  y_move = 0;
                            }
                            m_state_cycle = 0;

                          }

                        }

                    }
                }
                if (y_move < 0) {
                    y_move = -y_move;
                    for (i = 0;i < y_move;i++) {
                        m_y--;
                        if (map->collision_with_background(this, GLTM)) {
                            m_y++;
                            m_state_cycle = 22;
                        }
                        else {
                          if (map->collision_with_object(this, GLTM, bridge_symbol) != 0)  {
                             m_y++;
                             m_state_cycle = 22;
                          }
                        }
                    }
                }

                if (m_state == CSTATE_JUMPING_LEFT_LEFT) {
                    m_x -= (has_redbadbook ? m_walking_speed * 1.25f : m_walking_speed);
                    if (map->collision_with_background(this, GLTM)) {
                        m_x += (has_redbadbook ? m_walking_speed * 1.25f : m_walking_speed);
                        m_state = CSTATE_JUMPING_LEFT;
                    }
                    else  {
                        if (map->collision_with_object(this, GLTM, bridge_symbol) != 0)  {
                           m_x += (has_redbadbook ? m_walking_speed * 1.25f : m_walking_speed);
                           m_state = CSTATE_JUMPING_LEFT;
                        }
                    }

                }

                if (m_state == CSTATE_JUMPING_RIGHT_RIGHT) {
                    m_x += (has_redbadbook ? m_walking_speed * 1.25f : m_walking_speed);
                    if (map->collision_with_background(this, GLTM)) {
                        m_x -= (has_redbadbook ? m_walking_speed * 1.25f : m_walking_speed);
                        m_state = CSTATE_JUMPING_RIGHT;
                    }
                    else  {
                        if (map->collision_with_object(this, GLTM, bridge_symbol) != 0)  {
                           m_x -= (has_redbadbook ? m_walking_speed * 1.25f : m_walking_speed);
                           m_state = CSTATE_JUMPING_RIGHT;
                        }
                    }

                }

                m_state_cycle++;
            }
            break;

        case CSTATE_CLIMBING_UP:
            set_layer(3, map);
            m_y -= PLAYER_CLIMBING_SPEED;

            // test if end of rope:
            if (m_y > 8) {
                m_y -= 8;
                if (!map->collision_with_object(this, GLTM, rope_symbol)) {
                    set_layer(2, map);
                    m_state = CSTATE_STANDING_LEFT;
                    m_state_cycle = 0;
                    do {
                        m_y -= PLAYER_CLIMBING_SPEED;
                    } while (m_y >= 0 && map->collision_with_background(this, GLTM));
                } else {
                    m_y += 8;
                }
            }

            m_state_cycle++;
            if (!v->m_joystick[VC_UP]) {
                m_state = CSTATE_CLIMBING;
            }
            if (v->m_joystick[VC_DOWN] && !v->m_old_joystick[VC_DOWN]) {
                m_state = CSTATE_CLIMBING_DOWN;
            }
            break;
        case CSTATE_CLIMBING:
            set_layer(3, map);
            if (v->m_joystick[VC_UP]) {
                m_state = CSTATE_CLIMBING_UP;
            }
            if (v->m_joystick[VC_DOWN]) {
                m_state = CSTATE_CLIMBING_DOWN;
            }
            break;
        case CSTATE_CLIMBING_DOWN:
            set_layer(3, map);
            m_y += PLAYER_CLIMBING_SPEED;
            if (map->collision_with_background(this, GLTM)) {
                m_y -= PLAYER_CLIMBING_SPEED;
                if (!map->collision_with_background(this, GLTM)) {
                    set_layer(2, map);
                    m_state = CSTATE_STANDING_LEFT;
                    m_state_cycle = 0;
                } else {
                    m_y += PLAYER_CLIMBING_SPEED;
                }
            }

            m_state_cycle++;
            if (!v->m_joystick[VC_DOWN]) {
                m_state = CSTATE_CLIMBING;
            }
            if (v->m_joystick[VC_UP] && !v->m_old_joystick[VC_UP]) {
                m_state = CSTATE_CLIMBING_UP;
            }
            break;

        case CSTATE_SHOOT_LEFT:
            if (m_state_cycle == 35) {
                if (m_color == 4 || m_color == 5) {
                    // musical notes:
                    map->add_object(new GO_musicalnote((int)(m_x - 28), (int)(m_y - 16), m_sfx_volume, -MUSICALNOTE_SPEED, 0), 3);
                    map->add_object(new GO_musicalnote((int)(m_x - 28), (int)(m_y - 16), m_sfx_volume, -MUSICALNOTE_SPEED*0.9f, MUSICALNOTE_SPEED*0.15f), 3);
                    map->add_object(new GO_musicalnote((int)(m_x - 28), (int)(m_y - 16), m_sfx_volume, -MUSICALNOTE_SPEED*0.9f, -MUSICALNOTE_SPEED*0.15f), 3);
                    if (SFXM != 0)
                        SFXM->SFX_play("sfx/fratelli_sing", m_sfx_volume, get_angle(map), get_distance(map));
                } else {
                    List<int> l;
                    l.Add(new int(0));
                    map->add_object(GObject_create(bullet_symbol, (int)(m_x - 28), (int)(m_y - 16), m_sfx_volume, &l), 2);
                    if (SFXM != 0)
                        SFXM->SFX_play("sfx/fratelli_attack", m_sfx_volume, get_angle(map), get_distance(map));
                }
            }
            m_state_cycle++;
            if (m_state_cycle >= 50) {
                m_state = CSTATE_STANDING_LEFT;
                m_state_cycle = 0;
            }
            break;
        case CSTATE_SHOOT_RIGHT:
            if (m_state_cycle == 35) {
                if (m_color == 4 || m_color == 5) {
                    // musical notes:
                    map->add_object(new GO_musicalnote((int)(m_x + 28), (int)(m_y - 16), m_sfx_volume, MUSICALNOTE_SPEED, 0), 3);
                    map->add_object(new GO_musicalnote((int)(m_x + 28), (int)(m_y - 16), m_sfx_volume, MUSICALNOTE_SPEED*0.9f, MUSICALNOTE_SPEED*0.15f), 3);
                    map->add_object(new GO_musicalnote((int)(m_x + 28), (int)(m_y - 16), m_sfx_volume, MUSICALNOTE_SPEED*0.9f, -MUSICALNOTE_SPEED*0.15f), 3);
                    if (SFXM != 0)
                        SFXM->SFX_play("sfx/fratelli_sing", m_sfx_volume, get_angle(map), get_distance(map));
                } else {
                    List<int> l;
                    l.Add(new int(1));
                    map->add_object(GObject_create(bullet_symbol, (int)(m_x + 28), (int)(m_y - 16), m_sfx_volume, &l), 2);
                    if (SFXM != 0)
                        SFXM->SFX_play("sfx/fratelli_attack", m_sfx_volume, get_angle(map), get_distance(map));
                }
            }
            m_state_cycle++;
            if (m_state_cycle >= 50) {
                m_state = CSTATE_STANDING_RIGHT;
                m_state_cycle = 0;
            }
            break;
        case CSTATE_TAKENDOWN: {
                int i, j = 1;
                if (m_state_cycle > 4)
                    j++;
                if (m_state_cycle > 16)
                    j++;

                for (i = 0;i < j;i++) {
                    m_y++;
                    if (map->collision_with_background(this, GLTM)) {
                        m_y--;
                        if (i > 0 && SFXM != 0)
                            SFXM->SFX_play("sfx/player_land", m_sfx_volume, get_angle(map), get_distance(map));
                        i = j;
                    }
                    else  {
                       if (map->collision_with_object(this, GLTM, bridge_symbol) != 0)  {
                           m_y--;
                           if (i > 0 && SFXM != 0)
                              SFXM->SFX_play("sfx/player_land", m_sfx_volume, get_angle(map), get_distance(map));
                           i = j;
                       }
                    }

                }
            }

            if (m_state_cycle == 0 && SFXM != 0) {
                switch (m_color) {
                    case 0:
                        SFXM->SFX_play("sfx/fratelli_dead_ma", m_sfx_volume, get_angle(map), get_distance(map));
                        break;
                    case 1:
                        SFXM->SFX_play("sfx/fratelli_dead_francis", m_sfx_volume, get_angle(map), get_distance(map));
                        break;
                    case 2:
                        SFXM->SFX_play("sfx/fratelli_dead_ma", m_sfx_volume, get_angle(map), get_distance(map));
                        break;
                    case 3:
                        SFXM->SFX_play("sfx/fratelli_dead_francis", m_sfx_volume, get_angle(map), get_distance(map));
                        break;
                    case 4:
                        SFXM->SFX_play("sfx/fratelli_dead_jake", m_sfx_volume, get_angle(map), get_distance(map));
                        break;
                    case 5:
                        SFXM->SFX_play("sfx/fratelli_dead_jake", m_sfx_volume, get_angle(map), get_distance(map));
                        break;
                }
            }
            m_state_cycle++;
            if (m_state_cycle >= 200) {
                m_state = CSTATE_STANDING_LEFT;
                m_state_cycle = 0;
            }
            break;
    }

    // continuous SFX:
    switch (m_state) {
        case CSTATE_STANDING_LEFT:
        case CSTATE_STANDING_RIGHT:
        case CSTATE_JUMPING_LEFT:
        case CSTATE_JUMPING_RIGHT:
        case CSTATE_JUMPING_LEFT_LEFT:
        case CSTATE_JUMPING_RIGHT_RIGHT:
        case CSTATE_FALLING_LEFT:
        case CSTATE_FALLING_RIGHT:
        case CSTATE_CLIMBING:
        case CSTATE_SHOOT_LEFT:
        case CSTATE_SHOOT_RIGHT:
        case CSTATE_TAKENDOWN:
            if (m_walking_channel != -1) {
                Mix_HaltChannel(m_walking_channel);
                m_walking_channel = -1;
            }
            if (m_climbing_channel != -1) {
                Mix_HaltChannel(m_climbing_channel);
                m_climbing_channel = -1;
            }
            break;
        case CSTATE_WALKING_LEFT:
        case CSTATE_WALKING_RIGHT:
            if (m_climbing_channel != -1) {
                Mix_HaltChannel(m_climbing_channel);
                m_climbing_channel = -1;
            }
			
			if  (SFXM != 0 && (m_state_cycle % 16 == 0)) {
				if (m_walking_channel != -1) {
					SFXM->SFX_play_channel("sfx/fratelli_walk", m_walking_channel, get_angle(map), get_distance(map), m_sfx_volume);
				} else {
					m_walking_channel = SFXM->SFX_play("sfx/fratelli_walk", m_sfx_volume);
				}
            }
            break;
        case CSTATE_CLIMBING_UP:
        case CSTATE_CLIMBING_DOWN:
            if (m_walking_channel != -1) {
                Mix_HaltChannel(m_walking_channel);
                m_walking_channel = -1;
            }
            if (m_climbing_channel == -1 && SFXM != 0) {
				m_climbing_channel = SFXM->SFX_play_continuous("sfx/fratelli_climb", m_sfx_volume, get_angle(map), get_distance(map),this);
            }
            break;
    }

    if (m_layer != 3 &&
            map->collision_with_background(this, GLTM)) {
        int i, j;
        bool found = false;

        for (i = 1;i < 5 && !found;i++) {
            for (j = 0;j < i && !found;j++) {
                // x = j
                // y = i-j
                if (!found && !map->collision_with_background(this, j, i - j, GLTM)) {
                    m_x += j;
                    m_y += i - j;
                    found = true;
                }
                if (!found && (i - j) != 0) {
                    if (!map->collision_with_background(this, j, -(i - j), GLTM)) {
                        m_x += j;
                        m_y += -(i - j);
                        found = true;
                    }
                }
                if (!found && j != 0) {
                    if (!map->collision_with_background(this, -j, i - j, GLTM)) {
                        m_x += -j;
                        m_y += i - j;
                        found = true;
                    }
                }
                if (!found && j != 0 && (i - j) != 0) {
                    if (!map->collision_with_background(this, -j, -(i - j), GLTM)) {
                        m_x += -j;
                        m_y += -(i - j);
                        found = true;
                    }
                }
            }
        }

        if (!found && m_hit_counter == 0) {
            m_hit_counter = 64;
            if (SFXM != 0)
                SFXM->SFX_play("sfx/player_hit", m_sfx_volume, get_angle(map), get_distance(map));
        }
    }

    // check for enemies:
    if (m_hit_counter > 0) {
        m_hit_counter--;
    }

    if (((m_state == CSTATE_STANDING_LEFT) || (m_state == CSTATE_WALKING_LEFT)) &&
            ((m_previous_state == CSTATE_STANDING_RIGHT) || (m_previous_state == CSTATE_WALKING_RIGHT)))
        m_turning_counter = 6;

    if (((m_state == CSTATE_STANDING_RIGHT) || (m_state == CSTATE_WALKING_RIGHT)) &&
            ((m_previous_state == CSTATE_STANDING_LEFT) || (m_previous_state == CSTATE_WALKING_LEFT)))
        m_turning_counter = 6;

    if (m_turning_counter > 0)
        m_turning_counter--;

    return true;
}

void GO_fratelli::draw(GLTManager *GLTM)
{
    int s = (m_state_cycle / 8) % 4;
    float xo = 0, yo = 0;

    if (m_color == 0) {
        switch (m_state) {
            case CSTATE_STANDING_LEFT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_yellowguy-turn");
                } else {
                    m_last_tile_used = GLTM->get
                                       ("ob_yellowguy-l1");
                } // f
                break;
            case CSTATE_STANDING_RIGHT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_yellowguy-turn");
                } else {
                    m_last_tile_used = GLTM->get
                                       ("ob_yellowguy-r1");
                } // f
                break;
            case CSTATE_WALKING_LEFT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_yellowguy-turn");
                } else {
                    if (s == 0)
                        m_last_tile_used = GLTM->get
                                           ("ob_yellowguy-l2");
                    if (s == 1)
                        m_last_tile_used = GLTM->get
                                           ("ob_yellowguy-l1");
                    if (s == 2)
                        m_last_tile_used = GLTM->get
                                           ("ob_yellowguy-l3");
                    if (s == 3)
                        m_last_tile_used = GLTM->get
                                           ("ob_yellowguy-l1");
                }
                break;
            case CSTATE_WALKING_RIGHT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_yellowguy-turn");
                } else {
                    if (s == 0)
                        m_last_tile_used = GLTM->get
                                           ("ob_yellowguy-r2");
                    if (s == 1)
                        m_last_tile_used = GLTM->get
                                           ("ob_yellowguy-r1");
                    if (s == 2)
                        m_last_tile_used = GLTM->get
                                           ("ob_yellowguy-r3");
                    if (s == 3)
                        m_last_tile_used = GLTM->get
                                           ("ob_yellowguy-r1");
                }
                break;
            case CSTATE_FALLING_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_yellowguy-l1");
                break;
            case CSTATE_FALLING_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_yellowguy-r1");
                break;
            case CSTATE_JUMPING_LEFT:
            case CSTATE_JUMPING_LEFT_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_yellowguy-jump-l");
                break;
            case CSTATE_JUMPING_RIGHT:
            case CSTATE_JUMPING_RIGHT_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_yellowguy-jump-r");
                break;
            case CSTATE_CLIMBING_UP:
            case CSTATE_CLIMBING:
            case CSTATE_CLIMBING_DOWN:
                if (s == 0 || s == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_yellowguy-climbing-1");
                if (s == 1 || s == 3)
                    m_last_tile_used = GLTM->get
                                       ("ob_yellowguy-climbing-2");
                break;
            case CSTATE_SHOOT_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_yellowguy-shooting-l");
                break;
            case CSTATE_SHOOT_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_yellowguy-shooting-r");
                break;
            case CSTATE_TAKENDOWN:
                if (s == 0 || s == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_yellowguy-takendown-1");
                if (s == 1 || s == 3)
                    m_last_tile_used = GLTM->get
                                       ("ob_yellowguy-takendown-2");
                break;
        }
    }
    if (m_color == 1) {
        switch (m_state) {
            case CSTATE_STANDING_LEFT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_blueguy-turn");
                } else {
                    m_last_tile_used = GLTM->get
                                       ("ob_blueguy-l1");
                }
                break;
            case CSTATE_STANDING_RIGHT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_blueguy-turn");
                } else {
                    m_last_tile_used = GLTM->get
                                       ("ob_blueguy-r1");
                }
                break;
            case CSTATE_WALKING_LEFT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_blueguy-turn");
                } else {
                    if (s == 0)
                        m_last_tile_used = GLTM->get
                                           ("ob_blueguy-l2");
                    if (s == 1)
                        m_last_tile_used = GLTM->get
                                           ("ob_blueguy-l1");
                    if (s == 2)
                        m_last_tile_used = GLTM->get
                                           ("ob_blueguy-l3");
                    if (s == 3)
                        m_last_tile_used = GLTM->get
                                           ("ob_blueguy-l1");
                }
                break;
            case CSTATE_WALKING_RIGHT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_blueguy-turn");
                } else {
                    if (s == 0)
                        m_last_tile_used = GLTM->get
                                           ("ob_blueguy-r2");
                    if (s == 1)
                        m_last_tile_used = GLTM->get
                                           ("ob_blueguy-r1");
                    if (s == 2)
                        m_last_tile_used = GLTM->get
                                           ("ob_blueguy-r3");
                    if (s == 3)
                        m_last_tile_used = GLTM->get
                                           ("ob_blueguy-r1");
                }
                break;
            case CSTATE_FALLING_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_blueguy-l1");
                break;
            case CSTATE_FALLING_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_blueguy-r1");
                break;
            case CSTATE_JUMPING_LEFT:
            case CSTATE_JUMPING_LEFT_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_blueguy-jump-l");
                break;
            case CSTATE_JUMPING_RIGHT:
            case CSTATE_JUMPING_RIGHT_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_blueguy-jump-r");
                break;
            case CSTATE_CLIMBING_UP:
            case CSTATE_CLIMBING:
            case CSTATE_CLIMBING_DOWN:
                if (s == 0 || s == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_blueguy-climbing-1");
                if (s == 1 || s == 3)
                    m_last_tile_used = GLTM->get
                                       ("ob_blueguy-climbing-2");
                break;
            case CSTATE_SHOOT_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_blueguy-shooting-l");
                break;
            case CSTATE_SHOOT_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_blueguy-shooting-r");
                break;
            case CSTATE_TAKENDOWN:
                if (s == 0 || s == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_blueguy-takendown-1");
                if (s == 1 || s == 3)
                    m_last_tile_used = GLTM->get
                                       ("ob_blueguy-takendown-2");
                break;
        }
    }
    if (m_color == 2) {
        switch (m_state) {
            case CSTATE_STANDING_LEFT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_greenguy-turn");
                } else {
                    m_last_tile_used = GLTM->get
                                       ("ob_greenguy-l1");
                }
                break;
            case CSTATE_STANDING_RIGHT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_greenguy-turn");
                } else {
                    m_last_tile_used = GLTM->get
                                       ("ob_greenguy-r1");
                }
                break;
            case CSTATE_WALKING_LEFT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_greenguy-turn");
                } else {
                    if (s == 0)
                        m_last_tile_used = GLTM->get
                                           ("ob_greenguy-l2");
                    if (s == 1)
                        m_last_tile_used = GLTM->get
                                           ("ob_greenguy-l1");
                    if (s == 2)
                        m_last_tile_used = GLTM->get
                                           ("ob_greenguy-l3");
                    if (s == 3)
                        m_last_tile_used = GLTM->get
                                           ("ob_greenguy-l1");
                }
                break;
            case CSTATE_WALKING_RIGHT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_greenguy-turn");
                } else {
                    if (s == 0)
                        m_last_tile_used = GLTM->get
                                           ("ob_greenguy-r2");
                    if (s == 1)
                        m_last_tile_used = GLTM->get
                                           ("ob_greenguy-r1");
                    if (s == 2)
                        m_last_tile_used = GLTM->get
                                           ("ob_greenguy-r3");
                    if (s == 3)
                        m_last_tile_used = GLTM->get
                                           ("ob_greenguy-r1");
                }
                break;
            case CSTATE_FALLING_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_greenguy-l1");
                break;
            case CSTATE_FALLING_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_greenguy-r1");
                break;
            case CSTATE_JUMPING_LEFT:
            case CSTATE_JUMPING_LEFT_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_greenguy-jump-l");
                break;
            case CSTATE_JUMPING_RIGHT:
            case CSTATE_JUMPING_RIGHT_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_greenguy-jump-r");
                break;
            case CSTATE_CLIMBING_UP:
            case CSTATE_CLIMBING:
            case CSTATE_CLIMBING_DOWN:
                if (s == 0 || s == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_greenguy-climbing-1");
                if (s == 1 || s == 3)
                    m_last_tile_used = GLTM->get
                                       ("ob_greenguy-climbing-2");
                break;
            case CSTATE_SHOOT_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_greenguy-shooting-l");
                break;
            case CSTATE_SHOOT_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_greenguy-shooting-r");
                break;
            case CSTATE_TAKENDOWN:
                if (s == 0 || s == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_greenguy-takendown-1");
                if (s == 1 || s == 3)
                    m_last_tile_used = GLTM->get
                                       ("ob_greenguy-takendown-2");
                break;
        }
    }
    if (m_color == 3) {
        switch (m_state) {
            case CSTATE_STANDING_LEFT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_redguy-turn");
                } else {
                    m_last_tile_used = GLTM->get
                                       ("ob_redguy-l1");
                }
                break;
            case CSTATE_STANDING_RIGHT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_redguy-turn");
                } else {
                    m_last_tile_used = GLTM->get
                                       ("ob_redguy-r1");
                }
                break;
            case CSTATE_WALKING_LEFT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_redguy-turn");
                } else {
                    if (s == 0)
                        m_last_tile_used = GLTM->get
                                           ("ob_redguy-l2");
                    if (s == 1)
                        m_last_tile_used = GLTM->get
                                           ("ob_redguy-l1");
                    if (s == 2)
                        m_last_tile_used = GLTM->get
                                           ("ob_redguy-l3");
                    if (s == 3)
                        m_last_tile_used = GLTM->get
                                           ("ob_redguy-l1");
                }
                break;
            case CSTATE_WALKING_RIGHT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_redguy-turn");
                } else {
                    if (s == 0)
                        m_last_tile_used = GLTM->get
                                           ("ob_redguy-r2");
                    if (s == 1)
                        m_last_tile_used = GLTM->get
                                           ("ob_redguy-r1");
                    if (s == 2)
                        m_last_tile_used = GLTM->get
                                           ("ob_redguy-r3");
                    if (s == 3)
                        m_last_tile_used = GLTM->get
                                           ("ob_redguy-r1");
                }
                break;
            case CSTATE_FALLING_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_redguy-l1");
                break;
            case CSTATE_FALLING_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_redguy-r1");
                break;
            case CSTATE_JUMPING_LEFT:
            case CSTATE_JUMPING_LEFT_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_redguy-jump-l");
                break;
            case CSTATE_JUMPING_RIGHT:
            case CSTATE_JUMPING_RIGHT_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_redguy-jump-r");
                break;
            case CSTATE_CLIMBING_UP:
            case CSTATE_CLIMBING:
            case CSTATE_CLIMBING_DOWN:
                if (s == 0 || s == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_redguy-climbing-1");
                if (s == 1 || s == 3)
                    m_last_tile_used = GLTM->get
                                       ("ob_redguy-climbing-2");
                break;
            case CSTATE_SHOOT_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_redguy-shooting-l");
                break;
            case CSTATE_SHOOT_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_redguy-shooting-r");
                break;
            case CSTATE_TAKENDOWN:
                if (s == 0 || s == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_redguy-takendown-1");
                if (s == 1 || s == 3)
                    m_last_tile_used = GLTM->get
                                       ("ob_redguy-takendown-2");
                break;
        }
    }
    if (m_color == 4) {
        switch (m_state) {
            case CSTATE_STANDING_LEFT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_whiteguy-turn");
                } else {
                    m_last_tile_used = GLTM->get
                                       ("ob_whiteguy-l1");
                }
                break;
            case CSTATE_STANDING_RIGHT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_whiteguy-turn");
                } else {
                    m_last_tile_used = GLTM->get
                                       ("ob_whiteguy-r1");
                }
                break;
            case CSTATE_WALKING_LEFT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_whiteguy-turn");
                } else {
                    if (s == 0)
                        m_last_tile_used = GLTM->get
                                           ("ob_whiteguy-l2");
                    if (s == 1)
                        m_last_tile_used = GLTM->get
                                           ("ob_whiteguy-l1");
                    if (s == 2)
                        m_last_tile_used = GLTM->get
                                           ("ob_whiteguy-l3");
                    if (s == 3)
                        m_last_tile_used = GLTM->get
                                           ("ob_whiteguy-l1");
                }
                break;
            case CSTATE_WALKING_RIGHT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_whiteguy-turn");
                } else {
                    if (s == 0)
                        m_last_tile_used = GLTM->get
                                           ("ob_whiteguy-r2");
                    if (s == 1)
                        m_last_tile_used = GLTM->get
                                           ("ob_whiteguy-r1");
                    if (s == 2)
                        m_last_tile_used = GLTM->get
                                           ("ob_whiteguy-r3");
                    if (s == 3)
                        m_last_tile_used = GLTM->get
                                           ("ob_whiteguy-r1");
                }
                break;
            case CSTATE_FALLING_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_whiteguy-l1");
                break;
            case CSTATE_FALLING_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_whiteguy-r1");
                break;
            case CSTATE_JUMPING_LEFT:
            case CSTATE_JUMPING_LEFT_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_whiteguy-jump-l");
                break;
            case CSTATE_JUMPING_RIGHT:
            case CSTATE_JUMPING_RIGHT_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_whiteguy-jump-r");
                break;
            case CSTATE_CLIMBING_UP:
            case CSTATE_CLIMBING:
            case CSTATE_CLIMBING_DOWN:
                if (s == 0 || s == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_whiteguy-climbing-1");
                if (s == 1 || s == 3)
                    m_last_tile_used = GLTM->get
                                       ("ob_whiteguy-climbing-2");
                break;
            case CSTATE_SHOOT_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_whiteguy-shooting-l");
                break;
            case CSTATE_SHOOT_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_whiteguy-shooting-r");
                break;
            case CSTATE_TAKENDOWN:
                if (s == 0 || s == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_whiteguy-takendown-1");
                if (s == 1 || s == 3)
                    m_last_tile_used = GLTM->get
                                       ("ob_whiteguy-takendown-2");
                break;
        }
    }
    if (m_color == 5) {
        switch (m_state) {
            case CSTATE_STANDING_LEFT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_greyguy-turn");
                } else {
                    m_last_tile_used = GLTM->get
                                       ("ob_greyguy-l1");
                }
                break;
            case CSTATE_STANDING_RIGHT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_greyguy-turn");
                } else {
                    m_last_tile_used = GLTM->get
                                       ("ob_greyguy-r1");
                }
                break;
            case CSTATE_WALKING_LEFT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_greyguy-turn");
                } else {
                    if (s == 0)
                        m_last_tile_used = GLTM->get
                                           ("ob_greyguy-l2");
                    if (s == 1)
                        m_last_tile_used = GLTM->get
                                           ("ob_greyguy-l1");
                    if (s == 2)
                        m_last_tile_used = GLTM->get
                                           ("ob_greyguy-l3");
                    if (s == 3)
                        m_last_tile_used = GLTM->get
                                           ("ob_greyguy-l1");
                }
                break;
            case CSTATE_WALKING_RIGHT:
                if (m_turning_counter) {
                    m_last_tile_used = GLTM->get
                                       ("ob_greyguy-turn");
                } else {
                    if (s == 0)
                        m_last_tile_used = GLTM->get
                                           ("ob_greyguy-r2");
                    if (s == 1)
                        m_last_tile_used = GLTM->get
                                           ("ob_greyguy-r1");
                    if (s == 2)
                        m_last_tile_used = GLTM->get
                                           ("ob_greyguy-r3");
                    if (s == 3)
                        m_last_tile_used = GLTM->get
                                           ("ob_greyguy-r1");
                }
                break;
            case CSTATE_FALLING_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_greyguy-l1");
                break;
            case CSTATE_FALLING_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_greyguy-r1");
                break;
            case CSTATE_JUMPING_LEFT:
            case CSTATE_JUMPING_LEFT_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_greyguy-jump-l");
                break;
            case CSTATE_JUMPING_RIGHT:
            case CSTATE_JUMPING_RIGHT_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_greyguy-jump-r");
                break;
            case CSTATE_CLIMBING_UP:
            case CSTATE_CLIMBING:
            case CSTATE_CLIMBING_DOWN:
                if (s == 0 || s == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_greyguy-climbing-1");
                if (s == 1 || s == 3)
                    m_last_tile_used = GLTM->get
                                       ("ob_greyguy-climbing-2");
                break;
            case CSTATE_SHOOT_LEFT:
                m_last_tile_used = GLTM->get
                                   ("ob_greyguy-shooting-l");
                break;
            case CSTATE_SHOOT_RIGHT:
                m_last_tile_used = GLTM->get
                                   ("ob_greyguy-shooting-r");
                break;
            case CSTATE_TAKENDOWN:
                if (s == 0 || s == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_greyguy-takendown-1");
                if (s == 1 || s == 3)
                    m_last_tile_used = GLTM->get
                                       ("ob_greyguy-takendown-2");
                break;
        }
    }

    if (m_hit_counter > 0) {
        int bufi;
        float TexColorArray[4], bufvf[4];
        float f = float(0.5F + 0.5F * sin(m_hit_counter * ((m_state == CSTATE_DYING || m_state == CSTATE_DEAD) ? 0.8f : 0.4f)));

        glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &bufi);
        glGetTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, bufvf);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
        TexColorArray[0] = 1.0f;
        TexColorArray[1] = 1.0f;
        TexColorArray[2] = 1.0f;
        TexColorArray[3] = 0.0f;
        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, TexColorArray);

        if (xo == 0 && yo == 0)
            m_last_tile_used->draw(1, 1, 1, 1, m_x, m_y, 0, 0, 1);
        else
            m_last_tile_used->draw_toffs(1, 1, 1, 1, m_x, m_y, 0, 0, 1, xo, yo);

        glColor4f(1, 1, 1, 1);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, bufi);
        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, bufvf);

        glBindTexture(GL_TEXTURE_2D, 0); // I really don't understand this line of code, but if I don't add it,
        // I don't get the desired effect (I found this after like 2 hours of random guessing!)

        if (xo == 0 && yo == 0)
            m_last_tile_used->draw(1, 1, 1, f, m_x, m_y, 0, 0, 1);
        else
            m_last_tile_used->draw_toffs(1, 1, 1, f, m_x, m_y, 0, 0, 1, xo, yo);
    } else {
        if (xo == 0 && yo == 0)
            m_last_tile_used->draw(1, 1, 1, 1, m_x, m_y, 0, 0, 1);
        else
            m_last_tile_used->draw_toffs(1, 1, 1, 1, m_x, m_y, 0, 0, 1, xo, yo);
    }

}

bool GO_fratelli::is_a(Symbol *c)
{
    if (c->cmp(fratelli_symbol))
        return true;

    return GO_enemy::is_a(c);
} /* GO_fratelli::is_a */


bool GO_fratelli::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
}

bool GO_fratelli::player_hit(int *experience, int *score)
{
    if (m_state != CSTATE_TAKENDOWN &&
            m_state != CSTATE_CLIMBING &&
            m_state != CSTATE_CLIMBING_UP &&
            m_state != CSTATE_CLIMBING_DOWN) {
        m_state = CSTATE_TAKENDOWN;
        m_state_cycle = 0;
        *score = 200;
        return true;
    }
    return false;
}

int GO_fratelli::enemy_hit(void)
{
    if (m_state != CSTATE_TAKENDOWN)
        return 16;
    return 0;
}
