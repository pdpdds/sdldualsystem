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
#include "GMap.h"

#include "GO_enemy.h"
#include "GO_drop.h"
#include "GO_closingwall.h"
#include "GO_character.h"

#include "GObjectCreator.h"

GO_closingwall::GO_closingwall(int x, int y, int sfx_volume, int color) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(closingwall_symbol);

    m_state = 0;
    m_state_cycle = 0;
    m_color = color;
} /* GObject::GObject */


bool GO_closingwall::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    GO_character *c = (GO_character *)map->get_object(character_symbol);

    switch (m_state) {
        case 0:
            if (c != 0) {
                if (c->get_x() > m_x - 40 && c->get_x() < m_x + 40 &&
                        c->get_y() > m_y && c->get_y() < m_y + 100) {
                    // close the wall!
                    m_state = 1;
                    m_state_cycle = 0;
                } // if
            } // if
            break;
        case 1:
            m_state_cycle++;
            switch (m_color) {
                case 0:
                    if (m_state_cycle == 20) {
                        map->get_layer(3)->add_tile("wl_tile-wall-r1", int(m_x), int(m_y));
                        map->get_layer(3)->add_tile("wl_tile-wall-l1", int(m_x + 20), int(m_y));
                    } // if
                    if (m_state_cycle == 40) {
                        map->get_layer(3)->add_tile("wl_tile-wall-r1", int(m_x), int(m_y + 20));
                        map->get_layer(3)->add_tile("wl_tile-wall-l1", int(m_x + 20), int(m_y + 20));
                    } // if
                    if (m_state_cycle == 60) {
                        map->get_layer(3)->add_tile("wl_tile-wall-r1", int(m_x), int(m_y + 40));
                        map->get_layer(3)->add_tile("wl_tile-wall-l1", int(m_x + 20), int(m_y + 40));
                    } // if
                    if (m_state_cycle == 80) {
                        map->get_layer(3)->add_tile("wl_tile-wall-r1", int(m_x), int(m_y + 60));
                        map->get_layer(3)->add_tile("wl_tile-wall-l1", int(m_x + 20), int(m_y + 60));
                    } // if
                    if (m_state_cycle == 100) {
                        map->get_layer(3)->add_tile("wl_tile-ground-wall-r", int(m_x), int(m_y + 80));
                        map->get_layer(3)->add_tile("wl_tile-ground-wall-l", int(m_x + 20), int(m_y + 80));
                    } // if
                    if (m_state_cycle % 20 == 0) {
                        SFXM->SFX_play("sfx/closingwall", m_sfx_volume, get_angle(map), get_distance(map));
                    }
                    break;
                case 1:
                    if (m_state_cycle == 20) {
                        map->get_layer(3)->add_tile("wl_tileblue-wall-r1", int(m_x), int(m_y));
                        map->get_layer(3)->add_tile("wl_tileblue-wall-l1", int(m_x + 20), int(m_y));
                    } // if
                    if (m_state_cycle == 40) {
                        map->get_layer(3)->add_tile("wl_tileblue-wall-r1", int(m_x), int(m_y + 20));
                        map->get_layer(3)->add_tile("wl_tileblue-wall-l1", int(m_x + 20), int(m_y + 20));
                    } // if
                    if (m_state_cycle == 60) {
                        map->get_layer(3)->add_tile("wl_tileblue-wall-r1", int(m_x), int(m_y + 40));
                        map->get_layer(3)->add_tile("wl_tileblue-wall-l1", int(m_x + 20), int(m_y + 40));
                    } // if
                    if (m_state_cycle == 80) {
                        map->get_layer(3)->add_tile("wl_tileblue-wall-r1", int(m_x), int(m_y + 60));
                        map->get_layer(3)->add_tile("wl_tileblue-wall-l1", int(m_x + 20), int(m_y + 60));
                    } // if
                    if (m_state_cycle == 100) {
                        map->get_layer(3)->add_tile("wl_tileblue-ground-wall-r", int(m_x), int(m_y + 80));
                        map->get_layer(3)->add_tile("wl_tileblue-ground-wall-l", int(m_x + 20), int(m_y + 80));
                    } // if
                    if (m_state_cycle % 20 == 0) {
                        SFXM->SFX_play("sfx/closingwall", m_sfx_volume, get_angle(map), get_distance(map));
                    }
                    break;
                case 2:
                    if (m_state_cycle == 20) {
                        map->get_layer(3)->add_tile("wl_tilegreen-wall-r1", int(m_x), int(m_y));
                        map->get_layer(3)->add_tile("wl_tilegreen-wall-l1", int(m_x + 20), int(m_y));
                    } // if
                    if (m_state_cycle == 40) {
                        map->get_layer(3)->add_tile("wl_tilegreen-wall-r1", int(m_x), int(m_y + 20));
                        map->get_layer(3)->add_tile("wl_tilegreen-wall-l1", int(m_x + 20), int(m_y + 20));
                    } // if
                    if (m_state_cycle == 60) {
                        map->get_layer(3)->add_tile("wl_tilegreen-wall-r1", int(m_x), int(m_y + 40));
                        map->get_layer(3)->add_tile("wl_tilegreen-wall-l1", int(m_x + 20), int(m_y + 40));
                    } // if
                    if (m_state_cycle == 80) {
                        map->get_layer(3)->add_tile("wl_tilegreen-wall-r1", int(m_x), int(m_y + 60));
                        map->get_layer(3)->add_tile("wl_tilegreen-wall-l1", int(m_x + 20), int(m_y + 60));
                    } // if
                    if (m_state_cycle == 100) {
                        map->get_layer(3)->add_tile("wl_tilegreen-ground-wall-r", int(m_x), int(m_y + 80));
                        map->get_layer(3)->add_tile("wl_tilegreen-ground-wall-l", int(m_x + 20), int(m_y + 80));
                    } // if
                    if (m_state_cycle % 20 == 0) {
                        SFXM->SFX_play("sfx/closingwall", m_sfx_volume, get_angle(map), get_distance(map));
                    }
                    break;
                case 3:
                    if (m_state_cycle == 20) {
                        map->get_layer(3)->add_tile("wl_tileyellow-wall-r1", int(m_x), int(m_y));
                        map->get_layer(3)->add_tile("wl_tileyellow-wall-l1", int(m_x + 20), int(m_y));
                    } // if
                    if (m_state_cycle == 40) {
                        map->get_layer(3)->add_tile("wl_tileyellow-wall-r1", int(m_x), int(m_y + 20));
                        map->get_layer(3)->add_tile("wl_tileyellow-wall-l1", int(m_x + 20), int(m_y + 20));
                    } // if
                    if (m_state_cycle == 60) {
                        map->get_layer(3)->add_tile("wl_tileyellow-wall-r1", int(m_x), int(m_y + 40));
                        map->get_layer(3)->add_tile("wl_tileyellow-wall-l1", int(m_x + 20), int(m_y + 40));
                    } // if
                    if (m_state_cycle == 80) {
                        map->get_layer(3)->add_tile("wl_tileyellow-wall-r1", int(m_x), int(m_y + 60));
                        map->get_layer(3)->add_tile("wl_tileyellow-wall-l1", int(m_x + 20), int(m_y + 60));
                    } // if
                    if (m_state_cycle == 100) {
                        map->get_layer(3)->add_tile("wl_tileyellow-ground-wall-r", int(m_x), int(m_y + 80));
                        map->get_layer(3)->add_tile("wl_tileyellow-ground-wall-l", int(m_x + 20), int(m_y + 80));
                    } // if
                    if (m_state_cycle % 20 == 0) {
                        SFXM->SFX_play("sfx/closingwall", m_sfx_volume, get_angle(map), get_distance(map));
                    }
                    break;
            } // switch
            if (m_state_cycle == 100)
                return false;
            break;
    } // switch

    return true;
} /* GO_closingwall::cycle */



bool GO_closingwall::is_a(Symbol *c)
{
    if (c->cmp(closingwall_symbol))
        return true;

    return GObject::is_a(c);
} /* GO_closingwall::is_a */


bool GO_closingwall::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
} /* GO_closingwall::is_a */
