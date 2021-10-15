#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "assert.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include "List.h"
#include "Symbol.h"
#include "keyboardstate.h"
#include "auxiliar.h"
#include "2DCMC.h"
#include "GLTile.h"
#include "GLTManager.h"
#include "SoundManager.h"
#include "SFXManager.h"
#include "VirtualController.h"

#include "GObject.h"
#include "GMap.h"

#include "GObjectCreator.h"

#include "GooniesScript.h"

extern bool water_reflection; 
GMap::GMap(FILE *fp)
{
    GMapLayer *l;
    int i;
    int nl;
    float scale = 20.0f / 16.0f;
    fscanf(fp, "%i %i %i", &m_dx, &m_dy, &nl);

    m_dx = (int)(m_dx * scale);
    m_dy = (int)(m_dy * scale);

    for (i = 0;i < nl;i++) {
        l = new GMapLayer(fp);
        m_layers.Add(l);
    }

    m_script = new GooniesScript(fp);

    // Create the default bckground in case that there is no background:
    if (m_layers[0]->m_tiles.EmptyP()) {
        int background_type = -1;
        GMapTilePlace *tp;

        m_layers[1]->m_tiles.Rewind();
        while (m_layers[1]->m_tiles.Iterate(tp) && background_type == -1) {
            if (tp->m_tile_name->cmp("gr_tile-ground-m"))
                background_type = 0;
            if (tp->m_tile_name->cmp("gr_tilegreen-ground-m"))
                background_type = 1;
            if (tp->m_tile_name->cmp("gr_tileblue-ground-m"))
                background_type = 2;
            if (tp->m_tile_name->cmp("gr_tileyellow-ground-m"))
                background_type = 3;
        }

        tp = new GMapTilePlace();
		tp->m_tile_cache=0;
        switch (background_type) {
            case 0:
                tp->m_tile_name = new Symbol("bg_background");
                break;
            case 1:
                tp->m_tile_name = new Symbol("bg_background_green");
                break;
            case 2:
                tp->m_tile_name = new Symbol("bg_background_blue");
                break;
            case 3:
                tp->m_tile_name = new Symbol("bg_background_yellow");
                break;
            default:
                delete tp;
                tp = 0;
        }
        if (tp != 0) {
            tp->m_x = 0;
            tp->m_y = 0;
            m_layers[0]->m_tiles.Add(tp);
        }
    }

    // Look for all the water objects, and move them to the top layer:
    {
        int i;
        GObjectPlace *op;
        List<GObjectPlace> to_top_layer;
        for (i = 0;i < 4;i++)
        {
            m_layers[i]->m_objects.Rewind();
            while (m_layers[i]->m_objects.Iterate(op)) {
                if (op->m_object_name->cmp(water_symbol) ||
                        op->m_object_name->cmp(lava_symbol)) {
                    to_top_layer.Add(op);
                }
            }
            while (!to_top_layer.EmptyP()) {
                op = to_top_layer.ExtractIni();
                m_layers[i]->m_objects.DeleteElement(op);
                m_layers[4]->m_objects.Add(op);
            }
        }
    }

    m_searched_for_bridges = false;

    m_water_info_captured = false;
    m_water_level = -1;
    m_lava_level = -1;
    m_water_info_texture = 0;

    m_shake_timmer = 0;
}

GMap::~GMap()
{
    clear_water_info();

    delete m_script;
}

void GMap::reset(int sfx_volume)
{
    GMapLayer *l;

    m_shake_timmer = 0;

    m_layers.Rewind();
    while (m_layers.Iterate(l))
        l->reset(sfx_volume);
}

void GMap::reset_first(int sfx_volume)
{
    GMapLayer *l;

    m_shake_timmer = 0;

    m_layers.Rewind();
    while (m_layers.Iterate(l))
        l->reset_first(sfx_volume, this);
}

void GMap::cycle(VirtualController *v, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    int i = 0;
    GMapLayer *l;

    if (m_shake_timmer > 0)
        m_shake_timmer--;

    // look for bridges and other tiles that have non standard collisions:
    if (!m_searched_for_bridges)
        search_for_bridges(GLTM);

    m_layers.Rewind();
    while (m_layers.Iterate(l))
        l->cycle(v, this, i++, game, GLTM, SFXM);
}

void GMap::cycle_clock(VirtualController *v, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    int i = 0;
    GMapLayer *l;

    if (m_shake_timmer > 0)
        m_shake_timmer--;

    // look for bridges and other tiles that have non standard collisions:
    if (!m_searched_for_bridges)
        search_for_bridges(GLTM);

    m_layers.Rewind();
    while (m_layers.Iterate(l))
        l->cycle_clock(v, this, i++, game, GLTM, SFXM);
}

