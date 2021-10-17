
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <unistd.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "tennix.h"
#include "graphics.h"
#include "archive.hh"
#include "sound.h"

#include "SDL_rotozoom.h"

#define MAX(a, b)  (((a) > (b)) ? (a) : (b)) 
#define MIN(a, b)  (((a) < (b)) ? (a) : (b)) 

static Image* images;

static Font* fonts;
FreeList* font_freelist = NULL;

static FontDescription font_desc[] = {
    /**
     * This is the list of fonts to be loaded. Be sure to
     * use NULL as filename when using the same .ttf file
     * the second time in a row - this saves memory in the
     * loader function.
     **/
    { "dustismo.ttf", 15, TTF_STYLE_NORMAL },
    { NULL, 20, TTF_STYLE_NORMAL },
    { "itwasntme.ttf", 22, TTF_STYLE_NORMAL },
    { NULL, 28, TTF_STYLE_NORMAL }
};

static SDL_Surface *buffer;
static SDL_Surface *background;

static SDL_Rect *rect_update_cache;
static SDL_Rect *rect_update_old;
static int rect_update_cache_current;
static int rect_update_old_count;

Uint32 fading_start = 0;

static const char* graphics[] = {
    "court.png",
    "shadow.png",
    "player-racket.png",
    "ball.png",
    "referee.png",
    "ctt_hard.png",
    "ctt_clay.png",
    "ctt_grass.png",
    "sidebar.png",
    "tennixlogo.png",
    "btnplay.png",
    "btnresume.png",
    "btnquit.png",
    "stadium.png",
    "fog.png",
    "fog2.png",
    "night.png",
    "talk.png",
    "cursor.png",
    "worldmap.png",
    "input_gamepad.png",
    "input_keyboard_arrows.png",
    "input_keyboard_ol.png",
    "input_keyboard_ws.png",
    "input_maemo_dpad.png",
    "input_mouse.png",
    "input_touchscreen.png",
    "input_ai.png",
    "back.png",
    "forward.png",
    "loc_margaret_court_arena.png",
    "loc_stade_roland_garros.png",
    "loc_court_no_1.png",
    "loc_arthur_ashe_stadium.png",
#ifdef NONFREE_LOCATIONS
    "loc_training_camp.png",
    "loc_austrian_open.png",
    "loc_olympic_green_tennis.png"
#endif
};

