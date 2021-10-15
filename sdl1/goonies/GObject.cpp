#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif


#ifdef _WIN32
#include "windows.h"
#endif

#include "stdlib.h"
#include "string.h"
#include "math.h"

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

#include "GObjectCreator.h"

GObject::GObject(int x, int y, int sfx_volume)
{
    m_class = new Symbol(object_symbol);
    m_sfx_volume = sfx_volume;
    m_x = float(x);
    m_y = float(y);
    m_last_tile_used = 0;
    m_state = 0;
    m_state_cycle = 0;
} /* GObject::GObject */


GObject::~GObject()
{
    delete m_class;
} /* GObject::~GObject */


bool GObject::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    return true;
} /* GObject::cycle */


void GObject::draw(GLTManager *GLTM)
{} /* GObject::draw */


bool GObject::is_a(Symbol *c)
{
    return c->cmp(object_symbol);
} /* GObject::is_a */


bool GObject::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
} /* GObject::is_a */


bool GObject::collision(GObject *o)
{
    if (m_last_tile_used == 0)
        return false;
    return o->collision(m_last_tile_used, (int)m_x, (int)m_y);
} /* GObject::collision */


bool GObject::collision(int x_offs,int y_offs,GObject *o)
{
    if (m_last_tile_used == 0)
        return false;
    return o->collision(m_last_tile_used, (int)m_x+x_offs, (int)m_y+y_offs);
} /* GObject::collision */


bool GObject::collision(GLTile *t, int x, int y)
{
    if (m_last_tile_used == 0)
        return false;
    return m_last_tile_used->get_cmc()->collision_no_rotation(m_x, m_y, t->get_cmc(), float(x), float(y));
} /* GObject::collision */


bool GObject::collision(C2DCMC *c, int x, int y)
{
    if (m_last_tile_used == 0)
        return false;
    return m_last_tile_used->get_cmc()->collision_no_rotation(m_x, m_y, c, float(x), float(y));
} /* GObject::collision */



bool GObject::collision(int xoffs, int yoffs, GLTile *t, int x, int y)
{
    if (m_last_tile_used == 0)
        return false;
    return m_last_tile_used->get_cmc()->collision_no_rotation(m_x + xoffs, m_y + yoffs, t->get_cmc(), float(x), float(y));
} /* GObject::collision */


void GObject::set_layer(int layer, GMap *map)
{
    if (layer != m_layer) {
        //  output_debug_message("%i -> %i\n",m_layer,layer);
        if (map != 0)
            map->remove_object(this, m_layer);
        m_layer = layer;
        if (map != 0)
            map->add_object(this, m_layer);
    } // if
} /* GObject::set_layer */

/*
 * Calculates distance between this GObject and GO_character
 * Distance will be between 1 and 255 and will be used in Mix_SetDistance
 */
int GObject::get_distance(GMap *map)
{
    GObject *c = (GObject *)map->get_object(character_symbol);
    int dist = 1;

    if (c != 0) {
        dist = abs(int(m_x - c->get_x()));
        // be sneaky and take some minimum and maximum distance/'sfx volume'
        // into account
        if (dist > 150) {
            dist = 150;
        } else if (dist < 25) {
            dist = 1;
        }
    }
    return dist;
}

/*
 * Calculates the 'angle' of the current GObject, based on its distance from the player
 */
int GObject::get_angle(GMap *map)
{
    GObject *c = (GObject *)map->get_object(character_symbol);

    float x = c->get_x() - m_x;
    float y = c->get_y() - m_y;
    int angle;

    angle = (int)(atan2(y, x) * 180 / 3.149265);
    angle = 270 - angle;

    if (angle > 360) {
        angle = 360;
    }
    if (angle < 0) {
        angle = 0;
    }
    return angle;
}
