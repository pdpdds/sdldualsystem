#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif


#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

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
#include "GO_water.h"

#include "GObjectCreator.h"

#include "debug.h"

extern bool water_reflection;

GMapLayer::GMapLayer()
{
}

GMapLayer::GMapLayer(FILE *fp)
{
    int i, j;
    int nt, no, np, i_tmp, x, y;
    char tmp[256];
    GMapTilePlace *tp;
    GObjectPlace *op;
    float scale = 20.0f / 16.0f;

    fscanf(fp, "%i", &nt);
    for (i = 0;i < nt;i++) {
        tp = new GMapTilePlace();
        fscanf(fp, "%i %i %s", &x, &y, tmp);
        if ((strcmp(tmp,"gr_tilegreen-bridge-l")!=0) &&
            (strcmp(tmp,"gr_tilegreen-bridge-m")!=0) &&
			(strcmp(tmp,"gr_tilegreen-bridge-r")!=0) &&
			(strcmp(tmp,"gr_tileblue-bridge-l")!=0) &&
			(strcmp(tmp,"gr_tileblue-bridge-m")!=0) &&
			(strcmp(tmp,"gr_tileblue-bridge-r")!=0) &&
			(strcmp(tmp,"gr_tileyellow-bridge-l")!=0) &&
			(strcmp(tmp,"gr_tileyellow-bridge-m")!=0) &&
			(strcmp(tmp,"gr_tileyellow-bridge-r")!=0) &&
			(strcmp(tmp,"gr_tile-bridge-l")!=0) &&
			(strcmp(tmp,"gr_tile-bridge-m")!=0) &&
			(strcmp(tmp,"gr_tile-bridge-r")!=0))
        {
           tp->m_x = (int)(x * scale);
           tp->m_y = (int)(y * scale);
           tp->m_tile_name = new Symbol(tmp);
  		   tp->m_tile_cache = 0;
           m_tiles.Add(tp);
        }
        // add bridge as an object
        else {
           op = new GObjectPlace();
           op->m_x = (int)(x * scale);
           op->m_y = (int)(y * scale);
           op->m_object_name = new Symbol(bridge_symbol);
           if (strcmp(tmp,"gr_tile-bridge-l")==0)  {
                op->m_parameters.Add(new int(0));
           }
           else if (strcmp(tmp,"gr_tile-bridge-m")==0)  {
                op->m_parameters.Add(new int(1));
           }
           else if (strcmp(tmp,"gr_tile-bridge-r")==0)  {
                op->m_parameters.Add(new int(2));
           }
           else if (strcmp(tmp,"gr_tilegreen-bridge-l")==0)  {
                op->m_parameters.Add(new int(3));
           }
           else if (strcmp(tmp,"gr_tilegreen-bridge-m")==0)  {
                op->m_parameters.Add(new int(4));
           }
           else if (strcmp(tmp,"gr_tilegreen-bridge-r")==0)  {
                op->m_parameters.Add(new int(5));
           }
           else if (strcmp(tmp,"gr_tileblue-bridge-l")==0)  {
                op->m_parameters.Add(new int(6));
           }
           else if (strcmp(tmp,"gr_tileblue-bridge-m")==0)  {
                op->m_parameters.Add(new int(7));
           }
           else if (strcmp(tmp,"gr_tileblue-bridge-r")==0)  {
                op->m_parameters.Add(new int(8));
           }
           else if (strcmp(tmp,"gr_tileyellow-bridge-l")==0)  {
                op->m_parameters.Add(new int(9));
           }
           else if (strcmp(tmp,"gr_tileyellow-bridge-m")==0)  {
                op->m_parameters.Add(new int(10));
           }
           else if (strcmp(tmp,"gr_tileyellow-bridge-r")==0)  {
                op->m_parameters.Add(new int(11));
           }
           m_objects.Add(op);

        }


    }

    fscanf(fp, "%i", &no);
    for (i = 0;i < no;i++) {
        op = new GObjectPlace();
        fscanf(fp, "%i %i %s", &x, &y, tmp);
        op->m_x = (int)(x * scale);
        op->m_y = (int)(y * scale);
        op->m_object_name = new Symbol(tmp);
        fscanf(fp, "%i", &np);
        for (j = 0;j < np;j++) {
            fscanf(fp, "%i", &i_tmp);
            op->m_parameters.Add(new int(i_tmp));
        }
        m_objects.Add(op);
    }
}

GMapLayer::~GMapLayer()
{
}

