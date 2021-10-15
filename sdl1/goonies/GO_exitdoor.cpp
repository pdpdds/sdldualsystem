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

#include "GO_skulldoor.h"
#include "GO_exitdoor.h"
#include "GO_character.h"

#include "GMap.h"

#include "GObjectCreator.h"


GO_exitdoor::GO_exitdoor(int x, int y, int sfx_volume) : GO_skulldoor(x, y, sfx_volume, -1, -1, -1)
{
    m_class = new Symbol(exitdoor_symbol);

	m_state = 0;
    m_state_cycle = 0;
} /* GO_exitdoor::GO_exitdoor */


bool GO_exitdoor::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    if (m_state == 0) {
        GO_character *c = (GO_character *)map->get_object(character_symbol);
        if (c->get_rescued_goonies() == 7)
            m_state = 1;
    } else {} // if

    return true;
} /* GO_exitdoor::cycle */


void GO_exitdoor::draw(GLTManager *GLTM)
{
    if (m_state == 0)
        m_last_tile_used = GLTM->get
                           ("ob_exitdoor");
    else
        m_last_tile_used = GLTM->get
                           ("ob_entrydoor");
    m_last_tile_used->draw(1, 1, 1, 1, m_x, m_y, 0, 0, 1);
} /* GO_exitdoor::draw */


bool GO_exitdoor::is_a(Symbol *c)
{
    if (c->cmp(exitdoor_symbol))
        return true;

    return GO_skulldoor::is_a(c);
} /* GO_exitdoor::is_a */


bool GO_exitdoor::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
} /* GO_exitdoor::is_a */
