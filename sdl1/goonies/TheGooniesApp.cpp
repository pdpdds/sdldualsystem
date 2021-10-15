#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
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
#include "TheGooniesCtnt.h"
#include "TheGooniesApp.h"
#include "Level.h"
#include "LevelPack.h"

#include "font_extractor.h"

/* fps counter: */
extern int frames_per_sec;
extern int frames_per_sec_tmp;

extern int init_time;
extern bool show_fps;
extern bool fullscreen;
extern bool screen_shake;
extern bool water_reflection;
extern bool ambient_light;

extern int difficulty;
extern int hiscore;
extern int current_cycle;

TheGooniesApp::TheGooniesApp()
{
    m_state = THEGOONIES_STATE_MSX;
    // m_state = THEGOONIES_STATE_EDITOR;
    // m_state = THEGOONIES_STATE_ENDSEQUENCE;

	m_previous_state = m_state;
    m_state_cycle = 0;
    m_title_state = 0;
    m_title_option_selected = 0;
    m_title_waiting_keystroke = false;
    m_time_since_last_key = 0;

    m_skip_intro_screens = false;

    m_titleanimation_state = 0;
    m_titleanimation_cycle = 0;

	m_title_password[0]=0;

    m_interlevel_state = 0;
    m_interlevel_cycle = 0;

    m_game_state = 0;

    m_gameover_state = 0;
    m_endsequence_state = 0;
	m_endsequence_speed = 0;
	m_endsequence_shift = 0;

	m_mouse_x=0;
	m_mouse_y=0;
	m_draw_mouse_status=0;
	m_draw_mouse=0.0f;
	m_mouse_movement_timmer=0;
	m_mouse_button=0;

	m_editor_levelpack = 0;
	m_editor_level =0;

    m_screen_dx = SCREEN_X;
    m_screen_dy = SCREEN_Y;
    fullscreen = true;

	screen_shake = true;
	water_reflection = true;
	ambient_light = true;	
	
    m_GLTM = new GLTManager();

    m_SFXM = new SFXManager();
    m_SFXM->cache("sfx");
//	m_GLTM->cache();
    m_MusicM = new MusicManager();

    // default keyboard config
    m_keys_configuration[GKEY_UP] = SDLK_UP;
    m_keys_configuration[GKEY_RIGHT] = SDLK_RIGHT;
    m_keys_configuration[GKEY_DOWN] = SDLK_DOWN;
    m_keys_configuration[GKEY_LEFT] = SDLK_LEFT;
    m_keys_configuration[GKEY_FIRE] = SDLK_SPACE;
    m_keys_configuration[GKEY_PAUSE] = SDLK_F1;
    m_keys_configuration[GKEY_QUIT] = SDLK_ESCAPE;

    m_sfx_volume = MIX_MAX_VOLUME;
    m_music_volume = 96;
    m_ambient_volume = 96;
    m_game = 0;

    m_vc = new VirtualController();
	m_num_joysticks = SDL_NumJoysticks();
	if (m_num_joysticks > 0) {
		m_joystick = SDL_JoystickOpen(0);
	} else {
		m_joystick = 0;
	}

//    font_extract("font", "graphics/font.png", 10 + 13 + 13 + 10 + 14 + 8, "1234567890abcdefghijklmnopqrstuvwxyzXXXXXXXXXX-./:\"#$%!?:;.,'`[]{|}?);
//    font_extract("font_hl", "graphics/font_highlighted.png", 10 + 13 + 13 + 10 + 14 + 8, "1234567890abcdefghijklmnopqrstuvwxyzXXXXXXXXXX-./:\"#$%!?:;.,'`[]{|}?);
    font_extract("font", "graphics/font.png", 10 + 13 + 13 + 10 + 14 + 8, "1234567890abcdefghijklmnopqrstuvwxyz-!?:;.,'/\"#%[]{|}?");
    font_extract("font_hl", "graphics/font_highlighted.png", 10 + 13 + 13 + 10 + 14 + 8, "1234567890abcdefghijklmnopqrstuvwxyz-!?:;.,'/\"#%[]{|}?");
    font_extract("font_hud", "graphics/font_hud.png", 11, "0123456789-");

    m_test_game = 0;
    load_configuration();
	load_hiscores();
}


TheGooniesApp::~TheGooniesApp()
{
    if (m_game != 0) {
        delete m_game;
	    m_game = 0;
    } // if

	if (m_editor_levelpack !=0) {
		delete m_editor_levelpack;
		m_editor_levelpack = 0;
	} // if

    delete m_GLTM;
    delete m_SFXM;
    delete m_MusicM;

    font_release();

	if (m_joystick != 0) {
		SDL_JoystickClose(m_joystick);
		m_joystick=0;
	}
	
    if (m_vc != 0) {
		delete m_vc;
        m_vc = 0;
    }
    save_configuration();
	save_hiscores();
}


