#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <jni.h>
#include <android/bitmap.h>

#include <gamelib.h>
#include <types.h>
#include <tank.h>
#include <tweak.h>
#include <game.h>

#include "androiddata.h"
#include "require_android.h"

/* Game startup, and shutdown: */
int gamelib_init() {
	
	_DATA.gd = game_new();
	
	/* Just accept the default for everything: */
	game_finalize(_DATA.gd);
	
	/* Android has disappointed me, by not allowing you to specify a way to NOT
	 * clobber seemingly unused functions in a static library. As such, we have
	 * to resort to ugly hacks to get what we want: */
	JNI_OnLoad(NULL, NULL);
	JNI_OnUnload(NULL, NULL);
	
	return 0;
}

int gamelib_exit() {
	game_free(_DATA.gd);
	return 0;
}


/* All of the Android limits/capabilities: */
int gamelib_get_max_players()    { return 1; }
int gamelib_get_can_resize()     { return 0; }
int gamelib_get_can_fullscreen() { return 1; }
int gamelib_get_can_window()     { return 0; }
int gamelib_get_target_fps()     { return GAME_FPS; } /* << TODO: Remove this? */

/* The C code doesn't maintain the main loop in the Android port, so we don't
 * need to implement this: */
void gamelib_main_loop(draw_func func, void *data) {}

/* This lets you attach controllers to a tank: */
int gamelib_tank_attach(Tank *t, int tank_num, int num_players) {
	controller_android_attach(t);
	return 0;
}

/* A user can't ask for a resize (so gamelib_get_can_resize() returns 0) but the
 * bitmap can be resized by Android, so we will still issue RESIZE events: */
EventType gamelib_event_get_type() { return _DATA.next_event; }
Rect      gamelib_event_resize_get_size() { return _DATA.prev; }
void      gamelib_event_done() { _DATA.next_event = GAME_EVENT_NONE; }


/* We need to be able to switch resolutions: */
int  gamelib_set_fullscreen() {return 0; }
int  gamelib_set_window(unsigned w, unsigned h) { return 0; }
Rect gamelib_get_resolution() { return _DATA.prev; }
int  gamelib_get_fullscreen() { return 1; }




/* We need a way to draw: */
/* TODO: We are wasting a lot of time on all the range checking. We need to
 *       change the API so that the resource is reserved until we're done. That
 *       way, range-checking happens on a per-frame basis, and not a per-pixel
 *       basis. This will speed up SDL too... */
int  gamelib_draw_box(Rect *rect, Color c) { 
	Rect size;
	uint16_t color_data;
	uint16_t *row, *upper, *lower;
	register int x, step;
	
	/* Get the bounds of what we're drawing: */
	if(rect) size = *rect;
	else {
		size.x = size.y = 0;
		size.w = _DATA.bmpW; size.h = _DATA.bmpH;
	}
	
	/* Pack the color info into the color_data array for RGB565: */
	uint16_t tr = c.r, tg = c.g, tb = c.b;
	color_data = (uint16_t)( ((tr << 8) & 0xf800) | 
	                         ((tg << 3) & 0x07e0) |
	                         ((tb >> 3) & 0x001f) );
	
	/* Scan all pixels, and set the ones we need: */
	
	
	upper = _DATA.pixels + _DATA.bmpStride * size.y;
	lower = _DATA.pixels + _DATA.bmpStride * (size.y+size.h);
	step  = _DATA.bmpStride / sizeof(uint16_t);
	
	for(row=upper; row < lower; row += step) {
		for(x=size.x; x<size.x+size.w; x++) {
			
			/* TODO: Would there be a way of doing this more like SDL, so that
			 * we don't force the bitmap into RGB565 format, but instead let the
			 * platform select its favorite pixel format? Think on this... */
			row[ x ] = color_data;
		}
	}
	
	return 0;
}


/* Mobile devices have limited storage. Don't waste it with huge bmps: */
BMPFile *gamelib_bmp_new      (unsigned width, unsigned height) {return NULL;}
void     gamelib_bmp_set_pixel(BMPFile *f, unsigned x, unsigned y, Color c) {}
void     gamelib_bmp_finalize (BMPFile *f, char *filename) {}

