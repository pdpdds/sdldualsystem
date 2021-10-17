/*
 *    Source from Retrobattle, a NES-like collect-em-up.
 *    Copyright (C) 2010 Andreas Remar
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    Original authors contact info: andreas.remar@gmail.com
 */

#include "SoundManager.h"
#include <stdio.h>

SoundManager::SoundManager(char *datadir)
{
#ifndef NO_SDL_MIXER
  if(Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024))
    {
      printf("SDL_mixer says: %s\n", Mix_GetError());
    }

  const char *soundFiles[] =
    {
      "bomb.wav",  /* 0 */
      "coin.wav",  /* 1 */
      "death.wav", /* 2 */
      "explo.wav", /* 3 */
      "jump.wav",  /* 4 */
      "pain.wav",  /* 5 */
      "shoot.wav", /* 6 */
      "spawn.wav", /* 7 */
      "splat.wav", /* 8 */
      "stun.wav",  /* 9 */
      "wake.wav",  /* 10 */
      "walk.wav",  /* 11 */
      "dot.wav",   /* 12 */
      "pause.wav", /* 13 */
      "start.wav", /* 14 */
      "scoretick.wav", /* 15 */
      "bonus.wav", /* 16 */
      "pickup.wav", /* 17 */
      "quake.wav", /* 18 */
      0
    };
  
  const char *musicFiles[] =
    {
      "test.wav", /* 0 */
      "clear.wav", /* 1 */
      "bstage.wav", /* 2 */
      0
    };

  sounds = new Mix_Chunk*[sizeof(soundFiles)/sizeof(char *) - 1];

  printf("Loading sounds");
  for(int i = 0;soundFiles[i];i++)
    {
      char buf[1024];
      sprintf(buf, "%s/data/sfx/%s", datadir, soundFiles[i]);
      printf(".");
      sounds[i] = Mix_LoadWAV(buf);
      if(!sounds[i]) {
	printf("x");
      }
    }
  printf("\n");

  songs = new Mix_Music*[sizeof(musicFiles)/sizeof(char *) - 1];

  printf("Loading music");
  for(int i = 0;musicFiles[i];i++)
    {
      char buf[1024];
      sprintf(buf, "%s/data/sfx/%s", datadir, musicFiles[i]);
      printf(".");
      songs[i] = Mix_LoadMUS(buf);
      if(!songs[i]) {
	printf("x");
      }
    }
  printf("\n");

#else
  printf("No SDL_Mixer available at compile time!\n");
#endif

}

SoundManager::~SoundManager()
{
#ifndef NO_SDL_MIXER
  Mix_HaltChannel(-1);
  Mix_HaltMusic();
#endif
}

int
SoundManager::playSound(int i, bool loop)
{
#ifndef NO_SDL_MIXER
  return Mix_PlayChannel(-1, sounds[i], loop ? -1 : 0);
#endif
}

void
SoundManager::stopSound(int channel)
{
#ifndef NO_SDL_MIXER
  Mix_HaltChannel(channel);
#endif
}

void
SoundManager::playMusic(int i, bool loop)
{
#ifndef NO_SDL_MIXER
  Mix_PlayMusic(songs[i], loop ? -1 : 0);
#endif
}

void
SoundManager::stopMusic()
{
#ifndef NO_SDL_MIXER
  Mix_HaltMusic();
#endif
}
