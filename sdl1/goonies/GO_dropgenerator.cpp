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
#include "GO_dropgenerator.h"

#include "GObjectCreator.h"

GO_dropgenerator::GO_dropgenerator(int x, int y, int sfx_volume) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(dropgenerator_symbol);

    m_state_cycle = 96;
}
bool GO_dropgenerator::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    m_state_cycle++;

    if (m_state_cycle > ((rand() % 4) + 4) * 32) {
        // generate a new drop:
        GObject *o = GObject_create(drop_symbol, (int)m_x, (int)(m_y + 16), m_sfx_volume, 0);
        map->add_object(o, layer);
        m_state_cycle = 0;
    }
    return true;
}

bool GO_dropgenerator::is_a(Symbol *c)
{
    if (c->cmp(dropgenerator_symbol))
        return true;

    return GObject::is_a(c);
}

bool GO_dropgenerator::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
}

