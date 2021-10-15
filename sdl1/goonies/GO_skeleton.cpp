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

#include "GLTManager.h"
#include "SoundManager.h"
#include "SFXManager.h"
#include "GObject.h"
#include "GO_enemy.h"
#include "GO_skeleton.h"
#include "GO_character.h"

#include "GObjectCreator.h"

#include "GMap.h"

#include "TheGooniesCtnt.h"

#include "debug.h"

extern int difficulty;

GO_skeleton::GO_skeleton(int x, int y, int sfx_volume, int facing) : GO_enemy(x, y, sfx_volume)
{
    m_class = new Symbol(skeleton_symbol);

    m_facing = facing;
    m_state = 5;
    m_state_cycle = 0;
    m_time_since_last_bone = 0;
	m_turning_counter = 0;

    m_walking_channel = -1;
} /* GO_skeleton::GO_skeleton */



bool GO_skeleton::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{

#ifdef __DEBUG_MESSAGES
    output_debug_message("Skeleton %i,%i -> %g,%g\n", m_state, m_state_cycle, m_x, m_y);
#endif

    m_previous_facing = m_facing;

    GO_character *c = (GO_character *)map->get_object(character_symbol);
    bool has_purplebadbook = false;
    if (c != 0 && c->player_has("GO_purplebadbook") != 0)
        has_purplebadbook = true;

    switch (m_state) {
        case 0:  // appearing
            if (m_state_cycle > 64) {
                m_state = 1;
                m_state_cycle = 0;
            }
            break;
        case 1:  // moving
            // continually update our mixing based on player position
            if (m_walking_channel != 1) {
                Mix_SetPosition(m_walking_channel, get_angle(map), get_distance(map));
            }

            if (m_walking_channel == -1) {
                m_walking_channel = SFXM->SFX_play_continuous("sfx/skeleton_walk", m_sfx_volume,get_angle(map), get_distance(map) ,this);
            }
            if (m_facing == 0) {
                m_x -= (has_purplebadbook ? SKELETON_SPEED - (100 / 150 - difficulty / 150) : (SKELETON_SPEED - (100 / 150 - difficulty / 150)) * 1.25f);
                if (m_x < 0 ||
                        map->collision_with_background(this, -2, 0, GLTM) ||
                        (map->collision_with_background(this, -15, 32, GLTM) &&
                         !map->collision_with_background(this, -20, 32, GLTM) &&
						 !map->collision_with_object(this, -20, 32, GLTM, bridge_symbol))) {
                    m_x += (has_purplebadbook ? SKELETON_SPEED - (100 / 150 - difficulty / 150) : (SKELETON_SPEED - (100 / 150 - difficulty / 150)) * 1.25f);
                    m_facing = 1;
                }
            } else {
                m_x += (has_purplebadbook ? SKELETON_SPEED - (100 / 150 - difficulty / 150) : (SKELETON_SPEED - (100 / 150 - difficulty / 150)) * 1.25f);
                if (m_x + 16 > map->get_dx() ||
                        map->collision_with_background(this, 2, 0, GLTM) ||
                        (map->collision_with_background(this, 15, 32, GLTM) &&
                         !map->collision_with_background(this, 20, 32, GLTM) &&
						 !map->collision_with_object(this, 20, 32, GLTM, bridge_symbol))) {
                    m_x -= (has_purplebadbook ? SKELETON_SPEED - (100 / 150 - difficulty / 150) : (SKELETON_SPEED - (100 / 150 - difficulty / 150)) * 1.25f);
                    m_facing = 0;
                }
            }
            // if player on sight, shoot:

            if (m_state_cycle > 25 && m_time_since_last_bone > 150) {
                GO_character *c = (GO_character *)map->get_object(character_symbol);
                if (c != 0) {
                    if (m_facing == 0) {
                        if (c->get_x() < m_x && c->get_y() - 40 > m_y - 32 && c->get_y() - 40 < m_y + 32) {
                            m_state = 3;
                            m_state_cycle = 0;
                        } // if
                    } else {
                        if (c->get_x() > m_x && c->get_y() - 40 > m_y - 32 && c->get_y() - 40 < m_y + 32) {
                            m_state = 3;
                            m_state_cycle = 0;
                        }
                    }
                }
            }
			if (map->collision_with_object(this, GLTM, bigrock_symbol)) {
				m_state = 2;
				m_state_cycle = 0;
			}			
			
            break;
        case 2:  // dying
            if (m_walking_channel != -1) {
                Mix_HaltChannel(m_walking_channel);
                m_walking_channel = -1;
            }
            if (m_state_cycle == 0)
                SFXM->SFX_play("sfx/skeleton_dead", m_sfx_volume, get_angle(map), get_distance(map));

            if (m_state_cycle > 32) {
                m_state = 4;
                m_state_cycle = 0;
            }
            break;
        case 3:  // shooting:
            if (m_walking_channel != -1) {
                Mix_HaltChannel(m_walking_channel);
                m_walking_channel = -1;
            }
            if (m_state_cycle == 32) {
                // shoot:
                List<int> l;
                l.Add(new int(m_facing));
                if (m_facing == 0)
                    map->add_object(GObject_create(bone_symbol, (int)(m_x - 20), (int)(m_y)+20, m_sfx_volume, &l), 2);
                else
                    map->add_object(GObject_create(bone_symbol, (int)(m_x + 20), (int)(m_y)+20, m_sfx_volume, &l), 2);
                SFXM->SFX_play("sfx/skeleton_attack", m_sfx_volume, get_angle(map), get_distance(map));
            }

            if (m_state_cycle > 64) {
                m_time_since_last_bone = 0;
                m_state = 1;
                m_state_cycle = 0;
            }
            break;
        case 4:  // pause before reapearing:
            if (m_state_cycle > 500) {
                m_state = 0;
                m_state_cycle = 0;
				SFXM->SFX_play("sfx/skeleton_appear", m_sfx_volume, get_angle(map), get_distance(map));
            }
            break;
        case 5:  // standing and looking to both sides:
            if (map->collision_with_background(this, 0, 0, GLTM))
                m_y--;
            if (m_state_cycle > 50) {
                m_state = 1;
                m_state_cycle = 0;
            }
            break;
    }

    m_state_cycle++;
    m_time_since_last_bone++;

	if (m_facing != m_previous_facing)
        m_turning_counter = 6;

    if (m_turning_counter > 0)
        m_turning_counter--;

    return true;
}

