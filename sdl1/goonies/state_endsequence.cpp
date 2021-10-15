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



int TheGooniesApp::endsequence_cycle(KEYBOARDSTATE *k)
{
    switch (m_endsequence_state) {
        case 0:  // appearing
            if (m_state_cycle == 0)
				m_MusicM->music_play(MUSIC_ID,"levelclear",m_music_volume,0);

            if (m_state_cycle >= 50) {
                m_endsequence_state = 1;
                m_endsequence_cycle = 0;
                m_state_cycle = 0;
            }
            break;
        case 1:  // text
            m_endsequence_cycle++;
            if (m_endsequence_cycle >= 200 ||
                    (k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]) ||
                    (k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
					(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)]))
			{
                m_endsequence_state = 2;
                m_endsequence_cycle = 0;
                m_state_cycle = 0;

				m_MusicM->music_play(MUSIC_ID,"cutscene",m_music_volume,-1);

                if (m_game != 0)
                    m_game->clear();
					m_SFXM->SFX_stop_continuous();
            }
            break;
        case 2:  // animation
            m_endsequence_cycle++;
            if (m_endsequence_cycle >= 3500 ||
                    (k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]) ||
                    (k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
					(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)]))
			{
                m_endsequence_state = 3;
                m_state_cycle = 0;
				m_MusicM->music_fadeout(MUSIC_ID,50);
            }
            // plays punch sfx when Sloth punches Fratelli.
            if (m_endsequence_cycle == 1500)
                 m_SFXM->SFX_play("sfx/player_hit_enemy", m_sfx_volume);

            break;
        case 3:  // disappearing
            m_endsequence_cycle++;
            if (m_state_cycle >= 50) {
				m_MusicM->music_play(MUSIC_ID,"endcredits",m_music_volume,-1);
                m_endsequence_state = 4;
                m_endsequence_cycle = 0;
                m_state_cycle = 0;
            }
            break;
        case 4:  // appearing credits:
            m_endsequence_cycle++;
            if (m_state_cycle >= 50) {
                m_endsequence_state = 5;
                m_state_cycle = 0;
            }
            break;
        case 5:  // credits:
            m_endsequence_cycle++;
            if (m_endsequence_cycle >= 9250 ||
                    (k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]) ||
                    (k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
					(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)]))
			{
                m_endsequence_state = 6;
                m_state_cycle = 0;
				m_MusicM->music_fadeout(MUSIC_ID,50);
            }
            break;
        case 6:  // disappearing credits:
            m_endsequence_cycle++;
            if (m_state_cycle >= 50) {
				m_title_state = 0;
				m_title_option_selected = 0;
				m_title_waiting_keystroke = false;		
				m_title_current_menu = 0;
				m_title_next_menu = 0;
				return THEGOONIES_STATE_TITLE;
            }
            break;
    }
    return THEGOONIES_STATE_ENDSEQUENCE;
}

