#include <SDL.h>

#include <gamelib.h>
#include <types.h>
#include <memalloc.h>

#include "sdldata.h"
#include "require_sdl.h"


struct BMPFile {
	SDL_Surface *s;
} ;


BMPFile *gamelib_bmp_new(unsigned width, unsigned height) {
	BMPFile *out;
	
	out = malloc(sizeof(BMPFile));
	out->s = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 24, 0, 0, 0, 0);
	
	return out;
}

void gamelib_bmp_set_pixel(BMPFile *f, unsigned x, unsigned y, Color c) {
	Uint8 *p;
	Uint32 mapped_color;
	
	/* Raising this semaphore so often is probably gonna hurt performance, but
	 * it's not like we're generating a bmp every second...  :) */
	if(SDL_MUSTLOCK(f->s)) SDL_LockSurface(f->s);
	
	/* Get the address, and the mapped color: */
	p = &((Uint8*)f->s->pixels)[y*f->s->pitch + x*f->s->format->BytesPerPixel];
	mapped_color = SDL_MapRGB( f->s->format, c.r, c.g, c.b );
	
	/* Copy that value into the bitmap: */
	memcpy( p, &mapped_color, f->s->format->BytesPerPixel );
	
	/* ... and unlock, and we're good! :) */
	if(SDL_MUSTLOCK(f->s)) SDL_UnlockSurface(f->s);	
}

void gamelib_bmp_finalize(BMPFile *f, char *filename) {
	/* It's just save... */
	SDL_SaveBMP(f->s, filename);
	
	/* ... and free: */
	SDL_FreeSurface(f->s);
	free(f);
}

