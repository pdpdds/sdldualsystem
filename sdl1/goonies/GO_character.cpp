#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include "windows.h"
#endif

#include "math.h"
#include "stdlib.h"
#include "string.h"

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
#include "GO_skulldoor.h"
#include "GO_item.h"
#include "GO_bridge.h"

#include "GMap.h"

#include "TheGooniesCtnt.h"
#include "GObjectCreator.h"

// #include "debug.h"


extern int difficulty;
extern int score;

GO_character::GO_character(int x, int y, int sfx_volume, int facing) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(character_symbol);

    if (facing == 0)
        m_state = CSTATE_STANDING_LEFT;
    else
        m_state = CSTATE_STANDING_RIGHT;
    m_last_state = m_state;
    m_state_cycle = 0;
    m_punch_cycle = 0;
	m_step_cycle = 0;
    m_layer = 2;

    m_walking_channel = -1;
    m_climbing_channel = -1;

    m_facebefore_vine = 0;

    m_requested_room = false;
    m_requested_room_x = 0;
    m_requested_room_y = 0;
    m_requested_room_door = 0;

    m_player_hit_counter = 0;

    m_walking_speed = PLAYER_SPEED;
	
    m_energy = 64;
    m_experience = 0;
    m_key = false;
    m_goonies_rescued = 0;
	for (int i = 0; i < 7; ++i) m_rescued_goonies[i] = false;

    m_last_pick = 0;
    m_last_hit = 0;
    m_last_hit_by = 0;
    m_camefrom = 0;

    m_turning_counter = 0;

    // item variables:
    m_yellowhelmet_status = m_yellowhelmet_timer = 0;
    m_greycoat_status = m_greycoat_timer = 0;
    m_yellowcoat_timer = 0;
    m_hammer_status = 0;
    m_greenbook_status = 0;
    m_redbook_status = 0;
    m_lightbluebook_status = 0;
    m_bluebook_status = m_bluebook_timer = 0;
    m_greencoat_timer = 0;
    m_whitebook_status = 0;
    m_yellowshield_status = m_yellowshield_timer = 0;
    m_lightbluecoat_timer = 0;
    m_whiteshield_status = m_whiteshield_timer = 0;
    m_lightbluehelmet_status = m_lightbluehelmet_timer = 0;
    m_yellowbook_status = m_yellowbook_timer = 0;
    m_purpleshield_status = m_purpleshield_timer = 0;
    m_clock_timer = 0;
    m_bluebadbook_nghosts = 0;
}

GO_character::~GO_character()
{
    if (m_last_pick != 0)
        delete m_last_pick;
    if (m_last_hit != 0)
        delete m_last_hit;
    if (m_last_hit_by != 0)
        delete m_last_hit_by;
    if (m_camefrom != 0)
        delete m_camefrom;
}