void TheGooniesApp::MouseClick(int mousex,int mousey)
{
	m_mouse_click_x.Add(new int(mousex));
	m_mouse_click_y.Add(new int(mousey));
} /* TheGooniesApp::MouseClick */


bool TheGooniesApp::cycle(KEYBOARDSTATE *k)
{
    int old_state = m_state;

#ifdef __DEBUG_MESSAGES

    if (state_cycle == 0) {
        output_debug_message("First Cycle started for state %i...\n", m_state);
    }
#endif

    switch (m_state) {
        case THEGOONIES_STATE_SPLASH:
            m_state = splash_cycle(k);
            break;
        case THEGOONIES_STATE_MSX:
            m_state = msx_cycle(k);
            break;
        case THEGOONIES_STATE_KONAMI:
            m_state = konami_cycle(k);
            break;
        case THEGOONIES_STATE_TITLE:
            m_state = title_cycle(k);
            break;
        case THEGOONIES_STATE_TITLEANIMATION:
            m_state = titleanimation_cycle(k);
            break;
        case THEGOONIES_STATE_GAMESTART:
            m_state = gamestart_cycle(k);
            break;
        case THEGOONIES_STATE_GAME:
            m_state = game_cycle(k);
            break;
        case THEGOONIES_STATE_GAMEOVER:
            m_state = gameover_cycle(k);
            break;
        case THEGOONIES_STATE_ENDSEQUENCE:
            m_state = endsequence_cycle(k);
            break;
        case THEGOONIES_STATE_INTERLEVEL:
            m_state = interlevel_cycle(k);
            break;
        case THEGOONIES_STATE_HOWTOPLAY:
            m_state = howtoplay_cycle(k);
            break;
        case THEGOONIES_STATE_CREDITS:
            m_state = credits_cycle(k);
            break;
        case THEGOONIES_STATE_EDITOR:
            m_state = editor_cycle(k);
            break;
        case THEGOONIES_STATE_MAPEDITOR:
            m_state = mapeditor_cycle(k);
            break;
        default:
            return false;
    }

    if (m_game != 0 && m_game->get_hiscore() > hiscore) {
        set_hiscore(m_game->get_hiscore());
    }

    if (old_state == m_state) {
        m_state_cycle++;
    } else {
        m_state_cycle = 0;

#ifdef __DEBUG_MESSAGES
        output_debug_message("State change: %i -> %i\n", old_state, m_state);
#endif
    }

    m_SFXM->next_cycle();
    m_MusicM->next_cycle();

	m_previous_state = old_state;

    return true;
}

void TheGooniesApp::draw()
{
    float lightpos[4] = {0, 0, -1000, 0};
    float tmpls[4] = {1.0F, 1.0F, 1.0F, 1.0};
    float tmpld[4] = {1.0F, 1.0F, 1.0F, 1.0};
    float tmpla[4] = {1.0F, 1.0F, 1.0F, 1.0};
    float ratio;

    m_screen_dx = SCREEN_X;
    m_screen_dy = SCREEN_Y;

#ifdef __DEBUG_MESSAGES

    if (state_cycle == 0) {
        output_debug_message("First Drawing cycle started for state %i...\n", state);
    }
#endif

    /* Enable Lights, etc.: */
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective Calculations
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, tmpla);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, tmpld);
    glLightfv(GL_LIGHT0, GL_SPECULAR, tmpls);

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT, GL_FILL);

    glClearColor(0, 0, 0, 0.0);
    glViewport(0, 0, SCREEN_X, SCREEN_Y);
    ratio = (float)SCREEN_X / float(SCREEN_Y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, ratio, 1.0, 10240.0);
    gluLookAt(320, 240, PERSPECTIVE_DISTANCE, 320, 240, 0, 0, -1, 0); /* for 640x480 better */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    switch (m_state) {
        case THEGOONIES_STATE_SPLASH:
            splash_draw();
            break;
        case THEGOONIES_STATE_MSX:
            // FIXME: this is a bit jucky; m_SFXM should be passed in the
            // constructor, not the draw() method
            msx_draw(m_SFXM);
            break;
        case THEGOONIES_STATE_KONAMI:
            konami_draw();
            break;
        case THEGOONIES_STATE_TITLE:
            title_draw();
            break;
        case THEGOONIES_STATE_TITLEANIMATION:
            titleanimation_draw();
            break;
        case THEGOONIES_STATE_GAMESTART:
            gamestart_draw();
            break;
        case THEGOONIES_STATE_GAME:
            game_draw();
            break;
        case THEGOONIES_STATE_GAMEOVER:
            gameover_draw();
            break;
        case THEGOONIES_STATE_ENDSEQUENCE:
            endsequence_draw();
            break;
        case THEGOONIES_STATE_INTERLEVEL:
            interlevel_draw();
            break;
        case THEGOONIES_STATE_HOWTOPLAY:
            howtoplay_draw();
            break;
        case THEGOONIES_STATE_CREDITS:
            credits_draw();
            break;
        case THEGOONIES_STATE_EDITOR:
            editor_draw();
            break;
        case THEGOONIES_STATE_MAPEDITOR:
            mapeditor_draw();
            break;
    }

    if (show_fps) {
        char tmp[80];
        sprintf(tmp, "video mem: %.4gmb - fps: %i", float(GLTile::get_memory_used()) / float(1024 * 1024), frames_per_sec);
        font_print_c(320, 460, 0, 0, 0.5f, "font_hl", tmp, -2);
    }

    glDisable(GL_BLEND);

    SDL_GL_SwapBuffers();
}


