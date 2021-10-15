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

#include "GLTManager.h"
#include "SoundManager.h"
#include "SFXManager.h"
#include "MusicManager.h"
#include "GObject.h"
#include "GO_enemy.h"
#include "GO_fratelli.h"
#include "GMap.h"
#include "TheGoonies.h"
#include "TheGooniesApp.h"

#include "font_extractor.h"

#define TITLE_INTRO_TIME 300 
//#define TITLE_INTRO_TIME 50

extern bool fullscreen;
extern SDL_Surface *screen_sfc;
extern SDL_Surface *toogle_video_mode(bool fullscreen);


int TheGooniesApp::titleanimation_cycle(KEYBOARDSTATE *k)
{
    switch (m_titleanimation_state) {
        case 0:  // appearing
            if (m_state_cycle >= 50) {
                m_titleanimation_state = 1;
                m_titleanimation_cycle = 0;
                m_state_cycle = 0;
            }
            break;
        case 1:  // animation
            m_titleanimation_cycle++;
            if (m_titleanimation_cycle >= 2200 ||
                    (k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]) ||
                    (k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
					(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)]))
			{
                m_titleanimation_state = 2;
                m_state_cycle = 0;
            }
            break;
        case 2:  // disppearing
            m_titleanimation_cycle++;
            if (m_state_cycle >= 50) {
                m_title_current_menu = 0;
                m_title_next_menu = -1;
                m_title_state = 1;
                m_state_cycle = 0;
                m_title_option_selected = 0;
                return THEGOONIES_STATE_TITLE;
            }
            break;
    }

    return THEGOONIES_STATE_TITLEANIMATION;
} 

