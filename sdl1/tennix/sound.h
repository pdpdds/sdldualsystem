
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

#ifndef __SOUND_H
#define __SOUND_H

#include "archive.hh"

#include "SDL_mixer.h"

#define FADE_OUT_MS 500
#define FADE_IN_MS 3000

#define VOICE_QUEUE_MAX 10

extern int voice_finished_flag;

typedef unsigned char sound_id;
enum {
    SOUND_RAIN = 0,
    SOUND_RACKET1,
    SOUND_RACKET2,
    SOUND_GROUND1,
    SOUND_GROUND2,
    SOUND_AUDIENCE,
    SOUND_APPLAUSE,
    SOUND_OUT,
    SOUND_BACKGROUND,
    SOUND_MOUSEOVER,
    SOUND_MOUSECLICK,

#ifdef HAVE_VOICE_FILES
    VOICE_TO,
    VOICE_ALL,
    VOICE_LOVE_IN,
    VOICE_LOVE_OUT,
    VOICE_FIFTEEN_IN,
    VOICE_FIFTEEN_OUT,
    VOICE_THIRTY_IN,
    VOICE_THIRTY_OUT,
    VOICE_FORTY_IN,
    VOICE_FORTY_OUT,
    VOICE_DEUCE,
    VOICE_ADVANTAGE_PLAYER_ONE,
    VOICE_ADVANTAGE_PLAYER_TWO,

    VOICE_ZERO_IN,
    VOICE_ZERO_OUT,
    VOICE_ONE_IN,
    VOICE_ONE_OUT,
    VOICE_TWO_IN,
    VOICE_TWO_OUT,
    VOICE_THREE_IN,
    VOICE_THREE_OUT,
    VOICE_FOUR_IN,
    VOICE_FOUR_OUT,
    VOICE_FIVE_IN,
    VOICE_FIVE_OUT,
    VOICE_SIX_IN,
    VOICE_SIX_OUT,
    VOICE_SEVEN_IN,
    VOICE_SEVEN_OUT,

    VOICE_IN_THE_FIRST_SET,
    VOICE_IN_THE_SECOND_SET,
    VOICE_IN_THE_THIRD_SET,
    VOICE_IN_THE_FOURTH_SET,
    VOICE_IN_THE_FIFTH_SET,

    VOICE_QUIT_IT1,
    VOICE_QUIT_IT2,
    VOICE_QUIT_IT3,
    VOICE_QUIT_IT4,

    VOICE_NEW_GAME1,
    VOICE_NEW_GAME2,
    VOICE_NEW_GAME3,
    VOICE_NEW_GAME4,
    VOICE_NEW_GAME5,
    VOICE_NEW_GAME6,

    VOICE_LETS_GO1,
    VOICE_LETS_GO2,
    VOICE_LETS_GO3,
    VOICE_LETS_GO4,
#endif

    SOUND_MAX
};

#define LAST_SOUNDEFFECT_ID SOUND_MOUSECLICK

/* Channel by id gives: 1-N (of 0-N) */
#define CHANNEL_BY_ID(id, numchannels) ((id)%numchannels+1)
#define CHANNEL_VOICE 0

#define SOUND_RACKET_FIRST SOUND_RACKET1
#define SOUND_RACKET_LAST SOUND_RACKET2
#define SOUND_GROUND_FIRST SOUND_GROUND1
#define SOUND_GROUND_LAST SOUND_GROUND2
#ifdef HAVE_VOICE_FILES
#  define VOICE_QUIT_IT_FIRST VOICE_QUIT_IT1
#  define VOICE_QUIT_IT_LAST VOICE_QUIT_IT4
#  define VOICE_NEW_GAME_FIRST VOICE_NEW_GAME1
#  define VOICE_NEW_GAME_LAST VOICE_NEW_GAME6
#  define VOICE_LETS_GO_FIRST VOICE_LETS_GO1
#  define VOICE_LETS_GO_LAST VOICE_LETS_GO4
#endif

#define SOUND_GROUND (SOUND_GROUND_FIRST + rand()%(SOUND_GROUND_LAST-SOUND_GROUND_FIRST+1))
#define SOUND_RACKET (SOUND_RACKET_FIRST + rand()%(SOUND_RACKET_LAST-SOUND_RACKET_FIRST+1))
#ifdef HAVE_VOICE_FILES
#  define VOICE_QUIT_IT (VOICE_QUIT_IT_FIRST + rand()%(VOICE_QUIT_IT_LAST-VOICE_QUIT_IT_FIRST+1))
#  define VOICE_NEW_GAME (VOICE_NEW_GAME_FIRST + rand()%(VOICE_NEW_GAME_LAST-VOICE_NEW_GAME_FIRST+1))
#  define VOICE_LETS_GO (VOICE_LETS_GO_FIRST + rand()%(VOICE_LETS_GO_LAST-VOICE_LETS_GO_FIRST+1))
#endif

typedef struct {
    Mix_Chunk* data;
} Sound;

void init_sound(TennixArchive& tnxar);

void play_sample_n(sound_id id, int n);
#define play_sample(id) play_sample_n(id,0)
#define play_sample_loop(id) play_sample_n(id,-1)
#define play_sample_background(id) play_sample_n(id,-2)
void stop_sample(sound_id id);
void sample_volume(sound_id id, float volume);
void sample_volume_group(sound_id first, sound_id last, float volume);
void pan_sample(sound_id id, float position);
void pan_sample_group(sound_id first, sound_id last, float position);
#define unpan_sample(id) pan_sample(id,0.5)

void voice_clear();
void voice_enqueue(sound_id id);
void voice_say();
void voice_say_list(int n, ...);
void voice_channel_finished(int channel);

#endif
