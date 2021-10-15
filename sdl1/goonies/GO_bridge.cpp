#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#include "debug.h"

#ifdef _WIN32
#include "windows.h"
#endif

#include <iostream>
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
#include "GO_bridge.h"

#include "GMap.h"

#include "GObjectCreator.h"


GO_bridge::GO_bridge(int x, int y, int sfx_volume, int type) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(bridge_symbol);

    m_state_cycle = 0;
    m_type = type;
	m_last_tile_used = 0;
	m_y_offset = 0;
	if (m_type==1 || m_type==4 || m_type==7 || m_type==10) {
		m_forced_offset=false;
		m_y_forced_offset = 0;
	} else {
		m_forced_offset=true;
		m_y_forced_offset = 0;
	}

	m_searched_for_bridges=false;
	m_left_bridge=0;
	m_right_bridge=0;

	m_someone_over_in_the_last_cycle=false;

	m_y_old_offset = m_y_offset;
} /* GObject::GObject */


bool GO_bridge::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    m_state_cycle++;

	{
		// Find out if a character or Fratelli is over the bridge:
		if (m_type==1 || m_type==4 || m_type==7 || m_type==10) {
			GObject *o = map->collision_with_object(this,0,-4,GLTM,"GO_character");
			if (o==0) o = map->collision_with_object(this,0,-4,GLTM,"GO_fratelli");
			if (o==0) o = map->collision_with_object(this,0,-4,GLTM,"GO_skeleton");
			if (o!=0) {

				if (!m_someone_over_in_the_last_cycle) {
					SFXM->SFX_play("sfx/creak", m_sfx_volume, get_angle(map), get_distance(map));
				} // if

				m_someone_over_in_the_last_cycle=true;

				GO_bridge *lb=this, *rb=this;
				int left_offset_y = 0, right_offset_y = 0;
				while (lb != 0 && left_offset_y < 4) {
					if (!lb->m_searched_for_bridges) {
						lb->m_left_bridge = (GO_bridge *)map->collision_with_object(lb,-4,0,GLTM,"GO_bridge");
						lb->m_right_bridge = (GO_bridge *)map->collision_with_object(lb,4,0,GLTM,"GO_bridge");
						lb->m_searched_for_bridges=true;
					} // if
					lb = lb->m_left_bridge;
					if (lb != 0) left_offset_y++;
				} // while
				while (rb != 0 && right_offset_y < 4) {
					if (!rb->m_searched_for_bridges) {
						rb->m_left_bridge = (GO_bridge *)map->collision_with_object(rb,-4,0,GLTM,"GO_bridge");
						rb->m_right_bridge = (GO_bridge *)map->collision_with_object(rb,4,0,GLTM,"GO_bridge");
						rb->m_searched_for_bridges=true;
					} // if
					rb = rb->m_right_bridge;
					if (rb != 0) right_offset_y++;
				} // while
				m_forced_offset=true;
				m_y_forced_offset = (left_offset_y < right_offset_y)?left_offset_y:right_offset_y;
				m_y_offset = m_y_forced_offset;
			} else {

				m_forced_offset=false;
				m_someone_over_in_the_last_cycle=false;

				// check if there is someone over the bridge:
				{
					GO_bridge *tmp = this;
					while(tmp!=0) {
						if (tmp->m_forced_offset && tmp->m_y_forced_offset>0) m_someone_over_in_the_last_cycle=true;
						if (!tmp->m_searched_for_bridges) {
							tmp->m_left_bridge = (GO_bridge *)map->collision_with_object(tmp,-4,0,GLTM,"GO_bridge");
							tmp->m_right_bridge = (GO_bridge *)map->collision_with_object(tmp,4,0,GLTM,"GO_bridge");
							tmp->m_searched_for_bridges=true;
						} // if
						tmp = tmp->m_left_bridge;
					} // while
					tmp = this;
					while(tmp!=0) {
						if (tmp->m_forced_offset && tmp->m_y_forced_offset>0) m_someone_over_in_the_last_cycle=true;
						if (!tmp->m_searched_for_bridges) {
							tmp->m_left_bridge = (GO_bridge *)map->collision_with_object(tmp,-4,0,GLTM,"GO_bridge");
							tmp->m_right_bridge = (GO_bridge *)map->collision_with_object(tmp,4,0,GLTM,"GO_bridge");
							tmp->m_searched_for_bridges=true;
						} // if
						tmp = tmp->m_right_bridge;
					} // while
				}
			} // if

			// Transfer the tension across the bridge:
			if (!m_forced_offset) {
				GO_bridge *lb=this,*rb=this,*tmp;
				int forced_left_distance = 0;
				float forced_left = 0.0f;
				int forced_right_distance = 0;
				float forced_right = 0.0f;

				// find the closest forced to the left:
				while(lb!=0 && !lb->m_forced_offset) {
					if (!lb->m_searched_for_bridges) {
						lb->m_left_bridge = (GO_bridge *)map->collision_with_object(lb,-4,0,GLTM,"GO_bridge");
						lb->m_right_bridge = (GO_bridge *)map->collision_with_object(lb,4,0,GLTM,"GO_bridge");
						lb->m_searched_for_bridges=true;
					} // if
					tmp = lb->m_left_bridge;
					if (tmp!=0) {
						forced_left_distance++;
					} // if
					lb = tmp;
				} // while
				if (lb!=0) forced_left = lb->m_y_forced_offset;

				// find the closest forced to the right:
				while(rb!=0 && !rb->m_forced_offset) {
					if (!rb->m_searched_for_bridges) {
						rb->m_left_bridge = (GO_bridge *)map->collision_with_object(rb,-4,0,GLTM,"GO_bridge");
						rb->m_right_bridge = (GO_bridge *)map->collision_with_object(rb,4,0,GLTM,"GO_bridge");
						rb->m_searched_for_bridges=true;
					} // if
					tmp = rb->m_right_bridge;
					if (tmp!=0) {
						forced_right_distance++;
					} // if
					rb = tmp;
				} // while

				if (rb!=0) forced_right = rb->m_y_forced_offset;

				if ((forced_left_distance + forced_right_distance)>0) {
					float f = float(forced_left_distance)/float(forced_left_distance+forced_right_distance);
					m_y_offset = forced_left+f*(forced_right-forced_left);
				} else {
					m_y_offset = (forced_left+forced_right)/2;
				} // if

			} // if
		
		} else {
			m_forced_offset=true;
			m_y_forced_offset = 0;
			m_y_offset = 0;
		} // if

		if (m_y_offset>(m_y_old_offset+0.5f)) m_y_offset = m_y_old_offset+0.5f;
		if (m_y_offset<(m_y_old_offset-0.5f)) m_y_offset = m_y_old_offset-0.5f;

		m_y_old_offset = m_y_offset;

	}

    return true;
} /* GO_bridge::cycle */


