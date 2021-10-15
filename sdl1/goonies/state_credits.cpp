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

extern bool fullscreen;
extern SDL_Surface *screen_sfc;
extern SDL_Surface *toogle_video_mode(bool fullscreen);


int TheGooniesApp::credits_cycle(KEYBOARDSTATE *k)
{
    switch (m_credits_state) {
        case 0:  // appearing
			if (m_credits_cycle == 0) {
				m_MusicM->music_fadein(MUSIC_ID,"endcredits",m_music_volume,-1,50);
			}
            if (m_state_cycle >= 50) {
                m_credits_state = 1;
                m_credits_cycle = 0;
                m_state_cycle = 0;
            } // if
            break;
        case 1:  // animation
            m_credits_cycle++;
            if (m_credits_cycle >= 3600 ||
                    (k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]) ||
                    (k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
					(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)]))
			{
                m_credits_state = 2;
                m_state_cycle = 0;
            }
            break;
        case 2:  // disappearing
            m_credits_cycle++;
            if (m_state_cycle >= 50) {
                m_MusicM->music_fadein(MUSIC_ID,"title",m_music_volume,-1,50);
                m_title_current_menu = 2;
                m_title_next_menu = -1;
                m_title_state = 1;
                m_state_cycle = 0;
                m_title_option_selected = 6;
                return THEGOONIES_STATE_TITLE;
            }
            break;
    }
    return THEGOONIES_STATE_CREDITS;
}

void TheGooniesApp::credits_draw(void)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    switch (m_credits_state) {
        case 0:
        case 1:
        case 2: {
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
                                   ".david \"dfcastelao\" fernandez",
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
                int start_y = 500 - m_credits_cycle / 2;
                int i;

                if (m_credits_state == 1)
                    f = 1;
                if (m_credits_state == 0)
                    f = float(m_state_cycle) / 50;
                if (m_credits_state == 2)
                    f = float(50 - m_state_cycle) / 50;

                m_GLTM->get
                ("bg_copyright_2")->draw(0, 0, 0, 0, 1);

                for (i = 0;text[i][0] != 0;i++) {
                    if ((start_y + i*30) < 500)
                        if (text[i][0] == '.')
                            font_print_c(320, start_y + i*45, 0, 0, 0.78f, "font", text[i] + 1, -2);
                        else
                            font_print_c(320, start_y + i*45, 0, 0, 1, "font_hl", text[i] + 1, -2);
                } // for

                if (f != 1) {
                    m_GLTM->get
                    ("bg_copyright")->draw(1, 1, 1, 1 - f, 0, 0, 0, 0, 1);
                    m_GLTM->get_smooth("title_logo")->draw(1, 1, 1, 1 - f, 46, 17, 0, 0, 1);
                } // if

            }
            break;

    } // switch

} /* TheGooniesApp::credits_draw */