bool GO_character::cycle(VirtualController *v, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
	int salute = (m_state_cycle % 615);
    m_previous_state = m_state;
	
    if (m_last_tile_used == 0)
        m_last_tile_used = GLTM->get
                           ("ob_character-l1");

    // reset the variables for script conditions:
    m_last_state = m_state;
    if (m_last_pick != 0)
        delete m_last_pick;
    m_last_pick = 0;
    if (m_last_hit != 0)
        delete m_last_hit;
    m_last_hit = 0;
    if (m_last_hit_by != 0)
        delete m_last_hit_by;
    m_last_hit_by = 0;

#ifdef __DEBUG_MESSAGES

    output_debug_message("Character, %i (%f,%f)\n", m_state, m_x, m_y);
#endif

    if (player_has("GO_shoes")) {
        m_walking_speed = PLAYER_SPEED * 1.25f;
    } else {
        m_walking_speed = PLAYER_SPEED;
    }

    switch (m_state) {
        case CSTATE_STANDING_LEFT:
        case CSTATE_STANDING_RIGHT:
			m_state_cycle++;
            if (v->m_joystick[VC_UP] && !v->m_old_joystick[VC_UP]) {
                GObject *o;
                o = map->collision_with_object(this, GLTM, rope_symbol);
                if (o != 0) {
                    // climb a rope!
                    if (m_state == CSTATE_STANDING_LEFT)
                        m_facebefore_vine = 0;
                    else
                        m_facebefore_vine = 1;
                    m_state = CSTATE_CLIMBING_UP;
                    m_x = o->get_x() + 10;
                } else {
                    // check for skulldoors:
                    GO_skulldoor *sd = (GO_skulldoor *)map->collision_with_object(this, GLTM, skulldoor_symbol);

                    if (sd != 0) {
                        // Room change request:
                        if (sd->get_destination_x() != -1 ||
                                m_goonies_rescued >= 7) {
                            m_requested_room_x = sd->get_destination_x();
                            m_requested_room_y = sd->get_destination_y();
                            m_requested_room_door = sd->get_destination_door();
                            m_state = CSTATE_ENTERING_DOOR;
                            m_state_cycle = 0;
                            m_x = sd->get_x()-4;
                            m_y = sd->get_y()+16;
                        }
                    } else {
                        SFXM->SFX_play("sfx/player_jump", m_sfx_volume, get_player_angle(), get_player_distance());
                        if (m_state == CSTATE_STANDING_LEFT) {
                            m_state = CSTATE_JUMPING_LEFT;
                            m_state_cycle = 0;
                        } else {
                            m_state = CSTATE_JUMPING_RIGHT;
                            m_state_cycle = 0;
                        }
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
                        if (m_state == CSTATE_STANDING_LEFT)
                            m_facebefore_vine = 0;
                        else
                            m_facebefore_vine = 1;
                        m_state = CSTATE_CLIMBING_DOWN;
                        m_x = o->get_x() + 10;
                        m_y -= 28;
                    } else {
                        m_y -= 50;
                    }
                }

                // punch:
                if (v->m_button[0] && !v->m_old_button[0]) {
                    int e_gained = 0;
                    int points_gained = 0;
                    if (m_state == CSTATE_STANDING_LEFT || m_state == CSTATE_WALKING_LEFT) {
                        m_state = CSTATE_PUNCH_LEFT;
                        m_punch_cycle = 0;
                        GO_enemy *e = (GO_enemy *)map->collision_with_object(GLTM->get
                                      ("ob_character-punch-mask-l"), (int)m_x, (int)m_y, GLTM, enemy_symbol);
                        if (e != 0 && e->player_hit(&e_gained, &points_gained)) {
                            if (m_last_hit != 0)
                                delete m_last_hit;
                            m_last_hit = new Symbol(e->get_class());
                            SFXM->SFX_play("sfx/player_hit_enemy", m_sfx_volume, get_player_angle(), get_player_distance());
                            m_experience += e_gained;
                            inc_score(points_gained);
                        } else {
                            SFXM->SFX_play("sfx/player_attack", m_sfx_volume, get_player_angle(), get_player_distance());
                        }
                    } else {
                        m_state = CSTATE_PUNCH_RIGHT;
                        m_punch_cycle = 0;
                        GO_enemy *e = (GO_enemy *)map->collision_with_object(GLTM->get
                                      ("ob_character-punch-mask-r"), (int)m_x, (int)m_y, GLTM, enemy_symbol);
                        if (e != 0 && e->player_hit(&e_gained, &points_gained)) {
                            if (m_last_hit != 0)
                                delete m_last_hit;
                            m_last_hit = new Symbol(e->get_class());
                            SFXM->SFX_play("sfx/player_hit_enemy", m_sfx_volume, get_player_angle(), get_player_distance());
                            m_experience += e_gained;
                            inc_score(points_gained);
                        } else {
                            SFXM->SFX_play("sfx/player_attack", m_sfx_volume, get_player_angle(), get_player_distance());
                        }
                    }
                }
            }

			// salute; wink sfx
			if (salute == 250 || salute == 260 || salute == 270) {
				SFXM->SFX_play("sfx/player_wink", m_sfx_volume, get_player_angle(), get_player_distance());

			// salute; foot tap
			} else if (salute == 510 || salute == 530 || salute == 550) {
				SFXM->SFX_play("sfx/player_foottap", m_sfx_volume, get_player_angle(), get_player_distance());
				
			// salute; knock on monitor
			} else if (salute == 580 || salute == 590 || salute == 600) {
				SFXM->SFX_play("sfx/player_knock", m_sfx_volume, get_player_angle(), get_player_distance());
			}

            // test for fall:
            if (m_state != CSTATE_ENTERING_DOOR) {
                m_y+=2.0f;
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
                m_y-=2.0f;
            }

            break;
        case CSTATE_WALKING_LEFT:
            m_state_cycle++;
            m_x -= m_walking_speed;
            if (v->m_joystick[VC_UP] && !v->m_old_joystick[VC_UP]) {
                GObject *o;
                o = map->collision_with_object(this, GLTM, rope_symbol);
                if (o != 0) {
                    // climb a rope!
                    m_facebefore_vine = 0;
                    m_state = CSTATE_CLIMBING_UP;
                    m_x = o->get_x() + 10;
                } else {
                    // check for skulldoors:
                    GO_skulldoor *sd = (GO_skulldoor *)map->collision_with_object(this, GLTM, skulldoor_symbol);

                    if (sd != 0) {
                        // Room change request:
                        if (sd->get_destination_x() != -1 ||
                                m_goonies_rescued >= 7) {
                            m_requested_room_x = sd->get_destination_x();
                            m_requested_room_y = sd->get_destination_y();
                            m_requested_room_door = sd->get_destination_door();
                            m_state = CSTATE_ENTERING_DOOR;
                            m_state_cycle = 0;
                            m_x = sd->get_x()-4;
                            m_y = sd->get_y()+16;
                        }
                    } else {
                        SFXM->SFX_play("sfx/player_jump", m_sfx_volume, get_player_angle(), get_player_distance());
                        m_state = CSTATE_JUMPING_LEFT_LEFT;
                        m_state_cycle = 0;
                    }
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
                        m_facebefore_vine = 0;
                        m_state = CSTATE_CLIMBING_DOWN;
                        m_x = o->get_x() + 10;
                        m_y -= 28;
                    } else {
                        m_y -= 50;
                    }
                }

                // punch:
                if (v->m_button[0] && !v->m_old_button[0]) {
                    int e_gained = 0;
                    int points_gained = 0;
                    if (m_state == CSTATE_STANDING_LEFT || m_state == CSTATE_WALKING_LEFT) {
                        m_state = CSTATE_PUNCH_LEFT;
                        m_punch_cycle = 0;
                        GO_enemy *e = (GO_enemy *)map->collision_with_object(GLTM->get
                                      ("ob_character-punch-mask-l"), (int)m_x, (int)m_y, GLTM, enemy_symbol);
                        if (e != 0 && e->player_hit(&e_gained, &points_gained)) {
                            if (m_last_hit != 0)
                                delete m_last_hit;
                            m_last_hit = new Symbol(e->get_class());
                            SFXM->SFX_play("sfx/player_hit_enemy", m_sfx_volume, get_player_angle(), get_player_distance());
                            m_experience += e_gained;
                            inc_score(points_gained);
                        } else {
                            SFXM->SFX_play("sfx/player_attack", m_sfx_volume, get_player_angle(), get_player_distance());
                        }
                    } else {
                        m_state = CSTATE_PUNCH_RIGHT;
                        m_punch_cycle = 0;
                        GO_enemy *e = (GO_enemy *)map->collision_with_object(GLTM->get
                                      ("ob_character-punch-mask-r"), (int)m_x, (int)m_y, GLTM, enemy_symbol);
                        if (e != 0 && e->player_hit(&e_gained, &points_gained)) {
                            if (m_last_hit != 0)
                                delete m_last_hit;
                            m_last_hit = new Symbol(e->get_class());
                            SFXM->SFX_play("sfx/player_hit_enemy", m_sfx_volume, get_player_angle(), get_player_distance());
                            m_experience += e_gained;
                            inc_score(points_gained);
                        } else {
                            SFXM->SFX_play("sfx/player_attack", m_sfx_volume, get_player_angle(), get_player_distance());
                        }
                    } 
                }
            }

            // test for fall:
            if (m_state != CSTATE_ENTERING_DOOR) {
                m_y+=2.0f;
                if (!map->collision_with_background(this, GLTM)) {
                    GObject *go = map->collision_with_object(this, GLTM, bridge_symbol);
                    if (go == 0)  {
                       m_state = CSTATE_FALLING_LEFT;
                       m_state_cycle = 0;
                    }
                }
                m_y-=2.0f;
            }

            break;
        case CSTATE_WALKING_RIGHT:
            m_state_cycle++;
            m_x += m_walking_speed;
            if (v->m_joystick[VC_UP] && !v->m_old_joystick[VC_UP]) {
                GObject *o;
                o = map->collision_with_object(this, GLTM, rope_symbol);
                if (o != 0) {
                    // climb a rope!
                    m_facebefore_vine = 1;
                    m_state = CSTATE_CLIMBING_UP;
                    m_x = o->get_x() + 10;
                } else {
                    // check for skulldoors:
                    GO_skulldoor *sd = (GO_skulldoor *)map->collision_with_object(this, GLTM, skulldoor_symbol);

                    if (sd != 0) {
                        // Room change request:
                        if (sd->get_destination_x() != -1 ||
                                m_goonies_rescued >= 7) {
                            m_requested_room_x = sd->get_destination_x();
                            m_requested_room_y = sd->get_destination_y();
                            m_requested_room_door = sd->get_destination_door();
                            m_state = CSTATE_ENTERING_DOOR;
                            m_state_cycle = 0;
                            m_x = sd->get_x()-4;
                            m_y = sd->get_y()+16;
                        } 
                    } else {
                        SFXM->SFX_play("sfx/player_jump", m_sfx_volume, get_player_angle(), get_player_distance());
                        m_state = CSTATE_JUMPING_RIGHT_RIGHT;
                        m_state_cycle = 0;
                    }
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
                        m_facebefore_vine = 1;
                        m_state = CSTATE_CLIMBING_DOWN;
                        m_x = o->get_x() + 10;
                        m_y -= 28;
                    } else {
                        m_y -= 50;
                    }
                }

                // punch:
                if (v->m_button[0] && !v->m_old_button[0]) {
                    int e_gained = 0;
                    int points_gained = 0;
                    if (m_state == CSTATE_STANDING_LEFT || m_state == CSTATE_WALKING_LEFT) {
                        m_state = CSTATE_PUNCH_LEFT;
                        m_punch_cycle = 0;
                        GO_enemy *e = (GO_enemy *)map->collision_with_object(GLTM->get
                                      ("ob_character-punch-mask-l"), (int)m_x, (int)m_y, GLTM, enemy_symbol);
                        if (e != 0 && e->player_hit(&e_gained, &points_gained)) {
                            if (m_last_hit != 0)
                                delete m_last_hit;
                            m_last_hit = new Symbol(e->get_class());
                            SFXM->SFX_play("sfx/player_hit_enemy", m_sfx_volume, get_player_angle(), get_player_distance());
                            m_experience += e_gained;
                            inc_score(points_gained);
                        } else {
                            SFXM->SFX_play("sfx/player_attack", m_sfx_volume, get_player_angle(), get_player_distance());
                        }
                    } else {
                        m_state = CSTATE_PUNCH_RIGHT;
                        m_punch_cycle = 0;
                        GO_enemy *e = (GO_enemy *)map->collision_with_object(GLTM->get
                                      ("ob_character-punch-mask-r"), (int)m_x, (int)m_y, GLTM, enemy_symbol);
                        if (e != 0 && e->player_hit(&e_gained, &points_gained)) {
                            if (m_last_hit != 0)
                                delete m_last_hit;
                            m_last_hit = new Symbol(e->get_class());
                            SFXM->SFX_play("sfx/player_hit_enemy", m_sfx_volume, get_player_angle(), get_player_distance());
                            m_experience += e_gained;
                            inc_score(points_gained);
                        } else {
                            SFXM->SFX_play("sfx/player_attack", m_sfx_volume, get_player_angle(), get_player_distance());
                        }
                    }
                }
            }

            // test for fall:
            if (m_state != CSTATE_ENTERING_DOOR) {
                m_y+=2.0f;
                if (!map->collision_with_background(this, GLTM)) {
                    GObject *go = map->collision_with_object(this, GLTM, bridge_symbol);
                    if (go == 0) {
                       m_state = CSTATE_FALLING_RIGHT;
                       m_state_cycle = 0;
                    }
                }
                m_y-=2.0f;
            }

            break;
        case CSTATE_FALLING_LEFT: {
                int i, j = 1;
                if (m_state_cycle > 4)
                    j++;
                if (m_state_cycle > 12)
                    j++;
                if (m_state_cycle > 20)
                    j++;

                for (i = 0;i < j;i++) {
                    m_y++;
                    if (map->collision_with_background(this, GLTM)) {
                        m_state = CSTATE_STANDING_LEFT;
                        m_state_cycle = 0;
                        m_y--;
                        SFXM->SFX_play("sfx/player_land", m_sfx_volume, get_player_angle(), get_player_distance());
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
                if (m_state_cycle > 12)
                    j++;
                if (m_state_cycle > 20)
                    j++;

                for (i = 0;i < j;i++) {
                    m_y++;
                    if (map->collision_with_background(this, GLTM)) {
                        m_state = CSTATE_STANDING_RIGHT;
                        m_state_cycle = 0;
                        m_y--;
                        SFXM->SFX_play("sfx/player_land", m_sfx_volume, get_player_angle(), get_player_distance());
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
        case CSTATE_PUNCH_LEFT:
            m_punch_cycle++;
            if (m_punch_cycle >= 8) {
                m_state = CSTATE_STANDING_LEFT;
                m_state_cycle = 0;
            }
            break;
        case CSTATE_PUNCH_RIGHT:
            m_punch_cycle++;
            if (m_punch_cycle >= 8) {
                m_state = CSTATE_STANDING_RIGHT;
                m_state_cycle = 0;
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
                            SFXM->SFX_play("sfx/player_land", m_sfx_volume, get_player_angle(), get_player_distance());
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
                    m_x -= (m_walking_speed * 1.25f);
                    if (map->collision_with_background(this, GLTM)) {
                        m_x += (m_walking_speed * 1.25f);
                        if (m_state_cycle > 25)
                            m_state = CSTATE_JUMPING_LEFT;
                    } // if
                    else  {
                        if (map->collision_with_object(this, GLTM, bridge_symbol) != 0)  {
                            m_x += (m_walking_speed * 1.25f);
                            if (m_state_cycle > 25)
                                m_state = CSTATE_JUMPING_LEFT;
                        }
                    }
                } // if


                if (m_state == CSTATE_JUMPING_RIGHT_RIGHT) {
                    m_x += (m_walking_speed * 1.25f);
                    if (map->collision_with_background(this, GLTM)) {
                        m_x -= (m_walking_speed * 1.25f);
                        if (m_state_cycle > 25)
                            m_state = CSTATE_JUMPING_RIGHT;
                    }
                    else  {
                        if (map->collision_with_object(this, GLTM, bridge_symbol) != 0)  {
                            m_x -= (m_walking_speed * 1.25f);
                            if (m_state_cycle > 25)
                                m_state = CSTATE_JUMPING_RIGHT;
                        }
                    }
                }

                {
                    // punch:
                    if (v->m_button[0] && !v->m_old_button[0])
                    {
                        int e_gained = 0;
                        int points_gained = 0;
                        if (m_state == CSTATE_JUMPING_LEFT || m_state == CSTATE_JUMPING_LEFT_LEFT) {
                            GO_enemy *e = (GO_enemy *)map->collision_with_object(GLTM->get
                                          ("ob_character-punchjump-mask-l"), (int)m_x, (int)m_y, GLTM, enemy_symbol);
                            if (e != 0 && e->player_hit(&e_gained, &points_gained)) {
                                if (m_last_hit != 0)
                                    delete m_last_hit;
                                m_last_hit = new Symbol(e->get_class());
                                SFXM->SFX_play("sfx/player_hit_enemy", m_sfx_volume, get_player_angle(), get_player_distance());
                                m_experience += e_gained;
                                inc_score(points_gained);
                            } else {
                                SFXM->SFX_play("sfx/player_attack", m_sfx_volume, get_player_angle(), get_player_distance());
                            }
                        } else {
                            GO_enemy *e = (GO_enemy *)map->collision_with_object(GLTM->get
                                          ("ob_character-punchjump-mask-r"), (int)m_x, (int)m_y, GLTM, enemy_symbol);
                            if (e != 0 && e->player_hit(&e_gained, &points_gained)) {
                                if (m_last_hit != 0)
                                    delete m_last_hit;
                                m_last_hit = new Symbol(e->get_class());
                                SFXM->SFX_play("sfx/player_hit_enemy", m_sfx_volume, get_player_angle(), get_player_distance());
                                m_experience += e_gained;
                                inc_score(points_gained);
                            } else {
                                SFXM->SFX_play("sfx/player_attack", m_sfx_volume, get_player_angle(), get_player_distance());
                            }
                        }

                        m_punch_cycle = 0;
                        if (m_state == CSTATE_JUMPING_LEFT)
                            m_state = CSTATE_JUMPPUNCH_LEFT;
                        if (m_state == CSTATE_JUMPING_RIGHT)
                            m_state = CSTATE_JUMPPUNCH_RIGHT;
                        if (m_state == CSTATE_JUMPING_LEFT_LEFT)
                            m_state = CSTATE_JUMPPUNCH_LEFT_LEFT;
                        if (m_state == CSTATE_JUMPING_RIGHT_RIGHT)
                            m_state = CSTATE_JUMPPUNCH_RIGHT_RIGHT;
                    }
                }
                m_state_cycle++;
            }
            break;
        case CSTATE_JUMPPUNCH_LEFT:
        case CSTATE_JUMPPUNCH_RIGHT:
        case CSTATE_JUMPPUNCH_LEFT_LEFT:
        case CSTATE_JUMPPUNCH_RIGHT_RIGHT: {
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

                if (m_state == CSTATE_JUMPPUNCH_LEFT_LEFT) {
                    m_x -= (m_walking_speed * 1.25f);
                    if (map->collision_with_background(this, GLTM)) {
                        m_x += (m_walking_speed * 1.25f);
                        if (m_state_cycle > 25)
                            m_state = CSTATE_JUMPPUNCH_LEFT;
                    }
                    else  {
                        if (map->collision_with_object(this, GLTM, bridge_symbol) != 0)  {
                            m_x += (m_walking_speed * 1.25f);
                            if (m_state_cycle > 25)
                                m_state = CSTATE_JUMPPUNCH_LEFT;
                        }
                    }
                }
                if (m_state == CSTATE_JUMPPUNCH_RIGHT_RIGHT) {
                    m_x += (m_walking_speed * 1.25f);
                    if (map->collision_with_background(this, GLTM)) {
                        m_x -= (m_walking_speed * 1.25f);
                        if (m_state_cycle > 25)
                            m_state = CSTATE_JUMPPUNCH_RIGHT;
                    } 
                    else  {
                        if (map->collision_with_object(this, GLTM, bridge_symbol) != 0)  {
                            m_x -= (m_walking_speed * 1.25f);
                            if (m_state_cycle > 25)
                                m_state = CSTATE_JUMPPUNCH_RIGHT;
                        }
                    }
                }

                if (y_move > 0) {
                    for (i = 0;i < y_move;i++) {
                        m_y++;
                        if (map->collision_with_background(this, 0, 1, GLTM)) {
                            SFXM->SFX_play("sfx/player_land", m_sfx_volume, get_player_angle(), get_player_distance());
                            if (m_state == CSTATE_JUMPPUNCH_LEFT ||
                                    m_state == CSTATE_JUMPPUNCH_LEFT_LEFT) {
                                m_state = CSTATE_STANDING_LEFT;
                                y_move = 0;
                            }
                            if (m_state == CSTATE_JUMPPUNCH_RIGHT ||
                                    m_state == CSTATE_JUMPPUNCH_RIGHT_RIGHT) {
                                m_state = CSTATE_STANDING_RIGHT;
                                y_move = 0;
                            }
                            m_state_cycle = 0;
                        }
                        else  {
                            if (map->collision_with_object(this, GLTM, bridge_symbol) != 0)  {
                               SFXM->SFX_play("sfx/player_land", m_sfx_volume, get_player_angle(), get_player_distance());
                               if (m_state == CSTATE_JUMPPUNCH_LEFT ||
                                    m_state == CSTATE_JUMPPUNCH_LEFT_LEFT) {
                                    m_state = CSTATE_STANDING_LEFT;
                                    y_move = 0;
                               }
                               if (m_state == CSTATE_JUMPPUNCH_RIGHT ||
                                     m_state == CSTATE_JUMPPUNCH_RIGHT_RIGHT) {
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
                        else  {
                           if (map->collision_with_object(this, GLTM, bridge_symbol) != 0)  {
                               m_y++;
                               m_state_cycle = 22;
                           }
                        }
                        
                    }
                }
                m_state_cycle++;
                m_punch_cycle++;
                if (m_punch_cycle >= 8) {
                    if (m_state == CSTATE_JUMPPUNCH_LEFT)
                        m_state = CSTATE_JUMPING_LEFT;
                    if (m_state == CSTATE_JUMPPUNCH_RIGHT)
                        m_state = CSTATE_JUMPING_RIGHT;
                    if (m_state == CSTATE_JUMPPUNCH_LEFT_LEFT)
                        m_state = CSTATE_JUMPING_LEFT_LEFT;
                    if (m_state == CSTATE_JUMPPUNCH_RIGHT_RIGHT)
                        m_state = CSTATE_JUMPING_RIGHT_RIGHT;
                }
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
                    if (m_facebefore_vine == 0)
                        m_state = CSTATE_STANDING_LEFT;
                    else
                        m_state = CSTATE_STANDING_RIGHT;
                    m_state_cycle = 0;
                    do {
                        m_y -= PLAYER_CLIMBING_SPEED;
                    } while (m_y >= 0 && map->collision_with_background(this, GLTM));
                } else {
                    m_y += 8;
                }

                // check for reaching a platform:
                if (m_state == CSTATE_CLIMBING_UP &&
                        !map->collision_with_background(this, GLTM) &&
                        map->collision_with_background(this, 0, 1, GLTM)) {
                    set_layer(2, map);
                    if (m_facebefore_vine == 0)
                        m_state = CSTATE_STANDING_LEFT;
                    else
                        m_state = CSTATE_STANDING_RIGHT;
                    m_state_cycle = 0;
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
                if (!map->collision_with_background(this, GLTM) || !map->collision_with_object(this, GLTM, rope_symbol)) {
                    set_layer(2, map);
                    if (m_facebefore_vine == 0)
                        m_state = CSTATE_STANDING_LEFT;
                    else
                        m_state = CSTATE_STANDING_RIGHT;
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

        case CSTATE_ENTERING_DOOR:
            if (m_state_cycle == 0)
                SFXM->SFX_play("sfx/skulldoor_warp", m_sfx_volume);
            m_state_cycle++;
            if (m_state_cycle > 50) {
                m_requested_room = true;
                m_state = CSTATE_STANDING_RIGHT;
            }
            break;
        case CSTATE_DYING:
            m_state_cycle++;
			if (m_state_cycle / 8 == 5)
				SFXM->SFX_play("sfx/player_land", m_sfx_volume, get_player_angle(), get_player_distance());
				
            if (m_state_cycle > 100)
                m_state = CSTATE_DEAD;
            break;
        case CSTATE_DEAD:
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
        case CSTATE_PUNCH_LEFT:
        case CSTATE_PUNCH_RIGHT:
        case CSTATE_JUMPPUNCH_LEFT:
        case CSTATE_JUMPPUNCH_RIGHT:
        case CSTATE_JUMPPUNCH_LEFT_LEFT:
        case CSTATE_JUMPPUNCH_RIGHT_RIGHT:
        case CSTATE_CLIMBING:
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

			// time playing of sfx to step animation
			if (m_state_cycle % 16 == 0) {
                bool walk_on_bridge = ((map->collision_with_object(this, 0, 2, GLTM, bridge_symbol)!=0) ? true : false);

				if (m_walking_channel != -1) {
						if (m_step_cycle == 1) {
							if (walk_on_bridge) 
								SFXM->SFX_play_channel("sfx/player_walk_bridge1", m_walking_channel, get_player_angle(), get_player_distance(), m_sfx_volume);
							else
								SFXM->SFX_play_channel("sfx/player_walk1", m_walking_channel, get_player_angle(), get_player_distance(), m_sfx_volume);
							m_step_cycle = 0;
						} else {
							if (walk_on_bridge) 
								SFXM->SFX_play_channel("sfx/player_walk_bridge2", m_walking_channel, get_player_angle(), get_player_distance(), m_sfx_volume);
							else
								SFXM->SFX_play_channel("sfx/player_walk2", m_walking_channel, get_player_angle(), get_player_distance(), m_sfx_volume);
							m_step_cycle = 1;
						}
						Mix_SetPosition(m_walking_channel, get_player_angle(), get_player_distance());
				} else {
					if (walk_on_bridge) 
						m_walking_channel = SFXM->SFX_play("sfx/player_walk_bridge1", m_sfx_volume);
					else
						m_walking_channel = SFXM->SFX_play("sfx/player_walk1", m_sfx_volume);
					m_step_cycle = 1;
				}
			}
            break;
        case CSTATE_CLIMBING_UP:
        case CSTATE_CLIMBING_DOWN:
            if (m_walking_channel != -1) {
                Mix_HaltChannel(m_walking_channel);
                m_walking_channel = -1;
            }
            if (m_climbing_channel != -1) {
                Mix_SetPosition(m_climbing_channel, get_player_angle(), get_player_distance());
            }

            if (m_climbing_channel == -1) {
				m_climbing_channel = SFXM->SFX_play_continuous("sfx/player_climb", m_sfx_volume, get_player_angle(), get_player_distance(),this);
            }
            break;
    }
	
    if (m_layer != 3 &&
        map->collision_with_background(this, GLTM)) {
        int i, j;
        bool found = false;

#ifdef __DEBUG_MESSAGES
		output_debug_message("GO_character: finding a proper position from %i,%i\n",int(m_x),int(m_y));
#endif

        for (i = 1;i < 5 && !found;i++) {
            for (j = 0;j <= i && !found;j++) {
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

#ifdef __DEBUG_MESSAGES
		if (found) output_debug_message("GO_character: found proper position: %i,%i\n",int(m_x),int(m_y));
#endif

        if (!found && m_player_hit_counter == 0) {
            m_energy = 0;
            m_player_hit_counter = 64;
            SFXM->SFX_play("sfx/player_dead", m_sfx_volume, get_player_angle(), get_player_distance());
        }
    }

    // check for keys:
    if (!m_key) {
        GObject *o = map->collision_with_object(this, GLTM, key_symbol);
        if (o != 0) {
            if (o->get_state() == 0) {
                SFXM->SFX_play("sfx/player_pickup_key", m_sfx_volume, get_player_angle(), get_player_distance());
                o->set_state(1);
                m_key = true;
                inc_score(200);

                if (m_last_pick != 0)
                    delete m_last_pick;
                m_last_pick = new Symbol(key_symbol);
            }
        }
    }

    // check for coins:
    {
        GObject *o = map->collision_with_object(this, GLTM, coin_symbol);
        if (o != 0)
        {
            if (o->get_state() == 0) {
                SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                o->set_state(2);
                inc_score(500);
                if (m_last_pick != 0)
                    delete m_last_pick;
                m_last_pick = new Symbol("coin_symbol");
            }
        }
    }

    // check for other items:
    {
        GO_item *o = (GO_item *)map->collision_with_object(this, GLTM, item_symbol);
        if (o != 0)
        {
            if (o->get_state() == 0) {
                o->set_state(1);
                if (m_last_pick != 0)
                    delete m_last_pick;
                m_last_pick = new Symbol(item_symbol);

                // Pick up the object: update the internal status:
                switch (o->get_type()) {
                    case 0:
                        SFXM->SFX_play("sfx/rescue_goonie", m_sfx_volume);
                        m_goonies_rescued++;
                        inc_score(2000);
                        break;
                    case 1:
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        m_energy += 6;
                        if (m_energy > PLAYER_MAX_ENERGY)
                            m_energy = PLAYER_MAX_ENERGY;
                        break;
                    case 2:
                        m_items.Add(new Symbol("GO_yellowhelmet"));
                        m_yellowhelmet_status = 5;
                        m_yellowhelmet_timer = 0;
                        m_last_pick = new Symbol("GO_yellowhelmet");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 3:
                        m_items.Add(new Symbol("GO_shoes"));
                        if (m_last_pick != 0)
                            delete m_last_pick;
                        m_last_pick = new Symbol("GO_shoes");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 4:
                        m_items.Add(new Symbol("GO_greycoat"));
                        m_greycoat_status = 5;
                        m_greycoat_timer = 0;
                        m_last_pick = new Symbol("GO_greycoat");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 5:
                        m_items.Add(new Symbol("GO_yellowcoat"));
                        if (m_last_pick != 0)
                            delete m_last_pick;
                        m_last_pick = new Symbol("GO_yellowcoat");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        m_yellowcoat_timer = 500;
                        break;
                    case 6:
                        m_items.Add(new Symbol("GO_hammer"));
                        m_hammer_status = 30;	// each time a drop falls, this is decremented once, and every time it becomes
												// an odd number, the drop is converted into a coin
                        if (m_last_pick != 0)
                            delete m_last_pick;
                        m_last_pick = new Symbol("GO_hammer");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 7:  // energy increasing bag
                        m_energy += 8;
                        if (m_energy >= PLAYER_MAX_ENERGY)
                            m_energy = PLAYER_MAX_ENERGY;
                        break;
                    case 8:
                        m_items.Add(new Symbol("GO_lamp"));
                        if (m_last_pick != 0)
                            delete m_last_pick;
                        m_last_pick = new Symbol("GO_lamp");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 9:
                        m_items.Add(new Symbol("GO_greenbook"));
                        m_greenbook_status = 5;
                        m_last_pick = new Symbol("GO_greenbook");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 10:
                        m_items.Add(new Symbol("GO_redbook"));
                        m_redbook_status = 4;
                        m_last_pick = new Symbol("GO_redbook");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 11:
                        m_items.Add(new Symbol("GO_lightbluebook"));
                        m_lightbluebook_status = 5;
                        m_last_pick = new Symbol("GO_lightbluebook");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 12:
                        m_items.Add(new Symbol("GO_bluebook"));
                        m_bluebook_status = 5;
                        m_last_pick = new Symbol("GO_bluebook");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 13:
                        m_items.Add(new Symbol("GO_greencoat"));
                        m_greencoat_timer = 500;
                        m_last_pick = new Symbol("GO_greencoat");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 14:
                        m_items.Add(new Symbol("GO_whitebook"));
                        m_whitebook_status = 5;
                        m_last_pick = new Symbol("GO_whitebook");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 15:
                        m_items.Add(new Symbol("GO_yellowshield"));
                        m_yellowshield_status = 5;
                        m_last_pick = new Symbol("GO_yellowshield");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 16:
                        m_experience++;
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        break;
                    case 17:
                        m_items.Add(new Symbol("GO_lightbluecoat"));
                        m_lightbluecoat_timer = 500;
                        m_last_pick = new Symbol("GO_lightbluecoat");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 18:
                        m_items.Add(new Symbol("GO_whiteshield"));
                        m_yellowshield_status = 5;
                        m_last_pick = new Symbol("GO_whiteshield");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 19:
                        m_items.Add(new Symbol("GO_redbadbook"));
                        m_last_pick = new Symbol("GO_redbadbook");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 20:
                        m_items.Add(new Symbol("GO_purplebook"));
                        m_last_pick = new Symbol("GO_purplebook");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 21:
                        m_items.Add(new Symbol("GO_lightbluehelmet"));
                        m_lightbluehelmet_status = 5;
                        m_last_pick = new Symbol("GO_lightbluehelmet");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 22:
                        m_items.Add(new Symbol("GO_yellowbook"));
                        m_yellowbook_status = 5;
                        m_last_pick = new Symbol("GO_yellowbook");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 23:
                        m_items.Add(new Symbol("GO_purplebadbook"));
                        m_last_pick = new Symbol("GO_purplebadbook");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 24:
                        m_items.Add(new Symbol("GO_purpleshield"));
                        m_purpleshield_status = 5;
                        m_last_pick = new Symbol("GO_purpleshield");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 25:
                        m_items.Add(new Symbol("GO_clock"));
                        m_clock_timer = 1500;
                        m_last_pick = new Symbol("GO_clock");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 26:
                        m_items.Add(new Symbol("GO_bluebadbook"));
                        m_bluebadbook_nghosts = 2;
                        m_last_pick = new Symbol("GO_bluebadbook");
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        inc_score(1000);
                        break;
                    case 27:
                        SFXM->SFX_play("sfx/rescue_goonie", m_sfx_volume);
						if (!m_rescued_goonies[0])
						{
							m_rescued_goonies[0] = true;
							m_goonies_rescued++;
						}
                        inc_score(2000);
                        break;
                    case 28:
                        SFXM->SFX_play("sfx/rescue_goonie", m_sfx_volume);
						if (!m_rescued_goonies[1])
						{
							m_rescued_goonies[1] = true;
							m_goonies_rescued++;
						}
                        inc_score(2000);
                        break;
                    case 29:
                        SFXM->SFX_play("sfx/rescue_goonie", m_sfx_volume);
						if (!m_rescued_goonies[2])
						{
							m_rescued_goonies[2] = true;
							m_goonies_rescued++;
						}
                        inc_score(2000);
                        break;
                    case 30:
                        SFXM->SFX_play("sfx/rescue_goonie", m_sfx_volume);
						if (!m_rescued_goonies[3])
						{
							m_rescued_goonies[3] = true;
							m_goonies_rescued++;
						}
                        inc_score(2000);
                        break;
                    case 31:
                        SFXM->SFX_play("sfx/rescue_goonie", m_sfx_volume);
						if (!m_rescued_goonies[4])
						{
							m_rescued_goonies[4] = true;
							m_goonies_rescued++;
						}
                        inc_score(2000);
                        break;
                    case 32:
                        SFXM->SFX_play("sfx/rescue_goonie", m_sfx_volume);
						if (!m_rescued_goonies[5])
						{
							m_rescued_goonies[5] = true;
							m_goonies_rescued++;
						}
                        inc_score(2000);
                        break;
                    case 33:
                        SFXM->SFX_play("sfx/rescue_goonie", m_sfx_volume);
						if (!m_rescued_goonies[6])
						{
							m_rescued_goonies[6] = true;
							m_goonies_rescued++;
						}
                        inc_score(2000);
                        break;
                    default:
                        SFXM->SFX_play("sfx/player_pickup_item", m_sfx_volume, get_player_angle(), get_player_distance());
                        break;
                } 
            }
        }
    }

    // check for enemies:
    if (m_player_hit_counter > 0) {
        m_player_hit_counter--;
    } else {
        if (m_state != CSTATE_ENTERING_DOOR && !m_requested_room) {
            bool saved = false;
            int energy_taken = 0;
            GO_enemy *e = (GO_enemy *)map->collision_with_object(this, GLTM, "GO_enemy");
            if (e != 0)
                energy_taken = (int)(e->enemy_hit() * difficulty / 100);
            if (energy_taken != 0) {
                if (e->get_class()->cmp(bat_symbol))
                    SFXM->SFX_play("sfx/bat_attack", m_sfx_volume, get_angle(map), get_distance(map));
                if (e->get_class()->cmp(fallingrock_symbol)) {
                    if (player_has("GO_yellowhelmet") && (m_yellowhelmet_status > 0 || m_yellowhelmet_timer > 0)) {
                        if (m_yellowhelmet_timer == 0) {
                            m_yellowhelmet_timer = 32;
                            m_yellowhelmet_status--;
                        }
                        saved = true;
                    }
                }
                if (e->get_class()->cmp(bullet_symbol)) {
                    if (player_has("GO_yellowshield") && (m_yellowshield_status > 0 || m_yellowshield_timer > 0)) {
                        if (m_yellowshield_timer == 0) {
                            m_yellowshield_timer = 32;
                            m_yellowshield_status--;
                        }
                        saved = true;
                    }
                }
                if (e->get_class()->cmp(musicalnote_symbol)) {
                    if (player_has("GO_whiteshield") && (m_whiteshield_status > 0 || m_whiteshield_timer > 0)) {
                        if (m_whiteshield_timer == 0) {
                            m_whiteshield_timer = 32;
                            m_whiteshield_status--;
                        }
                        saved = true;
                    }
                }
                if (e->get_class()->cmp(bone_symbol)) {
                    if (player_has("GO_purpleshield") && (m_purpleshield_status > 0 || m_purpleshield_timer > 0)) {
                        if (m_purpleshield_timer == 0) {
                            m_purpleshield_timer = 32;
                            m_purpleshield_status--;
                        }
                        saved = true;
                    }
                }
                if (e->get_class()->cmp(drop_symbol)) {
                    if (player_has("GO_greycoat") && (m_greycoat_status > 0 || m_greycoat_timer > 0)) {
                        if (m_greycoat_timer == 0) {
                            m_greycoat_timer = 32;
                            m_greycoat_status--;
                        }
                        saved = true;
                    }
                }
                if (e->is_a(skull_symbol)) {
                    int exp;
                    int score;
                    if (player_has("GO_greenbook") && m_greenbook_status > 0) {
                        m_greenbook_status--;
                        e->player_hit(&exp, &score);
                        m_experience += exp;
                        saved = true;
                    }

                    if (!saved) {
                        if (player_has("GO_bluebook") && (m_bluebook_status > 0 || m_bluebook_timer > 0)) {
                            if (m_bluebook_timer == 0) {
                                m_bluebook_timer = 32;
                                m_bluebook_status--;
                            }
                            saved = true;
                        }
                    }
                }
                if (e->get_class()->cmp(bat_symbol)) {
                    int exp;
                    int score;
                    if (player_has("GO_lightbluebook") && m_lightbluebook_status > 0) {
                        m_lightbluebook_status--;
                        e->player_hit(&exp, &score);
                        m_experience += exp;
                        saved = true;
                    }

                    if (!saved) {
                        if (player_has("GO_lightbluehelmet") && (m_lightbluehelmet_status > 0 || m_lightbluehelmet_timer > 0)) {
                            if (m_lightbluehelmet_timer == 0) {
                                m_lightbluehelmet_timer = 32;
                                m_lightbluehelmet_status--;
                            }
                            saved = true;
                        }
                    }
                }
                if (e->get_class()->cmp(skeleton_symbol)) {
                    int exp;
                    int score;
                    if (player_has("GO_whitebook") && m_whitebook_status > 0) {
                        m_whitebook_status--;
                        e->player_hit(&exp, &score);
                        m_experience += exp;
                        saved = true;
                    }

                    if (!saved) {
                        if (player_has("GO_yellowbook") && (m_yellowbook_status > 0 || m_yellowbook_timer > 0)) {
                            if (m_yellowbook_timer == 0) {
                                m_yellowbook_timer = 32;
                                m_yellowbook_status--;
                            }
                            saved = true;
                        }
                    }
                }
                if (e->get_class()->cmp(pipe_water_symbol)) {
                    if (player_has("GO_yellowcoat") && m_yellowcoat_timer > 0) {
                        saved = true;
                        m_yellowcoat_timer--;
                    }
                }
                if (e->get_class()->cmp(flame_symbol)) {
                    if (player_has("GO_greencoat") && m_greencoat_timer > 0) {
                        saved = true;
                        m_greencoat_timer--;
                    }
                }
                if (e->get_class()->cmp(fallingwater_symbol)) {
                    if (player_has("GO_lightbluecoat") && m_lightbluecoat_timer > 0) {
                        saved = true;
                        m_lightbluecoat_timer--;
                    }
                }
                if (m_last_hit_by != 0)
                    delete m_last_hit_by;
                m_last_hit_by = new Symbol(e->get_class());
                if (!saved) {
                    m_energy -= energy_taken;
                    m_player_hit_counter = 64;
                    if (e->is_a(fallingwater_symbol))
                        m_player_hit_counter = 8;
                    if (e->is_a(flame_symbol))
                        m_player_hit_counter = 8;
                    if (e->is_a(pipe_water_symbol))
                        m_player_hit_counter = 8;
                    if (m_energy > 0)
                        SFXM->SFX_play("sfx/player_hit", m_sfx_volume, get_player_angle(), get_player_distance());
                    else
                        SFXM->SFX_play("sfx/player_dead", m_sfx_volume, get_player_angle(), get_player_distance());
                }
            }
        }
    }

    // check for experience:
    if (m_experience >= PLAYER_MAX_EXPERIENCE) {
        m_experience = 0;
        m_energy += 8;
        if (m_energy >= PLAYER_MAX_ENERGY)
            m_energy = PLAYER_MAX_ENERGY;
    }

    if (m_energy <= 0 && m_state != CSTATE_DYING && m_state != CSTATE_DEAD) {
        m_player_hit_counter = 512;
        m_energy = 0;
        m_state = CSTATE_DYING;
        m_state_cycle = 0;
		if (m_walking_channel!=0) SFXM->SFX_stop(m_walking_channel);
		if (m_climbing_channel!=0) SFXM->SFX_stop(m_climbing_channel);
		m_walking_channel=-1;
		m_climbing_channel=-1;
    }

    if (m_yellowhelmet_timer > 0)
        m_yellowhelmet_timer--;
    if (m_greycoat_timer > 0)
        m_greycoat_timer--;
    if (m_bluebook_timer > 0)
        m_bluebook_timer--;
    if (m_yellowshield_timer > 0)
        m_yellowshield_timer--;
    if (m_whiteshield_timer > 0)
        m_whiteshield_timer--;
    if (m_lightbluehelmet_timer > 0)
        m_lightbluehelmet_timer--;
    if (m_yellowbook_timer > 0)
        m_yellowbook_timer--;
    if (m_purpleshield_timer > 0)
        m_purpleshield_timer--;
    if (m_clock_timer > 0)
        m_clock_timer--;

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

void GO_character::draw(GLTManager *GLTM)
{
	int s2 = (m_state_cycle / 8) % 2;
    int s4 = (m_state_cycle / 8) % 4;
    int s10 = m_state_cycle / 8;
    float xo = 0, yo = 0;

    switch (m_state) {
        case CSTATE_STANDING_LEFT:
            if (m_turning_counter)
                m_last_tile_used = GLTM->get
                                   ("ob_character-turn");
            else {
				int salute = (m_state_cycle % 615);
				if (salute >= 250 && salute < 255 ) m_last_tile_used = GLTM->get ("ob_character_salute-l1");
				else if (salute >= 255 && salute < 260) m_last_tile_used = GLTM->get ("ob_character_salute-l2");
				else if (salute >= 260 && salute < 265) m_last_tile_used = GLTM->get ("ob_character_salute-l1");
				else if (salute >= 265 && salute < 270) m_last_tile_used = GLTM->get ("ob_character_salute-l2");
				else if (salute >= 270 && salute < 275) m_last_tile_used = GLTM->get ("ob_character_salute-l1");
				else if (salute >= 275 && salute < 280) m_last_tile_used = GLTM->get ("ob_character_salute-l2");

				else if (salute >= 500 && salute < 505) m_last_tile_used = GLTM->get ("ob_character-tapping-foot1");
				else if (salute >= 505 && salute < 510) m_last_tile_used = GLTM->get ("ob_character-tapping-foot2");
				else if (salute >= 510 && salute < 515) m_last_tile_used = GLTM->get ("ob_character-tapping-foot3");
				else if (salute >= 515 && salute < 520) m_last_tile_used = GLTM->get ("ob_character-tapping-foot2");
				else if (salute >= 520 && salute < 525) m_last_tile_used = GLTM->get ("ob_character-tapping-foot1");
				else if (salute >= 525 && salute < 530) m_last_tile_used = GLTM->get ("ob_character-tapping-foot2");
				else if (salute >= 530 && salute < 535) m_last_tile_used = GLTM->get ("ob_character-tapping-foot3");
				else if (salute >= 535 && salute < 540) m_last_tile_used = GLTM->get ("ob_character-tapping-foot2");
				else if (salute >= 540 && salute < 545) m_last_tile_used = GLTM->get ("ob_character-tapping-foot1");
				else if (salute >= 545 && salute < 550) m_last_tile_used = GLTM->get ("ob_character-tapping-foot2");
				else if (salute >= 550 && salute < 555) m_last_tile_used = GLTM->get ("ob_character-tapping-foot3");
				else if (salute >= 555 && salute < 560) m_last_tile_used = GLTM->get ("ob_character-tapping-foot2");
				else if (salute >= 560 && salute < 565) m_last_tile_used = GLTM->get ("ob_character-tapping-foot1");
				else if (salute >= 565 && salute < 570) m_last_tile_used = GLTM->get ("ob_character-knocking1");
				else if (salute >= 570 && salute < 575) m_last_tile_used = GLTM->get ("ob_character-knocking2");
				else if (salute >= 575 && salute < 580) m_last_tile_used = GLTM->get ("ob_character-knocking3");
				else if (salute >= 580 && salute < 585) m_last_tile_used = GLTM->get ("ob_character-knocking4");
				else if (salute >= 585 && salute < 590) m_last_tile_used = GLTM->get ("ob_character-knocking3");
				else if (salute >= 590 && salute < 595) m_last_tile_used = GLTM->get ("ob_character-knocking4");
				else if (salute >= 595 && salute < 600) m_last_tile_used = GLTM->get ("ob_character-knocking3");
				else if (salute >= 600 && salute < 605) m_last_tile_used = GLTM->get ("ob_character-knocking4");
				else if (salute >= 605 && salute < 610) m_last_tile_used = GLTM->get ("ob_character-knocking2");
				else if (salute >= 610 && salute < 615) m_last_tile_used = GLTM->get ("ob_character-knocking1");

                else if (s4 == 0 || s4 == 1) m_last_tile_used = GLTM->get ("ob_character_stand-l1");
                else if (s4 == 2 || s4 == 3) m_last_tile_used = GLTM->get ("ob_character_stand-l2");
			}
            break;
        case CSTATE_STANDING_RIGHT:
            if (m_turning_counter)
                m_last_tile_used = GLTM->get
                                   ("ob_character-turn");
            else {
				int salute = (m_state_cycle % 615);
				if (salute >= 250 && salute < 255 ) m_last_tile_used = GLTM->get ("ob_character_salute-r1");
				else if (salute >= 255 && salute < 260) m_last_tile_used = GLTM->get ("ob_character_salute-r2");
				else if (salute >= 260 && salute < 265) m_last_tile_used = GLTM->get ("ob_character_salute-r1");
				else if (salute >= 265 && salute < 270) m_last_tile_used = GLTM->get ("ob_character_salute-r2");
				else if (salute >= 270 && salute < 275) m_last_tile_used = GLTM->get ("ob_character_salute-r1");
				else if (salute >= 275 && salute < 280) m_last_tile_used = GLTM->get ("ob_character_salute-r2");

				else if (salute >= 500 && salute < 505) m_last_tile_used = GLTM->get ("ob_character-tapping-foot1");
				else if (salute >= 505 && salute < 510) m_last_tile_used = GLTM->get ("ob_character-tapping-foot2");
				else if (salute >= 510 && salute < 515) m_last_tile_used = GLTM->get ("ob_character-tapping-foot3");
				else if (salute >= 515 && salute < 520) m_last_tile_used = GLTM->get ("ob_character-tapping-foot2");
				else if (salute >= 520 && salute < 525) m_last_tile_used = GLTM->get ("ob_character-tapping-foot1");
				else if (salute >= 525 && salute < 530) m_last_tile_used = GLTM->get ("ob_character-tapping-foot2");
				else if (salute >= 530 && salute < 535) m_last_tile_used = GLTM->get ("ob_character-tapping-foot3");
				else if (salute >= 535 && salute < 540) m_last_tile_used = GLTM->get ("ob_character-tapping-foot2");
				else if (salute >= 540 && salute < 545) m_last_tile_used = GLTM->get ("ob_character-tapping-foot1");
				else if (salute >= 545 && salute < 550) m_last_tile_used = GLTM->get ("ob_character-tapping-foot2");
				else if (salute >= 550 && salute < 555) m_last_tile_used = GLTM->get ("ob_character-tapping-foot3");
				else if (salute >= 555 && salute < 560) m_last_tile_used = GLTM->get ("ob_character-tapping-foot2");
				else if (salute >= 560 && salute < 565) m_last_tile_used = GLTM->get ("ob_character-tapping-foot1");
				else if (salute >= 565 && salute < 570) m_last_tile_used = GLTM->get ("ob_character-knocking1");
				else if (salute >= 570 && salute < 575) m_last_tile_used = GLTM->get ("ob_character-knocking2");
				else if (salute >= 575 && salute < 580) m_last_tile_used = GLTM->get ("ob_character-knocking3");
				else if (salute >= 580 && salute < 585) m_last_tile_used = GLTM->get ("ob_character-knocking4");
				else if (salute >= 585 && salute < 590) m_last_tile_used = GLTM->get ("ob_character-knocking3");
				else if (salute >= 590 && salute < 595) m_last_tile_used = GLTM->get ("ob_character-knocking4");
				else if (salute >= 595 && salute < 600) m_last_tile_used = GLTM->get ("ob_character-knocking3");
				else if (salute >= 600 && salute < 605) m_last_tile_used = GLTM->get ("ob_character-knocking4");
				else if (salute >= 605 && salute < 610) m_last_tile_used = GLTM->get ("ob_character-knocking2");
				else if (salute >= 610 && salute < 615) m_last_tile_used = GLTM->get ("ob_character-knocking1");

                else if (s4 == 0 || s4 == 1) m_last_tile_used = GLTM->get ("ob_character_stand-r1");
                else if (s4 == 2 || s4 == 3) m_last_tile_used = GLTM->get ("ob_character_stand-r2");
			}
            break;
        case CSTATE_WALKING_LEFT:
            if (m_turning_counter) {
                m_last_tile_used = GLTM->get
                                   ("ob_character-turn");
            } else {
                if (s4 == 0)
                    m_last_tile_used = GLTM->get
                                       ("ob_character-l2");
                if (s4 == 1)
                    m_last_tile_used = GLTM->get
                                       ("ob_character-l1");
                if (s4 == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_character-l3");
                if (s4 == 3)
                    m_last_tile_used = GLTM->get
                                       ("ob_character-l1");
            }
            break;
        case CSTATE_WALKING_RIGHT:
            if (m_turning_counter) {
                m_last_tile_used = GLTM->get
                                   ("ob_character-turn");
            } else {
                if (s4 == 0)
                    m_last_tile_used = GLTM->get
                                       ("ob_character-r2");
                if (s4 == 1)
                    m_last_tile_used = GLTM->get
                                       ("ob_character-r1");
                if (s4 == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_character-r3");
                if (s4 == 3)
                    m_last_tile_used = GLTM->get
                                       ("ob_character-r1");
            }
            break;
        case CSTATE_PUNCH_LEFT:
            m_last_tile_used = GLTM->get
                               ("ob_character-punch-l");
            break;
        case CSTATE_PUNCH_RIGHT:
            m_last_tile_used = GLTM->get
                               ("ob_character-punch-r");
            break;
        case CSTATE_FALLING_LEFT:
			if (s2 == 0) m_last_tile_used = GLTM->get("ob_character-fall1");
			if (s2 == 1) m_last_tile_used = GLTM->get("ob_character-fall2");
            break;
        case CSTATE_FALLING_RIGHT:
			if (s2 == 0) m_last_tile_used = GLTM->get("ob_character-fall1");
			if (s2 == 1) m_last_tile_used = GLTM->get("ob_character-fall2");
            break;
        case CSTATE_JUMPING_LEFT:
        case CSTATE_JUMPING_LEFT_LEFT:
            m_last_tile_used = GLTM->get
                               ("ob_character-jump-l");
            break;
        case CSTATE_JUMPING_RIGHT:
        case CSTATE_JUMPING_RIGHT_RIGHT:
            m_last_tile_used = GLTM->get
                               ("ob_character-jump-r");
            break;
        case CSTATE_JUMPPUNCH_LEFT:
        case CSTATE_JUMPPUNCH_LEFT_LEFT:
            m_last_tile_used = GLTM->get
                               ("ob_character-punchjump-l");
            break;
        case CSTATE_JUMPPUNCH_RIGHT:
        case CSTATE_JUMPPUNCH_RIGHT_RIGHT:
            m_last_tile_used = GLTM->get
                               ("ob_character-punchjump-r");
            break;
        case CSTATE_CLIMBING_UP:
        case CSTATE_CLIMBING:
        case CSTATE_CLIMBING_DOWN:
            if (s4 == 0 || s4 == 2)
                m_last_tile_used = GLTM->get
                                   ("ob_character-climbing-1");
            if (s4 == 1 || s4 == 3)
                m_last_tile_used = GLTM->get
                                   ("ob_character-climbing-2");
            break;
        case CSTATE_ENTERING_DOOR:
            if (s4 == 0)
                m_last_tile_used = GLTM->get
                                   ("ob_character-r2");
            if (s4 == 1)
                m_last_tile_used = GLTM->get
                                   ("ob_character-r1");
            if (s4 == 2)
                m_last_tile_used = GLTM->get
                                   ("ob_character-r3");
            if (s4 == 3)
                m_last_tile_used = GLTM->get
                                   ("ob_character-r1");
            xo = -(m_state_cycle / 75.0F);
            yo = 0.0;
            break;
		case CSTATE_DYING:
			if (s10 == 0) m_last_tile_used = GLTM->get("ob_character-death1");
			if (s10 == 1) m_last_tile_used = GLTM->get("ob_character-death2");
			if (s10 == 2) m_last_tile_used = GLTM->get("ob_character-death3");
			if (s10 == 3) m_last_tile_used = GLTM->get("ob_character-death3");
			if (s10 == 4) m_last_tile_used = GLTM->get("ob_character-death4");
			if (s10 == 5) m_last_tile_used = GLTM->get("ob_character-death5");
			if (s10 == 6) m_last_tile_used = GLTM->get("ob_character-death6");
			if (s10 == 7) m_last_tile_used = GLTM->get("ob_character-death5");
			if (s10 == 8) m_last_tile_used = GLTM->get("ob_character-death6");
			if (s10 >= 9) m_last_tile_used = GLTM->get("ob_character-death5");
			break;
    }

    if ((m_player_hit_counter > 0) && (m_state != CSTATE_DYING) && (m_state != CSTATE_DEAD)) {
        int bufi;
        float TexColorArray[4], bufvf[4];
        float f = float(0.5F + 0.5F * sin(m_player_hit_counter * ((m_state == CSTATE_DYING || m_state == CSTATE_DEAD) ? 0.8f : 0.4f)));

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

bool GO_character::is_a(Symbol *c)
{
    if (c->cmp(character_symbol))
        return true;

    return GObject::is_a(c);
}

bool GO_character::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
}
void GO_character::inc_score(int m_score)
{
    score += m_score;
}

/*
 * Calculates the 'angle' of the player, based on its distance from the screen's center
 */
int GO_character::get_player_angle()
{
    float x = 320 - m_x;
    float y = 240 - m_y;
    int angle;

    angle = (int)(atan2(y, x) * 180 / 3.149265);
    angle = 270 - angle;

    if (angle > 360) {
        angle = 360;
    }
    if (angle < 0) {
        angle = 0;
    }
    return angle;
}

