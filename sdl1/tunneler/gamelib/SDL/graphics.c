#include <stdio.h>
#include <SDL.h>

#include <gamelib.h>
#include <types.h>

#include "sdldata.h"
#include "require_sdl.h"

/* Will select the best fullscreen resolution based on pixel count: */
static SDL_Rect screen_get_best_resolution() {
	SDL_Rect** modes;
	unsigned i;
	SDL_Rect out = {0,0,0,0};
	unsigned out_score = 0;
	
	modes = SDL_ListModes(NULL, SDL_OPTIONS_FS);
	if(!modes) return out;
	
	/* Are all resolutions available? */
	if (modes == (SDL_Rect**)-1) {
		/*out.w = SCREEN_WIDTH; out.h = SCREEN_HEIGHT;*/
		return out;
	}
	
	for (i=0; modes[i]; i++) {
		if(modes[i]->w * modes[i]->h > out_score) {
			out = *modes[i];
			out_score = out.w * out.h;
		}
	}
	
	return out;
}

/* Sets the display to fullscreen, calculating the best resolution: */
int gamelib_set_fullscreen() {
	SDL_Surface *newsurface;
	SDL_Rect     r = screen_get_best_resolution();
	
	/* Already fullscreen? */
	if( _DATA.is_fullscreen ) return 0;
	
	/* Actually set the new video mode: */
	if( !(newsurface = SDL_SetVideoMode(r.w, r.h, 0, SDL_OPTIONS_FS)) ) {
		gamelib_error("Failed to set video mode: %s\n", SDL_GetError());
		return 1;
	}
	
	/* Disable the mouse cursor in fullscreen mode: */
	SDL_ShowCursor( SDL_DISABLE );
	
	/* Update all of the internal variables: */
	_DATA.s = newsurface;
	_DATA.is_fullscreen = !!(_DATA.s->flags & SDL_FULLSCREEN);
	
	return 0;
}

/* Sets the display to windowed mode, with given dimensions: */
int gamelib_set_window(unsigned w, unsigned h) {
	SDL_Surface *newsurface;
	
	/* Actually set the new video mode: */
	if( !(newsurface = SDL_SetVideoMode(w, h, 0, SDL_OPTIONS)) ) {
		gamelib_error("Failed to set video mode: %s\n", SDL_GetError());
		return 1;
	}
	
	/* Enable the mouse cursor in windowed mode: */
	SDL_ShowCursor( SDL_ENABLE );
	
	/* Update all of the internal variables: */
	_DATA.s = newsurface;
	_DATA.is_fullscreen = !!(_DATA.s->flags & SDL_FULLSCREEN);
	
	return 0;
}

/* Returns the screen dimensions in a Rect struct: */
Rect gamelib_get_resolution() {
	Rect r = {0,0,0,0};
	if(!_DATA.s) return r;
	r.w = _DATA.s->w; r.h = _DATA.s->h;
	
	return r;
}

/* Returns whether the current graphics mode is fullscreen: */
int gamelib_get_fullscreen() {
	return _DATA.is_fullscreen;
}

/* Will set a given rectangle to a given color. (NULL rect is fullscreen): */
int gamelib_draw_box(Rect *rect, Color color) {
	Uint32 c = SDL_MapRGB(_DATA.s->format, color.r, color.g, color.b);
	
	if(rect) {
		SDL_Rect r = {rect->x, rect->y, rect->w, rect->h};
		SDL_FillRect(_DATA.s, &r, c);
		return 0;
	}
	
	SDL_FillRect(_DATA.s, NULL, c);
	return 0;
}

/* The display is double-buffered, so double buffer it: *
int gamelib_flip() {
	SDL_Flip(_DATA.s);
	return 0;
}
*/

