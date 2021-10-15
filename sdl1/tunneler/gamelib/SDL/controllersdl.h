#ifndef _CONTROLLER_SDL_H_
#define _CONTROLLER_SDL_H_

#include <SDL.h>
#include <tank.h>

void controller_keyboard_attach( Tank *, SDLKey, SDLKey, SDLKey, SDLKey, SDLKey);
void controller_joystick_attach( Tank *t ) ;

#endif

