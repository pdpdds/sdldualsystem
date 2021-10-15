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

#include "GO_key.h"
#include "GObjectCreator.h"


GO_key::GO_key(int x, int y, int sfx_volume) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(key_symbol);

    m_state_cycle = 0;
} /* GObject::GObject */


bool GO_key::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    m_state_cycle++;
    if (m_state == 1) {
        m_state = 2;
        m_state_cycle = 0;
    } // if
    if (m_state == 2) {
        if (m_state_cycle > 8)
            return false;
    } // if
    return true;
} /* GO_key::cycle */


void GO_key::draw(GLTManager *GLTM)
{
    float f;

    if (m_state == 0 || m_state == 1) {
        f = float(0.6F + 0.4F * sin(m_state_cycle * 0.2f));
    } else {
        f = (8 - m_state_cycle) / 8.0F;
    } // if
    if (f > 1)
        f = 1;
    if (f < 0)
        f = 0;
	if (m_last_tile_used == 0) m_last_tile_used = GLTM->get("ob_key");
    m_last_tile_used->draw(1, 1, 1, f, m_x, m_y, 0, 0, 1);
} /* GO_key::draw */


bool GO_key::is_a(Symbol *c)
{
    if (c->cmp(key_symbol))
        return true;

    return GObject::is_a(c);
} /* GO_key::is_a */


bool GO_key::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
} /* GO_key::is_a */
