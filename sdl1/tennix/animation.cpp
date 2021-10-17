
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


#include <stdlib.h>
#include <assert.h>

#include "tennix.h"
#include "graphics.h"
#include "animation.h"

#include "credits.h"

Animation* animation_new()
{
    Animation* animation = (Animation*)calloc(1, sizeof(Animation));
    assert(animation != NULL);
    return animation;
}

Animation* animation_append(Animation* animation, AnimationPart* part)
{
    if (part->end > animation->duration) {
        animation->duration = part->end;
    }

    if (animation->head == NULL) {
        /* insert into empty list */
        part->next = NULL;
        part->prev = NULL;
        animation->head = part;
        animation->tail = part;
    } else {
        /* insert into non-empty list */
        part->prev = animation->tail;
        part->next = NULL;
        animation->tail->next = part;
        animation->tail = part;
    }

    return animation;
}

void animation_free(Animation* animation)
{
    AnimationPart* part;
    AnimationPart* next;

    part = animation->head;
    while (part != NULL) {
        next = part->next;
        animation_part_free(part);
        part = next;
    }

    free(animation);
}

AnimationState* animation_state_new(Animation* animation)
{
    AnimationState* state = (AnimationState*)calloc(1, sizeof(AnimationState));
    assert(state != NULL);

    state->animation = animation;
    state->started = SDL_GetTicks();
    state->ending = state->started + animation->duration;

    return state;
}

void animation_state_run(AnimationState* state, int interruptible)
{
    AnimationPart* p;
    SDL_Event e;

    clear_screen();
    store_screen();
    while (animation_state_update(state)) {
        if (interruptible) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) {
                    return;
                }
            }
        }
        p = state->animation->head;
        while (p != NULL) {
            animation_part_display(p, state);
            p = p->next;
        }
        updatescr();
    }
}

int animation_state_update(AnimationState* state)
{
    assert(state != NULL);

    state->current = SDL_GetTicks();

    return state->current < state->ending;
}

void animation_state_free(AnimationState* state)
{
    free(state);
}

AnimationPart* animation_part_new_from_surface(Uint32 start, Uint32 end,
        unsigned int effects, int x, int y, int zindex, unsigned int gravity,
        SDL_Surface* surface, unsigned char free_surface, int pos, int count)
{
    AnimationPart* part = (AnimationPart*)calloc(1, sizeof(AnimationPart));
    assert(part != NULL);

    part->start = start;
    part->end = end;
    part->effects = effects;

    part->x = x;
    part->y = y;
    part->zindex = zindex;

    part->gravity = gravity;

    part->surface = surface;
    part->free_surface = free_surface;
    part->pos = pos;
    part->count = count;

    return part;
}

AnimationPart* animation_part_new_text(Uint32 start, Uint32 end,
        unsigned int effects, int x, int y, int zindex, unsigned int gravity,
        font_id id, const char* text, Uint8 r, Uint8 g, Uint8 b)
{
    return animation_part_new_from_surface(
            start,
            end,
            effects,
            x,
            y,
            zindex,
            gravity,
            font_render_surface(id, text, r, g, b),
            1,
            0,
            1
    );
}

void animation_part_display(AnimationPart* part, AnimationState* state)
{
    Uint32 start = state->started + part->start;
    Uint32 end = state->started + part->end;
    Uint32 x, y;
    float p;
    SDL_Surface* surface;

    if (state->current < start || state->current > end) {
        /* this part has had it's time (or is yet to have it ;) */
        return;
    }

    surface = part->surface;
    p = (float)(state->current - start)/(float)(end - start);

    if (part->effects & ANIMATION_ZOOM_2X) {
        surface = rotozoom_surface(surface, 0.0, p+1);
    }

    switch (part->gravity) {
        case GRAVITY_CENTER_UPPERHALF:
            x = (WIDTH - surface->w)/2 + part->x;
            y = HEIGHT/2 - surface->h + part->y;
            break;
        case GRAVITY_CENTER_BOTTOMHALF:
            x = (WIDTH - surface->w)/2 + part->x;
            y = HEIGHT/2 + part->y;
            break;
        case GRAVITY_CENTER_SCREEN:
            x = (WIDTH - surface->w)/2 + part->x;
            y = (HEIGHT - surface->h)/2 + part->y;
            break;
        case GRAVITY_TOP:
            x = (WIDTH - surface->w)/2 + part->x;
            y = part->y;
            break;
        case GRAVITY_TOPLEFT:
            /* fall-through to default */
        default:
            x = part->x;
            y = part->y;
            break;
    }

    if (part->effects & ANIMATION_FROM_BOTTOM) {
        y = y * p + (HEIGHT-part->surface->h) * (1-p);
    }

    if (part->effects & ANIMATION_SLIDE_LEFT) {
        x = x * p;
    }

    if (part->effects & ANIMATION_SLIDE_RIGHT) {
        x = (WIDTH-surface->w) * (1.-p) + x * p;
    }

    blit_surface_simple(surface, x, y);

    /* FIXME: fade in and out should be handled by blitting w/ alpha value */
    if (part->effects & ANIMATION_FADE_IN) {
        if (p < ANIMATION_FADE_PART) {
            rectangle_alpha(x, y, surface->w, surface->h, 0, 0, 0,
                255*(1-(1./ANIMATION_FADE_PART)*p));
        }
    }

    if (part->effects & ANIMATION_FADE_OUT) {
        if (p > (1-ANIMATION_FADE_PART)) {
            rectangle_alpha(x, y, surface->w, surface->h, 0, 0, 0,
                255*((1./ANIMATION_FADE_PART)*(p-(1-ANIMATION_FADE_PART))));
        }
    }

    if (part->effects & ANIMATION_ZOOM_2X) {
        SDL_FreeSurface(surface);
    }
}

void animation_part_free(AnimationPart* part)
{
    /* free surface if necessary */
    if (part->free_surface) {
        SDL_FreeSurface(part->surface);
    }

    free(part);
}


Animation* create_intro()
{
    Animation* intro = animation_new();

    /* Just some weird demo of how the animations work */
    animation_append(intro, animation_part_new_text(100, 4000, ANIMATION_FADE_IN | ANIMATION_FADE_OUT, 0, 0, 0, GRAVITY_CENTER_SCREEN, FONT_MEDIUM, "thpinfo.com presents", 255, 255, 255));
    animation_append(intro, animation_part_new_from_surface(4000, 8000, ANIMATION_FADE_IN | ANIMATION_FADE_OUT | ANIMATION_ZOOM_2X, 0, 0, 0, GRAVITY_CENTER_SCREEN, get_surface(GR_TENNIXLOGO), 0, 0, 1));

    return intro;
}

Animation* create_credits()
{
    int pos = 0;
    Animation* credits = animation_new();
    while (credits_text[pos] != NULL) {
        animation_append(credits,
                animation_part_new_text(100+pos*CREDITS_OFFSET,
                    100+pos*CREDITS_OFFSET + CREDITS_DURATION,
                    ANIMATION_FADE_IN | ANIMATION_FADE_OUT | ANIMATION_FROM_BOTTOM,
                    0, 0, 0,
                    GRAVITY_TOP,
                    FONT_MEDIUM,
                    credits_text[pos],
                    255, 255, 255));
        pos++;
    }
    return credits;
}