void GMapLayer::reset_first(int sfx_volume, GMap *map)
{
    GObject *o;
    GObjectPlace *po;

    m_running_objects.Delete();

    m_objects.Rewind();
    while (m_objects.Iterate(po)) {
        o = GObject_create(po->m_object_name, po->m_x, po->m_y, sfx_volume, &(po->m_parameters));
        m_running_objects.Add(o);

        // Water tiles need a pointer to the map to retrieve some data required for rendering water:
        if (water_reflection && (o->is_a(water_symbol) || o->is_a(lava_symbol))) {
            GO_water *ow = (GO_water *)o;
            ow->set_map(map);
        }
    }
}

void GMapLayer::reset(int sfx_volume)
{
    GObject *o;
    GObjectPlace *po;

    {
        List<GObject> to_delete;
        m_running_objects.Rewind();
        while (m_running_objects.Iterate(o)) {
            if (o->is_a(enemy_symbol) && !o->is_a(fratelli_symbol))
                to_delete.Add(o);
        }
        while (!to_delete.EmptyP()) {
            o = to_delete.ExtractIni();
            m_running_objects.DeleteElement(o);
            delete o;
        }
    }

    m_objects.Rewind();
    while (m_objects.Iterate(po)) {
        o = GObject_create(po->m_object_name, po->m_x, po->m_y, sfx_volume, &(po->m_parameters));
        if (o->is_a(enemy_symbol)) {
            m_running_objects.Add(o);
        } else {
            delete o;
        }
    }
}

void GMapLayer::cycle(VirtualController *v, GMap *m, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    List<GObject> to_delete;
    GObject *o;

    m_running_objects.Rewind();
    while (m_running_objects.Iterate(o)) {
        if (!o->cycle(v, m, layer, game, GLTM, SFXM))
            to_delete.Add(o);
    }

    while (!to_delete.EmptyP()) {
        o = to_delete.ExtractIni();
		SFXM->object_destroyed_notification(o);
        m_running_objects.DeleteElement(o);
        delete o;
    }

    m_auxiliar_objects.Rewind();
    while (m_auxiliar_objects.Iterate(o)) {
        if (!o->cycle(v, m, layer, game, GLTM, SFXM))
            to_delete.Add(o);
    }

    while (!to_delete.EmptyP()) {
        o = to_delete.ExtractIni();
        m_auxiliar_objects.DeleteElement(o);
        delete o;
    }
}

void GMapLayer::cycle_clock(VirtualController *v, GMap *m, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    List<GObject> to_delete;
    GObject *o;

    m_running_objects.Rewind();
    while (m_running_objects.Iterate(o)) {
        if (o->is_a(character_symbol) ||
                o->is_a(item_symbol) ||
                o->is_a(key_symbol) ||
                o->is_a(cagedoor_symbol) ||
                o->is_a(entrydoor_symbol) ||
                o->is_a(exitdoor_symbol)) {
            if (!o->cycle(v, m, layer, game, GLTM, SFXM))
                to_delete.Add(o);
        }
    }

    while (!to_delete.EmptyP()) {
        o = to_delete.ExtractIni();
		SFXM->object_destroyed_notification(o);
        m_running_objects.DeleteElement(o);
        delete o;
    }

    m_auxiliar_objects.Rewind();
    while (m_auxiliar_objects.Iterate(o)) {
        if (o->is_a(character_symbol) ||
                o->is_a(item_symbol) ||
                o->is_a(key_symbol) ||
                o->is_a(cagedoor_symbol) ||
                o->is_a(entrydoor_symbol) ||
                o->is_a(exitdoor_symbol)) {
            if (!o->cycle(v, m, layer, game, GLTM, SFXM))
                to_delete.Add(o);
        }
    }

    while (!to_delete.EmptyP()) {
        o = to_delete.ExtractIni();
        m_auxiliar_objects.DeleteElement(o);
        delete o;
    }
}

void GMapLayer::draw(GLTManager *GLTM)
{
    GMapTilePlace *tp;
    GObject *o;

    m_tiles.Rewind();
    while (m_tiles.Iterate(tp)) {
        if (tp->m_tile_cache==0) tp->m_tile_cache=GLTM->get(tp->m_tile_name);
		tp->m_tile_cache->draw(float(tp->m_x), float(tp->m_y), 0, 0, 1);
    }
    m_running_objects.Rewind();
    while (m_running_objects.Iterate(o))
        o->draw(GLTM);

    m_auxiliar_objects.Rewind();
    while (m_auxiliar_objects.Iterate(o))
        o->draw(GLTM);
}

