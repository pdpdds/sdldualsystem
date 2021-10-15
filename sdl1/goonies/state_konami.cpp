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



int TheGooniesApp::konami_cycle(KEYBOARDSTATE *k)
{
    if (m_game != 0) {
        delete m_game;
        m_game = 0;
		m_SFXM->SFX_stop_continuous();
    }

    // cache the graphics:
    if (m_state_cycle == 0) {
        m_GLTM->get_smooth("presentation_bg");
        m_GLTM->get
        ("presentation_texts");
    }

    if ((k->keyboard[m_keys_configuration[GKEY_QUIT]] && !k->old_keyboard[m_keys_configuration[GKEY_QUIT]]) ||
            (k->keyboard[m_keys_configuration[GKEY_FIRE]] && !k->old_keyboard[m_keys_configuration[GKEY_FIRE]]) ||
			(k->keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)] && !k->old_keyboard[(k->joystick_0_pos + 0 * k->joystick_size + 4)]))
	{
        if (m_state_cycle < 450) {
            if (m_state_cycle < 50)
                m_state_cycle = 500 - m_state_cycle;
            else
                m_state_cycle = 450;
        }
    }
    if (m_state_cycle == 150) {
        m_SFXM->SFX_play("sfx/intro", m_sfx_volume);
    }

    if (m_state_cycle >= 500) {
        m_title_state = 0;
        m_title_current_menu = 0;
        m_title_next_menu = 0;
        return THEGOONIES_STATE_TITLE;
    }

    return THEGOONIES_STATE_KONAMI;
} /* TheGooniesApp::konami_cycle */


void TheGooniesApp::konami_draw(void)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_state_cycle >= 0 && m_state_cycle < 50)
        m_GLTM->get_smooth("presentation_bg")->draw((m_state_cycle)*0.02F, (m_state_cycle)*0.02F, (m_state_cycle)*0.02F, 1.0F, 320, 240, 0, ( -1.0F + (m_state_cycle / 250.0F))*5, 1.1F + (m_state_cycle / 1000.0F));
    if (m_state_cycle >= 50 && m_state_cycle < 450)
        m_GLTM->get_smooth("presentation_bg")->draw(320, 240, 0, ( -1.0F + (m_state_cycle / 250.0F))*5, 1.1F + (m_state_cycle / 1000.0F));
    if (m_state_cycle >= 450 && m_state_cycle < 500)
        m_GLTM->get_smooth("presentation_bg")->draw((500 - m_state_cycle)*0.02F, (500 - m_state_cycle)*0.02F, (500 - m_state_cycle)*0.02F, 1.0F, 320, 240, 0, ( -1.0F + (m_state_cycle / 250.0F))*5, 1.1F + (m_state_cycle / 1000.0F));

    if (m_state_cycle >= 100 && m_state_cycle < 150)
        m_GLTM->get
        ("presentation_texts")->draw(1, 1, 1, (m_state_cycle - 100)*0.02F);
    if (m_state_cycle >= 150 && m_state_cycle < 450)
        m_GLTM->get
        ("presentation_texts")->draw();
    if (m_state_cycle >= 450 && m_state_cycle < 500)
        m_GLTM->get
        ("presentation_texts")->draw(1, 1, 1, (500 - m_state_cycle)*0.02F);


} /* TheGooniesApp::konami_draw */

