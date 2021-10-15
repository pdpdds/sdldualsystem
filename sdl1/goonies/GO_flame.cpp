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
#include "GO_character.h"
#include "GO_flame.h"

#include "GMap.h"

#include "TheGooniesCtnt.h"
#include "GObjectCreator.h"


GO_flame::GO_flame(int x, int y, int sfx_volume) : GO_enemy(x, y, sfx_volume)
{
    m_class = new Symbol(flame_symbol);

    m_state = 0;
    m_state_cycle = 0;
    m_flame_channel = -1;
}


bool GO_flame::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    // continually update our mixing based on player position
    if (m_flame_channel != 1) {
        Mix_SetPosition(m_flame_channel, get_angle(map), get_distance(map));
    }

    switch (m_state) {
        case 0:  // waiting for the player
            {
                GO_character *c = (GO_character *)map->get_object(character_symbol);
                if (c != 0)
                {
                    if (c->get_x() >= m_x - 60 && c->get_x() < m_x + 80 &&
                            c->get_y() >= m_y && c->get_y() < m_y + 100) {
                        m_state = 1;
                        m_state_cycle = 0;
                        if (m_flame_channel == -1) {
							m_flame_channel = SFXM->SFX_play_continuous("sfx/flame", m_sfx_volume, get_angle(map), get_distance(map),this);
                        }
                    }
                }
            }
            break;
        case 1:  // flaming
            if (m_state_cycle > 128) {
                m_state = 2;
                m_state_cycle = 0;
                if (m_flame_channel != -1) {
                    Mix_HaltChannel(m_flame_channel);
                    m_flame_channel = -1;
                }
            }
            break;
        case 2:  // waiting for the player to be far
            {
                GO_character *c = (GO_character *)map->get_object(character_symbol);
                if (c != 0)
                {
                    if (c->get_x() >= m_x - 60 && c->get_x() < m_x + 80 &&
                        c->get_y() >= m_y && c->get_y() < m_y + 100) {}
                    else {
                        m_state = 0;
                    }
                }
            }
            break;
    }
    m_state_cycle++;
    return true;
}

void GO_flame::draw(GLTManager *GLTM)
{
    int s = (m_state_cycle / 4) % 2;

    switch (m_state) {
        case 0:
            m_last_tile_used = 0;
            break;
        case 1:
            if (m_state_cycle < 8) {
                m_last_tile_used = GLTM->get
                                   ("ob_flame1");
            } else {
                if (s == 0)
                    m_last_tile_used = GLTM->get
                                       ("ob_flame2");
                else
                    m_last_tile_used = GLTM->get
                                       ("ob_flame3");
            }
            break;
        case 2:
            m_last_tile_used = 0;
            break;
    }

    if (m_last_tile_used != 0)
        m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
}

bool GO_flame::is_a(Symbol *c)
{
    if (c->cmp(flame_symbol))
        return true;

    return GO_enemy::is_a(c);
}

bool GO_flame::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
}

int GO_flame::enemy_hit(void)
{
    if (m_state == 1)
        return 1;
    return 0;
}