int TheGooniesApp::screen_x(int x)
{
    return ((x * m_screen_dx) / SCREEN_X);
}


int TheGooniesApp::screen_y(int y)
{
    return ((y*m_screen_dy) / SCREEN_Y);
}


void TheGooniesApp::save_configuration(void)
{
    int i;
    FILE *fp;
    char cfg[255];
#ifdef _WIN32

    sprintf(cfg, "goonies.cfg");
#else

    snprintf(cfg, 255, "%s/.goonies.cfg", getenv("HOME"));
#endif //_WIN32

    fp = fopen(cfg, "w");
    if (fullscreen) {
        fprintf(fp, "fullscreen\n");
    } else {
        fprintf(fp, "windowed\n");
    }
    fprintf(fp, "%i %i %i\n" , m_sfx_volume, m_music_volume, m_ambient_volume);
    fprintf(fp, "%i\n", difficulty);

    for (i = 0; i < 7; i++) {
        fprintf(fp, "%i ", m_keys_configuration[i]);
    }
    fprintf(fp, "\n");

	fprintf(fp, "screen_shake_%s\n", (screen_shake ? "on" : "off"));
	fprintf(fp, "water_reflection_%s\n", (water_reflection ? "on" : "off"));
	fprintf(fp, "ambient_light_%s\n", (ambient_light ? "on" : "off"));		

    fclose(fp);
}


void TheGooniesApp::load_configuration(void)
{
    FILE *fp;
    char cfg[255];
#ifdef _WIN32

    sprintf(cfg, "goonies.cfg");
#else

    snprintf(cfg, 255, "%s/.goonies.cfg", getenv("HOME"));
#endif //_WIN32

    fp = fopen(cfg, "r");
    if (fp == 0) {
        save_configuration();
    } else {
        char tmp_s[80];
        int i;

        // fullscreen / windowed
        fscanf(fp, "%s", tmp_s);
        if (strcmp(tmp_s, "fullscreen") == 0) {
            fullscreen = true;
        } else {
            fullscreen = false;
        }
        // volumes
        fscanf(fp, "%i %i %i", &m_sfx_volume, &m_music_volume, &m_ambient_volume);

        // difficulty
        fscanf(fp, "%i", &difficulty);

        // keyboard
        for (i = 0; i < 7; i++) {
            fscanf(fp, "%i", &(m_keys_configuration[i]));
        }
		// screen shake
		fscanf(fp, "%s", tmp_s);
		screen_shake = (strcmp(tmp_s, "screen_shake_on") ? false : true);

		// water reflections
		fscanf(fp, "%s", tmp_s);
		water_reflection = (strcmp(tmp_s, "water_reflection_on") ? false : true);

		fscanf(fp, "%s", tmp_s);
		ambient_light = (strcmp(tmp_s, "ambient_light_on") ? false : true);
		
        fclose(fp);
    }
}


void TheGooniesApp::save_hiscores(void)
{
    FILE *fp;
    char cfg[255];
#ifdef _WIN32

    sprintf(cfg, "goonies.hi");
#else

    snprintf(cfg, 255, "%s/.goonies.hi", getenv("HOME"));
#endif //_WIN32

    fp = fopen(cfg, "w");
	fprintf(fp, "%i\n", hiscore);
    fclose(fp);
}

void TheGooniesApp::load_hiscores(void)
{
    FILE *fp;
    char cfg[255];
#ifdef _WIN32

    sprintf(cfg, "goonies.hi");
#else

    snprintf(cfg, 255, "%s/.goonies.hi", getenv("HOME"));
#endif //_WIN32

    fp = fopen(cfg, "r");
    if (fp == 0) {
        save_hiscores();
    } else {
		hiscore = 0;
		fscanf(fp, "%i", &hiscore);

        fclose(fp);
    }
}
