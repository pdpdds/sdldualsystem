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

#ifndef INPUT_H
#define INPUT_H

#include "SDL.h"

typedef struct
{
  int pressed; /* key pressed? */
  int checked; /* key checked? */
  int timeout; /* when to repeat key */
} key_type;

class Input
{
 private:
  key_type keys[SDLK_LAST];
  int timeout;
  bool quitEvent;
  int actionKeys[4];

 public:
  Input();
  Input(int _timeout);

  enum ACTION {LEFT, RIGHT, FIRE, JUMP};

  /* Set a new timeout period to allow configurability. */
  void setTimeout(int _timeout);

  /* Read input states (only keyboard as of now) */
  void getInput();

  /* Checks if key has been pressed since last update (only returns
     'true' once for each keypress) */
  bool pressed(int key);

  /* Checks if key has been released since last update (only returns
     'true' once for each key release) */
  bool released(int key);

  /* Check if key is held down (returns 'true' every time the key is
     pressed) */
  bool held(int key);

  /* Return first found keypress (used to get a key when redefining keys) */
  int getKeyPressed();

  /* Define key to use for specific action */
  bool defineActionKey(ACTION action, int key);

  /* Reset action keys (used when redefining keys) */
  void resetActionKeys();

  /* Checks if the key defined for the ACTION has been pressed since
     last update */
  bool actionPressed(ACTION action);

  /* Checks if the key defined for the ACTION has been released since
     last update */
  bool actionReleased(ACTION action);

  /* Checks if the key defined for the ACTION is held */
  bool actionHeld(ACTION action);

  /* Return 'true' if a SDL_QUIT event has been seen.  This occurs
     when the user clicks the X in the corner of the window. */
  bool quit();
};

#endif