void init_graphics(TennixArchive& tnxar) {
    int i;
    char *d;
    SDL_Surface* data;
    SDL_Surface* tmp;
    struct SDL_RWops* rw;
    const char* font_data = NULL;

    if (TTF_Init() == -1) {
        fprintf(stderr, "Cannot init TTF: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    rect_update_cache = (SDL_Rect*)calloc(RECT_UPDATE_CACHE, sizeof(SDL_Rect));
    rect_update_old = (SDL_Rect*)calloc(RECT_UPDATE_CACHE, sizeof(SDL_Rect));
    rect_update_cache_current = 0;
    rect_update_old_count = 0;

    images = (Image*)calloc( GR_COUNT, sizeof( Image));
    fonts = (Font*)calloc(FONT_COUNT, sizeof(Font));

    buffer = SDL_CreateRGBSurface(SDL_SWSURFACE, WIDTH, HEIGHT,
                 screen->format->BitsPerPixel,
                 screen->format->Rmask,
                 screen->format->Gmask,
                 screen->format->Bmask,
                 screen->format->Amask);

    background = SDL_CreateRGBSurface(SDL_SWSURFACE, WIDTH, HEIGHT,
                     screen->format->BitsPerPixel,
                     screen->format->Rmask,
                     screen->format->Gmask,
                     screen->format->Bmask,
                     screen->format->Amask);

    if( buffer == NULL) {
        fprintf( stderr, "Cannot create buffer surface: %s\n", SDL_GetError());
    }

    font_freelist = freelist_create();

    /* Load fonts from resource file */
    for (i=0; i<FONT_COUNT; i++) {
        if (font_desc[i].filename != NULL) {
            assert(tnxar.setItemFilename(font_desc[i].filename) != 0);
            font_data = tnxar.getItemBytes();
            freelist_append(font_freelist, font_data);
        }
        assert(font_data != NULL);
        rw = SDL_RWFromMem((void*)font_data, tnxar.getItemSize());
        fonts[i].data = TTF_OpenFontRW(rw, 1, font_desc[i].size);
        if (fonts[i].data == NULL) {
            fprintf(stderr, "TTF error: %s\n", TTF_GetError());
            assert(fonts[i].data != NULL);
        }
        TTF_SetFontStyle(fonts[i].data, font_desc[i].style);
    }

    draw_button(40, (HEIGHT-40)/2, WIDTH-80, 40, 100, 100, 100, 1);
    store_screen();
    updatescr();

    for( i=0; i<GR_COUNT; i++) {
        if (tnxar.setItemFilename(graphics[i]) != 0 ) {
            d = tnxar.getItemBytes();
            tmp = IMG_Load_RW(SDL_RWFromMem(d, tnxar.getItemSize()), 1);
            free(d);
        } else {
            fprintf(stderr, "Cannot find file: %s\n", graphics[i]);
            exit(EXIT_FAILURE);
        }
        if( !tmp) {
            fprintf( stderr, "Error: %s\n", SDL_GetError());
            continue;
        }

        /* Convert to RGBA in the display format */
        data = SDL_DisplayFormatAlpha(tmp);
        SDL_FreeSurface(tmp);

        if( !data) {
            fprintf( stderr, "Error: %s\n", SDL_GetError());
            continue;
        }
        images[i].data = data;

        draw_button(40, (HEIGHT-40)/2, (WIDTH-80)*i/(GR_COUNT+SOUND_MAX), 40, 100, 250, 100, 0);
        rectangle(40+BUTTON_BORDER*2, (HEIGHT-40)/2+20+BUTTON_BORDER, (WIDTH-80)*i/(GR_COUNT+SOUND_MAX)-BUTTON_BORDER*2, 10, 170, 250, 170);
        updatescr();
    }
}

void uninit_graphics() {
    int i;

    for( i=0; i<GR_COUNT; i++) {
        SDL_FreeSurface( images[i].data);
    }

    if( buffer != NULL) {
        SDL_FreeSurface( buffer);
    }

    if (background != NULL) {
        SDL_FreeSurface(background);
    }

    free(rect_update_cache);
    free(rect_update_old);
    free(images);

    for (i=0; i<FONT_COUNT; i++) {
        TTF_CloseFont(fonts[i].data);
        fonts[i].data = NULL;
    }
    free(fonts);

    freelist_free_all(font_freelist);

    TTF_Quit();
}


FreeList* freelist_create()
{
    FreeList* list = (FreeList*)malloc(sizeof(FreeList));
    assert(list != NULL);

    list->head = NULL;

    return list;
}

void freelist_append(FreeList* list, const char* data)
{
    FreeListItem* new_item = (FreeListItem*)malloc(sizeof(FreeListItem));

    assert(list != NULL);
    assert(data != NULL);

    new_item->data = data;

    /* Insert item at the beginning of list */
    new_item->next = list->head;
    list->head = new_item;
}

void freelist_free_all(FreeList* list)
{
    FreeListItem* next;

    assert(list != NULL);

    while (list->head != NULL) {
        /* Remove one item from the head of the list */
        next = list->head->next;
        free((void*)list->head->data);
        free(list->head);
        list->head = next;
    }

    free(list);
}


int get_image_width( image_id id) {
    return images[id].data->w;
}

int get_image_height( image_id id) {
    return images[id].data->h;
}

int get_sprite_width( image_id id, int items) {
    return images[id].data->w / items;
}

void show_sprite( image_id id, int pos, int items, int x_offset,
        int y_offset, int opacity)
{
    assert(id < GR_COUNT);
    SDL_SetAlpha(images[id].data, SDL_SRCALPHA | SDL_RLEACCEL, opacity);
    blit_surface(images[id].data, x_offset, y_offset, pos, items);
}

void show_image_rotozoom(image_id id, int x, int y, float rotate, float zoom)
{
    SDL_Surface* tmp;
    SDL_Rect src, dst;

    assert(id < GR_COUNT);

    tmp = rotozoomSurface(images[id].data, rotate, zoom, SMOOTHING_OFF);

    dst.w = src.w = tmp->w;
    dst.h = src.h = tmp->h;
    src.x = 0;
    src.y = 0;
    dst.x = x - src.w/2;
    dst.y = y - src.h/2;

    SDL_BlitSurface(tmp, &src, screen, &dst);
    update_rect2(dst);
    SDL_FreeSurface(tmp);
}

void fill_image_offset( image_id id, int x, int y, int w, int h, int offset_x, int offset_y) {
    SDL_Surface *bitmap;
    SDL_Rect src, dst;
    int dx = 0, dy = 0, cx, cy;

    if( id >= GR_COUNT) return;

    bitmap = images[id].data;

    /* Make negative offsets positive */
    if (offset_x < 0) {
        offset_x = (offset_x%bitmap->w)+bitmap->w;
    }
    if (offset_y < 0) {
        offset_y = (offset_y%bitmap->h)+bitmap->h;
    }

    src.y = offset_y % bitmap->h;
    while( dy < h) {
        src.h = dst.h = cy = (h-dy > bitmap->h-src.y)?(bitmap->h-src.y):(h-dy);
        dst.y = y+dy;

        dx = 0;
        src.x = offset_x % bitmap->w;
        while( dx < w) {
            src.w = dst.w = cx = (w-dx > bitmap->w-src.x)?(bitmap->w-src.x):(w-dx);
            dst.x = x+dx;

            SDL_BlitSurface( bitmap, &src, screen, &dst);
            update_rect2(dst);

            dx += cx;
            src.x = 0;
        }

        dy += cy;
        src.y = 0;
    }
}

void line_horiz( int y, Uint8 r, Uint8 g, Uint8 b) {
    rectangle(0, y, screen->w, 1, r, g, b);
}

void line_vert( int x, Uint8 r, Uint8 g, Uint8 b) {
    rectangle(x, 0, 1, screen->h, r, g, b);
}

void rectangle( int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b) {
    Uint32 color;
    SDL_Rect rect;

    if (w <= 0 || h <= 0 || x < 0 || y < 0 || x+w > WIDTH || y+h > HEIGHT) {
        return;
    }

    color = SDL_MapRGB(screen->format, r, g, b);

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    SDL_FillRect( screen, &rect, color);
    update_rect(x, y, w, h);
}

void rectangle_alpha(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 opacity)
{
    Uint32 color;
    SDL_Surface *buf;
    SDL_Rect rect, rect2;

    if (w <= 0 || h <= 0 || x < 0 || y < 0 || x+w > WIDTH || y+h > HEIGHT) {
        return;
    }

    color = SDL_MapRGB(screen->format, r, g, b);

    buf = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h,
                 screen->format->BitsPerPixel,
                 screen->format->Rmask,
                 screen->format->Gmask,
                 screen->format->Bmask,
                 screen->format->Amask);

    rect.x = rect.y = 0;
    rect2.x = x;
    rect2.y = y;
    rect.w = rect2.w = w;
    rect.h = rect2.h = h;

    SDL_FillRect(buf, &rect, color);
    SDL_SetAlpha(buf, SDL_RLEACCEL | SDL_SRCALPHA, opacity);
    SDL_BlitSurface(buf, &rect, screen, &rect2);
    update_rect(x, y, w, h);

    SDL_FreeSurface(buf);
}

void draw_button( int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, char pressed) {
    float diff = (pressed?1.0-BUTTON_HIGHLIGHT:1.0+BUTTON_HIGHLIGHT);
    rectangle(x, y, w, h, MIN(r*diff, 255), MIN(g*diff, 255), MIN(b*diff, 255));
    rectangle(x+BUTTON_BORDER, y+BUTTON_BORDER, w-BUTTON_BORDER, h-BUTTON_BORDER, MIN(r/diff, 255), MIN(g/diff, 255), MIN(b/diff, 255));
    rectangle(x+BUTTON_BORDER, y+BUTTON_BORDER, w-2*BUTTON_BORDER, h-2*BUTTON_BORDER, r, g, b);
}

void draw_button_text(const char* s, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, char pressed) {
    int font_x, font_y;
    float factor = 0.3;
    pressed = pressed?1:0;
    draw_button( x, y, w, h, r, g, b, pressed);
    font_x = x + w/2 - font_get_string_width(FONT_SMALL, s)/2 + pressed*BUTTON_BORDER;
    font_y = y + h/2 - font_get_height(FONT_SMALL)/2 + pressed*BUTTON_BORDER;
    font_draw_string_color(FONT_SMALL, s, font_x, font_y, (Uint8)(r*factor), (Uint8)(g*factor), (Uint8)(b*factor));
}

void draw_button_object(MenuButton* b, int mx, int my)
{
    if (b->image_id == GR_COUNT) {
        draw_button_text(b->text, b->x, b->y, b->w, b->h, b->r, b->g, b->b, M_POS_BUTTON(*b, mx, my));
    } else {
        show_image_rotozoom(b->image_id, b->x+b->w/2, b->y+b->h/2, 0.0, 1.0-0.2*!M_POS_BUTTON(*b, mx, my));
    }
}

void clear_screen()
{
    SDL_Rect rect;

    rect.x = rect.y = 0;
    rect.w = WIDTH;
    rect.h = HEIGHT;

    SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
}

void store_screen()
{
    SDL_BlitSurface(screen, NULL, background, NULL);
    rect_update_old[0].x = rect_update_old[0].y = 0;
    rect_update_old[0].w = WIDTH;
    rect_update_old[0].h = HEIGHT;
    rect_update_old_count = 1;
    rect_update_cache_current = 0;
}

void reset_screen()
{
    int i;
    SDL_Rect* tmp;
    SDL_BlitSurface(background, NULL, screen, NULL);

    for (i=0; i<rect_update_cache_current; i++) {
        SDL_BlitSurface(background, &rect_update_cache[i], screen, &rect_update_cache[i]);
    }

    /* Save rects I've just updated for redraw later */
    tmp = rect_update_cache;
    rect_update_cache = rect_update_old;
    rect_update_old = tmp;
    rect_update_old_count = rect_update_cache_current;
    rect_update_cache_current = 0;
}

void update_rect(Sint32 x, Sint32 y, Sint32 w, Sint32 h)
{
    SDL_Rect* u = NULL;
    static int inside = 0;
    if (inside) return;
    inside = 1;

#ifdef DRAW_UPDATE_RECTANGLE
    rectangle(x, y, w, h, 50+rand()%200, 50+rand()%200 ,50+rand()%200);
#endif

    if ((x >= WIDTH) | (y >= HEIGHT) | (x+w <= 0) | (y+h <= 0) | (!w) | (!h)) {
        inside = 0;
        return;
    }

    if (rect_update_cache_current == RECT_UPDATE_CACHE) {
        fprintf(stderr, "Overflow\n");
        rect_update_cache_current = 0;
    }

    u = &(rect_update_cache[rect_update_cache_current]);

    if (x < 0/* && x+w > 0*/) {
        w += x;
        x = 0;
    }
    if (y < 0/* && y+h > 0*/) {
        h += y;
        y = 0;
    }

    if (x+w >= WIDTH) {
        w -= (x+w-WIDTH+1);
    }
    
    if (y+h >= HEIGHT) {
        h -= (y+h-HEIGHT+1);
    }

    if (w==0 || h==0) {
        inside = 0;
        return;
    }

    u->x = x;
    u->y = y;
    u->w = w;
    u->h = h;

    rect_update_cache_current++;

    inside = 0;
}

void updatescr()
{
    Uint32 ticks = SDL_GetTicks();
    SDL_Rect *r_current = NULL, *end_current = NULL;
    SDL_Rect *r_old = NULL;

    static int fading_last_time = 0;
    unsigned char fading_now = ticks < fading_start+FADE_DURATION;

    if (fading_now) {
        SDL_SetAlpha(buffer, SDL_SRCALPHA | SDL_RLEACCEL, 255-255*(ticks-fading_start)/FADE_DURATION);
        SDL_BlitSurface(buffer, NULL, screen, NULL);
        SDL_UpdateRect(screen, 0, 0, 0, 0);
    } else if (fading_last_time && !fading_now) {
        SDL_UpdateRect(screen, 0, 0, 0, 0);
    } else {
        if (rect_update_old_count == rect_update_cache_current) {
            /* Merge rects into one single rect list */
            r_old = rect_update_old;
            r_current = rect_update_cache;
            end_current = rect_update_cache + rect_update_cache_current;
            while (r_current != end_current) {
                r_old->w = MAX(r_current->x+r_current->w, r_old->x+r_old->w);
                r_old->h = MAX(r_current->y+r_current->h, r_old->y+r_old->h);
                r_old->x = MIN(r_current->x, r_old->x);
                r_old->y = MIN(r_current->y, r_old->y);
                r_old->w -= r_old->x;
                r_old->h -= r_old->y;

                r_current++;
                r_old++;
            }
            SDL_UpdateRects(screen, rect_update_old_count, rect_update_old);
        } else {
            SDL_UpdateRects(screen, rect_update_old_count, rect_update_old);
            SDL_UpdateRects(screen, rect_update_cache_current, rect_update_cache);
        }
    }

    reset_screen();
    fading_last_time = fading_now;
}

void start_fade() {
    SDL_BlitSurface( screen, NULL, buffer, NULL);
    fading_start = SDL_GetTicks();
}

SDL_Surface* font_render_surface(font_id id, const char* text, Uint8 r,
        Uint8 g, Uint8 b)
{
    SDL_Surface *result;
    SDL_Color color;

    color.r = r;
    color.g = g;
    color.b = b;

    assert(id < FONT_COUNT);

    result = TTF_RenderText_Blended(fonts[id].data, text, color);

    assert(result != NULL);

    return result;
}

void blit_surface(SDL_Surface* surface, int x, int y, int pos, int count)
{
    SDL_Rect src, dst;

    dst.w = src.w = surface->w/count;
    dst.h = src.h = surface->h;
    src.x = src.w*pos;
    src.y = 0;
    dst.x = x;
    dst.y = y;

    SDL_BlitSurface(surface, &src, screen, &dst);
    update_rect2(dst);
}

SDL_Surface* get_surface(image_id id)
{
    assert(id < GR_COUNT);
    return images[id].data;
}

void font_draw_string_color(font_id id, const char* s, int x_offset, int y_offset, Uint8 r, Uint8 g, Uint8 b) {
    SDL_Surface *text;

    text = font_render_surface(id, s, r, g, b);
    blit_surface_simple(text, x_offset, y_offset);
    SDL_FreeSurface(text);
}

int font_get_string_width(font_id id, const char* s) {
    int w, h;

    assert(id < FONT_COUNT);

    if (TTF_SizeText(fonts[id].data, s, &w, &h) != 0) {
        return 0;
    }

    return w;
}

int font_get_height(font_id id) {
    assert(id < FONT_COUNT);
    return TTF_FontHeight(fonts[id].data);
}

void draw_line_faded( int x1, int y1, int x2, int y2, int r, int g, int b, int r2, int g2, int b2) {
    float step, dx, dy, x = x1, y = y1;
    int i;
    char fade = (r!=r2 || g!=g2 || b!=b2);

    step = (float)(abs(x2-x1)>abs(y2-y1)?abs(x2-x1):abs(y2-y1));
    dx = (float)(x2-x1) / step;
    dy = (float)(y2-y1) / step;

    SDL_LockSurface( screen);
    for( i=0; i<step; i++) {
        x += dx;
        y += dy;
        if( x < 0.0 || x >= WIDTH || y < 0.0 || y >= HEIGHT) {
            continue;
        }
        if( fade) {
            SET_PIXEL_RGB( screen, (int)x, (int)y, (r*(step-i)+r2*i)/step, (g*(step-i)+g2*i)/step, (b*(step-i)+b2*i)/step);
        } else {
            SET_PIXEL_RGB( screen, (int)x, (int)y, r, g, b);
        }
    }
    SDL_UnlockSurface( screen);
}

