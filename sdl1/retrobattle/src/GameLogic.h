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

#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <remar2d.h>
#include "SoundManager.h"
#include "Input.h"
#include "ScoreKeeper.h"
#include "GameMode.h"

class GameLogic
{
 public:
  GameLogic(Input *i, remar2d *gfx, SoundManager *sfx, char *datadir);
  ~GameLogic();
  void update();
  bool quit();

 private:
  void makeGfxPath(char *buf, char *datadir, const char *file);

  Input *input;
  remar2d *graphics;
  SoundManager *sound;

  ScoreKeeper *scoreKeeper;

  /* Game mode */
  GameMode *gameMode;
  Mode mode;

  /* Fixed interval time-based animation */
  static const int maximumFrameRate = 60;
  static const int minimumFrameRate = 15;
  const float updateInterval = 1.0 / maximumFrameRate;
  const float maxCyclesPerFrame = maximumFrameRate / minimumFrameRate;

  float lastFrameTime;
  float cyclesLeftOver;

  bool quitGame;

  bool fullScreen;

  char *datadir;
};

#endif
