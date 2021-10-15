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

#include "GO_item.h"
#include "GObjectCreator.h"


GO_item::GO_item(int x, int y, int sfx_volume, int type) : GObject(x, y, sfx_volume)
{
    m_class = new Symbol(item_symbol);

    m_type = type;

    m_state_cycle = 0;
} /* GObject::GObject */


bool GO_item::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    m_state_cycle++;
    if (m_state == 1) {
        m_state = 2;
        m_state_cycle = 0;
    } // if
    if (m_state == 2) {
        if (m_state_cycle > 8)
            return false;
    } // if
    return true;
} /* GO_item::cycle */


void GO_item::draw(GLTManager *GLTM)
{
    int s = (m_state_cycle / 8) % 3;

    switch (m_type) {
        case 0:
            if (s == 0)
                m_last_tile_used = GLTM->get
                                   ("it_goonie1");
            else
                m_last_tile_used = GLTM->get
                                   ("it_goonie2");
            break;
        case 1:
            if (s == 0)
                m_last_tile_used = GLTM->get
                                   ("it_greenbottle1");
            if (s == 1)
                m_last_tile_used = GLTM->get
                                   ("it_greenbottle2");
            if (s == 2)
                m_last_tile_used = GLTM->get
                                   ("it_greenbottle3");
            break;
        case 2:  // m_last_tile_used=GLTM->get("it_yellowhelmet");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 3:  // m_last_tile_used=GLTM->get("it_shoes");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 4:  //m_last_tile_used=GLTM->get("it_greycoat");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 5:  // m_last_tile_used=GLTM->get("it_yellowcoat");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 6:  //m_last_tile_used=GLTM->get("it_hammer");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 7:
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 8:  //m_last_tile_used=GLTM->get("it_lamp");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 9:  //m_last_tile_used=GLTM->get("it_greenbook");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 10: //m_last_tile_used=GLTM->get("it_redbook");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 11: //m_last_tile_used=GLTM->get("it_lightbluebook");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 12: //m_last_tile_used=GLTM->get("it_bluebook");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 13: //m_last_tile_used=GLTM->get("it_greencoat");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 14: //m_last_tile_used=GLTM->get("it_whitebook");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 15: //m_last_tile_used=GLTM->get("it_yellowshield");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 16:
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 17: //m_last_tile_used=GLTM->get("it_lightbluecoat");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 18: //m_last_tile_used=GLTM->get("it_whiteshield");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 19: //m_last_tile_used=GLTM->get("it_redanti");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 20: //m_last_tile_used=GLTM->get("it_purplebook");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 21: //m_last_tile_used=GLTM->get("it_lightbluehelmet");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 22: //m_last_tile_used=GLTM->get("it_yelowbook");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 23: //m_last_tile_used=GLTM->get("it_purpleanti");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 24: //m_last_tile_used=GLTM->get("it_purpleshield");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 25: //m_last_tile_used=GLTM->get("it_clock");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 26: //m_last_tile_used=GLTM->get("it_blueanti");
            m_last_tile_used = GLTM->get
                               ("it_bag");
            break;
        case 27:
            if (s == 0)
                m_last_tile_used = GLTM->get
                                   ("it_goonie_mike1");
            else
                m_last_tile_used = GLTM->get
                                   ("it_goonie_mike2");
            break;
        case 28:
            if (s == 0)
                m_last_tile_used = GLTM->get
                                   ("it_goonie_mouth1");
            else
                m_last_tile_used = GLTM->get
                                   ("it_goonie_mouth2");
            break;
        case 29:
            if (s == 0)
                m_last_tile_used = GLTM->get
                                   ("it_goonie_data1");
            else
                m_last_tile_used = GLTM->get
                                   ("it_goonie_data2");
            break;
        case 30:
            if (s == 0)
                m_last_tile_used = GLTM->get
                                   ("it_goonie_chunk1");
            else
                m_last_tile_used = GLTM->get
                                   ("it_goonie_chunk2");
            break;
        case 31:
            if (s == 0)
                m_last_tile_used = GLTM->get
                                   ("it_goonie_brand1");
            else
                m_last_tile_used = GLTM->get
                                   ("it_goonie_brand2");
            break;
        case 32:
            if (s == 0)
                m_last_tile_used = GLTM->get
                                   ("it_goonie_andy1");
            else
                m_last_tile_used = GLTM->get
                                   ("it_goonie_andy2");
            break;
        case 33:
            if (s == 0)
                m_last_tile_used = GLTM->get
                                   ("it_goonie_steff1");
            else
                m_last_tile_used = GLTM->get
                                   ("it_goonie_steff2");
            break;
        default:
            m_last_tile_used = GLTM->get
                               ("it_goonie");
            break;
    } // switch
    float f;

    if (m_state == 0 || m_state == 1) {
        f = float(0.8F + 0.2F * sin(m_state_cycle * 0.2f));
    } else {
        f = (8 - m_state_cycle) / 8.0F;
    } // if
    if (f > 1)
        f = 1;
    if (f < 0)
        f = 0;

	// we don't want flashing goonies
	if (m_type >= 27 && m_type <= 33) {
		f = 1;
	}
	
	m_last_tile_used->draw(1, 1, 1, f, m_x, m_y, 0, 0, 1);
} /* GO_item::draw */


bool GO_item::is_a(Symbol *c)
{
    if (c->cmp(item_symbol))
        return true;

    return GObject::is_a(c);
} /* GO_item::is_a */


bool GO_item::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
} /* GO_item::is_a */
