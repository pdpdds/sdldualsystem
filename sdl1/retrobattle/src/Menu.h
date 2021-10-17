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

#ifndef MENU_H
#define MENU_H

#include <remar2d.h>
#include "SoundManager.h"
#include "Input.h"
#include "ScoreKeeper.h"
#include "GameMode.h"
#include "Counter.h"

class Menu : public GameMode
{
 public:
  Menu(remar2d *gfx, SoundManager *sfx, Input *input,
       ScoreKeeper *scoreKeeper);
  ~Menu();
  Mode update();

 private:
  enum SUBMODE {NORMAL, REDEFINE_LEFT, REDEFINE_RIGHT, REDEFINE_FIRE,
		REDEFINE_JUMP};

  void drawBackground();
  void drawPattern(int *arr, int xOffset ,int yOffset, int width, int height);
  void increaseLevel();
  void decreaseLevel();
  void increaseSkillLevel();
  void moveArrows();
  
  char *mainTiles;

  Counter *levelCounter;
  int level;

  Counter *skillCounter;
  int skill;

  int nextTimer;

  int leftArrow;
  int rightArrow;
  int downArrow;

  /* Strings */
  int remar_games_2012;
  int enter_to_start;
  int space_to_set_keys;
  int stage;
  int skillLevel;
  int score;
  int topScore;

  int actionStrings[4];

  SUBMODE subMode;

  int blinkDelay;

  int keyConfig[4];

  int moveArrowsTimer;
  int arrowOffset;
};

#endif
