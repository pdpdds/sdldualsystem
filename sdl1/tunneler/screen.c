#include <stdlib.h>

#include <screen.h>
#include <tweak.h>
#include <memalloc.h>
#include <random.h>
#include <level.h>
#include <tank.h>
#include <types.h>
#include <tanksprites.h>
#include <drawbuffer.h>
#include <gamelib.h>


typedef enum ScreenDrawMode {
	SCREEN_DRAW_INVALID,
	SCREEN_DRAW_LEVEL
} ScreenDrawMode;

typedef struct Window {
	Rect     r;
	Tank    *t;
	unsigned counter;
	unsigned showing_static;
} Window;

typedef struct StatusBar {
	Rect     r;
	Tank    *t;
	int      decreases_to_left;
} StatusBar;

typedef struct Bitmap {
	Rect     r;
	char    *data;
	Color   *color;
} Bitmap;

typedef struct GUIController {
	Rect r;
} GUIController;


struct Screen {
	
	unsigned is_fullscreen;
	
	/* Various variables for the current resolution: */
	unsigned width, height, xstart, ystart, pixelw, pixelh, xskips, yskips;
	
	/* Window shit: */
	unsigned  window_count;
	Window    window[SCREEN_MAX_WINDOWS];

	/* Status bar shit: */
	unsigned  status_count;
	StatusBar status[SCREEN_MAX_STATUS];

	/* Bitmap shit: */
	unsigned  bitmap_count;
	Bitmap    bitmap[SCREEN_MAX_BITMAPS];
	
	/* GUI Controller shit: */
	unsigned controller_count;
	GUIController controller;
	/* Variables used for drawing: */
	ScreenDrawMode mode;
	union {
		struct {
			DrawBuffer *b;
		} level;
	} drawing;
};


/* Fills a surface with a blue/black pattern: */
static void fill_background() {
	register unsigned x, y;
	Rect dim;
	
	dim = gamelib_get_resolution();
	
	gamelib_draw_box(NULL, color_bg);
	for(y=0; y<dim.h; y++) {
		for(x=(y%2)*2; x<dim.w; x+=4) {
			Rect r = {x, y, 1, 1};
			gamelib_draw_box(&r, color_bg_dot);
		}
	}
}

void screen_draw_pixel(Screen *s, unsigned x, unsigned y, Color color) {
	unsigned w, h, xs, ys;
	Rect r;
	
	xs = (x * s->xskips)/GAME_WIDTH;
	ys = (y * s->yskips)/GAME_HEIGHT;
	
	x = x * s->pixelw + s->xstart + xs;
	y = y * s->pixelh + s->ystart + ys;
	
	w = s->pixelw + (xs!=((x+1)*s->xskips/GAME_WIDTH));
	h = s->pixelh + (ys!=((y+1)*s->yskips/GAME_HEIGHT));
	
	r = RECT(x,y,w,h);
	gamelib_draw_box(&r, color);
}

/* These will say what virtual pixel a physical pixel resides on: */
int  screen_map_x(Screen *s, int x) {
	x -= s->xstart;
	x -= x/(int)s->pixelw * (int)s->xskips/GAME_WIDTH;
	x /= (int)s->pixelw;
	
	return x;
}

int  screen_map_y(Screen *s, int y) {
	y -= s->ystart;
	y -= y/(int)s->pixelh * (int)s->yskips/GAME_HEIGHT;
	y /= (int)s->pixelh;
	
	return y;
}


/* Will randomly draw static to a window, based on a tank's health. Returns 1 if
 * static was drawn: */