void GMapLayer::add_object(GObject *o)
{
    if (!m_running_objects.MemberRefP(o))
        m_running_objects.Add(o);
}

bool GMapLayer::object_exists(GObject *o)
{
	if (m_running_objects.MemberRefP(o)) return true;

	return false;
} /* GMapLayer::object_exists */


void GMapLayer::add_auxiliar_object(GObject *o)
{
    if (!m_auxiliar_objects.MemberRefP(o))
        m_auxiliar_objects.Add(o);
}

bool GMapLayer::remove_object(GObject *o)
{
    return m_running_objects.DeleteElement(o);
}

void GMapLayer::add_tile(char *name, int x, int y)
{
    GMapTilePlace *t;

    t = new GMapTilePlace();
    t->m_tile_name = new Symbol(name);
	t->m_tile_cache = 0;
    t->m_x = x;
    t->m_y = y;

    m_tiles.Add(t);
}

bool GMapLayer::collision(GObject *o2, GLTManager *GLTM)
{
    GMapTilePlace *tp;
    GObject *o;
    List<GObject> l;

    m_tiles.Rewind();
    while (m_tiles.Iterate(tp)) {
		if (tp->m_tile_cache==0) tp->m_tile_cache = GLTM->get(tp->m_tile_name);
        if (o2->collision(tp->m_tile_cache, tp->m_x, tp->m_y)) return true;
    }

    l.Instance(m_running_objects);
    l.Rewind();
    while (l.Iterate(o))
        if (o != o2 && o2->collision(o))
            return true;

    return false;
}

bool GMapLayer::collision_with_background(GObject *o2, GLTManager *GLTM)
{
    GMapTilePlace *tp;
    GObject *o;
    List<GObject> l;

    m_tiles.Rewind();
    while (m_tiles.Iterate(tp)) {
		if (tp->m_tile_cache==0) tp->m_tile_cache = GLTM->get(tp->m_tile_name);
        if (o2->collision(tp->m_tile_cache, tp->m_x, tp->m_y)) return true;
    }

    // The big rock and bridges are part of the background:
    l.Instance(m_running_objects);
    l.Rewind();
    while (l.Iterate(o))
        if ((o->is_a(bigrock_symbol) || o->is_a(bridge_symbol)) && o != o2 && o2->collision(o))
            return true;
	
    return false;
}


bool GMapLayer::collision_with_background(GObject *o2, int x_offs, int y_offs, GLTManager *GLTM)
{
    GMapTilePlace *tp;
    GObject *o;
    List<GObject> l;

    m_tiles.Rewind();
    while (m_tiles.Iterate(tp)) {
		if (tp->m_tile_cache==0) tp->m_tile_cache=GLTM->get(tp->m_tile_name);
        if (o2->collision(x_offs, y_offs, tp->m_tile_cache, tp->m_x, tp->m_y)) return true;
    }

    // the big rock and bridges are part of the background:
    l.Instance(m_running_objects);
    l.Rewind();
    while (l.Iterate(o))
        if ((o->is_a(bigrock_symbol) || o->is_a(bridge_symbol)) && o != o2 && o2->collision(x_offs,y_offs,o)) {
            return true;
        }

    return false;
}

bool GMapLayer::collision_with_background(GLTile *t, int x, int y, GLTManager *GLTM)
{
    GMapTilePlace *tp;

    m_tiles.Rewind();
    while (m_tiles.Iterate(tp)) {
		if (tp->m_tile_cache==0) tp->m_tile_cache=GLTM->get(tp->m_tile_name);
        if (t->get_cmc()->collision_no_rotation(float(x), float(y),
												tp->m_tile_cache->get_cmc(),
												float(tp->m_x), float(tp->m_y))) return true;
    }

    return false;
}

bool GMapLayer::collision_with_background_wo_bigrocks(GObject *o2, int x_offs, int y_offs, GLTManager *GLTM)
{
    GMapTilePlace *tp;
    List<GObject> l;

    m_tiles.Rewind();
    while (m_tiles.Iterate(tp)) {
		if (tp->m_tile_cache==0) tp->m_tile_cache=GLTM->get(tp->m_tile_name);
        if (o2->collision(x_offs, y_offs, tp->m_tile_cache, tp->m_x, tp->m_y)) return true;
    }

    return false;
}

