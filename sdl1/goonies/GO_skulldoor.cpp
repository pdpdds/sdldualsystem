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

#include "GO_skulldoor.h"
#include "GO_character.h"
#include "GMap.h"

#include "GObjectCreator.h"


enum {
	SKULLDOOR_STATE_OPEN,
	SKULLDOOR_STATE_CLOSE,
};


GO_skulldoor::GO_skulldoor(int x, int y, int sfx_volume) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(skulldoor_symbol);

	m_state = SKULLDOOR_STATE_CLOSE;
    m_state_cycle = 0;
    m_destination_x = 0;
    m_destination_y = 0;
    m_destination_door = 0;
	m_opening_counter = 7;
	m_glow_counter = 0;
} /* GObject::GObject */


GO_skulldoor::GO_skulldoor(int x, int y, int sfx_volume, int dx, int dy, int dd) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(skulldoor_symbol);

	m_state = SKULLDOOR_STATE_CLOSE;
    m_state_cycle = 0;
    m_destination_x = dx;
    m_destination_y = dy;
    m_destination_door = dd;
	m_opening_counter = 7;
	m_glow_counter = 0;
} /* GObject::GObject */


bool GO_skulldoor::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
	GO_character *c = (GO_character *)map->get_object(character_symbol);
	float h_distance = get_x() - c->get_x();
	float v_distance = get_y() - c->get_y();

	m_state_cycle++;
	switch (m_state) {
		case SKULLDOOR_STATE_OPEN:
			if ((h_distance > 80 || h_distance < -80) || (v_distance > 80 || v_distance < -70)) {
				m_state = SKULLDOOR_STATE_CLOSE;
				m_state_cycle = 0;
				SFXM->SFX_play("sfx/skulldoor_close", m_sfx_volume, get_angle(map), get_distance(map));
			}
			else {
				if ((m_opening_counter > 0) && !(m_state_cycle % 6))
					--m_opening_counter;
				if ((h_distance < 20 && h_distance > -20) && (v_distance < 20 && v_distance > -20)) {
					m_opening_counter = 0;
				}
			}
			break;
		case SKULLDOOR_STATE_CLOSE:
			if ((h_distance < 80 && h_distance > -80) && (v_distance < 80 && v_distance > -70)) {
				m_state = SKULLDOOR_STATE_OPEN;
				m_state_cycle = 0;
				SFXM->SFX_play("sfx/skulldoor_open", m_sfx_volume, get_angle(map), get_distance(map));
				if (m_opening_counter > 5) m_opening_counter = 5;
			}
			else {
				if ((m_opening_counter < 7) && !(m_state_cycle % 6))
					m_opening_counter++;
			}
			break;
	}

	if (m_opening_counter != 0)
		m_glow_counter = 0;
	else
		++m_glow_counter;

    return true;
} /* GO_skulldoor::cycle */


void GO_skulldoor::draw(GLTManager *GLTM)
{
	if (m_opening_counter == 0)
		m_last_tile_used  = GLTM->get("ob_skull-entrance_01");
	if (m_opening_counter == 1)
		m_last_tile_used  = GLTM->get("ob_skull-entrance_02");
	if (m_opening_counter == 2)
		m_last_tile_used  = GLTM->get("ob_skull-entrance_03");
	if (m_opening_counter == 3)
		m_last_tile_used  = GLTM->get("ob_skull-entrance_04");
	if (m_opening_counter == 4)
		m_last_tile_used  = GLTM->get("ob_skull-entrance_05");
	if (m_opening_counter == 5)
		m_last_tile_used  = GLTM->get("ob_skull-entrance_06");
	if (m_opening_counter == 6)
		m_last_tile_used  = GLTM->get("ob_skull-entrance_05");
	if (m_opening_counter >= 7)
		m_last_tile_used  = GLTM->get("ob_skull-entrance_06");

	if (m_last_tile_used != 0)
		m_last_tile_used->draw(m_x, m_y, 0, 0, 1);

	if (m_glow_counter != 0) {
		float glow_alpha = (sin(float(m_glow_counter + 175) / 16.0f) + 1.0f)/2.0f;
		GLTile *glow_tile = GLTM->get("ob_skull-entrance_glow");
		
		glow_tile->draw(1, 1, 1, glow_alpha, m_x, m_y, 0, 0, 1);
	}
} /* GO_skulldoor::draw */


bool GO_skulldoor::is_a(Symbol *c)
{
    if (c->cmp(skulldoor_symbol))
        return true;

    return GObject::is_a(c);
} /* GO_skulldoor::is_a */


bool GO_skulldoor::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
} /* GO_skulldoor::is_a */