void GO_skeleton::draw(GLTManager *GLTM)
{
    int s = (m_state_cycle / 8) % 4;
	int s2 = m_state_cycle / 8;

    switch (m_state) {
        case 0:
			if (s2 == 0) m_last_tile_used = GLTM->get("ob_skeleton-appear1");
			if (s2 == 1) m_last_tile_used = GLTM->get("ob_skeleton-appear2");
			if (s2 == 2) m_last_tile_used = GLTM->get("ob_skeleton-appear3");
			if (s2 == 3) m_last_tile_used = GLTM->get("ob_skeleton-appear4");
			if (s2 == 4) m_last_tile_used = GLTM->get("ob_skeleton-appear5");
			if (s2 == 5) m_last_tile_used = GLTM->get("ob_skeleton-appear6");
			if (s2 == 6) m_last_tile_used = GLTM->get("ob_skeleton-appear7");
			if (s2 >= 7) m_last_tile_used = GLTM->get("ob_skeleton-turn");
            break;
        case 1:
			if (m_turning_counter) {
				m_last_tile_used = GLTM->get
					("ob_skeleton-turn");
			} else {
				if (m_facing == 0) {
					if (s == 0)
						m_last_tile_used = GLTM->get
							("ob_skeleton-l1");
					if (s == 1)
						m_last_tile_used = GLTM->get
							("ob_skeleton-l2");
					if (s == 2)
						m_last_tile_used = GLTM->get
							("ob_skeleton-l3");
					if (s == 3)
						m_last_tile_used = GLTM->get
							("ob_skeleton-l2");
				} else {
					if (s == 0)
						m_last_tile_used = GLTM->get
							("ob_skeleton-r1");
					if (s == 1)
						m_last_tile_used = GLTM->get
							("ob_skeleton-r2");
					if (s == 2)
						m_last_tile_used = GLTM->get
							("ob_skeleton-r3");
					if (s == 3)
						m_last_tile_used = GLTM->get
							("ob_skeleton-r2");
				}
			}
            break;
        case 2:
            if (s2 == 0) m_last_tile_used = GLTM->get("ob_skeleton-turn");
            if (s2 == 1) m_last_tile_used = GLTM->get("ob_skeleton-death_1");
            if (s2 == 2) m_last_tile_used = GLTM->get("ob_skeleton-death_2");
            if (s2 == 3) m_last_tile_used = GLTM->get("ob_skeleton-death_3");
            if (s2 == 4) m_last_tile_used = GLTM->get("ob_skeleton-death_4");
            break;
        case 3:
            if (m_facing == 0) {
                m_last_tile_used = GLTM->get
                                   ("ob_skeleton-l2");
            } else {
                m_last_tile_used = GLTM->get
                                   ("ob_skeleton-r2");
            }
            break;
        case 4:
            m_last_tile_used = 0;
            break;
        case 5:
			{
				int sp = (m_state_cycle / 12) % 4;
				if (sp == 0)
					m_last_tile_used = GLTM->get
						("ob_skeleton-turn-l");
				if (sp == 1)
					m_last_tile_used = GLTM->get
						("ob_skeleton-turn");
				if (sp == 2)
					m_last_tile_used = GLTM->get
						("ob_skeleton-turn-r");
				if (sp == 3)
					m_last_tile_used = GLTM->get
						("ob_skeleton-turn");
			}
            break;
    }
    if (m_last_tile_used != 0)
        m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
}

bool GO_skeleton::is_a(Symbol *c)
{
    if (c->cmp(skeleton_symbol))
        return true;

    return GO_enemy::is_a(c);
}

bool GO_skeleton::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
}

bool GO_skeleton::player_hit(int *experience, int *score)
{
    if (m_state != 0 && m_state != 2 && m_state != 4) {
        m_state = 2;
        m_state_cycle = 0;
        *experience = 1;
        *score = 100;
        return true;
    }
    return false;
}

int GO_skeleton::enemy_hit(void)
{
    if (m_state != 0 && m_state != 2 && m_state != 4)
        return 11;
    return 0;
}

