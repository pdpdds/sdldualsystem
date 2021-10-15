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
#include "GO_musicalnote.h"

#include "GMap.h"

#include "TheGooniesCtnt.h"
#include "GObjectCreator.h"

GO_musicalnote::GO_musicalnote(int x, int y, int sfx_volume, float speedx, float speedy) : GO_enemy(x, y, sfx_volume)
{
    m_class = new Symbol(musicalnote_symbol);

    m_state = 0;
    m_state_cycle = 0;

    m_speedx = speedx;
    m_speedy = speedy;
} /* GO_musicalnote::GO_musicalnote */


bool GO_musicalnote::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    m_x += m_speedx;
    m_y += m_speedy;
    m_state_cycle++;

    if (m_x < 0 || m_x > map->get_dx() ||
            m_y < 0 || m_y > map->get_dy())
        return false;
    if (m_state == 1)
        return false;
    return true;
} /* GO_musicalnote::cycle */


void GO_musicalnote::draw(GLTManager *GLTM)
{
	if (m_last_tile_used == 0) m_last_tile_used = GLTM->get("ob_musicalnote");
    m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
} /* GO_musicalnote::draw */


bool GO_musicalnote::is_a(Symbol *c)
{
    if (c->cmp(musicalnote_symbol))
        return true;

    return GO_enemy::is_a(c);
} /* GO_musicalnote::is_a */


bool GO_musicalnote::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
} /* GO_musicalnote::is_a */


int GO_musicalnote::enemy_hit(void)
{
    m_state = 1;
    return 8;
} /* GO_musicalnote::enemy_hit */

