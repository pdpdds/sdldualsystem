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
#include "GO_character.h"
#include "GO_cagedoor.h"
#include "GObjectCreator.h"

#include "GMap.h"

GO_cagedoor::GO_cagedoor(int x, int y, int sfx_volume, int n_locks, int item) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(cagedoor_symbol);

    m_n_locks = n_locks;
    m_item = item;
    m_state = 0;
    m_state_cycle = 0;
	m_lock = 0;
} /* GO_cagedoor::GO_cagedoor */


bool GO_cagedoor::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    switch (m_state) {
        case 0:  // closed:
            {
				if (m_lock == 0) m_lock = GLTM->get("ob_lock");
                GO_character *c = (GO_character *)map->collision_with_object(m_lock, (int)(m_x - 20), (int)(m_y + 20), GLTM, character_symbol);
                if (c != 0)
                {
                    if (c->has_key()) {
                        c->use_key();
                        m_n_locks--;
                        if (m_n_locks <= 0) {
                            SFXM->SFX_play("sfx/open_door", m_sfx_volume, get_angle(map), get_distance(map));
                            m_state = 1;
                            m_state_cycle = 0;
                        } else {
                            SFXM->SFX_play("sfx/open_lock", m_sfx_volume, get_angle(map), get_distance(map));
                        } // if
                    } // if
                } // if
            }
            break;

        case 1:  // opening:
            m_state_cycle++;
            if (m_state_cycle >= 5) {
                m_state = 2;
                m_state_cycle = 0;

                // create item:
                List<int> l;
                l.Add(new int(m_item));
                GObject *o = GObject_create(item_symbol, (int)m_x + 8, (int)(m_y + 37), m_sfx_volume, &l);
                map->add_object(o, layer);
            } // if
            break;

        case 2:  // open:
            break;
    } // switch

    return true;
} /* GO_cagedoor::cycle */


void GO_cagedoor::draw(GLTManager *GLTM)
{
    int i;

    for (i = 0;i < m_n_locks;i++) {
		if (m_lock == 0) m_lock = GLTM->get("ob_lock");
        m_lock->draw(m_x - 20, m_y + 40 - i*20, 0, 0, 1);
    } /* for */

    switch (m_state) {
        case 0:
            m_last_tile_used = GLTM->get
                               ("ob_cagedoor-closed");
            m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
            break;
        case 1:
            m_last_tile_used = GLTM->get
                               ("ob_cagedoor-closed");
            m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
            break;
        case 2:
            m_last_tile_used = GLTM->get
                               ("ob_cagedoor-open");
            m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
            break;
    } // switch
} /* GO_cagedoor::draw */


bool GO_cagedoor::is_a(Symbol *c)
{
    if (c->cmp(cagedoor_symbol))
        return true;

    return GObject::is_a(c);
} /* GO_cagedoor::is_a */


bool GO_cagedoor::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
} /* GO_cagedoor::is_a */