void GMap::search_for_bridges(GLTManager *GLTM)
{
    if (!m_searched_for_bridges) {
        GMapTilePlace *pt;
        GMapCMCPlace *pc;
        GMapLayer *l;
        int added_cmcs = 0;

        m_layers.Rewind();
        while (m_layers.Iterate(l)) {
            l->m_tiles.Rewind();
            while (l->m_tiles.Iterate(pt)) {
                if (pt->m_tile_name->cmp("gr_tilegreen-bridge-l") ||
                        pt->m_tile_name->cmp("gr_tilegreen-bridge-m") ||
                        pt->m_tile_name->cmp("gr_tilegreen-bridge-r") ||
                        pt->m_tile_name->cmp("gr_tileblue-bridge-l") ||
                        pt->m_tile_name->cmp("gr_tileblue-bridge-m") ||
                        pt->m_tile_name->cmp("gr_tileblue-bridge-r") ||
                        pt->m_tile_name->cmp("gr_tileyellow-bridge-l") ||
                        pt->m_tile_name->cmp("gr_tileyellow-bridge-m") ||
                        pt->m_tile_name->cmp("gr_tileyellow-bridge-r") ||
                        pt->m_tile_name->cmp("gr_tile-bridge-l") ||
                        pt->m_tile_name->cmp("gr_tile-bridge-m") ||
                        pt->m_tile_name->cmp("gr_tile-bridge-r")) {
                    pc = new GMapCMCPlace();
                    pc->m_cmc = GLTM->get
                                (pt->m_tile_name)->get_cmc();
                    pc->m_x = pt->m_x;
                    pc->m_y = pt->m_y;
                    m_extra_collisions.Add(pc);
                    added_cmcs++;
                }
                if (pt->m_tile_name->cmp("gr_tilegreen-ground-l") ||
                        pt->m_tile_name->cmp("gr_tilegreen-ground-m") ||
                        pt->m_tile_name->cmp("gr_tilegreen-ground-r") ||
                        pt->m_tile_name->cmp("gr_tileblue-ground-l") ||
                        pt->m_tile_name->cmp("gr_tileblue-ground-m") ||
                        pt->m_tile_name->cmp("gr_tileblue-ground-r") ||
                        pt->m_tile_name->cmp("gr_tileyellow-ground-l") ||
                        pt->m_tile_name->cmp("gr_tileyellow-ground-m") ||
                        pt->m_tile_name->cmp("gr_tileyellow-ground-r") ||
                        pt->m_tile_name->cmp("gr_tile-ground-l") ||
                        pt->m_tile_name->cmp("gr_tile-ground-m") ||
                        pt->m_tile_name->cmp("gr_tile-ground-r")) {
                    pc = new GMapCMCPlace();
                    pc->m_cmc = GLTM->get
                                (pt->m_tile_name)->get_cmc();
                    pc->m_x = pt->m_x;
                    pc->m_y = pt->m_y;
                    m_extra_collisions.Add(pc);
                    added_cmcs++;
                }
            }
        }
#ifdef __DEBUG_MESSAGES
        output_debug_message("Number of extra CMCs added to map: %i\n", added_cmcs);
#endif

        assert(added_cmcs > 0);
        m_searched_for_bridges = true;
    }
}

void GMap::draw(GLTManager *GLTM, bool water)
{
    GMapLayer *l;

    clear_water_info();

    if (m_shake_timmer > 0) {
        glPushMatrix();
        glTranslatef(0, float(sin(m_shake_timmer)*(m_shake_timmer / 15.0f)), 0);
    }

    if (water_reflection && water &&
            (m_layers[4]->get_object(water_symbol) != 0 ||
             m_layers[4]->get_object(lava_symbol) != 0)) {
        // Special drawing routing for rooms with water:
        int i;
        int vp[4];

        glGetIntegerv(GL_VIEWPORT, vp);
        glViewport(0, 0, 256, 256);
        for (i = 0;i < 4;i++)
            m_layers[i]->draw(GLTM);
        get_water_info();
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(vp[0], vp[1], vp[2], vp[3]);

        m_layers.Rewind();
        while (m_layers.Iterate(l))
            l->draw(GLTM);
    } else {
        m_layers.Rewind();
        while (m_layers.Iterate(l))
            l->draw(GLTM);
    }

    if (m_shake_timmer > 0) {
        glPopMatrix();
    }
}

