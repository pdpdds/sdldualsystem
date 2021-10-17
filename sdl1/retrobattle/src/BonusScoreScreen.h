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

#ifndef BONUS_SCORE_SCREEN_H
#define BONUS_SCORE_SCREEN_H

#include <remar2d.h>
#include "SoundManager.h"
#include "Input.h"
#include "ScoreKeeper.h"
#include "GameMode.h"
#include "Counter.h"

class BonusScoreScreen : public GameMode
{
 public:
  BonusScoreScreen(remar2d *gfx, SoundManager *sfx, Input *input,
		   ScoreKeeper *scoreKeeper);
  ~BonusScoreScreen();
  Mode update();

 private:
  void drawBackground();

  enum State { TIME, COINS, PERFECT, DONE };
  enum SubState { SHOW, TICK, TICKDELAY };

  State state;
  SubState subState;

  int delayTimer;
  int tickDelayTimer;

  char *mainTiles;

  int nextTimer;

  /* Strings */
  int top;
  int score;
  int stage;

  Counter *topScoreCounter;
  Counter *scoreCounter;
  Counter *level;

  int clock;
  Counter *time;
  int scorePerTime;

  int coinSprite;
  Counter *coins;
  int coinsCounted;
  int scorePerCoin;

  int perfect;
  int twothousand;

/*   int fuzzSprite; */
/*   Counter *fuzzes; */
/*   int fuzzesCounted; */
/*   int scorePerFuzz; */

/*   int droneSprite; */
/*   Counter *drones; */
/*   int dronesCounted; */
/*   int scorePerDrone; */

/*   int viperSprite; */
/*   Counter *vipers; */
/*   int vipersCounted; */
/*   int scorePerViper; */

/*   int hunterSprite; */
/*   Counter *hunters; */
/*   int huntersCounted; */
/*   int scorePerHunter; */
};

#endif
