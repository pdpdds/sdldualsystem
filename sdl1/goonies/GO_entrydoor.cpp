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

#include "GO_entrydoor.h"

#include "GObjectCreator.h"

GO_entrydoor::GO_entrydoor(int x, int y, int sfx_volume) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(entrydoor_symbol);

    m_state_cycle = 0;
} /* GObject::GObject */


bool GO_entrydoor::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    m_state_cycle++;

    if (m_state_cycle > 64)
        return false;
    return true;
} /* GO_entrydoor::cycle */


void GO_entrydoor::draw(GLTManager *GLTM)
{
    float f = (64 - m_state_cycle) / 64.0f;
    if (f < 0)
        f = 0;
    if (f > 1)
        f = 1;
	if (m_last_tile_used == 0) m_last_tile_used = GLTM->get("ob_entrydoor");
    m_last_tile_used->draw(1, 1, 1, f, m_x, m_y, 0, 0, 1);
} /* GO_entrydoor::draw */


bool GO_entrydoor::is_a(Symbol *c)
{
    if (c->cmp(entrydoor_symbol))
        return true;

    return GObject::is_a(c);
} /* GO_entrydoor::is_a */


bool GO_entrydoor::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
} /* GO_entrydoor::is_a */