void GMap::draw(int layer, GLTManager *GLTM)
{
    if (layer >= 0 && layer < m_layers.Length())
        m_layers[layer]->draw(GLTM);
}

void GMap::add_object(GObject *o, int layer)
{
    if (layer >= 0 && layer < m_layers.Length())
        m_layers[layer]->add_object(o);
}


bool GMap::object_exists(GObject *o)
{
	int i;

	for(i=0;i<m_layers.Length();i++) {
		if (m_layers[i]->object_exists(o)) return true;
	} // for

	return false;
} /* GMap::object_exists */ 


void GMap::add_auxiliar_object(GObject *o, int layer)
{
    if (layer >= 0 && layer < m_layers.Length())
        m_layers[layer]->add_auxiliar_object(o);
}

bool GMap::remove_object(GObject *o, int layer)
{
    if (layer >= 0 && layer < m_layers.Length())
        return m_layers[layer]->remove_object(o);
    return false;
}

bool GMap::collision(GObject *o, GLTManager *GLTM)
{
    // Collision only with layers 2 and 3 (should I also check layer 1?)
    if (m_layers[2]->collision(o, GLTM) ||
            m_layers[3]->collision(o, GLTM))
        return true;

    return false;
}

bool GMap::collision_with_background(GObject *o, GLTManager *GLTM)
{
    // Collision only with layers 2 and 3 (should I also check layer 1?)
    if (m_layers[2]->collision_with_background(o, GLTM) ||
            m_layers[3]->collision_with_background(o, GLTM))
        return true;

    GMapCMCPlace *c;

#ifdef __DEBUG_MESSAGES

    output_debug_message("<cwb> (%i,%i)\n", o->get_x(), o->get_y());
#endif

    m_extra_collisions.Rewind();
    while (m_extra_collisions.Iterate(c)) {
#ifdef __DEBUG_MESSAGES
        output_debug_message("- (%i,%i) [%f,%f]-[%f,%f]\n", c->m_x, c->m_y, c->m_cmc->x[0], c->m_cmc->y[0],
                             c->m_cmc->x[1], c->m_cmc->y[1]);
#endif

        if (o->collision(c->m_cmc, c->m_x, c->m_y))
            return true;
    }

#ifdef __DEBUG_MESSAGES
    output_debug_message("</cwb>\n");
#endif

    return false;
}

bool GMap::collision_with_background(GObject *o, int x_offs, int y_offs, GLTManager *GLTM)
{
    // Collision only with layers 2 and 3 (should I also check layer 1?)
    if (m_layers[2]->collision_with_background(o, x_offs, y_offs, GLTM) ||
            m_layers[3]->collision_with_background(o, x_offs, y_offs, GLTM))
        return true;

    GMapCMCPlace *c;

    m_extra_collisions.Rewind();
    while (m_extra_collisions.Iterate(c)) {
        if (o->collision(c->m_cmc, c->m_x - x_offs, c->m_y - y_offs))
            return true;
    }

    return false;
}

bool GMap::collision_with_background(GLTile *t, int x, int y, GLTManager *GLTM)
{
    // Collision only with layers 2 and 3 (should I also check layer 1?)
    if (m_layers[2]->collision_with_background(t, x, y, GLTM) ||
            m_layers[3]->collision_with_background(t, x, y, GLTM))
        return true;

    GMapCMCPlace *c;
    m_extra_collisions.Rewind();
    while (m_extra_collisions.Iterate(c)) {
        if (t->get_cmc()->collision_no_rotation(float(x), float(y), c->m_cmc, float(c->m_x), float(c->m_y)))
            return true;
    }

    return false;
}


bool GMap::collision_with_background_wo_bridges(GObject *o, GLTManager *GLTM)
{
    // Collision only with layers 2 and 3 (should I also check layer 1?)
    if (m_layers[2]->collision_with_background(o, 0, 0, GLTM) ||
        m_layers[3]->collision_with_background(o, 0, 0, GLTM))
        return true;
    return false;
}

bool GMap::collision_with_background_wo_bridges(GObject *o, int x_offs, int y_offs, GLTManager *GLTM)
{
    // Collision only with layers 2 and 3 (should I also check layer 1?)
    if (m_layers[2]->collision_with_background(o, x_offs, y_offs, GLTM) ||
        m_layers[3]->collision_with_background(o, x_offs, y_offs, GLTM))
        return true;
    return false;
}

