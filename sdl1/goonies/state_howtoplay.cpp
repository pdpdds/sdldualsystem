#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include "windows.h"
#endif

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_mixer.h"

#include "List.h"
#include "Symbol.h"
#include "2DCMC.h"
#include "auxiliar.h"
#include "GLTile.h"
#include "2DCMC.h"
#include "keyboardstate.h"
#include "randomc.h"
#include "VirtualController.h"

#include "GLTManager.h"
#include "SoundManager.h"
#include "SFXManager.h"
#include "MusicManager.h"
#include "GObject.h"
#include "GO_enemy.h"
#include "GO_fratelli.h"
#include "GO_character.h"
#include "GMap.h"
#include "TheGoonies.h"
#include "TheGooniesApp.h"

#include "font_extractor.h"

#define TITLE_INTRO_TIME 300 
//#define TITLE_INTRO_TIME 50

extern bool fullscreen;
extern SDL_Surface *screen_sfc;
extern SDL_Surface *toogle_video_mode(bool fullscreen);
extern int difficulty;

int TheGooniesApp::howtoplay_cycle(KEYBOARDSTATE *k)
{
    if (m_test_game == 0) {
        m_test_game = new TheGoonies(0, 0, 0, 0, 0, 0, m_GLTM);
        m_test_game->get_character()->set_rescued_goonies(6);
        m_test_game->get_character()->set_goonie_is_rescued(0, true);
        m_test_game->get_character()->set_goonie_is_rescued(1, true);
        m_test_game->get_character()->set_goonie_is_rescued(2, false);
        m_test_game->get_character()->set_goonie_is_rescued(3, true);
        m_test_game->get_character()->set_goonie_is_rescued(4, true);
        m_test_game->get_character()->set_goonie_is_rescued(5, true);
        m_test_game->get_character()->set_goonie_is_rescued(6, true);
        m_howtoplay_difficulty = difficulty;
        difficulty = 100;
    }

    switch (m_howtoplay_state) {
        case 0:  // appearing
            if (m_howtoplay_cycle >= 50) {
                m_howtoplay_state = 1;
                m_howtoplay_cycle = 0;
                m_state_cycle = 0;
            }
			m_howtoplay_cycle++;
            break;
        case 1:  // animation
            m_howtoplay_cycle++;
            if (m_howtoplay_cycle >= 2150 ||
                    (k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]) ||
                    (k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
					(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)]))
			{
                m_howtoplay_state = 2;
                m_howtoplay_cycle = 2;
            } else {
                VirtualController *k = new VirtualController();
                k->reset();

                // move
                if (m_howtoplay_cycle > 100 && m_howtoplay_cycle < 150)
                    k->m_joystick[VC_RIGHT] = true;
                if (m_howtoplay_cycle > 150 && m_howtoplay_cycle < 200)
                    k->m_joystick[VC_LEFT] = true;

                // jump
                if (m_howtoplay_cycle == 250)
                    k->m_joystick[VC_UP] = true;

                // take key
                if (m_howtoplay_cycle > 350 && m_howtoplay_cycle < 450)
                    k->m_joystick[VC_RIGHT] = true;

                // climb vine (and avoid being hit by the enemies)
                if (m_howtoplay_cycle > 500 && m_howtoplay_cycle < 635)
                    k->m_joystick[VC_LEFT] = true;
                if (m_howtoplay_cycle > 635 && m_howtoplay_cycle < 700)
                    k->m_joystick[VC_DOWN] = true;
                if (m_howtoplay_cycle > 800 && m_howtoplay_cycle < 850)
                    k->m_joystick[VC_DOWN] = true;

                // punch
                if (m_howtoplay_cycle > 850 && m_howtoplay_cycle < 920)
                    k->m_joystick[VC_RIGHT] = true;
                if (m_howtoplay_cycle == 950)
                    k->m_button[0] = true;
                if (m_howtoplay_cycle == 975)
                    k->m_button[0] = true;
                if (m_howtoplay_cycle == 1000)
                    k->m_button[0] = true;
                if (m_howtoplay_cycle == 1025)
                    k->m_button[0] = true;

                // kill skull
                if (m_howtoplay_cycle == 1230)
                    k->m_button[0] = true;

                // open cage
                if (m_howtoplay_cycle > 1250 && m_howtoplay_cycle < 1310)
                    k->m_joystick[VC_RIGHT] = true;

                // rescue goonie
                if (m_howtoplay_cycle > 1400 && m_howtoplay_cycle < 1450)
                    k->m_joystick[VC_RIGHT] = true;

                // enter skulldoor
                if (m_howtoplay_cycle > 1550 && m_howtoplay_cycle < 1665)
                    k->m_joystick[VC_RIGHT] = true;
                if (m_howtoplay_cycle == 1750)
                    k->m_joystick[VC_UP] = true;

                // Walk to exit door
                if (m_howtoplay_cycle > 1900 && m_howtoplay_cycle < 1970)
                    k->m_joystick[VC_RIGHT] = true;
                if (m_howtoplay_cycle == 2100)
                    k->m_joystick[VC_UP] = true;

                m_test_game->cycle(k, m_GLTM, m_SFXM, m_MusicM);
                delete k;
            } /* if */
            break;
        case 2:  // disppearing
            m_howtoplay_cycle++;
            if (m_howtoplay_cycle >= 50) {
                m_howtoplay_state = 3;
                m_howtoplay_cycle = 0;
            } // if
            break;
		case 3: // Appearing screen 1 of special items:
            m_howtoplay_cycle++;
            if (m_howtoplay_cycle >= 75) {
                m_howtoplay_state = 4;
                m_howtoplay_cycle = 0;
			} // if
			break;
		case 4: // waiting...
            m_howtoplay_cycle++;
            if (m_howtoplay_cycle >= 1000 ||
                (k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]) ||
                (k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
				(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)])) {
                m_howtoplay_state = 5;
                m_howtoplay_cycle = 0;
			} // if
			break;
		case 5: // Disappearing screen 1 of special items:
            m_howtoplay_cycle++;
            if (m_howtoplay_cycle >= 75) {
                m_howtoplay_state = 6;
                m_howtoplay_cycle = 0;
            } // if
			break;
		case 6: // Appearing screen 2 of special items:
            m_howtoplay_cycle++;
            if (m_howtoplay_cycle >= 75) {
                m_howtoplay_state = 7;
                m_howtoplay_cycle = 0;
			} // if
			break;
		case 7: // waiting...
            m_howtoplay_cycle++;
            if (m_howtoplay_cycle >= 1000 ||
                (k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]) ||
                (k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
				(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)])) {
                m_howtoplay_state = 8;
                m_howtoplay_cycle = 0;
			} // if
			break;
		case 8: // Disappearing screen 2 of special items:
            m_howtoplay_cycle++;
            if (m_howtoplay_cycle >= 75) {
                m_howtoplay_state = 9;
                m_howtoplay_cycle = 0;
            } // if
			break;
		case 9: // Appearing screen 3 of special items:
            m_howtoplay_cycle++;
            if (m_howtoplay_cycle >= 75) {
                m_howtoplay_state = 10;
                m_howtoplay_cycle = 0;
			} // if
			break;
		case 10: // waiting...
            m_howtoplay_cycle++;
            if (m_howtoplay_cycle >= 1000 ||
                (k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]) ||
                (k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
				(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)])) {
                m_howtoplay_state = 11;
                m_howtoplay_cycle = 0;
			} // if
			break;
		case 11: // Disappearing screen 3 of special items:
            m_howtoplay_cycle++;
            if (m_howtoplay_cycle >= 75) {
                m_howtoplay_state = 100;
                m_howtoplay_cycle = 0;
            } // if
			break;
		case 100: // Disappearing:
            m_howtoplay_cycle++;
            if (m_howtoplay_cycle >= 50) {
                m_title_current_menu = 0;
                m_title_next_menu = -1;
                m_title_state = 1;
                m_state_cycle = 0;
                m_title_option_selected = 0;
                difficulty = m_howtoplay_difficulty;

                delete m_test_game;
                m_test_game = 0;
                return THEGOONIES_STATE_TITLE;
            } // if
			break;
    } // switch

    return THEGOONIES_STATE_HOWTOPLAY;
} /* TheGooniesApp::howtoplay_cycle */