static void screen_draw_static(Screen *s, Window *w) {
	register unsigned x, y;
	unsigned health, energy;
	unsigned black_counter, drawing_black;
	
	tank_get_stats(w->t, &energy, &health);

	/* Don't do static if we have a lot of energy: */
	if(energy > STATIC_THRESHOLD) {
		w->counter = w->showing_static = 0;
		return;
	}

	if(!w->counter) {
		unsigned intensity = 1000 * energy / STATIC_THRESHOLD;
		w->showing_static = !rand_bool(intensity);
		w->counter = rand_int(GAME_FPS/16, GAME_FPS/8) * w->showing_static ? 1 : 4;

	} else w->counter--;
	
	if(!w->showing_static) return;

#define _BLACK_BAR_RAND rand_int(1, w->r.w*w->r.h * STATIC_BLACK_BAR_SIZE/1000)
#define _RAND_COLOR     color_primary[rand_int(0,7)]

	/* Should we draw a black bar in the image? */
	black_counter = rand_bool(STATIC_BLACK_BAR_ODDS) ? _BLACK_BAR_RAND : 0;
	drawing_black = black_counter && rand_bool(STATIC_BLACK_BAR_ODDS);
	
	/* Develop a static thing image for the window: */
	for(y=0; y<w->r.h; y++)
		for(x=0; x<w->r.w; x++) {
			Color color;

			if(!energy) {
				screen_draw_pixel(s, x + w->r.x, y + w->r.y, _RAND_COLOR);
				continue;
			}

			/* Handle all of the black bar logic: */
			if(black_counter) {
				black_counter--;
				if(!black_counter) {
					black_counter = _BLACK_BAR_RAND;
					drawing_black = !drawing_black;
				}
			}

			/* Make this semi-transparent: */
			if(rand_bool(STATIC_TRANSPARENCY)) continue;

			/* Finally, select a color (either black or random) and draw: */
			color = drawing_black ? color_blank : _RAND_COLOR;
			screen_draw_pixel(s, x + w->r.x, y + w->r.y, color);
		}

	return;
}

#undef _RAND_COLOR
#undef _BLACK_BAR_RAND

/* Will draw a window using the level's drawbuffer: */
static void screen_draw_window(Screen *s, Window *w) {
	DrawBuffer *b = s->drawing.level.b;
	register unsigned x, y;
	unsigned tx, ty;

	tank_get_position(w->t, &tx, &ty);
	
	for(y=0; y < w->r.h; y++) {
		for(x=0; x < w->r.w; x++) {
			unsigned screenx = x + w->r.x, screeny = y + w->r.y;
			Color c = drawbuffer_get_pixel(b, x + tx - w->r.w/2, y + ty - w->r.h/2);
			screen_draw_pixel(s, screenx, screeny, c);
		}
	}
	
	screen_draw_static(s, w);
}

/* Will draw two bars indicating the charge/health of a tank: */
/* TODO: This currently draws every frame. Can we make a dirty flag, and only
 *       redraw when it's needed? Also, can we put some of these calculations in
 *       the StatusBar structure, so they don't have to be done every frame? */
