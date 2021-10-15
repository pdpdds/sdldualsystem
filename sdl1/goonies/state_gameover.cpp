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

extern int score;

int TheGooniesApp::gameover_cycle(KEYBOARDSTATE *k)
{
    if (m_game != 0)
        m_game->cycle(m_vc, m_GLTM, m_SFXM, m_MusicM);

    if (m_gameover_state == 0) {
		score = 0;
        if (m_state_cycle > 50) {
            m_gameover_state = 1;
            m_state_cycle = 0;
        }
    }
    if (m_gameover_state == 1) {
        if (m_state_cycle > 50) {
            if (m_game != 0) {
                delete m_game;
                m_game = 0;
				m_SFXM->SFX_stop_continuous();
            }
            m_gameover_state = 2;
            m_state_cycle = 0;
        }
    }
    if (m_gameover_state == 2) {
        if (m_state_cycle > 250) {
			m_title_state = 0;
			m_title_option_selected = 0;
			m_title_waiting_keystroke = false;		
			m_title_current_menu = 0;
			m_title_next_menu = 0;
			return THEGOONIES_STATE_TITLE;
        }
    }
    return THEGOONIES_STATE_GAMEOVER;
}

void TheGooniesApp::gameover_draw(void)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_game != 0)
        m_game->draw(m_GLTM);
    if (m_gameover_state == 1) {
        float f = m_state_cycle / 50.0F;
        glEnable(GL_COLOR_MATERIAL);
        glColor4f(0, 0, 0, 1);

        glNormal3f(0.0, 0.0, 1.0);

        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 480, 0);
        glVertex3f(f*640, 480, 0);
        glVertex3f(f*640, 0, 0);
        glEnd();
    }
	
    if (m_gameover_state == 2) {

        // 1: text appearing
        if (m_state_cycle >= 0 && m_state_cycle < 50) {
            int x;
            float f = (1 - (m_state_cycle / 50.0f));
			
            f *= f;
            x = (int)(112 + f * 640);
			m_GLTM->get("gameover")->draw(1, 1, 1, 1, float(x), 150, 0, 0, 1);
        }
		
        // 2: text braking
        if (m_state_cycle >= 50 && m_state_cycle < 150) {
			m_GLTM->get("gameover")->draw(1, 1, 1, 1, 112, 150, 0, 0, 1);
        }
		
        // 3: text disappearing
        if (m_state_cycle >= 150 && m_state_cycle < 200) {
            int x;
            float f = (((m_state_cycle - 150) / 50.0f));

            f *= f;
            x = (int)(112 - f * 640);
			m_GLTM->get("gameover")->draw(1, 1, 1, 1, float(x), 150, 0, 0, 1);
        }
    }
}
