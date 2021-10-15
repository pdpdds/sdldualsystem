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
#include "GO_skull.h"

#include "GMap.h"

#include "TheGooniesCtnt.h"
#include "GObjectCreator.h"

extern int difficulty;

GO_skull::GO_skull(int x, int y, int sfx_volume, int facing) : GO_enemy(x, y, sfx_volume)
{
    m_class = new Symbol(skull_symbol);

    m_facing = facing;
    m_state = 0;
    m_state_cycle = 0;
	m_turning_counter = 0;
}

bool GO_skull::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    m_previous_facing = m_facing;

    switch (m_state) {
        case 0:  // initial pause
            if (m_state_cycle > 5) {
                m_state = 1;
                m_state_cycle = 0;
                SFXM->SFX_play("sfx/enemy_appear", m_sfx_volume, get_angle(map), get_distance(map));
            }
            break;
        case 1:  // appearing
            if (m_state_cycle > 64) {
                m_state = 2;
                m_state_cycle = 0;
            }
            break;
        case 2:  // moving
            if (m_facing == 0) {
                m_x -= SKULL_SPEED - (100 / 150 - difficulty / 150);
                if (m_x < 0 ||
                        map->collision_with_background_wo_bridges_bigrocks(this, -2, 0, GLTM) ||
                        (map->collision_with_background_wo_bridges_bigrocks(this, -18, 24, GLTM) &&
                         !map->collision_with_background_wo_bridges_bigrocks(this, -20, 24, GLTM)))
                    m_facing = 1;
            } else {
                m_x += SKULL_SPEED - (100 / 150 - difficulty / 150);

                if (m_x + 16 > map->get_dx() ||
                        map->collision_with_background_wo_bridges_bigrocks(this, 2, 0, GLTM) ||
                        (map->collision_with_background_wo_bridges_bigrocks(this, 18, 24, GLTM) &&
                         !map->collision_with_background_wo_bridges_bigrocks(this, 20, 24, GLTM)))
                    m_facing = 0;
            }
            break;
        case 3:  // dying
            if (m_state_cycle == 0) {
                SFXM->SFX_play("sfx/skull_dead", m_sfx_volume);
            }
            if (m_state_cycle > 32)
                return false;
            break;
    }
    m_state_cycle++;

	if (m_facing != m_previous_facing)
        m_turning_counter = 6;

    if (m_turning_counter > 0)
        m_turning_counter--;

    return true;
}

void GO_skull::draw(GLTManager *GLTM)
{
    int s = (m_state_cycle / 8) % 2;
	int s2 = (m_state_cycle / 3);
	int s3 = (m_state_cycle / 4);
	GLTile *glow_tile = 0;

    switch (m_state) {
        case 0:
			glow_tile = 0;
            m_last_tile_used = 0;
            break;
        case 1:
			glow_tile = 0;
            if (s3 == 0) m_last_tile_used = GLTM->get ("ob_skull-appear_1");
            if (s3 == 1) m_last_tile_used = GLTM->get ("ob_skull-appear_2");
            if (s3 == 2) m_last_tile_used = GLTM->get ("ob_skull-appear_3");
            if (s3 == 3) m_last_tile_used = GLTM->get ("ob_skull-appear_4");
            if (s3 == 4) m_last_tile_used = GLTM->get ("ob_skull-appear_5");
            if (s3 == 5) m_last_tile_used = GLTM->get ("ob_skull-appear_6");
            if (s3 == 6) m_last_tile_used = GLTM->get ("ob_skull-appear_7");
            if (s3 == 7) m_last_tile_used = GLTM->get ("ob_skull-appear_8");
            if (s3 == 8) m_last_tile_used = GLTM->get ("ob_skull-appear_9");
            if (s3 == 9) m_last_tile_used = GLTM->get ("ob_skull-appear_10");
            if (s3 == 10) m_last_tile_used = GLTM->get ("ob_skull-appear_11");
            if (s3 == 11) m_last_tile_used = GLTM->get ("ob_skull-appear_12");
            if (s3 == 12) m_last_tile_used = GLTM->get ("ob_skull-appear_13");
            if (s3 == 13) m_last_tile_used = GLTM->get ("ob_skull-appear_14");
            if (s3 == 14) m_last_tile_used = GLTM->get ("ob_skull-appear_15");
            if (s3 >= 15) m_last_tile_used = GLTM->get ("ob_skull-turn");
            break;
        case 2:
			if (m_turning_counter) {
				m_last_tile_used = GLTM->get
					("ob_skull-turn");
			} else {
				if (m_facing == 0) {
					glow_tile = GLTM->get
						("ob_skull_glow-l1");
					if (s == 0)
						m_last_tile_used = GLTM->get
							("ob_skull-l1");
					else
						m_last_tile_used = GLTM->get
							("ob_skull-l2");

				} else {
					glow_tile = GLTM->get
						("ob_skull_glow-r1");
					if (s == 0)
						m_last_tile_used = GLTM->get
							("ob_skull-r1");
					else
						m_last_tile_used = GLTM->get
							("ob_skull-r2");
				}
			}
            break;
        case 3:
			glow_tile = 0;
			if (s2 == 0)
				m_last_tile_used = GLTM->get("ob_skull-death_1");
			if (s2 == 1)
				m_last_tile_used = GLTM->get("ob_skull-death_2");
			if (s2 == 2)
				m_last_tile_used = GLTM->get("ob_skull-death_3");
			if (s2 == 3)
				m_last_tile_used = GLTM->get("ob_skull-death_4");
			if (s2 == 4)
				m_last_tile_used = GLTM->get("ob_skull-death_5");
			if (s2 == 5)
				m_last_tile_used = 0;
			break;
    }

    if (m_last_tile_used != 0)
        m_last_tile_used->draw(m_x, m_y, 0, 0, 1);

	if (glow_tile != 0) {
		float glow_alpha = (float)(m_state_cycle % 51) / 25.0f;
		if (glow_alpha > 1) glow_alpha = 2.0f - glow_alpha;
		glow_tile->draw(1, 1, 1, glow_alpha, m_x, m_y, 0, 0, 1);
	}
}

bool GO_skull::is_a(Symbol *c)
{
    if (c->cmp(skull_symbol))
        return true;

    return GO_enemy::is_a(c);
}

bool GO_skull::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
}

bool GO_skull::player_hit(int *experience, int *score)
{
    if (m_state == 2) {
        m_state = 3;
        m_state_cycle = 0;
        *experience = 1;
        *score = 50;
        return true;
    } 
    return false;
}

int GO_skull::enemy_hit(void)
{
    if (m_state == 2)
        return 8;
    return 0;
}