void TheGooniesApp::titleanimation_draw(void)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

	m_titleanimation_shift -= m_titleanimation_speed;
	m_titleanimation_shift += 40;
	m_titleanimation_shift = (m_titleanimation_shift%40)-40;
	if ((m_state_cycle%8)==0) {
		if (m_titleanimation_state==0 || m_titleanimation_state==1) {
			if (m_titleanimation_speed<4) m_titleanimation_speed++;
		} else {
			if (m_titleanimation_speed>0) m_titleanimation_speed--;
		}
	} 

    m_GLTM->get
    ("bg_copyright")->draw(0, 0, 0, 0, 1);
    m_GLTM->get_smooth("title_logo")->draw(46, 17, 0, 0, 1);

    switch (m_titleanimation_state) {
        case 0: {
                float f = float(m_state_cycle) / 50;

                glColor4f(0, 0, 0, f);
                glBegin(GL_QUADS);
                glVertex3f(0, 256, 0);
                glVertex3f(0, 400, 0);
                glVertex3f(640, 400, 0);
                glVertex3f(640, 256, 0);
                glEnd();

				// film strip
				{
					GLTile *strip = m_GLTM->get("intro-film-strip");
					for (int x = m_titleanimation_shift; x < 640; x+= 40)
					{
						strip->draw(1, 1, 1, f, float(x), 236, 0, 0, 1);
						strip->draw(1, 1, 1, f, float(x), 400, 0, 0, 1);
					}
				}
            }
            break;
        case 1:
        case 2: {
                float f = 1;
                bool scissor_state = (glIsEnabled(GL_SCISSOR_TEST) ? true : false);
                int scissor_window[4];

                if (m_titleanimation_state == 1)
                    f = 1;
                if (m_titleanimation_state == 2)
                    f = float(50 - m_state_cycle) / 50;
                glEnable(GL_SCISSOR_TEST);
                if (scissor_state)
                    glGetIntegerv(GL_SCISSOR_BOX, scissor_window);
                glScissor(screen_x(0), screen_y(60), screen_x(640), screen_y(184));

                glColor4f(0, 0, 0, f);
                glBegin(GL_QUADS);
                glVertex3f(0, 0, 0);
                glVertex3f(0, 480, 0);
                glVertex3f(640, 480, 0);
                glVertex3f(640, 0, 0);
                glEnd();

				// film strip
				{
//					int shift = -(m_titleanimation_cycle % 20) * 2;
					GLTile *strip = m_GLTM->get("intro-film-strip");
					for (int x = m_titleanimation_shift; x < 640; x+= 40)
					{
						strip->draw(1, 1, 1, f, float(x), 236, 0, 0, 1);
						strip->draw(1, 1, 1, f, float(x), 400, 0, 0, 1);
					}
				}

                // character:
                if (m_titleanimation_cycle > 50 &&
                        m_titleanimation_cycle <= 300) {
                    // running right:
                    int sp = (m_titleanimation_cycle / 8) % 4;
                    if (sp == 0)
                        m_GLTM->get
                        ("intro-character-r1")->draw(1, 1, 1, f, 512 - (300 - m_titleanimation_cycle)*2.5f, 296, 0, 0, 1);
                    if (sp == 1)
                        m_GLTM->get
                        ("intro-character-r2")->draw(1, 1, 1, f, 512 - (300 - m_titleanimation_cycle)*2.5f, 296, 0, 0, 1);
                    if (sp == 2)
                        m_GLTM->get
                        ("intro-character-r1")->draw(1, 1, 1, f, 512 - (300 - m_titleanimation_cycle)*2.5f, 296, 0, 0, 1);
                    if (sp == 3)
                        m_GLTM->get
                        ("intro-character-r3")->draw(1, 1, 1, f, 512 - (300 - m_titleanimation_cycle)*2.5f, 296, 0, 0, 1);
                } // if
				if (m_titleanimation_cycle > 300 &&
						m_titleanimation_cycle <= 312) {
					// turning:
					int sp = (m_titleanimation_cycle - 301) / 4;
					if (sp == 0)
						m_GLTM->get ("intro-character-turning_rl_1")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get ("intro-character-turning_rl_2")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get ("intro-character-turning_rl_3")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
				}
                if (m_titleanimation_cycle > 312 &&
                        m_titleanimation_cycle <= 1500) {
                    // standing left:
					int sp = (m_titleanimation_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
						("intro-character-standing-l1")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
						("intro-character-standing-l2")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
						("intro-character-standing-l3")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
						("intro-character-standing-l2")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
                } // if
                if (m_titleanimation_cycle > 1500 &&
                        m_titleanimation_cycle <= 1850) {
                    int sp = (m_titleanimation_cycle / 8) % 2;
					int s2 = (m_titleanimation_cycle / 5) % 3;
                    // knocked:
					if (s2 == 0) m_GLTM->get("intro-character-stars1")->draw(1, 1, 1, f, 532, 334, 0, 0, 1);
					if (s2 == 1) m_GLTM->get("intro-character-stars2")->draw(1, 1, 1, f, 532, 334, 0, 0, 1);
					if (s2 == 2) m_GLTM->get("intro-character-stars3")->draw(1, 1, 1, f, 532, 334, 0, 0, 1);

                    if (sp == 0)
                        m_GLTM->get
                        ("intro-character-knocked1")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
                    if (sp == 1)
                        m_GLTM->get
                        ("intro-character-knocked2")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);

                } // if
                if (m_titleanimation_cycle > 1850 &&
                        m_titleanimation_cycle <= 1900) {
                    // standing left:
					int sp = (m_titleanimation_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
						("intro-character-standing-l1")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
						("intro-character-standing-l2")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
						("intro-character-standing-l3")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
						("intro-character-standing-l2")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
                } // if
                if (m_titleanimation_cycle > 1900 &&
                        m_titleanimation_cycle <= 2200) {
                    // running right:
                    int sp = (m_titleanimation_cycle / 8) % 4;
                    if (sp == 0)
                        m_GLTM->get
                        ("intro-character-l1")->draw(1, 1, 1, f, 512 - (m_titleanimation_cycle - 1900)*2.5f, 296, 0, 0, 1);
                    if (sp == 1)
                        m_GLTM->get
                        ("intro-character-l2")->draw(1, 1, 1, f, 512 - (m_titleanimation_cycle - 1900)*2.5f, 296, 0, 0, 1);
                    if (sp == 2)
                        m_GLTM->get
                        ("intro-character-l1")->draw(1, 1, 1, f, 512 - (m_titleanimation_cycle - 1900)*2.5f, 296, 0, 0, 1);
                    if (sp == 3)
                        m_GLTM->get
                        ("intro-character-l3")->draw(1, 1, 1, f, 512 - (m_titleanimation_cycle - 1900)*2.5f, 296, 0, 0, 1);
                } // if

                // Mike
				if (m_titleanimation_cycle > 300 + 0*150 &&
						m_titleanimation_cycle <= 500 + 0*150) {
					// running right:
					int sp = (m_titleanimation_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
							("intro-mike-r1")->draw(1, 1, 1, f, (464 - 0*60) - (500 + 0*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-mike-r2")->draw(1, 1, 1, f, (464 - 0*60) - (500 + 0*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
							("intro-mike-r3")->draw(1, 1, 1, f, (464 - 0*60) - (500 + 0*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
							("intro-mike-r2")->draw(1, 1, 1, f, (464 - 0*60) - (500 + 0*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
				}
				if (m_titleanimation_cycle > 500 + 0*150 &&
						m_titleanimation_cycle <= 1624 + 0*24) {
					// standing:
					int sp = (m_titleanimation_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-mike-standing1")->draw(1, 1, 1, f, float(464 - 0*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-mike-standing2")->draw(1, 1, 1, f, float(464 - 0*60), 312, 0, 0, 1);
				}

				// Mouth
				if (m_titleanimation_cycle > 300 + 1*150 &&
						m_titleanimation_cycle <= 500 + 1*150) {
					// running right:
					int sp = (m_titleanimation_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
							("intro-mouth-r1")->draw(1, 1, 1, f, (464 - 1*60) - (500 + 1*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-mouth-r2")->draw(1, 1, 1, f, (464 - 1*60) - (500 + 1*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
							("intro-mouth-r3")->draw(1, 1, 1, f, (464 - 1*60) - (500 + 1*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
							("intro-mouth-r2")->draw(1, 1, 1, f, (464 - 1*60) - (500 + 1*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
				}
				if (m_titleanimation_cycle > 500 + 1*150 &&
						m_titleanimation_cycle <= 1624 + 1*24) {
					// standing:
					int sp = (m_titleanimation_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-mouth-standing1")->draw(1, 1, 1, f, float(464 - 1*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-mouth-standing2")->draw(1, 1, 1, f, float(464 - 1*60), 312, 0, 0, 1);
				}

				//
				// Data
				//
				//		Added some variables to look for best values in the rope effect. Enjoy ^o^.
				//		Should be replaced by constants or something once we feel happy with the values.
				//		Ps: yes, I know Bresenham's algorithm to draw lines. But mee too lazy, too tired, too late :-).
				//
				float wave_amplitude = 1.0;		// Wave amplitude when launching the rope
				float wave_lenght = 120.0;		// Wave lenght when launching the rope
				int rope_cycles = 100;			// Cycles to reach the other side of the screen
				float rope_y = 260;				// Y coordinate for the left side of the rope
				float rope_y_delta = 10;		// Hight delta between left and right sides
				float data_weight = 10;			// "weight" of Data using the rope
				float data_shift = -30;			// Data shift to adjust Data graphic to the rope :-P

				int rope_start_cycle = 500 + 1*150 - rope_cycles;
				GLTile *rope = m_GLTM->get("intro-data-rope");

				// Throwing rope
				if (m_titleanimation_cycle > rope_start_cycle &&
						m_titleanimation_cycle <= rope_start_cycle + rope_cycles) {
					int fragments = (m_titleanimation_cycle - rope_start_cycle)*64 / rope_cycles;
					for (int i = 0; i < fragments; ++i)
						rope->draw(1, 1, 1, f, float(i * 10), float(rope_y + i*10*rope_y_delta/640 + wave_amplitude * sin(i*10 * 2*M_PI/wave_lenght)), 0, 0, 1);
				}

				// Data descending
				if (m_titleanimation_cycle > 300 + 2*150 &&
						m_titleanimation_cycle <= 500 + 2*150) {
					float x = (464 - 2*60) - (500 + 2*150 - m_titleanimation_cycle)*2.5f;
					float y = rope_y + data_weight + x*rope_y_delta/640.0f;

					// The Rope
					if (x >= 0 && x < 640) {
						int i;
						float fst_y_delta = data_weight + x*rope_y_delta/640;
						float snd_y_delta = rope_y_delta - fst_y_delta;

						for (i = 0; i < x; i+=10)
							rope->draw(1, 1, 1, f, float(i), rope_y + (i)*fst_y_delta/x, 0, 0, 1);

						for (i = 0; i < (640 - x); i+=10)
							rope->draw(1, 1, 1, f, float(i+x), rope_y + fst_y_delta + (i)*snd_y_delta/(640-x), 0, 0, 1);
					}
					else if (m_titleanimation_cycle > rope_start_cycle + rope_cycles) {
						int i;
						for (i = 0; i < 640; i+=10)
							rope->draw(1, 1, 1, f, float(i), rope_y + i*rope_y_delta/640, 0, 0, 1);
					}

					// The kiddo
					int sp = (m_titleanimation_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
							("intro-data-hang1")->draw(1, 1, 1, f, x+data_shift, y, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-data-hang2")->draw(1, 1, 1, f, x+data_shift, y, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
							("intro-data-hang3")->draw(1, 1, 1, f, x+data_shift, y, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
							("intro-data-hang2")->draw(1, 1, 1, f, x+data_shift, y, 0, 0, 1);
				}
				else if (m_titleanimation_cycle > rope_start_cycle + rope_cycles) {
					// Rope dissapearing
					float rf;
					if (m_titleanimation_cycle < 900)
						rf = 1.0;
					else if (m_titleanimation_cycle > 1000)
						rf = 0.0;
					else
						rf = (1000 - m_titleanimation_cycle) / 100.0f;

					for (int i = 0; i < 640; i+=10)
						rope->draw(1, 1, 1, rf, float(i), rope_y + i*rope_y_delta/640, 0, 0, 1);
				}

				if (m_titleanimation_cycle > 500 + 2*150 &&
						m_titleanimation_cycle <= 1624 + 2*24) {
					// standing:
					int sp = (m_titleanimation_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-data-standing1")->draw(1, 1, 1, f, float(464 - 2*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-data-standing2")->draw(1, 1, 1, f, float(464 - 2*60), 312, 0, 0, 1);
				}

				// Chunk
				if (m_titleanimation_cycle > 300 + 3*150 &&
						m_titleanimation_cycle <= 500 + 3*150) {
					// running right:
					int sp = (m_titleanimation_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
							("intro-chunk-r1")->draw(1, 1, 1, f, (464 - 3*60) - (500 + 3*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-chunk-r2")->draw(1, 1, 1, f, (464 - 3*60) - (500 + 3*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
							("intro-chunk-r3")->draw(1, 1, 1, f, (464 - 3*60) - (500 + 3*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
							("intro-chunk-r2")->draw(1, 1, 1, f, (464 - 3*60) - (500 + 3*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
				}
				if (m_titleanimation_cycle > 500 + 3*150 &&
						m_titleanimation_cycle <= 1624 + 3*24) {
					// standing:
					int sp = (m_titleanimation_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-chunk-standing1")->draw(1, 1, 1, f, float(464 - 3*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-chunk-standing2")->draw(1, 1, 1, f, float(464 - 3*60), 312, 0, 0, 1);
				}

				// Brand
				if (m_titleanimation_cycle > 300 + 4*150 &&
						m_titleanimation_cycle <= 500 + 4*150) {
					// running right:
					int sp = (m_titleanimation_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
							("intro-brand-r1")->draw(1, 1, 1, f, (464 - 4*60) - (500 + 4*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-brand-r2")->draw(1, 1, 1, f, (464 - 4*60) - (500 + 4*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
							("intro-brand-r3")->draw(1, 1, 1, f, (464 - 4*60) - (500 + 4*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
							("intro-brand-r2")->draw(1, 1, 1, f, (464 - 4*60) - (500 + 4*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
				}
				if (m_titleanimation_cycle > 500 + 4*150 &&
						m_titleanimation_cycle <= 1624 + 4*24) {
					// standing:
					int sp = (m_titleanimation_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-brand-standing1")->draw(1, 1, 1, f, float(464 - 4*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-brand-standing2")->draw(1, 1, 1, f, float(464 - 4*60), 312, 0, 0, 1);
				}

				// Andy
				if (m_titleanimation_cycle > 300 + 5*150 &&
						m_titleanimation_cycle <= 500 + 5*150) {
					// running right:
					int sp = (m_titleanimation_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
							("intro-andy-r1")->draw(1, 1, 1, f, (464 - 5*60) - (500 + 5*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-andy-r2")->draw(1, 1, 1, f, (464 - 5*60) - (500 + 5*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
							("intro-andy-r3")->draw(1, 1, 1, f, (464 - 5*60) - (500 + 5*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
							("intro-andy-r2")->draw(1, 1, 1, f, (464 - 5*60) - (500 + 5*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
				}
				if (m_titleanimation_cycle > 500 + 5*150 &&
						m_titleanimation_cycle <= 1624 + 5*24) {
					// standing:
					int sp = (m_titleanimation_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-andy-standing1")->draw(1, 1, 1, f, float(464 - 5*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-andy-standing2")->draw(1, 1, 1, f, float(464 - 5*60), 312, 0, 0, 1);
				}

				// Steff
				if (m_titleanimation_cycle > 300 + 6*150 &&
						m_titleanimation_cycle <= 500 + 6*150) {
					// running right:
					int sp = (m_titleanimation_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
							("intro-steff-r1")->draw(1, 1, 1, f, (464 - 6*60) - (500 + 6*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-steff-r2")->draw(1, 1, 1, f, (464 - 6*60) - (500 + 6*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
							("intro-steff-r3")->draw(1, 1, 1, f, (464 - 6*60) - (500 + 6*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
							("intro-steff-r2")->draw(1, 1, 1, f, (464 - 6*60) - (500 + 6*150 - m_titleanimation_cycle)*2.5f, 312, 0, 0, 1);
				}
				if (m_titleanimation_cycle > 500 + 6*150 &&
						m_titleanimation_cycle <= 1624 + 6*24) {
					// standing:
					int sp = (m_titleanimation_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-steff-standing1")->draw(1, 1, 1, f, float(464 - 6*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-steff-standing2")->draw(1, 1, 1, f, float(464 - 6*60), 312, 0, 0, 1);
				}

                // Fratelli:
                if (m_titleanimation_cycle > 1400 &&
                        m_titleanimation_cycle <= 1500) {
                    // wlking left:
                    int sp = (m_titleanimation_cycle / 8) % 2;
                    if (sp == 0)
                        m_GLTM->get
                        ("intro-fratelli-l1")->draw(1, 1, 1, f, 544 + (1500 - m_titleanimation_cycle)*2.5f, 296, 0, 0, 1);
                    if (sp == 1)
                        m_GLTM->get
                        ("intro-fratelli-l2")->draw(1, 1, 1, f, 544 + (1500 - m_titleanimation_cycle)*2.5f, 296, 0, 0, 1);
                } // if
                if (m_titleanimation_cycle > 1500 &&
                        m_titleanimation_cycle <= 1600) {
                    // standing left:
                    m_GLTM->get
                    ("intro-fratelli-l1")->draw(1, 1, 1, f, 544, 296, 0, 0, 1);
                } // if
                if (m_titleanimation_cycle > 1600 &&
                        m_titleanimation_cycle <= 1780) {
                    // wlking left:
                    int sp = (m_titleanimation_cycle / 8) % 2;
                    if (sp == 0)
                        m_GLTM->get
                        ("intro-fratelli-l1")->draw(1, 1, 1, f, 544 - (m_titleanimation_cycle - 1600)*2.5f, 296, 0, 0, 1);
                    if (sp == 1)
                        m_GLTM->get
                        ("intro-fratelli-l2")->draw(1, 1, 1, f, 544 - (m_titleanimation_cycle - 1600)*2.5f, 296, 0, 0, 1);
                } // if
                if (m_titleanimation_cycle > 1780 &&
                        m_titleanimation_cycle <= 1900) {
                    // laughting:
                    int sp = (m_titleanimation_cycle / 8) % 2;
                    if (sp == 0) m_GLTM->get("intro-fratelli-laught-r1")->draw(1, 1, 1, f, 94, 296, 0, 0, 1);
                    if (sp == 1) m_GLTM->get("intro-fratelli-laught-r2")->draw(1, 1, 1, f, 94, 296, 0, 0, 1);
                } // if
                if (m_titleanimation_cycle > 1900 &&
                        m_titleanimation_cycle <= 2000) {
                    // wlking left:
                    int sp = (m_titleanimation_cycle / 8) % 2;
                    if (sp == 0)
                        m_GLTM->get
                        ("intro-fratelli-l1")->draw(1, 1, 1, f, 94 - (m_titleanimation_cycle - 1900)*2.5f, 296, 0, 0, 1);
                    if (sp == 1)
                        m_GLTM->get
                        ("intro-fratelli-l2")->draw(1, 1, 1, f, 94 - (m_titleanimation_cycle - 1900)*2.5f, 296, 0, 0, 1);
                } // if

                if (!scissor_state)
                    glDisable(GL_SCISSOR_TEST);
                if (scissor_state)
                    glScissor(scissor_window[0], scissor_window[1], scissor_window[2], scissor_window[3]);

            }
            break;
    } // switch

} /* TheGooniesApp::titleanimation_draw */

