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
#include "GO_bullet.h"

#include "GMap.h"

#include "TheGooniesCtnt.h"
#include "GObjectCreator.h"

GO_bullet::GO_bullet(int x, int y, int sfx_volume, int facing) : GO_enemy(x, y, sfx_volume)
{
    m_class = new Symbol(bullet_symbol);

    m_state = 0;
    m_state_cycle = 0;

    m_facing = facing;
} /* GO_bullet::GO_bullet */


bool GO_bullet::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    if (m_facing == 0)
        m_x -= BULLET_SPEED;
    else
        m_x += BULLET_SPEED;
    m_state_cycle++;

    if (m_x < 0 || m_x > map->get_dx())
        return false;
    if (map->collision_with_background(this, GLTM))
        return false;
    if (m_state == 1)
        return false;
    return true;
} /* GO_bullet::cycle */


void GO_bullet::draw(GLTManager *GLTM)
{
	if (m_last_tile_used==0) {
		if (m_facing == 0)
			m_last_tile_used = GLTM->get
							   ("ob_bullet-l");
		else
			m_last_tile_used = GLTM->get
							   ("ob_bullet-r");
	} // if 
    m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
} /* GO_bullet::draw */


bool GO_bullet::is_a(Symbol *c)
{
    if (c->cmp(bullet_symbol))
        return true;

    return GO_enemy::is_a(c);
} /* GO_bullet::is_a */


bool GO_bullet::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
} /* GO_bullet::is_a */


bool GO_bullet::player_hit(int *experience, int *score)
{
    return false;
} /* GO_bullet::player_hit */


int GO_bullet::enemy_hit(void)
{
    m_state = 1;
    return 7;
} /* GO_bullet::enemy_hit */

