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
#include "GO_bigrock.h"

#include "TheGooniesCtnt.h"
#include "GObjectCreator.h"

#include "GMap.h"

extern bool screen_shake;

GO_bigrock::GO_bigrock(int x, int y, int sfx_volume, int color) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(bigrock_symbol);

    m_color = color;
    m_original_y = y;
    m_chain_channel = -1;
    m_top_y = m_middle_y = -1;

    m_state_cycle = ((rand() % 4) + 1) * 16;

    m_n_fall = 0;
	m_chain = 0;
}


bool GO_bigrock::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    // detect whether is normal big rock or a two heights big rock:
    if (m_top_y == -1) {
		if (m_last_tile_used==0) {
			if (m_color == 0) m_last_tile_used = GLTM->get ("ob_rock");
			if (m_color == 1) m_last_tile_used = GLTM->get ("ob_rock_blue");
			if (m_color == 2) m_last_tile_used = GLTM->get ("ob_rock_green");
			if (m_color == 3) m_last_tile_used = GLTM->get ("ob_rock_yellow");
		} // if

        if (map->collision_with_background(this, 0, -20, GLTM)) {
            // normal big rock:
            m_top_y = m_original_y;
        } else {
            // two heights big rock:
            int offs = 0;
            while (!map->collision_with_background(this, 0, offs - 20, GLTM) && m_original_y + offs > -40) {
                offs -= 20;
            }
            m_top_y = m_original_y + offs;
            m_y = float(m_top_y);
            m_middle_y = m_original_y;
        }
    }

    switch (m_state) {
        case 0:  // rock up
            m_top_y = (int)m_y;
            m_state_cycle++;
            if (m_state_cycle > ((rand() % 4) + 4) * 16) {
                m_state = 1;
                m_state_cycle = 0;
                m_n_fall++;
            }
            break;
        case 1:  // rock falling
            // continually update our mixing based on player position
            if (m_chain_channel != 1) {
                Mix_SetPosition(m_chain_channel, get_angle(map), get_distance(map));
            }

            if (m_chain_channel == -1) {
                m_chain_channel = SFXM->SFX_play_continuous("sfx/bigrock_winch", m_sfx_volume, get_angle(map), get_distance(map),this);
            }
            m_y += 4 * BIGROCK_SPEED;
            if (m_middle_y == -1 || (m_n_fall % 4) == 3) {
                // normal fall
                if (map->collision_with_background(this, 0, 4, GLTM)) {
                    if (m_chain_channel != -1) {
                        Mix_HaltChannel(m_chain_channel);
                        m_chain_channel = -1;
                    }
                    SFXM->SFX_play("sfx/bigrock_fall", m_sfx_volume, get_angle(map), get_distance(map));
					if (screen_shake) {
						map->set_shake();
					}
                    m_state = 2;
                    m_state_cycle = 0;
                }
            } else {
                // middle height fall
                if (m_y >= m_middle_y) {
                    if (m_chain_channel != -1) {
                        Mix_HaltChannel(m_chain_channel);
                        m_chain_channel = -1;
                    }
                    SFXM->SFX_play("sfx/bigrock_fall", m_sfx_volume, get_angle(map), get_distance(map));
                    m_state = 2;
                    m_state_cycle = 0;
                }
            }
            break;
        case 2:  // rock down
            m_state_cycle++;
            if (m_state_cycle > 64) {
                m_state = 3;
                m_state_cycle = 0;
            }
            break;
        case 3:  // rock rising
            // continually update our mixing based on player position
            if (m_chain_channel != 1) {
                Mix_SetPosition(m_chain_channel, get_angle(map), get_distance(map));
            }

            if (m_chain_channel == -1) {
                m_chain_channel = SFXM->SFX_play_continuous("sfx/bigrock_winch", m_sfx_volume, get_angle(map), get_distance(map),this);
            }
            m_y -= (1 * BIGROCK_SPEED);
            if (m_y <= m_top_y) {
                if (m_chain_channel != -1) {
                    Mix_HaltChannel(m_chain_channel);
                    m_chain_channel = -1;
                }
                m_y = float(m_top_y);
                m_state = 0;
                m_state_cycle = (rand() % 4) * 16;
            }
            break;
    }
    return true;
}

void GO_bigrock::draw(GLTManager *GLTM)
{
    int i;
    if (m_top_y != -1) {
        for (i = m_top_y - 20;i < m_y;i += 20) {
			if (m_chain==0) m_chain = GLTM->get("ob_chain");
			m_chain->draw(m_x, float(i), 0, 0, 1);
        }
    }
	if (m_last_tile_used==0) {
		if (m_color == 0)
			m_last_tile_used = GLTM->get
							   ("ob_rock");
		if (m_color == 1)
			m_last_tile_used = GLTM->get
							   ("ob_rock_blue");
		if (m_color == 2)
			m_last_tile_used = GLTM->get
							   ("ob_rock_green");
		if (m_color == 3)
			m_last_tile_used = GLTM->get
							   ("ob_rock_yellow");
	} // if
    m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
}

bool GO_bigrock::is_a(Symbol *c)
{
    if (c->cmp(bigrock_symbol))
        return true;

    return GObject::is_a(c);
}

bool GO_bigrock::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
}