GObject *GMapLayer::collision_with_object(GObject *o2, GLTManager *GLTM)
{
    GObject *o;
    List<GObject> l;

    l.Instance(m_running_objects);
    l.Rewind();
    while (l.Iterate(o))
        if (o->collision(o2))
            return o;

    return 0;
} /* GMapLayer::collision_with_object */

GObject *GMapLayer::collision_with_object(GObject *o2, GLTManager *GLTM, char *type)
{
    GObject *o;
    List<GObject> l;

    l.Instance(m_running_objects);
    l.Rewind();
    while (l.Iterate(o))
        if (o != o2 && o->is_a(type) && o->collision(o2))
            return o;

    return 0;
} /* GMapLayer::collision_with_object */

GObject *GMapLayer::collision_with_object(GObject *o2, GLTManager *GLTM, Symbol *type)
{
    GObject *o;
    List<GObject> l;

    l.Instance(m_running_objects);
    l.Rewind();
    while (l.Iterate(o))
        if (o != o2 && o->is_a(type) && o->collision(o2))
            return o;

    return 0;
} /* GMapLayer::collision_with_object */


GObject *GMapLayer::collision_with_object(GObject *o2, int x_offs, int y_offs, GLTManager *GLTM, char *type)
{
    GObject *o;
    List<GObject> l;

    l.Instance(m_running_objects);
    l.Rewind();
    while (l.Iterate(o))
        if (o != o2 && o->is_a(type) && o2->collision(x_offs,y_offs,o))
            return o;

    return 0;
} /* GMapLayer::collision_with_object */


GObject *GMapLayer::collision_with_object(GObject *o2, int x_offs, int y_offs, GLTManager *GLTM, Symbol *type)
{
    GObject *o;
    List<GObject> l;

    l.Instance(m_running_objects);
    l.Rewind();
    while (l.Iterate(o))
        if (o != o2 && o->is_a(type) && o2->collision(x_offs,y_offs,o))
            return o;

    return 0;
} /* GMapLayer::collision_with_object */


GObject *GMapLayer::collision_with_object(GLTile *t, int x, int y, GLTManager *GLTM, char *type)
{
    GObject *o;
    List<GObject> l;

    l.Instance(m_running_objects);
    l.Rewind();
    while (l.Iterate(o))
        if (o->is_a(type) && o->collision(0, 0, t, x, y))
            return o;

    return 0;
} /* GMapLayer::collision_with_object */


GObject *GMapLayer::collision_with_object(GLTile *t, int x, int y, GLTManager *GLTM, Symbol *type)
{
    GObject *o;
    List<GObject> l;

    l.Instance(m_running_objects);
    l.Rewind();
    while (l.Iterate(o))
        if (o->is_a(type) && o->collision(0, 0, t, x, y))
            return o;

    return 0;
} /* GMapLayer::collision_with_object */


GObject *GMapLayer::get_object(char *type)
{
    GObject *o;
    List<GObject> l;

    l.Instance(m_running_objects);
    l.Rewind();
    while (l.Iterate(o))
        if (o->is_a(type))
            return o;

    return 0;
}


GObject *GMapLayer::get_object(Symbol *type)
{
    GObject *o;
    List<GObject> l;

    l.Instance(m_running_objects);
    l.Rewind();
    while (l.Iterate(o))
        if (o->is_a(type))
            return o;

    return 0;
}


void GMapLayer::get_objects(List<GObject> *res, char *type)
{
    GObject *o;
    List<GObject> l;

    l.Instance(m_running_objects);
    l.Rewind();
    while (l.Iterate(o))
        if (o->is_a(type))
            res->Add(o);
} /* GMapLayer::get_objects */


void GMapLayer::get_objects(List<GObject> *res, Symbol *type)
{
    GObject *o;
    List<GObject> l;

    l.Instance(m_running_objects);
    l.Rewind();
    while (l.Iterate(o))
        if (o->is_a(type))
            res->Add(o);
} /* GMapLayer::get_objects */


GMapTilePlace::~GMapTilePlace()
{
	if (m_tile_name!=0) delete m_tile_name;
	m_tile_name=0;
	m_tile_cache=0;
} /* GMapTilePlace::~GMapTilePlace */


GMapCMCPlace::~GMapCMCPlace()
{
	m_cmc=0;
} /* GMapCMCPlace::~GMapCMCPlace */


GObjectPlace::~GObjectPlace()
{
	if (m_object_name!=0) delete m_object_name;
	m_object_name=0;
} /* GObjectPlace::~GObjectPlace */

