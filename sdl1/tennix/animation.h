
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

#ifndef __ANIMATION_H
#define __ANIMATION_H

#include "graphics.h"

enum {
    ANIMATION_FADE_IN     = 1<<0,
    ANIMATION_FADE_OUT    = 1<<1,
    ANIMATION_FROM_BOTTOM = 1<<2,
    ANIMATION_ZOOM_2X     = 1<<3,
    ANIMATION_SLIDE_LEFT  = 1<<4,
    ANIMATION_SLIDE_RIGHT = 1<<5
};

#define ANIMATION_INTRO_EFFECTS (ANIMATION_FADE_IN | ANIMATION_FADE_OUT | ANIMATION_ZOOM_2X)

/**
 * How much time of the animation should be spent
 * on fading (0.0..1.0) - usually 1/4 or so...
 **/
#define ANIMATION_FADE_PART (0.25)

enum {
    GRAVITY_TOPLEFT = 0,
    GRAVITY_TOP,
    GRAVITY_CENTER_SCREEN,
    GRAVITY_CENTER_UPPERHALF,
    GRAVITY_CENTER_BOTTOMHALF
};

struct _AnimationPart {
    Uint32 start;
    Uint32 end;

    unsigned int effects;

    int x;
    int y;
    int zindex;

    SDL_Surface* surface;
    unsigned char free_surface;
    int pos;
    int count;

    unsigned int gravity;

    struct _AnimationPart* prev;
    struct _AnimationPart* next;
};

typedef struct _AnimationPart AnimationPart;

typedef struct {
    AnimationPart *head;
    AnimationPart *tail;
    Uint32 duration;
} Animation;

typedef struct {
    Animation* animation;
    Uint32 started;
    Uint32 current;
    Uint32 ending;
} AnimationState;


Animation* animation_new();
Animation* animation_append(Animation*, AnimationPart*);
void animation_free(Animation*);

AnimationState* animation_state_new(Animation*);
void animation_state_run(AnimationState*, int);
int animation_state_update(AnimationState*);
void animation_state_free(AnimationState*);

AnimationPart* animation_part_new_from_surface(Uint32, Uint32, unsigned int,
        int, int, int, unsigned int, SDL_Surface*, unsigned char, int, int);

AnimationPart* animation_part_new_text(Uint32, Uint32, unsigned int,
        int, int, int, unsigned int, font_id, const char*, Uint8, Uint8,
        Uint8);

void animation_part_display(AnimationPart*, AnimationState*);
void animation_part_free(AnimationPart*);


/* simplified function calls */
#define animation_part_new_from_surface_simple(start, end, effects, \
        surface, free_surface) \
        (animation_part_new_from_surface(start, end, effects, 0, 0, 0, \
                     GRAVITY_CENTER_SCREEN, surface, free_surface, 0, 1))

#define animation_part_new_text_simple(start, end, effects, id, text) \
    (animation_part_new_text(start, end, effects, 0, 0, 0, \
                             GRAVITY_CENTER_SCREEN, id, text, 255, 255, 255))


/* available animations */
Animation* create_intro();
Animation* create_credits();


#endif