void TheGooniesApp::howtoplay_draw(void)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    switch (m_howtoplay_state) {
	case 0:
	case 1:
		if (m_test_game != 0) {
			float f = 1;

			if (m_howtoplay_state == 0)
				f = float(m_howtoplay_cycle) / 50;
			if (m_howtoplay_state == 1)
				f = 1;

			m_test_game->draw(m_GLTM);

			if (m_howtoplay_cycle > 50 && m_howtoplay_cycle < 200) {
				font_print_c(320, 350, 0, 0, 0.5f, "font_hl", "move sloth around using", -2);
				font_print_c(320, 370, 0, 0, 0.5f, "font_hl", "the right and left key", -2);
			}
			if (m_howtoplay_cycle > 225 && m_howtoplay_cycle < 325) {
				font_print_c(320, 350, 0, 0, 0.5f, "font_hl", "press up to jump", -2);
			}
			if (m_howtoplay_cycle > 350 && m_howtoplay_cycle < 475) {
				font_print_c(320, 350, 0, 0, 0.5f, "font_hl", "pick up keys by", -2);
				font_print_c(320, 370, 0, 0, 0.5f, "font_hl", "walking over them", -2);
			}
			if (m_howtoplay_cycle > 500 && m_howtoplay_cycle < 700) {
				font_print_c(320, 350, 0, 0, 0.5f, "font_hl", "climb vines by walking", -2);
				font_print_c(320, 370, 0, 0, 0.5f, "font_hl", "to them and using the up", -2);
				font_print_c(320, 390, 0, 0, 0.5f, "font_hl", "and down keys", -2);
			}
			if (m_howtoplay_cycle > 725 && m_howtoplay_cycle < 850) {
				font_print_c(320, 350, 0, 0, 0.5f, "font_hl", "avoid contact with enemies", -2);
				font_print_c(320, 370, 0, 0, 0.5f, "font_hl", "to prevent damage", -2);
			}
			if (m_howtoplay_cycle > 875 && m_howtoplay_cycle < 1000) {
				font_print_c(320, 350, 0, 0, 0.5f, "font_hl", "make sloth attack using", -2);
				font_print_c(320, 370, 0, 0, 0.5f, "font_hl", "the fire key", -2);
			}
			if (m_howtoplay_cycle > 1050 && m_howtoplay_cycle < 1250) {
				font_print_c(320, 350, 0, 0, 0.5f, "font_hl", "kill enemies by punching", -2);
				font_print_c(320, 370, 0, 0, 0.5f, "font_hl", "them", -2);
			}
			if (m_howtoplay_cycle > 1250 && m_howtoplay_cycle < 1350) {
				font_print_c(320, 350, 0, 0, 0.5f, "font_hl", "locks can be opened if", -2);
				font_print_c(320, 370, 0, 0, 0.5f, "font_hl", "you have a key", -2);
			}
			if (m_howtoplay_cycle > 1400 && m_howtoplay_cycle < 1550) {
				font_print_c(320, 350, 0, 0, 0.5f, "font_hl", "rescue the seven goonies", -2);
				font_print_c(320, 370, 0, 0, 0.5f, "font_hl", "to complete a level", -2);
			}
			if (m_howtoplay_cycle > 1600 && m_howtoplay_cycle < 1750) {
				font_print_c(320, 350, 0, 0, 0.5f, "font_hl", "you can enter skull doors", -2);
				font_print_c(320, 370, 0, 0, 0.5f, "font_hl", "with the up key", -2);
			}
			if (m_howtoplay_cycle > 1850 && m_howtoplay_cycle < 2100) {
				font_print_c(320, 350, 0, 0, 0.5f, "font_hl", "once you have rescued the", -2);
				font_print_c(320, 370, 0, 0, 0.5f, "font_hl", "seven goonies, head to the", -2);
				font_print_c(320, 390, 0, 0, 0.5f, "font_hl", "exit door and enter", -2);
			}

			if (f != 1) {
				m_GLTM->get
				("bg_copyright")->draw(1, 1, 1, 1 - f, 0, 0, 0, 0, 1);
				m_GLTM->get_smooth("title_logo")->draw(1, 1, 1, 1 - f, 46, 17, 0, 0, 1);
			}
		} else {
			m_GLTM->get
			("bg_copyright")->draw(0, 0, 0, 0, 1);
			m_GLTM->get_smooth("title_logo")->draw(46, 17, 0, 0, 1);
		}
		break;
	case 2:
		{
			m_test_game->draw(m_GLTM);

			float f = 1;
			f = float(m_howtoplay_cycle) / 50;
            glColor4f(0, 0, 0, f);
			glNormal3f(0.0, 0.0, 1.0);
			glBegin(GL_QUADS);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 480, 0);
			glVertex3f(640, 480, 0);
			glVertex3f(640, 0, 0);
			glEnd();
		}
		break;
	case 3:
	case 4:
	case 5:
		{
			int i;
			float f1 = 0;
			char *item_tiles[]={"it_bag",
								"it_shoes",
								"it_hammer",
								"it_lamp",
								"it_clock",
								"it_purpleshield",
								"it_yellowshield",
								"it_whiteshield"
								};
			char *item_descriptions[]={"increases life or experience",
									   "let you run faster",
									   "drops will turn into coins",
									   "shows hidden bags",
									   "stops time!",
									   "protects you from bones",
									   "protects you from gunshots",
									   "protects you from musical notes"
									   	};

			if (m_howtoplay_state==3) {
				f1 = (m_howtoplay_cycle -75)/50.0f;
				if (f1>0) f1 = 0;
			} // if 
			if (m_howtoplay_state==5) {
				f1 = (m_howtoplay_cycle)/50.0f;
				if (f1<0) f1 = 0;
			} // if 
			if (f1>0) f1=f1*f1;
				 else f1=-f1*f1;
			font_print_c(int(320 - 640*f1), 10, 0, 0, 0.5f, "font_hl", "effect of special objects (1/3)", -2);

			for(i=0;i<8;i++) {
				if (m_howtoplay_state==3) {
					f1 = (m_howtoplay_cycle - 75 - i*5)/50.0f;
					if (f1>0) f1 = 0;
				} // if 
				if (m_howtoplay_state==4) {
					f1 = (m_howtoplay_cycle - i*5)/50.0f;
					if (f1>0) f1 = 0;
				} // if 
				if (m_howtoplay_state==5) {
					f1 = (m_howtoplay_cycle - i*5)/50.0f;
					if (f1<0) f1 = 0;
				} // if 
				if (f1>0) f1=f1*f1;
					 else f1=-f1*f1;
				m_GLTM->get(item_tiles[i])->draw(float(75 - 640*f1), float(50+52*i), 0, 0, 2);
				font_print(int(140 - 640*f1), int(58+52*i), 0, 0, 0.5f, "font", item_descriptions[i], -2);
			} // for
		}
		break;
	case 6:
	case 7:
	case 8:
		{
			int i;
			float f1 = 0;
			char *item_tiles[]={
								"it_yellowhelmet",
								"it_lightbluehelmet",
								"it_greencoat",
								"it_greycoat",
								"it_yellowcoat",
								"it_lightbluecoat",
								"it_redbook",
								"it_lightbluebook",
								};
			char *item_descriptions[]={
									   "protects you from falling rocks",
									   "protects you from bats",
									   "protects you from flames",
									   "protects you from water drops",
									   "protects you from steam",
									   "protects you from waterfalls",
									   "easily kill ghosts",
									   "kills bats by touch",
										};

			if (m_howtoplay_state==6) {
				f1 = (m_howtoplay_cycle -75)/50.0f;
				if (f1>0) f1 = 0;
			} // if 
			if (m_howtoplay_state==8) {
				f1 = (m_howtoplay_cycle)/50.0f;
				if (f1<0) f1 = 0;
			} // if 
			if (f1>0) f1=f1*f1;
				 else f1=-f1*f1;
			font_print_c(int(320 - 640*f1), 10, 0, 0, 0.5f, "font_hl", "effect of special objects (2/3)", -2);

			for(i=0;i<8;i++) {
				if (m_howtoplay_state==6) {
					f1 = (m_howtoplay_cycle - 75 - i*5)/50.0f;
					if (f1>0) f1 = 0;
				} // if 
				if (m_howtoplay_state==7) {
					f1 = (m_howtoplay_cycle - i*5)/50.0f;
					if (f1>0) f1 = 0;
				} // if 
				if (m_howtoplay_state==8) {
					f1 = (m_howtoplay_cycle - i*5)/50.0f;
					if (f1<0) f1 = 0;
				} // if 
				if (f1>0) f1=f1*f1;
					 else f1=-f1*f1;
				m_GLTM->get(item_tiles[i])->draw(float(75 - 640*f1), float(50+52*i), 0, 0, 2);
				font_print(int(140 - 640*f1), int(58+52*i), 0, 0, 0.5f, "font", item_descriptions[i], -2);
			} // for
		}
		break;
	case 9:
	case 10:
	case 11:
		{
			int i;
			float f1 = 0;
			char *item_tiles[]={"it_bluebook",
								"it_whitebook",
								"it_greenbook",
								"it_purplebook",
								"it_yellowbook",
								"it_purpleanti",
								"it_redanti",
								"it_blueanti"};
			char *item_descriptions[]={"protects you from skulls",
									   "kills skeletons by touch",
									   "kills skulls by touch",									
									   "fratellies won't be able to shoot",
									   "protects you from skeletons",
									   "skeletons will walk faster",
									   "fratellies will walk faster",
									   "will awake ghosts"
										};

			if (m_howtoplay_state==9) {
				f1 = (m_howtoplay_cycle -75)/50.0f;
				if (f1>0) f1 = 0;
			} // if 
			if (m_howtoplay_state==11) {
				f1 = (m_howtoplay_cycle)/50.0f;
				if (f1<0) f1 = 0;
			} // if 
			if (f1>0) f1=f1*f1;
				 else f1=-f1*f1;
			font_print_c(int(320 - 640*f1), 10, 0, 0, 0.5f, "font_hl", "effect of special objects (3/3)", -2);

			for(i=0;i<8;i++) {
				if (m_howtoplay_state==9) {
					f1 = (m_howtoplay_cycle - 75 - i*5)/50.0f;
					if (f1>0) f1 = 0;
				} // if 
				if (m_howtoplay_state==10) {
					f1 = (m_howtoplay_cycle - i*5)/50.0f;
					if (f1>0) f1 = 0;
				} // if 
				if (m_howtoplay_state==11) {
					f1 = (m_howtoplay_cycle - i*5)/50.0f;
					if (f1<0) f1 = 0;
				} // if 
				if (f1>0) f1=f1*f1;
					 else f1=-f1*f1;
				m_GLTM->get(item_tiles[i])->draw(float(75 - 640*f1), float(50+52*i), 0, 0, 2);
				font_print(int(140 - 640*f1), int(58+52*i), 0, 0, 0.5f, "font", item_descriptions[i], -2);
			} // for
		}
		break;

	case 100:
		{
			float f = 1;
			f = float(m_howtoplay_cycle) / 50;

			m_GLTM->get
			("bg_copyright")->draw(1, 1, 1, f, 0, 0, 0, 0, 1);
			m_GLTM->get_smooth("title_logo")->draw(1, 1, 1, f, 46, 17, 0, 0, 1);
		}
		break;
	} // switch
}