bool GMap::collision_with_background_wo_bridges_bigrocks(GObject *o, GLTManager *GLTM)
{
    // Collision only with layers 2 and 3 (should I also check layer 1?)
    if (m_layers[2]->collision_with_background_wo_bigrocks(o, 0, 0, GLTM) ||
		m_layers[3]->collision_with_background_wo_bigrocks(o, 0, 0, GLTM))
        return true;
    return false;
}

bool GMap::collision_with_background_wo_bridges_bigrocks(GObject *o, int x_offs, int y_offs, GLTManager *GLTM)
{
    // Collision only with layers 2 and 3 (should I also check layer 1?)
    if (m_layers[2]->collision_with_background_wo_bigrocks(o, x_offs, y_offs, GLTM) ||
		m_layers[3]->collision_with_background_wo_bigrocks(o, x_offs, y_offs, GLTM))
        return true;
    return false;
}


GObject *GMap::collision_with_object(GObject *o, GLTManager *GLTM)
{
    GObject *o2;

    o2 = m_layers[1]->collision_with_object(o, GLTM);
    if (o2 != 0)
        return o2;
    o2 = m_layers[2]->collision_with_object(o, GLTM);
    if (o2 != 0)
        return o2;
    o2 = m_layers[3]->collision_with_object(o, GLTM);
    if (o2 != 0)
        return o2;
    o2 = m_layers[4]->collision_with_object(o, GLTM);
    if (o2 != 0)
        return o2;

    return 0;
} /* GMap::collision_with_object */ 