static void screen_draw_status(Screen *s, StatusBar *b) {
	register unsigned x, y;
	
	/* At what y value does the median divider start: */
	unsigned mid_y = (b->r.h - 1) / 2;
	
	/* How many pixels high is the median divider: */
	unsigned mid_h = (b->r.h % 2) ? 1 : 2;
	
	/* How many pixels are filled in? */
	unsigned energy_filled, health_filled, half_energy_pixel;
	half_energy_pixel = TANK_STARTING_FUEL/((b->r.w - STATUS_BORDER*2)*2);
	
	tank_get_stats(b->t, &energy_filled, &health_filled);
	energy_filled += half_energy_pixel;
	
	energy_filled *= (b->r.w - STATUS_BORDER*2);
	energy_filled /= TANK_STARTING_FUEL;
	health_filled *= (b->r.w - STATUS_BORDER*2);
	health_filled /= TANK_STARTING_SHIELD;

	/* If we are decreasing to the right, we need to invert those values: */
	if(!b->decreases_to_left) {
		energy_filled = b->r.w - STATUS_BORDER - energy_filled;
		health_filled = b->r.w - STATUS_BORDER - health_filled;
		
	/* Else, we still need to shift it to the right by STATUS_BORDER: */
	} else {
		energy_filled += STATUS_BORDER;
		health_filled += STATUS_BORDER;
	}
	
	/* Ok, lets draw this thing: */
	for(y=0; y < b->r.h; y++) {
		for(x=0; x < b->r.w; x++) {
			Color c;

			/* We round the corners of the status box: */
			if((x == 0 || x == b->r.w - 1) && (y == 0 || y == b->r.h - 1))
				continue;
			
			/* Outer border draws background: */
			else if(y < STATUS_BORDER || y >= b->r.h-STATUS_BORDER ||
			   x < STATUS_BORDER || x >= b->r.w-STATUS_BORDER)
				c = color_status_bg;

			/* We round the corners here a little bit too: */
			else if((x == STATUS_BORDER || x == b->r.w - STATUS_BORDER - 1) &&
			        (y == STATUS_BORDER || y == b->r.h - STATUS_BORDER - 1))
				c = color_status_bg;

			/* Middle seperator draws as backround, as well: */
			else if(y >= mid_y && y < mid_y + mid_h)
				c = color_status_bg;

			/* Ok, this must be one of the bars. */
			/* Is this the filled part of the energy bar? */
			else if(y < mid_y && 
				(( b->decreases_to_left && x< energy_filled) ||
				 (!b->decreases_to_left && x>=energy_filled)))
				c = color_status_energy;

			/* Is this the filled part of the health bar? */
			else if(y > mid_y && 
				(( b->decreases_to_left && x< health_filled) ||
				 (!b->decreases_to_left && x>=health_filled)))
				c = color_status_health;

			/* Else, this must be the empty part of a bar: */
			else
				c = color_blank;

			screen_draw_pixel(s, x + b->r.x, y + b->r.y, c);
		}
	}
}

static void screen_draw_bitmap(Screen *s, Bitmap *b) {
	register unsigned x, y, i;

	for(x=y=i=0; i < (b->r.w * b->r.h); i++) {
		if(b->data[i]) screen_draw_pixel(s, x + b->r.x, y + b->r.y, *b->color);
		if(++x >= b->r.w) { y++; x=0; }
	}
}

static void screen_draw_level(Screen *s) {
	register unsigned i;
	
	for(i=0; i<s->window_count; i++) screen_draw_window(s, &s->window[i]);
	for(i=0; i<s->status_count; i++) screen_draw_status(s, &s->status[i]);
	for(i=0; i<s->bitmap_count; i++) screen_draw_bitmap(s, &s->bitmap[i]);
	if(s->controller_count)
		gamelib_gui_draw(s, s->controller.r);
}

void screen_draw(Screen *s) {	
	if(s->mode == SCREEN_DRAW_LEVEL) {
		screen_draw_level(s);
	}
}


/* The constructor sets the video mode: */
Screen *screen_new(int is_fullscreen) {
	Screen *out = malloc(sizeof(Screen));
	
	out->is_fullscreen = is_fullscreen;
	out->mode = SCREEN_DRAW_INVALID;
	out->window_count = out->status_count = out->bitmap_count = 0;
	out->controller_count = 0;
	
	/* Set the window size to the default one: */
	if( screen_resize(out, SCREEN_WIDTH, SCREEN_HEIGHT) ) {
		free(out);
		return NULL;
	}
	
	return out;
}

void screen_destroy(Screen *s) {
	if(!s) return;
	free(s);
}

/* TODO: Change the screen API to better match gamelib... */

void screen_set_fullscreen(Screen *s, int is_fullscreen) {
	
	if(s->is_fullscreen == is_fullscreen) return;
	
	/* -1 will toggle: */
	if(is_fullscreen < 0) is_fullscreen = !s->is_fullscreen;
	
	s->is_fullscreen = is_fullscreen;
	
	/* Resize the screen to include the new fullscreen mode: */
	if(!is_fullscreen) screen_resize(s, SCREEN_WIDTH, SCREEN_HEIGHT);
	else               screen_resize(s, s->width, s->height);
}


