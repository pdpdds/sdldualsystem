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
#include "GMap.h"
#include "TheGoonies.h"
#include "TheGooniesApp.h"

extern int hiscore;

int TheGooniesApp::gamestart_cycle(KEYBOARDSTATE *k)
{
    int i;
    char *passwords[5] = {"goonies", "mr sloth", "goon docks", "doubloon", "one eyed willy"};

    m_current_level = 0;

    for (i = 0;i < 5;i++) {
        if (strcmp(m_title_password, passwords[i]) == 0) {
            m_current_level = i;
            break;
        }
    }

    switch (m_current_level) {
        case 1:
            m_game = new TheGoonies(2, 1, 2, m_sfx_volume, m_music_volume, m_ambient_volume, m_GLTM);
            break;
        case 2:
            m_game = new TheGoonies(3, 1, 1, m_sfx_volume, m_music_volume, m_ambient_volume, m_GLTM);
            break;
        case 3:
            m_game = new TheGoonies(4, 4, 1, m_sfx_volume, m_music_volume, m_ambient_volume, m_GLTM);
            break;
        case 4:
            m_game = new TheGoonies(5, 1, 2, m_sfx_volume, m_music_volume, m_ambient_volume, m_GLTM);
            break;
        default:
            m_game = new TheGoonies(1, 1, 1, m_sfx_volume, m_music_volume, m_ambient_volume, m_GLTM);
    }

    m_game->set_hiscore(hiscore);

    m_vc->reset();
    m_game_state = 0;
    return THEGOONIES_STATE_GAME;
}

void TheGooniesApp::gamestart_draw(void)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}