void TheGooniesApp::endsequence_draw(void)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

	m_endsequence_shift -= m_endsequence_speed;
	m_endsequence_shift += 40;
	m_endsequence_shift = (m_endsequence_shift%40)-40;
	if ((m_state_cycle%8)==0) {
		if (m_endsequence_state==2 || m_endsequence_state==3) {
			if (m_endsequence_speed<4) m_endsequence_speed++;
		} else {
			if (m_endsequence_speed>0) m_endsequence_speed--;
		} // if
	} // if

    switch (m_endsequence_state) {
        case 0: {
                float f = float(m_state_cycle) / 50;

                if (m_game != 0)
                    m_game->draw(m_GLTM);

                glColor4f(0, 0, 0, f);
                glBegin(GL_QUADS);
                glVertex3f(0, 256, 0);
                glVertex3f(0, 400, 0);
                glVertex3f(640, 400, 0);
                glVertex3f(640, 256, 0);
                glEnd();
            }
            break;
        case 1: {			
                if (m_game != 0)
                    m_game->draw(m_GLTM);
			
                // 1: text appearing
                if (m_endsequence_cycle >= 0 && m_endsequence_cycle < 50) {
                    float f = (1 - (m_endsequence_cycle / 50.0f));
					int x;
                    f *= f;
                    x = (int)(86 + f * 640);
					m_GLTM->get("stageclear")->draw(1, 1, 1, 1, float(x), 150, 0, 0, 1);
                }

                // 2: text braking
                if (m_endsequence_cycle >= 50 && m_endsequence_cycle < 150) {
                    m_GLTM->get("stageclear")->draw(1, 1, 1, 1, 86, 150, 0, 0, 1);
				}

                // 3: text disappearing
                if (m_endsequence_cycle >= 150 && m_endsequence_cycle < 200) {
                    int x;
                    float f = (((m_endsequence_cycle - 150) / 50.0f));

                    f *= f;
                    x = (int)(86 - f * 640);
                    m_GLTM->get("stageclear")->draw(1, 1, 1, 1, float(x), 150, 0, 0, 1);
				}
            }
            break;
        case 2:
        case 3: {
                float f = 1;
				float f2 = 0;
                bool scissor_state = (glIsEnabled(GL_SCISSOR_TEST) ? true : false);
                int scissor_window[4];

                glEnable(GL_SCISSOR_TEST);
                if (scissor_state)
                    glGetIntegerv(GL_SCISSOR_BOX, scissor_window);
                glScissor(screen_x(0), screen_y(60), screen_x(640), screen_y(360));

                glColor4f(0, 0, 0, f);
                glBegin(GL_QUADS);
                glVertex3f(0, 0, 0);
                glVertex3f(0, 480, 0);
                glVertex3f(640, 480, 0);
                glVertex3f(640, 0, 0);
                glEnd();

				// sky:
				m_GLTM->get
					("ending-sky1")->draw(1, 1, 1, f, 0, 0, 0, 0, 1);

				// sea:
				{
					int sp = (m_endsequence_cycle / 15) % 3;
					if (sp == 0)
						m_GLTM->get
							("ending-sea1")->draw(1, 1, 1, f, 0, 226, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("ending-sea2")->draw(1, 1, 1, f, 0, 226, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
							("ending-sea3")->draw(1, 1, 1, f, 0, 226, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
							("ending-sea4")->draw(1, 1, 1, f, 0, 226, 0, 0, 1);
				}

				// rear rocks
				m_GLTM->get
					("ending-rock-5")->draw(1, 1, 1, f, 0, 172, 0, 0, 1);

				// pirate ship:
				int ship_first_state = 1600;
				float ship_speed = 0.40f;
				int ship_step = -20;
				int ship_last_state = ship_first_state + int((640+80)/ship_speed);
				if (m_endsequence_cycle > ship_first_state &&
						m_endsequence_cycle <= ship_last_state) {
					float ship_x = -80 + (m_endsequence_cycle - ship_first_state) * ship_speed;
					float ship_y = 170 + (ship_x * ship_step / (640+80));
					int sp = (m_endsequence_cycle / 16) % 4;

					if (sp == 0)
						m_GLTM->get
						("ending-pirate-ship1")->draw(1, 1, 1, f, ship_x, ship_y, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
						("ending-pirate-ship2")->draw(1, 1, 1, f, ship_x, ship_y, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
						("ending-pirate-ship3")->draw(1, 1, 1, f, ship_x, ship_y, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
						("ending-pirate-ship2")->draw(1, 1, 1, f, ship_x, ship_y, 0, 0, 1);
				}

				// rocks:
				m_GLTM->get
					("ending-rock-1")->draw(1, 1, 1, f, 3, 234, 0, 0, 1);
				m_GLTM->get
					("ending-rock-2")->draw(1, 1, 1, f, 225, 222, 0, 0, 1);
				m_GLTM->get
					("ending-rock-3")->draw(1, 1, 1, f, 363, 234, 0, 0, 1);
				m_GLTM->get
					("ending-rock-4")->draw(1, 1, 1, f, 507, 202, 0, 0, 1);
				m_GLTM->get
					("ending-floor")->draw(1, 1, 1, f, 0, 310, 0, 0, 1);

				// film strip
				{
					GLTile *strip = m_GLTM->get("intro-film-strip");
					for (int x = m_endsequence_shift; x < 640; x+= 40)
					{
						strip->draw(1, 1, 1, f, float(x), 60, 0, 0, 1);
						strip->draw(1, 1, 1, f, float(x), 400, 0, 0, 1);
					}
				}

                // character:
                if (m_endsequence_cycle > 50 &&
                        m_endsequence_cycle <= 300) {
                    // running right:
                    int sp = (m_endsequence_cycle / 8) % 4;
                    if (sp == 0)
                        m_GLTM->get
                        ("intro-character-r1")->draw(1, 1, 1, f, 512 - (300 - m_endsequence_cycle)*2.5f, 296, 0, 0, 1);
                    if (sp == 1)
                        m_GLTM->get
                        ("intro-character-r2")->draw(1, 1, 1, f, 512 - (300 - m_endsequence_cycle)*2.5f, 296, 0, 0, 1);
                    if (sp == 2)
                        m_GLTM->get
                        ("intro-character-r1")->draw(1, 1, 1, f, 512 - (300 - m_endsequence_cycle)*2.5f, 296, 0, 0, 1);
                    if (sp == 3)
                        m_GLTM->get
                        ("intro-character-r3")->draw(1, 1, 1, f, 512 - (300 - m_endsequence_cycle)*2.5f, 296, 0, 0, 1);
                }
				if (m_endsequence_cycle > 300 &&
						m_endsequence_cycle <= 312) {
					// turning:
					int sp = (m_endsequence_cycle - 301) / 4;
					if (sp == 0)
						m_GLTM->get ("intro-character-turning_rl_1")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get ("intro-character-turning_rl_2")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get ("intro-character-turning_rl_3")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
				}
                if (m_endsequence_cycle > 312 &&
                        m_endsequence_cycle <= 1478) {
                    // standing left:
					int sp = (m_endsequence_cycle / 8) % 4;
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
                }
				if (m_endsequence_cycle > 1478 &&
						m_endsequence_cycle <= 1490) {
					// turning:
					int sp = (m_endsequence_cycle - 1479) / 4;
					if (sp == 0)
						m_GLTM->get ("intro-character-turning_lr_1")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get ("intro-character-turning_lr_2")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get ("intro-character-turning_lr_3")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
				}
				if (m_endsequence_cycle > 1490 &&
						m_endsequence_cycle <= 1512) {
					// punching:
					int sp = ((m_endsequence_cycle - 1491) / 6) % 4;
					if (sp == 0)
						m_GLTM->get
						("intro-character-standing-r1")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
						("intro-character-punching_1")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
						("intro-character-punching_2")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
						("intro-character-punching_1")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
				}
                if (m_endsequence_cycle > 1512 &&
                        m_endsequence_cycle <= 1600) {
                    // standing right:
					int sp = (m_endsequence_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
						("intro-character-standing-r1")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
						("intro-character-standing-r2")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
						("intro-character-standing-r3")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
						("intro-character-standing-r2")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
                }
				if (m_endsequence_cycle > 1600 &&
						m_endsequence_cycle <= 1612) {
					// turning:
					int sp = (m_endsequence_cycle - 1601) / 4;
					if (sp == 0)
						m_GLTM->get ("intro-character-turning_rl_1")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get ("intro-character-turning_rl_2")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get ("intro-character-turning_rl_3")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
				}
                if (m_endsequence_cycle > 1612) {
                    // thumbs up:
					int sp = (m_endsequence_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
						("intro-character-thumbsup1")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
						("intro-character-thumbsup2")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
						("intro-character-thumbsup3")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
						("intro-character-thumbsup2")->draw(1, 1, 1, f, 512, 296, 0, 0, 1);
                }

                // Mike
				if (m_endsequence_cycle > 300 + 0*150 &&
						m_endsequence_cycle <= 500 + 0*150) {
					// running right:
					int sp = (m_endsequence_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
							("intro-mike-r1")->draw(1, 1, 1, f, (464 - 0*60) - (500 + 0*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-mike-r2")->draw(1, 1, 1, f, (464 - 0*60) - (500 + 0*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
							("intro-mike-r3")->draw(1, 1, 1, f, (464 - 0*60) - (500 + 0*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
							("intro-mike-r2")->draw(1, 1, 1, f, (464 - 0*60) - (500 + 0*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
				}
				if (m_endsequence_cycle > 500 + 0*150 &&
						m_endsequence_cycle <= 1600) {
					// standing:
					int sp = (m_endsequence_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-mike-standing1")->draw(1, 1, 1, f, float(464 - 0*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-mike-standing2")->draw(1, 1, 1, f, float(464 - 0*60), 312, 0, 0, 1);
				}
				if (m_endsequence_cycle > 1600) {
					// happy:
					int sp = (m_endsequence_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-mike-happy1")->draw(1, 1, 1, f, float(464 - 0*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-mike-happy2")->draw(1, 1, 1, f, float(464 - 0*60), 312, 0, 0, 1);
				}

				// Mouth
				if (m_endsequence_cycle > 300 + 1*150 &&
						m_endsequence_cycle <= 500 + 1*150) {
					// running right:
					int sp = (m_endsequence_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
							("intro-mouth-r1")->draw(1, 1, 1, f, (464 - 1*60) - (500 + 1*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-mouth-r2")->draw(1, 1, 1, f, (464 - 1*60) - (500 + 1*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
							("intro-mouth-r3")->draw(1, 1, 1, f, (464 - 1*60) - (500 + 1*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
							("intro-mouth-r2")->draw(1, 1, 1, f, (464 - 1*60) - (500 + 1*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
				}
				if (m_endsequence_cycle > 500 + 1*150 &&
						m_endsequence_cycle <= 1600) {
					// standing:
					int sp = (m_endsequence_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-mouth-standing1")->draw(1, 1, 1, f, float(464 - 1*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-mouth-standing2")->draw(1, 1, 1, f, float(464 - 1*60), 312, 0, 0, 1);
				}
				if (m_endsequence_cycle > 1600) {
					// happy:
					int sp = (m_endsequence_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-mouth-happy1")->draw(1, 1, 1, f, float(464 - 1*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-mouth-happy2")->draw(1, 1, 1, f, float(464 - 1*60), 312, 0, 0, 1);
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
				if (m_endsequence_cycle > rope_start_cycle &&
						m_endsequence_cycle <= rope_start_cycle + rope_cycles) {
					int fragments = (m_endsequence_cycle - rope_start_cycle)*64 / rope_cycles;
					for (int i = 0; i < fragments; ++i)
						rope->draw(1, 1, 1, f, float(i * 10), float(rope_y + i*10*rope_y_delta/640 + wave_amplitude * sin(i*10 * 2*M_PI/wave_lenght)), 0, 0, 1);
				}

				// Data descending
				if (m_endsequence_cycle > 300 + 2*150 &&
						m_endsequence_cycle <= 500 + 2*150) {
					float x = (464 - 2*60) - (500 + 2*150 - m_endsequence_cycle)*2.5f;
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
					else if (m_endsequence_cycle > rope_start_cycle + rope_cycles) {
						for (int i = 0; i < 640; i+=10)
							rope->draw(1, 1, 1, f, float(i), rope_y + i*rope_y_delta/640, 0, 0, 1);
					}

					// The kiddo
					int sp = (m_endsequence_cycle / 8) % 4;
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
				else if (m_endsequence_cycle > rope_start_cycle + rope_cycles) {
					// Rope dissapearing
					float rf;
					if (m_endsequence_cycle < 900)
						rf = 1.0;
					else if (m_endsequence_cycle > 1000)
						rf = 0.0;
					else
						rf = (1000 - m_endsequence_cycle) / 100.0f;

					for (int i = 0; i < 640; i+=10)
						rope->draw(1, 1, 1, rf, float(i), rope_y + i*rope_y_delta/640, 0, 0, 1);
				}

				if (m_endsequence_cycle > 500 + 2*150 &&
						m_endsequence_cycle <= 1600) {
					// standing:
					int sp = (m_endsequence_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-data-standing1")->draw(1, 1, 1, f, float(464 - 2*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-data-standing2")->draw(1, 1, 1, f, float(464 - 2*60), 312, 0, 0, 1);
				}
				if (m_endsequence_cycle > 1600) {
					// happy:
					int sp = (m_endsequence_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-data-happy1")->draw(1, 1, 1, f, float(464 - 2*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-data-happy2")->draw(1, 1, 1, f, float(464 - 2*60), 312, 0, 0, 1);
				}

				// Chunk
				if (m_endsequence_cycle > 300 + 3*150 &&
						m_endsequence_cycle <= 500 + 3*150) {
					// running right:
					int sp = (m_endsequence_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
							("intro-chunk-r1")->draw(1, 1, 1, f, (464 - 3*60) - (500 + 3*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-chunk-r2")->draw(1, 1, 1, f, (464 - 3*60) - (500 + 3*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
							("intro-chunk-r3")->draw(1, 1, 1, f, (464 - 3*60) - (500 + 3*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
							("intro-chunk-r2")->draw(1, 1, 1, f, (464 - 3*60) - (500 + 3*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
				}
				if (m_endsequence_cycle > 500 + 3*150 &&
						m_endsequence_cycle <= 1600) {
					// standing:
					int sp = (m_endsequence_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-chunk-standing1")->draw(1, 1, 1, f, float(464 - 3*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-chunk-standing2")->draw(1, 1, 1, f, float(464 - 3*60), 312, 0, 0, 1);
				}
				if (m_endsequence_cycle > 1600) {
					// happy:
					int sp = (m_endsequence_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-chunk-happy1")->draw(1, 1, 1, f, float(464 - 3*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-chunk-happy2")->draw(1, 1, 1, f, float(464 - 3*60), 312, 0, 0, 1);
				}

				// Brand
				if (m_endsequence_cycle > 300 + 4*150 &&
						m_endsequence_cycle <= 500 + 4*150) {
					// running right:
					int sp = (m_endsequence_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
							("intro-brand-r1")->draw(1, 1, 1, f, (464 - 4*60) - (500 + 4*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-brand-r2")->draw(1, 1, 1, f, (464 - 4*60) - (500 + 4*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
							("intro-brand-r3")->draw(1, 1, 1, f, (464 - 4*60) - (500 + 4*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
							("intro-brand-r2")->draw(1, 1, 1, f, (464 - 4*60) - (500 + 4*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
				}
				if (m_endsequence_cycle > 500 + 4*150 &&
						m_endsequence_cycle <= 1600) {
					// standing:
					int sp = (m_endsequence_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-brand-standing1")->draw(1, 1, 1, f, float(464 - 4*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-brand-standing2")->draw(1, 1, 1, f, float(464 - 4*60), 312, 0, 0, 1);
				}
				if (m_endsequence_cycle > 1600) {
					// happy:
					int sp = (m_endsequence_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-brand-happy1")->draw(1, 1, 1, f, float(464 - 4*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-brand-happy2")->draw(1, 1, 1, f, float(464 - 4*60), 312, 0, 0, 1);
				}

				// Andy
				if (m_endsequence_cycle > 300 + 5*150 &&
						m_endsequence_cycle <= 500 + 5*150) {
					// running right:
					int sp = (m_endsequence_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
							("intro-andy-r1")->draw(1, 1, 1, f, (464 - 5*60) - (500 + 5*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-andy-r2")->draw(1, 1, 1, f, (464 - 5*60) - (500 + 5*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
							("intro-andy-r3")->draw(1, 1, 1, f, (464 - 5*60) - (500 + 5*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
							("intro-andy-r2")->draw(1, 1, 1, f, (464 - 5*60) - (500 + 5*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
				}
				if (m_endsequence_cycle > 500 + 5*150 &&
						m_endsequence_cycle <= 1600) {
					// standing:
					int sp = (m_endsequence_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-andy-standing1")->draw(1, 1, 1, f, float(464 - 5*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-andy-standing2")->draw(1, 1, 1, f, float(464 - 5*60), 312, 0, 0, 1);
				}
				if (m_endsequence_cycle > 1600) {
					// happy:
					int sp = (m_endsequence_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-andy-happy1")->draw(1, 1, 1, f, float(464 - 5*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-andy-happy2")->draw(1, 1, 1, f, float(464 - 5*60), 312, 0, 0, 1);
				}

				// Steff
				if (m_endsequence_cycle > 300 + 6*150 &&
						m_endsequence_cycle <= 500 + 6*150) {
					// running right:
					int sp = (m_endsequence_cycle / 8) % 4;
					if (sp == 0)
						m_GLTM->get
							("intro-steff-r1")->draw(1, 1, 1, f, (464 - 6*60) - (500 + 6*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-steff-r2")->draw(1, 1, 1, f, (464 - 6*60) - (500 + 6*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 2)
						m_GLTM->get
							("intro-steff-r3")->draw(1, 1, 1, f, (464 - 6*60) - (500 + 6*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
					if (sp == 3)
						m_GLTM->get
							("intro-steff-r2")->draw(1, 1, 1, f, (464 - 6*60) - (500 + 6*150 - m_endsequence_cycle)*2.5f, 312, 0, 0, 1);
				}
				if (m_endsequence_cycle > 500 + 6*150 &&
						m_endsequence_cycle <= 1600) {
					// standing:
					int sp = (m_endsequence_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-steff-standing1")->draw(1, 1, 1, f, float(464 - 6*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-steff-standing2")->draw(1, 1, 1, f, float(464 - 6*60), 312, 0, 0, 1);
				}
				if (m_endsequence_cycle > 1600) {
					// happy:
					int sp = (m_endsequence_cycle / 16) % 2;
					if (sp == 0)
						m_GLTM->get
							("intro-steff-happy1")->draw(1, 1, 1, f, float(464 - 6*60), 312, 0, 0, 1);
					if (sp == 1)
						m_GLTM->get
							("intro-steff-happy2")->draw(1, 1, 1, f, float(464 - 6*60), 312, 0, 0, 1);
				}

                // Fratelli:
                if (m_endsequence_cycle > 1400 &&
                        m_endsequence_cycle <= 1500) {
                    // walking left:
                    int sp = (m_endsequence_cycle / 8) % 2;
                    if (sp == 0)
                        m_GLTM->get
                        ("intro-fratelli-l1")->draw(1, 1, 1, f, 560 + (1500 - m_endsequence_cycle)*2.5f, 296, 0, 0, 1);
                    if (sp == 1)
                        m_GLTM->get
                        ("intro-fratelli-l2")->draw(1, 1, 1, f, 560 + (1500 - m_endsequence_cycle)*2.5f, 296, 0, 0, 1);
                }
                if (m_endsequence_cycle > 1500) {
                    // taken down:
                    int sp = (m_endsequence_cycle / 8) % 2;
					int s2 = (m_endsequence_cycle / 5) % 3;
					if (s2 == 0) m_GLTM->get("intro-character-stars1")->draw(1, 1, 1, f, 580, 334, 0, 0, 1);
					if (s2 == 1) m_GLTM->get("intro-character-stars2")->draw(1, 1, 1, f, 580, 334, 0, 0, 1);
					if (s2 == 2) m_GLTM->get("intro-character-stars3")->draw(1, 1, 1, f, 580, 334, 0, 0, 1);

                    if (sp == 0)
                        m_GLTM->get
                        ("intro-fratelli-down1")->draw(1, 1, 1, f, 560, 296, 0, 0, 1);
                    if (sp == 1)
                        m_GLTM->get
                        ("intro-fratelli-down2")->draw(1, 1, 1, f, 560, 296, 0, 0, 1);
                }
                if (!scissor_state)
                    glDisable(GL_SCISSOR_TEST);
                if (scissor_state)
                    glScissor(scissor_window[0], scissor_window[1], scissor_window[2], scissor_window[3]);


                {
                    char tmp[24];

                    sprintf(tmp, "the end");
                    // 1: text appearing
                    if (m_endsequence_state == 2 && m_endsequence_cycle > 1500 && m_endsequence_cycle < 1550) {
                        int x;
                        float f = (1 - ((m_endsequence_cycle - 1500) / 50.0f));
                        f *= f;
                        x = (int)(320 + f * 640);
                        font_print_c(x, 150, 0, 0, 1, "font", tmp, -2);
                    }
                    // 2: text braking
                    if (m_endsequence_state == 2 && m_endsequence_cycle >= 1550) {
                        font_print_c(320, 150, 0, 0, 1, "font", tmp, -2);
                    }
                    // 3: text disappearing
                    if (m_endsequence_state == 3 && m_endsequence_cycle > 1950) {
                        int x;
                        float f = (((m_state_cycle - 1950) / 50.0f));
                        f *= f;
                        x = (int)(320 - f * 640);
                        font_print_c(x, 150, 0, 0, 1, "font", tmp, -2);
                    }
                }

				// fading square
                if (m_endsequence_state == 2)
                    f2 = float(50 - m_state_cycle) / 50;

                if (m_endsequence_state == 3)
					f2 = float(m_state_cycle) / 50;

                glColor4f(0, 0, 0, f2);
                glBegin(GL_QUADS);
                glVertex3f(0, 0, 0);
                glVertex3f(0, 480, 0);
                glVertex3f(640, 480, 0);
                glVertex3f(640, 0, 0);
                glEnd();
            }
            break;

        case 4:
        case 5:
        case 6: {
                char *text[] = {
                                   "*- original game -",
                                   ".konami 1986",
                                   ".",
                                   "*- remake -",
                                   ".brain games 2006",
                                   ".",
                                   "*coding:",
                                   ".santi \"popolon\" onta–on",
                                   ".jorrith \"jorito\" schaap",
                                   ".patrick \"patsie\" lina",
                                   ".francisco \"nenefranz\" wis",
                                   ".",
                                   "*graphics:",
                                   ".david \"dfcastelao\" fernandez",
                                   ".miikka \"mp83\" poikela",
                                   ".daedalus",
                                   ".kelesisv",
                                   ".francisco \"nenefranz\" wis",
                                   ".",
                                   "*music/sfx:",
                                   ".jorrith \"jorito\" schaap",
                                   ".",
                                   "*map creation:",
                                   ".jesus \"[dk]\" perez rosales",
                                   ".mauricio \"iamweasel\" braga",
                                   ".patrick \"patsie\" lina",
                                   ".santi \"popolon\" onta–on",
                                   ".",
                                   "*beta testers:",
                                   ".all of the above",
                                   ".albert \"bifimsx\" beevendorp",
                                   ".patrick \"vampier\" van arkel",
                                   ".",
                                   "*special thanks:",
                                   ".jason eames for web hosting",
                                   ".jay r zay for the menu font",
                                   ".the openmsx and bluemsx teams",
                                   ".",
                                   ""};

                float f = 1;
                int start_y = 500 - m_endsequence_cycle / 4;
                int i;

                if (m_endsequence_state == 5)
                    f = 1;
                if (m_endsequence_state == 4)
                    f = float(m_state_cycle) / 50;
                if (m_endsequence_state == 6)
                    f = float(50 - m_state_cycle) / 50;

                m_GLTM->get
                ("bg_copyright_3")->draw(0, 0, 0, 0, 1);

                for (i = 0;text[i][0] != 0;i++) {
                    if ((start_y + i*30) < 500)
                        if (text[i][0] == '.')
                            font_print_c(320, start_y + i*45, 0, 0, 0.8f, "font", text[i] + 1, -2);
                        else
                            font_print_c(320, start_y + i*45, 0, 0, 1, "font_hl", text[i] + 1, -2);
                }

                if (f != 1) {
                    glColor4f(0, 0, 0, 1 - f);
                    glBegin(GL_QUADS);
                    glVertex3f(0, 0, 0);
                    glVertex3f(0, 480, 0);
                    glVertex3f(640, 480, 0);
                    glVertex3f(640, 0, 0);
                    glEnd();
                }

            }
            break;
    }
}