void GO_bridge::draw(GLTManager *GLTM)
{
	if (m_last_tile_used==0) {
		switch (m_type) {
			case 0:
				m_last_tile_used = GLTM->get("gr_tile-bridge-l");
				break;
			case 1:
				m_last_tile_used = GLTM->get("gr_tile-bridge-m");
				break;
			case 2:
				m_last_tile_used = GLTM->get("gr_tile-bridge-r");
				break;
			case 3:
				m_last_tile_used = GLTM->get("gr_tilegreen-bridge-l");
				break;
			case 4:
				 m_last_tile_used = GLTM->get("gr_tilegreen-bridge-m");
				break;
			case 5:
				 m_last_tile_used = GLTM->get("gr_tilegreen-bridge-r");
				break;
			case 6:
				 m_last_tile_used = GLTM->get("gr_tileblue-bridge-l");
				break;
			case 7:
				 m_last_tile_used = GLTM->get("gr_tileblue-bridge-m");
				break;
			case 8:
				 m_last_tile_used = GLTM->get("gr_tileblue-bridge-r");
				break;
			case 9:
				 m_last_tile_used = GLTM->get("gr_tileyellow-bridge-l");
				break;
			case 10:
				 m_last_tile_used = GLTM->get("gr_tileyellow-bridge-m");
				break;
			case 11:
				 m_last_tile_used = GLTM->get("gr_tileyellow-bridge-r");
				  break;          
		} // switch
	} // if

      m_last_tile_used->draw(m_x, m_y+m_y_offset, 0, 0, 1);

} /* GO_bridge::draw */



bool GO_bridge::is_a(Symbol *c)
{
    if (c->cmp(bridge_symbol))
        return true;

    return GObject::is_a(c);
} /* GO_bridge::is_a */


bool GO_bridge::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
} /* GO_bridge::is_a */