GObject *GMap::collision_with_object(GObject *o, GLTManager *GLTM, char *type)
{
    GObject *o2;

    o2 = m_layers[1]->collision_with_object(o, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[2]->collision_with_object(o, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[3]->collision_with_object(o, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[4]->collision_with_object(o, GLTM, type);
    if (o2 != 0)
        return o2;

    return 0;
} /* GMap::collision_with_object */

GObject *GMap::collision_with_object(GObject *o, GLTManager *GLTM, Symbol *type)
{
    GObject *o2;

    o2 = m_layers[1]->collision_with_object(o, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[2]->collision_with_object(o, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[3]->collision_with_object(o, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[4]->collision_with_object(o, GLTM, type);
    if (o2 != 0)
        return o2;

    return 0;
} /* GMap::collision_with_object */

GObject *GMap::collision_with_object(GObject *o, int x_offs,int y_offs, GLTManager *GLTM, char *type)
{
    GObject *o2;

    o2 = m_layers[1]->collision_with_object(o, x_offs, y_offs, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[2]->collision_with_object(o, x_offs, y_offs, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[3]->collision_with_object(o, x_offs, y_offs, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[4]->collision_with_object(o, x_offs, y_offs, GLTM, type);
    if (o2 != 0)
        return o2;

    return 0;
} /* GMap::collision_with_object */

GObject *GMap::collision_with_object(GObject *o, int x_offs,int y_offs, GLTManager *GLTM, Symbol *type)
{
    GObject *o2;

    o2 = m_layers[1]->collision_with_object(o, x_offs, y_offs, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[2]->collision_with_object(o, x_offs, y_offs, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[3]->collision_with_object(o, x_offs, y_offs, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[4]->collision_with_object(o, x_offs, y_offs, GLTM, type);
    if (o2 != 0)
        return o2;

    return 0;
} /* GMap::collision_with_object */ 

GObject *GMap::collision_with_object(GLTile *t, int x, int y, GLTManager *GLTM, char *type)
{
    GObject *o2;

    o2 = m_layers[1]->collision_with_object(t, x, y, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[2]->collision_with_object(t, x, y, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[3]->collision_with_object(t, x, y, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[4]->collision_with_object(t, x, y, GLTM, type);
    if (o2 != 0)
        return o2;

    return 0;
} /* GMap::collision_with_object */ 


GObject *GMap::collision_with_object(GLTile *t, int x, int y, GLTManager *GLTM, Symbol *type)
{
    GObject *o2;

    o2 = m_layers[1]->collision_with_object(t, x, y, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[2]->collision_with_object(t, x, y, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[3]->collision_with_object(t, x, y, GLTM, type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[4]->collision_with_object(t, x, y, GLTM, type);
    if (o2 != 0)
        return o2;

    return 0;
} /* GMap::collision_with_object */



GObject *GMap::get_object(char *type)
{
    GObject *o2;

    o2 = m_layers[1]->get_object(type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[2]->get_object(type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[3]->get_object(type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[4]->get_object(type);
    if (o2 != 0)
        return o2;

    return 0;
}


GObject *GMap::get_object(Symbol *type)
{
    GObject *o2;

    o2 = m_layers[1]->get_object(type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[2]->get_object(type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[3]->get_object(type);
    if (o2 != 0)
        return o2;
    o2 = m_layers[4]->get_object(type);
    if (o2 != 0)
        return o2;

    return 0;
}


List<GObject> *GMap::get_objects(char *type)
{
    List<GObject> *l = new List<GObject>;

    m_layers[1]->get_objects(l, type);
    m_layers[2]->get_objects(l, type);
    m_layers[3]->get_objects(l, type);
    m_layers[4]->get_objects(l, type);

    return l;
} /* GMap::get_objects */ 


List<GObject> *GMap::get_objects(Symbol *type)
{
    List<GObject> *l = new List<GObject>;

    m_layers[1]->get_objects(l, type);
    m_layers[2]->get_objects(l, type);
    m_layers[3]->get_objects(l, type);
    m_layers[4]->get_objects(l, type);

    return l;
} /* GMap::get_objects */


void GMap::get_water_info(void)
{
    if (!m_water_info_captured) {
        glGenTextures(1, &m_water_info_texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, m_water_info_texture);
        glBindTexture(GL_TEXTURE_2D, m_water_info_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, 256, 256, 0);

        m_water_info_captured = true;
    }
}

GLuint GMap::get_water_info_texture(void)
{
    get_water_info();

    return m_water_info_texture;
}

void GMap::clear_water_info(void)
{
    if (m_water_info_captured) {
        glDeleteTextures(1, &m_water_info_texture);
        m_water_info_captured = false;
    }
}

int GMap::get_water_level(void)
{
    if (m_water_level == -1) {
        m_water_level = m_dy;

        GObjectPlace *op;
        List<GObjectPlace> to_top_layer;
        m_layers[4]->m_objects.Rewind();
        while (m_layers[4]->m_objects.Iterate(op)) {
            if (op->m_object_name->cmp(water_symbol)) {
                if (op->m_y < m_water_level)
                    m_water_level = op->m_y;
            }
        }
    }

    return m_water_level;
}

int GMap::get_lava_level(void)
{
    if (m_lava_level == -1) {
        m_lava_level = m_dy;

        GObjectPlace *op;
        List<GObjectPlace> to_top_layer;
        m_layers[4]->m_objects.Rewind();
        while (m_layers[4]->m_objects.Iterate(op)) {
            if (op->m_object_name->cmp(lava_symbol)) {
                if (op->m_y < m_lava_level)
                    m_lava_level = op->m_y;
            }
        }
    }
    return m_lava_level;
}

void GMap::draw_darkened_area(int player_x, int player_y)
{
    int i;
    int n_steps = 16;
    float x_radius = 100;
    float y_radius = 100;
    float crispness = 1.5f; /* between 1.0 and infinite */
    float darkness = 0.85f;

    float inc = (M_PI * 2) / n_steps;

    glPushMatrix();
    glTranslatef(float(player_x), float(player_y), 0);
    for (i = 0;i < n_steps;i++) {
        glBegin(GL_QUADS);
        glColor4f(0, 0, 0, 0);
        glVertex3f(float(cos(inc*i)*x_radius), float(sin(inc*i)*y_radius), 0);
        glColor4f(0, 0, 0, darkness);
        glVertex3f(float(cos(inc*i)*x_radius*crispness), float(sin(inc*i)*y_radius*crispness), 0);
        glColor4f(0, 0, 0, darkness);
        glVertex3f(float(cos(inc*(i + 1))*x_radius*crispness), float(sin(inc*(i + 1))*y_radius*crispness), 0);
        glColor4f(0, 0, 0, 0);
        glVertex3f(float(cos(inc*(i + 1))*x_radius), float(sin(inc*(i + 1))*y_radius), 0);
        glEnd();
    }
    for (i = 0;i < n_steps;i++) {
        glBegin(GL_QUADS);
        glColor4f(0, 0, 0, darkness);
        glVertex3f(float(cos(inc*i)*x_radius*crispness), float(sin(inc*i)*y_radius*crispness), 0);
        glColor4f(0, 0, 0, darkness);
        glVertex3f(float(cos(inc*i)*10000), float(sin(inc*i)*10000), 0);
        glColor4f(0, 0, 0, darkness);
        glVertex3f(float(cos(inc*(i + 1))*10000), float(sin(inc*(i + 1))*10000), 0);
        glColor4f(0, 0, 0, darkness);
        glVertex3f(float(cos(inc*(i + 1))*x_radius*crispness), float(sin(inc*(i + 1))*y_radius*crispness), 0);
        glEnd();
    }
    glPopMatrix();

}

