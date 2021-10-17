
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

#include "tennix.h"
#include "sound.h"
#include "archive.hh"
#include "graphics.h"

#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>

static Sound* sounds;

static const char* samples[] = {
    "rain.ogg",
    "racket1.ogg",
    "racket2.ogg",
    "ground1.ogg",
    "ground2.ogg",
    "audience.ogg",
    "applause.ogg",
    "out.ogg",
    "background.ogg",
    "mouseover.ogg",
    "mouseclick.ogg",

#ifdef HAVE_VOICE_FILES
    "to.ogg",
    "all.ogg",
    "love-in.ogg",
    "love-out.ogg",
    "fifteen-in.ogg",
    "fifteen-out.ogg",
    "thirty-in.ogg",
    "thirty-out.ogg",
    "forty-in.ogg",
    "forty-out.ogg",
    "deuce.ogg",
    "advantage-player-one.ogg",
    "advantage-player-two.ogg",

    "zero-in.ogg",
    "zero-out.ogg",
    "one-in.ogg",
    "one-out.ogg",
    "two-in.ogg",
    "two-out.ogg",
    "three-in.ogg",
    "three-out.ogg",
    "four-in.ogg",
    "four-out.ogg",
    "five-in.ogg",
    "five-out.ogg",
    "six-in.ogg",
    "six-out.ogg",
    "seven-in.ogg",
    "seven-out.ogg",

    "in-the-first-set.ogg",
    "in-the-second-set.ogg",
    "in-the-third-set.ogg",
    "in-the-fourth-set.ogg",
    "in-the-fifth-set.ogg",

    "quit-it1.ogg",
    "quit-it2.ogg",
    "quit-it3.ogg",
    "quit-it4.ogg",

    "new-game1.ogg",
    "new-game2.ogg",
    "new-game3.ogg",
    "new-game4.ogg",
    "new-game5.ogg",
    "new-game6.ogg",

    "lets-go1.ogg",
    "lets-go2.ogg",
    "lets-go3.ogg",
    "lets-go4.ogg",
#endif
};

static sound_id voice_queue[VOICE_QUEUE_MAX];
static int voice_queue_size = 0;
static int voice_queue_position = 0;

int voice_finished_flag = 1;
static int available_channels = 0;

void init_sound(TennixArchive& tnxar) {
    int i;
    char *d;
    Mix_Chunk* data;

    if( Mix_OpenAudio( 44100, AUDIO_S16SYS, 2, 1024) < 0) {
        fprintf( stderr, "Error initializing SDL_mixer: %s\n", Mix_GetError());
    }

    sounds = (Sound*)calloc(SOUND_MAX, sizeof(Sound));

    draw_button(40, (HEIGHT-40)/2, WIDTH-80, 40, 100, 100, 100, 1);
    store_screen();

    /* +1 because of zero-based indexing AND +1 for voice channel */
    available_channels = Mix_AllocateChannels(LAST_SOUNDEFFECT_ID+1+1);

    if (available_channels < LAST_SOUNDEFFECT_ID+1+1) {
        fprintf(stderr, "Warning: Got only %d audio channels (requested %d).\n",
                available_channels,
                LAST_SOUNDEFFECT_ID+1+1);
    }

    for( i=0; i<SOUND_MAX; i++) {
        if (tnxar.setItemFilename(samples[i]) != 0) {
            d = tnxar.getItemBytes();
            data = Mix_LoadWAV_RW(SDL_RWFromMem(d, tnxar.getItemSize()), 0);
            free(d);
        } else {
            fprintf(stderr, "Cannot find %s\n", samples[i]);
            exit(EXIT_FAILURE);
        }
        if( !data) {
            fprintf( stderr, "Error: %s\n", SDL_GetError());
            continue;
        }

        sounds[i].data = data;

        draw_button(40, (HEIGHT-40)/2, (WIDTH-80)*(GR_COUNT+i)/(SOUND_MAX+GR_COUNT), 40, 100, 250, 100, 0);
        rectangle(40+BUTTON_BORDER, (HEIGHT-40)/2+20, (WIDTH-80)*(GR_COUNT+i)/(SOUND_MAX+GR_COUNT)-2*BUTTON_BORDER, 10, 170, 250, 170);
        updatescr();
    }

    clear_screen();
    store_screen();
    updatescr();

    /* for Voice Queue processing */
    Mix_ChannelFinished(voice_channel_finished);
}

void play_sample_n(sound_id id, int n)
{
    if (id >= SOUND_MAX) {
        fprintf(stderr, "Cannot play sound #%d.\n", id);
        return;
    }

    if (n == -2) {
        Mix_FadeInChannel(CHANNEL_BY_ID(id, available_channels), sounds[id].data, -1, FADE_IN_MS);
    } else {
        Mix_PlayChannel(CHANNEL_BY_ID(id, available_channels), sounds[id].data, n);
    }

    /* Audience stops clapping when ball is served */
    if (id >= SOUND_RACKET_FIRST && id <= SOUND_RACKET_LAST) {
        stop_sample(SOUND_APPLAUSE);
    }
}

void stop_sample(sound_id id)
{
    Mix_FadeOutChannel(CHANNEL_BY_ID(id, available_channels), FADE_OUT_MS);
}

void sample_volume(sound_id id, float volume)
{
    Mix_Volume(CHANNEL_BY_ID(id, available_channels), volume*MIX_MAX_VOLUME);
}

void sample_volume_group(sound_id first, sound_id last, float volume)
{
    unsigned int i;

    for (i=first; i<=last; i++) {
        sample_volume(i, volume);
    }
}
void pan_sample(sound_id id, float position)
{
    if (position == 0.5) {
        Mix_SetPanning(CHANNEL_BY_ID(id, available_channels), 255, 255);
    }
    else {
        Mix_SetPanning(CHANNEL_BY_ID(id, available_channels), 255*(1.0-position), 255*(position));
    }
}

void pan_sample_group(sound_id first, sound_id last, float position)
{
    unsigned int i;

    for (i=first; i<=last; i++) {
        pan_sample(i, position);
    }
}
void voice_clear()
{
    voice_queue_size = 0;
    Mix_HaltChannel(CHANNEL_VOICE);
}

void voice_enqueue(sound_id id)
{
    if (voice_queue_size < VOICE_QUEUE_MAX) {
        voice_queue[voice_queue_size++] = id;
    } else {
        fprintf(stderr, "Voice queue overflow. Skipping: %d\n", id);
    }
}

void voice_say()
{
    if (voice_queue_size > 0) {
        voice_queue_position = 0;
        voice_finished_flag = 0;
        Mix_PlayChannel(CHANNEL_VOICE, sounds[voice_queue[voice_queue_position]].data, 0);
        voice_queue_position++;
    }
}

void voice_say_list(int n, ...)
{
    va_list ap;
    int i;
    sound_id id;

    voice_clear();

    va_start(ap, n);
    for (i=0; i<n; i++) {
        id = (sound_id)va_arg(ap, int);
        voice_enqueue(id);
    }
    va_end(ap);
    voice_say();
}

void voice_channel_finished(int channel)
{
    if (channel == CHANNEL_VOICE) {
        if (voice_queue_position < voice_queue_size) {
            Mix_PlayChannel(CHANNEL_VOICE, sounds[voice_queue[voice_queue_position]].data, 0);
            voice_queue_position++;
        } else {
            voice_finished_flag = 1;
            voice_queue_position = 0;
        }
    }
}

