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

int TheGooniesApp::msx_cycle(KEYBOARDSTATE *k)
{
    if ((k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]) ||
            (k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
			(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)]))
	{
            m_skip_intro_screens = true;
        if (m_state_cycle < 450) {
            if (m_state_cycle < 50)
                m_state_cycle = 500 - m_state_cycle;
            else
                m_state_cycle = 450;
        } 
    }

    if (m_state_cycle >= 500) {
        if (m_skip_intro_screens) {
            m_title_state = 0;
            m_title_current_menu = 0;
            m_title_next_menu = 0;
            return THEGOONIES_STATE_TITLE;
        } else {
            return THEGOONIES_STATE_KONAMI;
        }
    }

    return THEGOONIES_STATE_MSX;
}

void TheGooniesApp::msx_draw(SFXManager *SFXM)
{
    if (m_state_cycle < 50) {
        glClearColor(0, 0, float(m_state_cycle) / 50, 1);
    } else {
        glClearColor(0, 0, 1, 1);
    }
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_state_cycle > 100) {
        if (m_state_cycle < 150) {
            bool scissor_state = (glIsEnabled(GL_SCISSOR_TEST) ? true : false);
            int scissor_window[4];

            glEnable(GL_SCISSOR_TEST);
            if (scissor_state)
                glGetIntegerv(GL_SCISSOR_BOX, scissor_window);
            glScissor(screen_x(0), screen_y(206), screen_x(640), screen_y(480 - 206));
            // MSX sign appearing:
            m_GLTM->get
            ("msx")->draw(110, 274 - 160*((m_state_cycle - 100) / 50.0F), 0, 0, 1);
            if (!scissor_state)
                glDisable(GL_SCISSOR_TEST);
            if (scissor_state)
                glScissor(scissor_window[0], scissor_window[1], scissor_window[2], scissor_window[3]);
        } else {
            m_GLTM->get
            ("msx")->draw(110, 114, 0, 0, 1);
        }
        if (m_state_cycle > 150) {
            if (m_state_cycle < 150 + 25) {
                // MSX text 1 appearing:
                bool scissor_state = (glIsEnabled(GL_SCISSOR_TEST) ? true : false);
                int scissor_window[4];

                glEnable(GL_SCISSOR_TEST);
                if (scissor_state)
                    glGetIntegerv(GL_SCISSOR_BOX, scissor_window);
                glScissor(screen_x(110), screen_y(0), screen_x((int)(423*((m_state_cycle - 150) / 25.0F))), screen_y(480));
                // MSX sign appearing:
                m_GLTM->get
                ("msx-text1")->draw(160, 282, 0, 0, 1);
                if (!scissor_state)
                    glDisable(GL_SCISSOR_TEST);
                if (scissor_state)
                    glScissor(scissor_window[0], scissor_window[1], scissor_window[2], scissor_window[3]);
            } else {
                m_GLTM->get
                ("msx-text1")->draw(160, 282, 0, 0, 1);
            }
        }
        if (m_state_cycle > 163) {
            if (m_state_cycle < 163 + 25) {
                // MSX text 2 appearing:
                bool scissor_state = (glIsEnabled(GL_SCISSOR_TEST) ? true : false);
                int scissor_window[4];

                glEnable(GL_SCISSOR_TEST);
                if (scissor_state)
                    glGetIntegerv(GL_SCISSOR_BOX, scissor_window);
                glScissor(screen_x(110), screen_y(0), screen_x((int)(423*((m_state_cycle - 163) / 25.0F))), screen_y(480));
                // MSX sign appearing:
                m_GLTM->get
                ("msx-text2")->draw(160, 302, 0, 0, 1);
                if (!scissor_state)
                    glDisable(GL_SCISSOR_TEST);
                if (scissor_state)
                    glScissor(scissor_window[0], scissor_window[1], scissor_window[2], scissor_window[3]);
            } else {
                m_GLTM->get
                ("msx-text2")->draw(160, 302, 0, 0, 1);
            }
        }
    }

    if (m_state_cycle == 350) {
        SFXM->SFX_play("sfx/msxbeep", m_sfx_volume);
    }

    if (m_state_cycle > 450) {
        glEnable(GL_COLOR_MATERIAL);

        {
            float f = 0;
            f = abs(int(m_state_cycle - 450)) / 50.0F;
            glColor4f(0, 0, 0, f);
        }
        glNormal3f(0.0, 0.0, 1.0);

        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 480, 0);
        glVertex3f(640, 480, 0);
        glVertex3f(640, 0, 0);
        glEnd();
    }
}
