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
#include "time.h"

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
#include "sound.h"
#include "keyboardstate.h"
#include "randomc.h"
#include "debug.h"

#include "GLTManager.h"
#include "SoundManager.h"
#include "MusicManager.h"
#include "SFXManager.h"
#include "GObject.h"
#include "GO_enemy.h"
#include "GO_fratelli.h"
#include "GMap.h"
#include "TheGooniesCtnt.h"
#include "TheGoonies.h"
#include "TheGooniesApp.h"



/*      GLOBAL VARIABLES INITIALIZATION:       */

char *application_name = "The Goonies";
int application_version = 0;
int N_SFX_CHANNELS = 16;
int COLOUR_DEPTH = 32;
bool sound = true;

bool fullscreen = false;
bool screen_shake = false;
bool water_reflection = false;
bool ambient_light = false;

SDL_Surface *screen_sfc = 0;
int difficulty = 100;
int score = 0;
int hiscore = 0;

TRanrotBGenerator *rg = 0;

/* Redrawing constant: */
const int REDRAWING_PERIOD = 20;

/* Frames per second counter: */
int frames_per_sec = 0;
int frames_per_sec_tmp = 0;
int init_time = 0;
bool show_fps = false;
int current_cycle = 0;



/*      AUXILIAR FUNCTION DEFINITION:       */


SDL_Surface *toogle_video_mode(bool fullscreen)
{
    SDL_Surface *sfc = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, COLOUR_DEPTH, SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0));
    SDL_WM_SetCaption(application_name, 0);
    SDL_ShowCursor(SDL_DISABLE);
    GLTile::reload_textures();

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    return sfc;
} /* toogle_video_mode */


SDL_Surface *initialization(int flags)
{
    const SDL_VideoInfo* info = 0;
    int bpp = 0;
    SDL_Surface *screen;

    rg = new TRanrotBGenerator(0);

#ifdef __DEBUG_MESSAGES
    output_debug_message("Initializing SDL\n");
#endif

    if (SDL_Init(SDL_INIT_VIDEO | (sound ? SDL_INIT_AUDIO : 0) | SDL_INIT_JOYSTICK) < 0) {
#ifdef __DEBUG_MESSAGES
        output_debug_message("Video initialization failed: %s\n", SDL_GetError());
#endif

        return 0;
    } /* if */

#ifdef __DEBUG_MESSAGES
    output_debug_message("SDL initialized\n");
#endif

    info = SDL_GetVideoInfo();

    if (!info) {
#ifdef __DEBUG_MESSAGES
        output_debug_message("Video query failed: %s\n", SDL_GetError());
#endif

        return 0;
    } /* if */

    if (fullscreen) {
        bpp = COLOUR_DEPTH;
    } else {
        bpp = info->vfmt->BitsPerPixel;
    } /* if */

#ifdef __DEBUG_MESSAGES
    output_debug_message("Setting OpenGL attributes\n");
#endif

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef __DEBUG_MESSAGES

    output_debug_message("OpenGL attributes set\n");
#endif

#ifdef __DEBUG_MESSAGES

    output_debug_message("Initializing video mode\n");
#endif

    flags = SDL_OPENGL | flags;

    // check for openGL support
    if (!SDL_VideoModeOK(SCREEN_X, SCREEN_Y, bpp, flags)) {
        // no support; print message and abort
        printf("Hmm, your system doesn't support OpenGL...\n");
        return 0;
    }

    screen = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, bpp, flags);
    if (screen == 0) {
#ifdef __DEBUG_MESSAGES
        output_debug_message("Video mode set failed: %s\n", SDL_GetError());
#endif

        return 0;
    } /* if */

#ifdef __DEBUG_MESSAGES
    output_debug_message("Video mode initialized\n");
#endif

    SDL_WM_SetCaption(application_name, 0);
    SDL_ShowCursor(SDL_DISABLE);

    if (sound) {
#ifdef __DEBUG_MESSAGES
        output_debug_message("Initializing Audio\n");
#endif

        N_SFX_CHANNELS = Sound_initialization(N_SFX_CHANNELS, 0);

#ifdef __DEBUG_MESSAGES

        output_debug_message("Audio initialized\n");
#endif

    } /* if */

    SDL_EnableUNICODE(1);

    return screen;
} /* initialization */


void finalization()
{
#ifdef __DEBUG_MESSAGES
    output_debug_message("Finalizing SDL\n");
#endif


    delete rg;
    rg = 0;

    // FIXME: apparently, sometimes windows' resolution doesn't get restored properly
    // a stupid workaround is to switch back to windowed mode before exit
    if (fullscreen) {
        SDL_SetVideoMode(SCREEN_X, SCREEN_Y, COLOUR_DEPTH, SDL_OPENGL | 0);
    }
    if (sound) {
        Sound_release();
    }
    SDL_Quit();

#ifdef __DEBUG_MESSAGES

    output_debug_message("SDL finalized\n");
#endif

} /* finalization */

FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}

int main(int argc, char** argv) {

    TheGooniesApp *game;
    KEYBOARDSTATE *k;

    // hmm... apparently we didn't seed earlier... weird...
    srand(time(0));

    int time, act_time;
    SDL_Event event;
    bool quit = false;
    bool need_to_redraw = true;

#ifdef __DEBUG_MESSAGES

    output_debug_message("Application started\n");
#endif

    screen_sfc = initialization((fullscreen ? SDL_FULLSCREEN : 0));
    if (screen_sfc == 0)
        return 0;

    k = new KEYBOARDSTATE();

    {
        bool fs = fullscreen;
        game = new TheGooniesApp();
        if (fs != fullscreen)
            screen_sfc = toogle_video_mode(fullscreen);
    }

    time = init_time = SDL_GetTicks();

    while (!quit) {
        while ( SDL_PollEvent( &event ) ) {
            switch ( event.type ) {
                    /* Keyboard event */

                    /* NICETOHAVE: a keyboard event handler that deals with key combos
                       and sets certain status flags to deal with quit/fullscreen nicely */
                case SDL_KEYDOWN:
#ifdef __APPLE__
                    // different quit shortcut on OSX: apple+Q
                    if (event.key.keysym.sym == SDLK_q) {
                        SDLMod modifiers;
                        modifiers = SDL_GetModState();
                        if ((modifiers&KMOD_META) != 0) {
                            quit = true;
                        }
                    }
#endif
#ifdef _WIN32
                    // different quit shortcut on WIN32: ALT+F4
                    if (event.key.keysym.sym == SDLK_F4) {
                        SDLMod modifiers;
                        modifiers = SDL_GetModState();
                        if ((modifiers&KMOD_ALT) != 0) {
                            quit = true;
                        }
                    }
#endif
                    // default quit: F12
                    if (event.key.keysym.sym == SDLK_F12) {
                        quit = true;
                    } /* if */

#ifdef __APPLE__
                    if (event.key.keysym.sym == SDLK_f) {
                        SDLMod modifiers;
                        modifiers = SDL_GetModState();
                        if ((modifiers&KMOD_META) != 0) {
                            fullscreen = (fullscreen ? false : true);
                            screen_sfc = toogle_video_mode(fullscreen);
                            game->save_configuration();
                        } // if
                    } // if
#else
                    if (event.key.keysym.sym == SDLK_RETURN)
                    {
                        SDLMod modifiers;
                        modifiers = SDL_GetModState();
                        if ((modifiers&KMOD_ALT) != 0) {
                            fullscreen = (fullscreen ? false : true);
                            screen_sfc = toogle_video_mode(fullscreen);
                            game->save_configuration();
                        } // if
                    } // if
#endif
                    if (event.key.keysym.sym == SDLK_f)
                    {
                        SDLMod modifiers;

                        modifiers = SDL_GetModState();

                        if ((modifiers&KMOD_ALT) != 0) {
                            /* Toogle FPS mode: */
                            if (show_fps)
                                show_fps = false;
                            else
                                show_fps = true;
                        } /* if */
                    } /* if */

                    /* Keyboard event */
                    SDL_keysym *ks;
                    ks = new SDL_keysym();
                    *ks = event.key.keysym;
                    k->keyevents.Add(ks);

                    break;

				case SDL_MOUSEBUTTONDOWN:					
					game->MouseClick(event.button.x,event.button.y);
					break;


                    /* SDL_QUIT event (window close) */
                case SDL_QUIT:
                    quit = true;
                    break;
            } /* switch */
        } /* while */

        act_time = SDL_GetTicks();
        if (act_time - time >= REDRAWING_PERIOD) {
            int max_frame_step = 10;
            do {
                time += REDRAWING_PERIOD;
                if ((act_time - time) > 10*REDRAWING_PERIOD)
                    time = act_time;

                /* cycle */
                k->cycle();
                current_cycle++;
                if (!game->cycle(k))
                    quit = true;
                need_to_redraw = true;

                k->keyevents.Delete();

                act_time = SDL_GetTicks();
                max_frame_step--;
            } while (act_time - time >= REDRAWING_PERIOD && max_frame_step > 0);

        } /* if */

        /* Redraw */
        if (need_to_redraw) {
            GLTile::recheck_textures();
            game->draw();
            need_to_redraw = false;
            frames_per_sec_tmp += 1;
        } /* if */

        if ((act_time - init_time) >= 1000) {
            frames_per_sec = frames_per_sec_tmp;
            frames_per_sec_tmp = 0;
            init_time = act_time;
        } /* if */

        SDL_Delay(1);

    } /* while */


    delete k;
    k = 0;

    delete game;
    game = 0;

    finalization();

#ifdef __DEBUG_MESSAGES

    output_debug_message("Application finished\n");
    close_debug_messages();
#endif

    return 0;
} /* main */
