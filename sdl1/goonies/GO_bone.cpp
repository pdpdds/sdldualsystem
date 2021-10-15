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
#include "GO_bone.h"

#include "GMap.h"

#include "TheGooniesCtnt.h"
#include "GObjectCreator.h"

extern int difficulty;

GO_bone::GO_bone(int x, int y, int sfx_volume, int facing) : GO_enemy(x, y, sfx_volume)
{
    m_class = new Symbol(bone_symbol);

    m_state = 0;
    m_state_cycle = 0;

    m_facing = facing;
}

bool GO_bone::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    if (m_state == 0) {
        if (m_facing == 0)
            m_x -= BONE_SPEED - (100 / 150 - difficulty / 150);
        else
            m_x += BONE_SPEED - (100 / 150 - difficulty / 150);

		if (m_x < 0 || m_x > map->get_dx())
			return false;
		if (map->collision_with_background(this, GLTM)) {
			m_state = 1;
			m_state_cycle = 0;
			return true;
	}
    }
    m_state_cycle++;

    if (m_state == 1 && m_state_cycle > 32)
        m_state = 2;
    if (m_state == 2)
        return false;
    return true;
}

void GO_bone::draw(GLTManager *GLTM)
{
    if (m_state == 0) {
		int s = (m_state_cycle / 8) % 4;
		if (m_facing == 0) {
			switch (s) {
				case 0:
					m_last_tile_used = GLTM->get ("ob_bone1");
					break;
				case 1:
					m_last_tile_used = GLTM->get ("ob_bone2");
					break;
				case 2:
					m_last_tile_used = GLTM->get ("ob_bone3");
					break;
				case 3:
					m_last_tile_used = GLTM->get ("ob_bone4");
					break;
			}
		} else {
			switch (s) {
				case 0:
					m_last_tile_used = GLTM->get ("ob_bone4");
					break;
				case 1:
					m_last_tile_used = GLTM->get ("ob_bone3");
					break;
				case 2:
					m_last_tile_used = GLTM->get ("ob_bone2");
					break;
				case 3:
					m_last_tile_used = GLTM->get ("ob_bone1");
					break;
			}
		}
    } else {
		int s = (m_state_cycle / 5);
        if (s == 0) m_last_tile_used = GLTM->get ("ob_bone-crash1");
        if (s == 1) m_last_tile_used = GLTM->get ("ob_bone-crash2");
        if (s == 2) m_last_tile_used = GLTM->get ("ob_bone-crash3");
		if (s >= 3) m_last_tile_used = 0;
    }

	if (m_last_tile_used != 0)
		m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
}

bool GO_bone::is_a(Symbol *c)
{
    if (c->cmp(bone_symbol))
        return true;

    return GO_enemy::is_a(c);
}

bool GO_bone::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
}

bool GO_bone::player_hit(int *experience, int *score)
{
    if (m_state == 0) {
        m_state = 1;
        m_state_cycle = 0;
        *experience = 1;
		*score = 100;
        return true;
    }
    return false;
}

int GO_bone::enemy_hit(void)
{
    if (m_state == 0) {
        m_state = 2;
        m_state_cycle = 0;
        return 7;
    } 
    return 0;
}
