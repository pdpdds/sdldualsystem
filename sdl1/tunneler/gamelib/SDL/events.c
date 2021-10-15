#include <SDL.h>

#include <gamelib.h>
#include <types.h>

#include "require_sdl.h"


#define FULLSCREEN_KEY   SDLK_F10
#define EXIT_KEY         SDLK_ESCAPE

/* TODO: Move this into the _DATA structure? Maybe... */
typedef struct Event {
	SDL_Event e;
	EventType type;
	Rect      dim;
} Event;


static Event cur_event = {
	 NULL, GAME_EVENT_NONE, {0,0,0,0} };


/* Will check the SDL event stack for a new event, if we don't have one... */
static void check_for_event() {
	SDL_Event e;
	
	/* Don't do jack if the current event hasn't been released: */
	if(cur_event.type != GAME_EVENT_NONE) return;
	
	/* Grab the next event (if it exists) off the stack: */
	while(cur_event.type == GAME_EVENT_NONE) {
		
		/* Grab the next event, and only continue if we got something: */
		if(!SDL_PollEvent(&e)) break;
		
		/* Resize event: */
		if(e.type == SDL_VIDEORESIZE) {
			cur_event.type = GAME_EVENT_RESIZE;
			cur_event.dim = RECT(0,0, e.resize.w, e.resize.h);
		
		/* Keyboard events: */
		} else if(e.type == SDL_KEYDOWN) {
			if(e.key.keysym.sym == FULLSCREEN_KEY)
				cur_event.type = GAME_EVENT_TOGGLE_FULLSCREEN;
			
			else if(e.key.keysym.sym == EXIT_KEY)
				cur_event.type = GAME_EVENT_EXIT;
		
		/* Window close event: */
		} else if(e.type == SDL_QUIT)
			cur_event.type = GAME_EVENT_EXIT;
	}
}


EventType gamelib_event_get_type() {
	check_for_event();
	
	return cur_event.type;
}

Rect gamelib_event_resize_get_size() {
	check_for_event();
	
	if(cur_event.type != GAME_EVENT_RESIZE) return RECT(0,0,0,0);
	return cur_event.dim;
}

void gamelib_event_done() {
	cur_event.type = GAME_EVENT_NONE;
}
