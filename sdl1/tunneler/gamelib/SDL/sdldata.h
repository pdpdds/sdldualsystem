#ifndef _SDL_DATA_H_
#define _SDL_DATA_H_

#include <SDL.h>

#define SDL_OPTIONS    (SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE)
#define SDL_OPTIONS_FS (SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN)

typedef struct SDLData {
	SDL_Surface *s;
	int is_fullscreen;
} SDLData;

extern SDLData _DATA;

#endif 
