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
#include "GMap.h"

#include "GO_wateropening.h"
#include "GO_fallingwater.h"

#include "GObjectCreator.h"


GO_wateropening::GO_wateropening(int x, int y, int sfx_volume, int color) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(wateropening_symbol);

    m_color = color;

    // random waterfall sfx
    if (rand() % 2 == 0) {
        watersfx = "sfx/waterfall";
    } else {
        watersfx = "sfx/waterfall2";
    }
    m_water_channel = -1;
	m_water_volume = 0;
}



bool GO_wateropening::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    m_state_cycle++;

    if (m_state_cycle > 150) {
        int i;

        // continually update our mixing based on player position
        if (m_water_channel != -1) {
			if (m_water_volume <= m_sfx_volume) {
				m_water_volume += 2;
			}
            Mix_SetPosition(m_water_channel, get_angle(map), get_distance(map));
			Mix_Volume(m_water_channel, m_water_volume);
        }

        if (m_water_channel == -1) {
			m_water_channel = SFXM->SFX_play_continuous(watersfx, m_water_volume, get_angle(map), get_distance(map),this);
        }

        for (i = 0;i < 2;i++) {
            int x1 = 0, x2 = 0;
            int dy = (rand() % 8) * 8;
            if (dy > m_state_cycle - 150)
                dy = m_state_cycle - 150;
            while (x1 == x2) {
                x1 = rand() % 36;
                x2 = rand() % 36;
                if (x2 < x1) {
                    int tmp = x1;
                    x1 = x2;
                    x2 = tmp;
                }
            }
            if (x1 > 0)
                x1 -= 2;
            if (x2 < 7)
                x2 += 2;
            GO_fallingwater *o = new GO_fallingwater((int)(m_x + 8 + x1), (int)(m_y + 16), (x2 - x1), 8 + dy, m_sfx_volume);
            o->set_map(map);
            map->add_object(o, 4);
        }
    } else {
        if (map->get_object(fallingwater_symbol) == 0) {			
			if (m_water_channel != -1) {
				if (m_water_volume >= 0) {
					Mix_Volume(m_water_channel, m_water_volume);
					m_water_volume -= 2;
				} else {
					SFXM->SFX_stop(m_water_channel);
					m_water_channel = -1;
					m_water_volume = 0;
				}
			}
        }
    }
    if (m_state_cycle > 300) {
        m_state_cycle = rand() % 10 * 5;
	}
    return true;
}

void GO_wateropening::draw(GLTManager *GLTM)
{
	if (m_last_tile_used == 0) {
		if (m_color == 0)
			m_last_tile_used = GLTM->get
							   ("ob_water-opening");
		if (m_color == 1)
			m_last_tile_used = GLTM->get
							   ("ob_water-opening-blue");
		if (m_color == 2)
			m_last_tile_used = GLTM->get
							   ("ob_water-opening-green");
		if (m_color == 3)
			m_last_tile_used = GLTM->get
							   ("ob_water-opening-yellow");
	} // if 

    m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
}

bool GO_wateropening::is_a(Symbol *c)
{
    if (c->cmp(wateropening_symbol))
        return true;

    return GObject::is_a(c);
}

bool GO_wateropening::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
}

