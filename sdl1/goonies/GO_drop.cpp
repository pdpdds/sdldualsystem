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
#include "MusicManager.h"
#include "GObject.h"
#include "GO_enemy.h"
#include "GO_drop.h"
#include "GO_character.h"
#include "GObjectCreator.h"

#include "GMap.h"
#include "TheGoonies.h"


GO_drop::GO_drop(int x, int y, int sfx_volume) : GO_enemy(x, y, sfx_volume)
{
    m_class = new Symbol(drop_symbol);

    m_state = 0;
    m_state_cycle = 0;
} 

bool GO_drop::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    m_state_cycle++;

    switch (m_state) {
        case 0:
            if (m_state_cycle > 32) {
                m_state = 1;
                m_state_cycle = 0;
            }
            break;
        case 1:
            m_y += 2;

            if (map->collision_with_background(this, 0, 2, GLTM)) {

                switch (rand() % 3) {
                    case 0:
                        SFXM->SFX_play("sfx/waterdrop", m_sfx_volume, get_angle(map), get_distance(map));
                        break;
                    case 1:
                        SFXM->SFX_play("sfx/waterdrop2", m_sfx_volume, get_angle(map), get_distance(map));
                        break;
                    case 2:
                        SFXM->SFX_play("sfx/waterdrop3", m_sfx_volume, get_angle(map), get_distance(map));
                        break;
                }
                m_state = 2;
                m_state_cycle = 0;

                // If the player has the hammer, then, create a coin:
                {
                    GO_character *c = (GO_character *)map->get_object(character_symbol);
                    if (c != 0) {
                        if (c->player_has("GO_hammer")) {
                            int hs = c->get_hammer_status();
                            if (hs > 0) {
                                hs--;
                                c->set_hammer_status(hs);
                                if ((hs % 2) == 0) {
                                    // create a coin:
                                    GObject *o = GObject_create(coin_symbol, (int)m_x, (int)(m_y), m_sfx_volume, 0);
                                    o->set_layer(2, 0);
                                    game->add_multiroom_object(o, map);
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 2:
            if (m_state_cycle > 18)
                return false;
            break;
    }
	
    return true;
}

void GO_drop::draw(GLTManager *GLTM)
{
    switch (m_state) {
        case 0: {
                int s = m_state_cycle / 16;
                if (s == 0)
                    m_last_tile_used = GLTM->get
                                       ("ob_drop1");
                if (s == 1)
                    m_last_tile_used = GLTM->get
                                       ("ob_drop2");
                if (s == 2)
                    m_last_tile_used = GLTM->get
                                       ("ob_drop3");
            }
            break;
        case 1:
            m_last_tile_used = GLTM->get
                               ("ob_drop3");
            break;
        case 2:	{
				int s = m_state_cycle / 3;
				if (s == 0) m_last_tile_used = GLTM->get ("ob_drop4");
            	if (s == 1) m_last_tile_used = GLTM->get ("ob_drop5");
            	if (s == 2) m_last_tile_used = GLTM->get ("ob_drop6");
            	if (s == 3) m_last_tile_used = GLTM->get ("ob_drop7");
            	if (s == 4) m_last_tile_used = GLTM->get ("ob_drop8");
            	if (s == 5) m_last_tile_used = GLTM->get ("ob_drop9");
            	if (s >= 6) m_last_tile_used = 0;
			}
            break;
    }

	if (m_last_tile_used)
		m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
}

bool GO_drop::is_a(Symbol *c)
{
    if (c->cmp(drop_symbol))
        return true;

    return GO_enemy::is_a(c);
}

bool GO_drop::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
}

int GO_drop::enemy_hit(void)
{
    if (m_state == 0 || m_state == 1) {
        m_state = 2;
        m_state_cycle = 0;
        return 8;
    }
    return 0;
}
