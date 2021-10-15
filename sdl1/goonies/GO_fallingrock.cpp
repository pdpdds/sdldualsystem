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
#include "GO_fallingrock.h"

#include "GMap.h"

#include "GObjectCreator.h"


GO_fallingrock::GO_fallingrock(int x, int y, int sfx_volume, int color) : GO_enemy(x, y, sfx_volume)
{
    m_class = new Symbol(fallingrock_symbol);

    m_color = color;
    m_state = 0;
    m_state_cycle = 0;
} /* GO_fallingrock::GO_fallingrock */


bool GO_fallingrock::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    m_state_cycle++;

    switch (m_state) {
        case 0: {
                int i;
                float bkp = m_y;
                for (i = (int)m_y;i < map->get_dy() && m_state == 0;i += 32) {
                    if (map->collision_with_object(GLTM->get
                                                   ("ob_falling-rock1"), (int)(m_x - 16), i, GLTM, character_symbol) ||
                            map->collision_with_object(GLTM->get
                                                       ("ob_falling-rock1"), (int)m_x, i, GLTM, character_symbol) ||
                            map->collision_with_object(GLTM->get
                                                       ("ob_falling-rock1"), (int)(m_x + 16), i, GLTM, character_symbol)) {
                         m_state = 1;
                         break;
                    }
                    else {

                         m_y = float(i);
                         if ( map->collision_with_background(this, GLTM))  {
                              m_y = bkp;
                              break;
                         }
                         else {
                              m_y = bkp;
                         }
                   } // else
                } // for
            }
            break;
        case 1:
            m_y += 4;

            if (map->collision_with_background(this, 0, 4, GLTM)) {
                m_y += 6;
                m_state = 2;
                m_state_cycle = 0;
            } // if
            break;
        case 2:
            if (m_state_cycle == 1)
                SFXM->SFX_play("sfx/rockfall", m_sfx_volume, get_angle(map), get_distance(map));
            if (m_state_cycle > 16)
                return false;
            break;
        case 3:
            if (m_state_cycle == 1)
                SFXM->SFX_play("sfx/rockfall", m_sfx_volume, get_angle(map), get_distance(map));
            if (m_state_cycle > 16)
                return false;
            break;
    } // switch
    return true;
} /* GO_cagedoor::cycle */


void GO_fallingrock::draw(GLTManager *GLTM)
{
    switch (m_state) {
        case 0:
            if (m_color == 0)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-rock1");
            if (m_color == 1)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-bluerock1");
            if (m_color == 2)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-greenrock1");
            if (m_color == 3)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-yellowrock1");
            break;
        case 1:
            if (m_color == 0)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-rock1");
            if (m_color == 1)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-bluerock1");
            if (m_color == 2)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-greenrock1");
            if (m_color == 3)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-yellowrock1");
            break;
        case 2:
            if (m_color == 0)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-rock3");
            if (m_color == 1)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-bluerock3");
            if (m_color == 2)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-greenrock3");
            if (m_color == 3)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-yellowrock3");
            break;
        case 3:
            if (m_color == 0)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-rock2");
            if (m_color == 1)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-bluerock2");
            if (m_color == 2)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-greenrock2");
            if (m_color == 3)
                m_last_tile_used = GLTM->get
                                   ("ob_falling-yellowrock2");
            break;
    } // switch

    m_last_tile_used->draw(m_x, m_y, 0, 0, 1);
} /* GO_fallingrock::draw */


bool GO_fallingrock::is_a(Symbol *c)
{
    if (c->cmp(fallingrock_symbol))
        return true;

    return GO_enemy::is_a(c);
} /* GO_fallingrock::is_a */


bool GO_fallingrock::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
} /* GO_fallingrock::is_a */


int GO_fallingrock::enemy_hit(void)
{
    if (m_state == 1) {
        m_state = 3;
        m_state_cycle = 0;
        return 8;
    } // if
    return 0;
} /* GO_skull::enemy_hit */

