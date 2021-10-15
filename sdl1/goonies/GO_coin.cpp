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
#include "MusicManager.h"
#include "GObject.h"
#include "GO_enemy.h"
#include "GO_coin.h"

#include "GMap.h"
#include "TheGoonies.h"

#include "TheGooniesCtnt.h"
#include "GObjectCreator.h"
#include "GO_bridge.h"


GO_coin::GO_coin(int x, int y, int sfx_volume) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(coin_symbol);

    m_facing = 0;
    m_state = 0;
    m_state_cycle = 0;
} /* GO_coin::GO_coin */


bool GO_coin::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    if (m_state_cycle == 0 && SFXM != 0) {
        SFXM->SFX_play("sfx/doubloon", m_sfx_volume, get_angle(map), get_distance(map));
    }

    switch (m_state) {
        case 0:  // moving
            if (map->collision_with_background(this, GLTM))
                m_y--;

            if (m_facing == 0) {
                m_x -= COIN_SPEED;
                if (map->collision_with_background(this, -2, 0, GLTM))
                    m_facing = 1;
            } else {
                m_x += COIN_SPEED;
                if (map->collision_with_background(this, 2, 0, GLTM))
                    m_facing = 0;
            } // if
			if (!map->collision_with_background(this, 0, 2, GLTM) &&
					!map->collision_with_object(this, 0, 2, GLTM, bridge_symbol)) {
				m_state = 1;
				m_state_cycle = 0;
			} // if
            m_state_cycle++;
            break;
        case 1:  // falling
            {
                int i, j = 1;
                if (m_state_cycle > 4)
                    j++;
                if (m_state_cycle > 12)
                    j++;
                if (m_state_cycle > 20)
                    j++;

                for (i = 0;i < j;i++)
                {
                    m_y++;
					if (map->collision_with_background(this, GLTM) ||
							map->collision_with_object(this, GLTM, bridge_symbol)) {
						m_state = 0;
						m_state_cycle = 0;
						m_y--;
					} // if
				} // for
                m_state_cycle++;
            }
            break;
        case 2:  // disappearing
            game->delete_multiroom_object(this);
            break;
    } // switch
    m_state_cycle++;
    return true;
} /* GO_coin::cycle */


void GO_coin::draw(GLTManager *GLTM)
{
    int s = (m_state_cycle / 8) % 4;

	if (m_facing == 0) {
		if (s == 0) m_last_tile_used = GLTM->get ("ob_coin1");
		if (s == 1) m_last_tile_used = GLTM->get ("ob_coin2");
		if (s == 2) m_last_tile_used = GLTM->get ("ob_coin3");
		if (s == 3) m_last_tile_used = GLTM->get ("ob_coin4");
	}
	else {
		if (s == 0) m_last_tile_used = GLTM->get ("ob_coin1");
		if (s == 1) m_last_tile_used = GLTM->get ("ob_coin4");
		if (s == 2) m_last_tile_used = GLTM->get ("ob_coin3");
		if (s == 3) m_last_tile_used = GLTM->get ("ob_coin2");
	}

    if (m_last_tile_used != 0)
        m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
} /* GO_coin::draw */


bool GO_coin::is_a(Symbol *c)
{
    if (c->cmp(coin_symbol))
        return true;

    return GObject::is_a(c);
} /* GO_coin::is_a */


bool GO_coin::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
} /* GO_coin::is_a */


