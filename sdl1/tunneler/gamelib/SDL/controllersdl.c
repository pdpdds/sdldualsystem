#include <stdlib.h>
#include <SDL.h>

#include <gamelib.h>
#include <tank.h>
#include <memalloc.h>

#include "require_sdl.h"

/* Any SDL-based controllers go in this file. */

/*----------------------------------------------------------------------------*
 *   KEYBOARD                                                                 *
 *----------------------------------------------------------------------------*/

typedef struct KeyboardPrivateData {
	SDLKey left, right, up, down, shoot;
} KeyboardPrivateData;

static void keyboard_controller(PublicTankInfo *i, void *d, int *vx, int *vy, unsigned *s) {
	KeyboardPrivateData *data = d;
	Uint8 *keys = SDL_GetKeyState( NULL );
	
	*vx = keys[data->right] - keys[data->left];
	*vy = keys[data->down]  - keys[data->up];
	*s  = keys[data->shoot];
}

void controller_keyboard_attach( Tank *t,
	SDLKey left, SDLKey right, SDLKey up, SDLKey down, SDLKey shoot) {
	
	KeyboardPrivateData *data = malloc(sizeof(KeyboardPrivateData));
	*data = (KeyboardPrivateData){left, right, up, down, shoot};
	
	tank_set_controller(t, keyboard_controller, data);
}


/*----------------------------------------------------------------------------*
 *   JOYSTICK                                                                 *
 *----------------------------------------------------------------------------*/

/* TODO: We are currently just letting the joystick be closed indirectly by the
 *       program exit. Fix this be making it possible for a controller to define
 *       a tear-down function. */

/* The SDL-based keyboard controller: */
typedef struct JoystickPrivateData {
	SDL_Joystick *joystick;
} JoystickPrivateData;


/* This is the joystick value (between 1 and 32767) where a joystick axis gets
 * interpretted as going in that direction: */
#define CUTOFF (10000)

static void joystick_controller(PublicTankInfo *i, void *d, int *vx, int *vy, unsigned *s) {
	JoystickPrivateData *data = d;
	Sint32 jx, jy;
	Uint32 dist;
	
	/* Where is this joystick pointing? */
	jx = SDL_JoystickGetAxis(data->joystick, 0);
	jy = SDL_JoystickGetAxis(data->joystick, 1);
	
	/* Don't do jack if the joystick is too close to its orgin: */
	dist = jx*jx + jy*jy;
	if(dist < CUTOFF * CUTOFF) {
		*vx = *vy = 0;
		
	/* Else, find out what direction the joystick is closest to: */
	} else {
		int tx, ty;
		
		tx = (jx==0) ? 0 : ( abs(jy * 1000 / jx) < 2000 );
		ty = (jx==0) ? 1 : ( abs(jy * 1000 / jx) > 500 );
		
		*vx = tx * (jx > 0 ? 1 : -1);
		*vy = ty * (jy > 0 ? 1 : -1);
	}
	*s  = SDL_JoystickGetButton(data->joystick, 0);
}

void controller_joystick_attach( Tank *t ) {
	
	JoystickPrivateData *data;
	
	/* Make sure that this is even a joystick to connect to: */
	if(SDL_NumJoysticks() == 0) {
		/* TODO: exiting isn't all that friendly... we need a better controller API... */
		gamelib_debug("No joysticks connected.\n");
		exit(1);
	}
	
	data = malloc(sizeof(JoystickPrivateData));
	data->joystick = SDL_JoystickOpen(0);
	
	if(data->joystick) {
		gamelib_debug("Using Joystick #0:\n");
		gamelib_debug("  Name:    %s\n", SDL_JoystickName(0));
		gamelib_debug("  Axes:    %d\n", SDL_JoystickNumAxes(data->joystick));
		gamelib_debug("  Buttons: %d\n", SDL_JoystickNumButtons(data->joystick));
		gamelib_debug("  Balls:   %d\n", SDL_JoystickNumBalls(data->joystick));
	
	} else {
		gamelib_debug("Failed to open Joystick #0.\n");
		exit(1);
	}
	
	tank_set_controller(t, joystick_controller, data);
}