/* Returns 0 if successful, 1 if failed: */
int screen_resize(Screen *s, unsigned width, unsigned height) {
	
	unsigned pixelw, pixelh, xskips, yskips, xstart, ystart, vw, vh, a, b;
	Rect temp_rect;
	
	/* Make sure that we aren't scaling to something too small: */
	if(width < GAME_WIDTH)   width = GAME_WIDTH;
	if(height < GAME_HEIGHT) height = GAME_HEIGHT;
	
	/* A little extra logic for fullscreen: */
	if(s->is_fullscreen) gamelib_set_fullscreen();
	else                 gamelib_set_window    (width, height);
	
	temp_rect = gamelib_get_resolution();
	width = temp_rect.w; height = temp_rect.h;
	
	s->is_fullscreen = gamelib_get_fullscreen();
	
	/* What is the limiting factor in our scaling? */
	a = height * GAME_WIDTH; b = width * GAME_HEIGHT;
	if(a<b) {
		/* Height is. */
		vh = height; vw = (GAME_WIDTH * height) / (GAME_HEIGHT);
		xstart = width/2 - vw/2; ystart = 0;
	} else {
		/* Width is. */
		vw = width; vh = (GAME_HEIGHT * width) / (GAME_WIDTH);
		xstart = 0; ystart = height/2 - vh/2;
	}
	
	/* Calculate the pixel sizing variables: */
	pixelw = vw / GAME_WIDTH;  xskips = vw % GAME_WIDTH;
	pixelh = vh / GAME_HEIGHT; yskips = vh % GAME_HEIGHT;
	
	/* Draw a nice bg: */
	fill_background();
	
	/* Ok, the hard part is over. Copy in all of our data: */
	s->width = width;   s->height = height;
	s->xstart = xstart; s->ystart = ystart;
	s->pixelw = pixelw; s->pixelh = pixelh;
	s->xskips = xskips; s->yskips = yskips;
	
	/* Redraw the game: */
	screen_draw(s);
	
	return 0;
}

/* Set the current drawing mode: */
void screen_set_mode_level(Screen *s, DrawBuffer *b) {
	s->mode = SCREEN_DRAW_LEVEL;
	s->drawing.level.b = b;
}

/*
void screen_set_mode_menu(Screen *s, Menu *m) ;
void screen_set_mode_map(Screen *s, Map *m) ;
*/

/* Window creation should only happen in Level-drawing mode: */
void screen_add_window(Screen *s, Rect r, Tank *t) {
	if(s->mode != SCREEN_DRAW_LEVEL) return;
	
	if(s->window_count >= SCREEN_MAX_WINDOWS) return;
	s->window[ s->window_count++ ] = (Window) {r, t, 0, 0};
}

/* We can add the health/energy status bars here: */
void screen_add_status(Screen *s, Rect r, Tank *t, int decreases_to_left) {
	/* Verify that we're in the right mode, and that we have room: */
	if(s->mode != SCREEN_DRAW_LEVEL) return;
	if(s->status_count >= SCREEN_MAX_STATUS) return;

	/* Make sure that this status bar isn't too small: */
	if(r.w <= 2 || r.h <= 4) return;
	
	s->status[ s->status_count++ ] = (StatusBar) {r, t, decreases_to_left};
}

/* We tell the graphics system about GUI graphics here: 
 * 'color' has to be an ADDRESS of a color, so it can monitor changes to the
 * value, especially if the bit depth is changed... 
 * TODO: That really isn't needed anymore, since we haven't cached mapped RGB
 *       values since the switch to gamelib... */
void screen_add_bitmap(Screen *s, Rect r, char *bitmap, Color *color) {
	/* Bitmaps are only for game mode: */
	if(s->mode != SCREEN_DRAW_LEVEL) return;
	if(s->bitmap_count >= SCREEN_MAX_BITMAPS) return;
	if(!bitmap || !color) return;
	
	s->bitmap[ s->bitmap_count++ ] = (Bitmap) {r, bitmap, color};
}

/* We don't check to see if gamelib needs the gui controller thing in this file.
 * That is handled in game.c: */
void screen_add_controller(Screen *s, Rect r) {
	if(s->mode != SCREEN_DRAW_LEVEL) return;
	if(s->controller_count) return;
	
	s->controller_count = 1;
	s->controller.r = r;
}

