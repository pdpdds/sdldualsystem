
/**
 *
 * Tennix! SDL Port
 * Copyright (C) 2003, 2007, 2008, 2009 Thomas Perl <thp@thpinfo.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA  02110-1301, USA.
 *
 **/

#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include "archive.hh"
#include "tennix.h"

#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_rotozoom.h"

#define RECT_UPDATE_CACHE 150

#define FADE_DURATION 500
#define BUTTON_HIGHLIGHT 0.4
#define BUTTON_BORDER 2

#define GET_PIXEL_DATA(surface,x,y) (*((Uint32*)((char*)surface->pixels + x * surface->format->BytesPerPixel + y * surface->pitch)))

#define GET_PIXEL_RGB(surface,x,y,r,g,b) (SDL_GetRGB( GET_PIXEL_DATA(surface,x,y), surface->format, r, g, b))
#define SET_PIXEL_RGB(surface,x,y,r,g,b) (GET_PIXEL_DATA(surface,x,y)=SDL_MapRGB(surface->format, r, g, b))

struct _FreeListItem {
    const char* data;
    struct _FreeListItem* next;
};

typedef struct _FreeListItem FreeListItem;

typedef struct {
    FreeListItem* head;
} FreeList;

FreeList* freelist_create();
void freelist_append(FreeList* list, const char* data);
void freelist_free_all(FreeList* list);

typedef struct {
    TTF_Font* data;
} Font;

typedef struct {
    const char *filename;
    int size;
    int style;
} FontDescription;

typedef unsigned int font_id;
enum {
    FONT_SMALL = 0,
    FONT_MEDIUM,
    FONT_LARGE,
    FONT_XLARGE,
    FONT_COUNT
};

typedef struct {
    SDL_Surface* data;
} Image;

typedef unsigned int image_id;
enum {
    GR_COURT = 0,
    GR_SHADOW,
    GR_RACKET,
    GR_BALL,
    GR_REFEREE,
    GR_CTT_HARD,
    GR_CTT_CLAY,
    GR_CTT_GRASS,
    GR_SIDEBAR,
    GR_TENNIXLOGO,
    GR_BTN_PLAY,
    GR_BTN_RESUME,
    GR_BTN_QUIT,
    GR_STADIUM,
    GR_FOG,
    GR_FOG2,
    GR_NIGHT,
    GR_TALK,
    GR_CURSOR,
    GR_WORLDMAP,
    GR_INPUT_GAMEPAD,
    GR_INPUT_KEYBOARD_ARROWS,
    GR_INPUT_KEYBOARD_OL,
    GR_INPUT_KEYBOARD_WS,
    GR_INPUT_MAEMO_DPAD,
    GR_INPUT_MOUSE,
    GR_INPUT_TOUCHSCREEN,
    GR_INPUT_AI,
    GR_BACK,
    GR_FORWARD,
    GR_LOC_MARGARET_COURT_ARENA,
    GR_LOC_STADE_ROLAND_GARROS,
    GR_LOC_COURT_NO_1,
    GR_LOC_ARTHUR_ASHE_STADIUM,
#ifdef NONFREE_LOCATIONS
    GR_LOC_TRAINING_CAMP,
    GR_LOC_AUSTRIAN_OPEN,
    GR_LOC_OLYMPIC_GREEN_TENNIS,
#endif
    GR_COUNT
};

#define GR_CTT_FIRST GR_CTT_HARD
#define GR_CTT_LAST GR_CTT_GRASS

void init_graphics(TennixArchive& tnxar);
void uninit_graphics();
int get_image_width(image_id);
int get_image_height(image_id);
int get_sprite_width(image_id, int);
void show_sprite(image_id, int, int, int, int, int);
#define show_image(id, x_offset, y_offset, opacity) show_sprite(id, 0, 1, x_offset, y_offset, opacity)
void show_image_rotozoom(image_id id, int x, int y, float rotate, float zoom);
void line_horiz( int y, Uint8 r, Uint8 g, Uint8 b);
void line_vert( int x, Uint8 r, Uint8 g, Uint8 b);
void rectangle( int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b);
void rectangle_alpha(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 opacity);
void fill_image_offset(image_id id, int x, int y, int w, int h, int offset_x, int offset_y);
#define fill_image(id, x, y, w, h) fill_image_offset(id, x, y, w, h, 0, 0)
void draw_button( int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, char pressed);
void draw_button_text(const char* s, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, char pressed);
void draw_button_object(MenuButton*, int, int);

void clear_screen();
void store_screen();
void reset_screen();

void clearscr();
void update_rect(Sint32 x, Sint32 y, Sint32 w, Sint32 h);
#define update_rect2(r) (update_rect(r.x, r.y, r.w, r.h))
void updatescr();
void start_fade();

SDL_Surface* font_render_surface(font_id id, const char* text, Uint8 r,
        Uint8 g, Uint8 b);

#define font_render_surface_simple(id, text) \
    (font_render_surface(id, text, 255, 255, 255))

#define rotozoom_surface(surface, rotate, zoom) \
    (rotozoomSurface(surface, rotate, zoom, SMOOTHING_OFF))

void blit_surface(SDL_Surface* surface, int x, int y, int pos, int count);

#define blit_surface_simple(surface, x, y) (blit_surface(surface, x, y, 0, 1))

SDL_Surface* get_surface(image_id id);

void font_draw_string_color(font_id id, const char* s, int x_offset, int y_offset, Uint8 r, Uint8 g, Uint8 b);
#define font_draw_string(id,s,x_offset,y_offset) font_draw_string_color(id,s,x_offset,y_offset,255,255,255)
int font_get_string_width(font_id id, const char* s);
int font_get_height(font_id id);

void draw_line_faded( int x1, int y1, int x2, int y2, int r, int g, int b, int r2, int g2, int b2);
#define draw_line(x1,y1,x2,y2,r,g,b) draw_line_faded(x1,y1,x2,y2,r,g,b,r,g,b)

extern Uint32 fading_start;

#endif

