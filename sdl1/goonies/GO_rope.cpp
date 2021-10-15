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

#include "GO_rope.h"
#include "GObjectCreator.h"


GO_rope::GO_rope(int x, int y, int sfx_volume, int type) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(rope_symbol);

    m_state_cycle = 0;
    m_type = type;
} /* GObject::GObject */


bool GO_rope::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    m_state_cycle++;
    return true;
} /* GO_rope::cycle */


void GO_rope::draw(GLTManager *GLTM)
{
	if (m_last_tile_used==0) {
		if (m_type == 0)
			m_last_tile_used = GLTM->get
							   ("ob_rope-top");
		if (m_type == 1)
			m_last_tile_used = GLTM->get
							   ("ob_rope");
		if (m_type == 2)
			m_last_tile_used = GLTM->get
							   ("ob_rope-bottom");
	} // if
    m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
} /* GO_rope::draw */


bool GO_rope::is_a(Symbol *c)
{
    if (c->cmp(rope_symbol))
        return true;

    return GObject::is_a(c);
} /* GO_rope::is_a */


bool GO_rope::is_a(char *c)
{	
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
} /* GO_rope::is_a */
