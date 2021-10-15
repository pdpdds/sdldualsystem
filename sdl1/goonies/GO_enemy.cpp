#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include "windows.h"
#endif

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

#include "GObjectCreator.h"

GO_enemy::GO_enemy(int x, int y, int sfx_volume) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(enemy_symbol);
}

bool GO_enemy::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    return true;
}

void GO_enemy::draw(GLTManager *GLTM)
{
}

bool GO_enemy::is_a(Symbol *c)
{
    if (c->cmp(enemy_symbol))
        return true;

    return GObject::is_a(c);
}

bool GO_enemy::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
}

bool GO_enemy::player_hit(int *experience, int *score)
{
    return false;
}

int GO_enemy::enemy_hit(void)
{
    return 1;
}
