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
#include "GO_bat.h"
#include "GO_character.h"

#include "GMap.h"

#include "TheGooniesCtnt.h"
#include "GObjectCreator.h"

extern int difficulty;


GO_bat::GO_bat(int x, int y, int sfx_volume) : GO_enemy(x, y, sfx_volume)
{
    m_class = new Symbol(bat_symbol);

    m_cave_x = x;
    m_cave_y = y;
    m_original_y = y;
    m_state = 0;
    m_state_cycle = 0;

	m_flying_channel=-1;

}


bool GO_bat::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    m_state_cycle++;

    // continually update our mixing based on player position
    if (m_flying_channel != 1) {
        Mix_SetPosition(m_flying_channel, get_angle(map), get_distance(map));
    }

    switch (m_state) {

        case 0: {
                GO_character *c = (GO_character *)map->get_object(character_symbol);
                m_x = float(m_cave_x);
                m_y = float(m_cave_y);
                m_original_y = m_cave_y;
                if (c != 0) {
                    if (c->get_x() >= m_x + 120 && c->get_x() < m_x + 180 &&
                            c->get_y() >= m_y - 40 && c->get_y() < m_y + 40) {
                        m_state = 1;
                        m_state_cycle = 0;
                    }
                    if (c->get_x() >= m_x - 180 && c->get_x() < m_x - 120 &&
                            c->get_y() >= m_y - 40 && c->get_y() < m_y + 40) {
                        m_state = 1;
                        m_state_cycle = 0;
                    }

                }
            }
            break;

        case 1:  // initial pause

            if (m_flying_channel == -1) {
               m_flying_channel = SFXM->SFX_play_continuous("sfx/bat_fly", m_sfx_volume, get_angle(map), get_distance(map),this);
            }
            if (m_state_cycle >= 128) {
                m_state_cycle = 0;
                m_original_y -= 2;
                GObject *o = map->get_object(character_symbol);
                if (o != 0) {
                    if (o->get_x() < m_x)
                        m_state = 2;
                    else
                        m_state = 3;
                } else {
                    m_state = 2;
                }
            }
            break;
        case 2:  // left
            {
                float s = float(sin(m_state_cycle * 0.06));
                m_y = (m_original_y) - 20 * s;
            }
            m_x -= BAT_SPEED - (100 / 150 - difficulty / 150);

            if (m_x < 0 ||
                    map->collision_with_background_wo_bridges(this, -2, (int)(m_original_y - m_y), GLTM))
                m_state = 3;

            if (m_state_cycle >= 512 &&
                    !map->collision_with_background_wo_bridges(this, 0, -12, GLTM) &&
                    !map->collision_with_background_wo_bridges(this, 0, + 12, GLTM)) {
                m_state_cycle = 0;
                m_original_y = (int)m_y;
                GObject *o = map->get_object(character_symbol);
                if (o != 0) {
                    if (o->get_y() < m_y)
                        m_state = 4;
                    else
                        m_state = 5;
                }
            }
			if (map->collision_with_object(this, GLTM, bigrock_symbol)) {
				m_state = 6;
				m_state_cycle = 0;
			}			
				
            break;
        case 3:  // right
            {
                float s = float(sin(m_state_cycle * 0.06));
                m_y = (m_original_y) - 20 * s;
            }
            m_x += BAT_SPEED - (100 / 150 - difficulty / 150);

            if (m_x + 16 > map->get_dx() ||
                    map->collision_with_background_wo_bridges(this, 2, (int)(m_original_y - m_y), GLTM))
                m_state = 2;

            if (m_state_cycle >= 512 &&
                    !map->collision_with_background_wo_bridges(this, 0, -12, GLTM) &&
                    !map->collision_with_background_wo_bridges(this, 0, + 12, GLTM)) {
                m_state_cycle = 0;
                m_original_y = (int)m_y;
                GObject *o = map->get_object(character_symbol);
                if (o != 0) {
                    if (o->get_y() < m_y)
                        m_state = 4;
                    else
                        m_state = 5;
                }
            }
			if (map->collision_with_object(this, GLTM, bigrock_symbol)) {
				m_state = 6;
				m_state_cycle = 0;
			}			
			break;
        case 4:  // up
            m_y -= 1;
            m_original_y -= 1;
            if (m_state_cycle >= 64 ||
                    map->collision_with_background_wo_bridges(this, 0, -12, GLTM)) {
                GObject *o = map->get_object(character_symbol);
                m_state_cycle = 0;
                if (o != 0) {
                    if (o->get_x() < m_x)
                        m_state = 2;
                    else
                        m_state = 3;
                } else {
                    m_state = 2;
                }
            }
			if (map->collision_with_object(this, GLTM, bigrock_symbol)) {
				m_state = 6;
				m_state_cycle = 0;
			}			
			break;
        case 5:  // down
            m_y += 1;
            m_original_y += 1;
            if (m_state_cycle >= 64 ||
                    map->collision_with_background_wo_bridges(this, 0, 12, GLTM)) {
                GObject *o = map->get_object(character_symbol);
                m_state_cycle = 0;
                if (o != 0) {
                    if (o->get_x() < m_x)
                        m_state = 2;
                    else
                        m_state = 3;
                } else {
                    m_state = 2;
                }
            }
			if (map->collision_with_object(this, GLTM, bigrock_symbol)) {
				m_state = 6;
				m_state_cycle = 0;
			}			
			break;
        case 6:  // dying
            if (m_flying_channel != -1) {
				SFXM->SFX_stop(m_flying_channel);
                m_flying_channel = -1;
            }
            if (m_state_cycle > 32) {
                m_state = 0;
                m_state_cycle = 0;
            }
            break;
    }
    return true;
}

void GO_bat::draw(GLTManager *GLTM)
{
    if (m_state == 0) {
        m_last_tile_used = 0;
        return ;
    }
    if (m_state == 6) {
		int s = (m_state_cycle / 4) % 5;
		if (s == 0) m_last_tile_used = GLTM->get("ob_bat-death_1");
		if (s == 1) m_last_tile_used = GLTM->get("ob_bat-death_2");
		if (s == 2) m_last_tile_used = GLTM->get("ob_bat-death_3");
		if (s == 3) m_last_tile_used = GLTM->get("ob_bat-death_4");
		if (s == 4) m_last_tile_used = GLTM->get("ob_bat-death_5");
    } else {
        int s = (m_state_cycle / 8) % 2;

        if (s == 0)
            m_last_tile_used = GLTM->get
                               ("ob_bat1");
        else
            m_last_tile_used = GLTM->get
                               ("ob_bat2");
    }
    m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
}

bool GO_bat::is_a(Symbol *c)
{
    if (c->cmp(m_class))
        return true;

    return GO_enemy::is_a(c);
}

bool GO_bat::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
}

bool GO_bat::player_hit(int *experience, int *score)
{
    if (m_state != 6) {
        m_state = 6;
        m_state_cycle = 0;
        *experience = 1;
        *score = 100;
        return true;
    }
    return false;
}

int GO_bat::enemy_hit(void)
{
    if (m_state != 6 && m_state != 0)
        return 11;
    return 0;
}
