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

#ifndef FUZZ_H
#define FUZZ_H

#include <remar2d.h>
#include "Object.h"
#include "Enemy.h"
#include "Field.h"
#include "Hero.h"

class Fuzz : public Enemy // Object
{
 public:
  enum Direction {NONE, LEFT, RIGHT, UP, DOWN};

  Fuzz(remar2d *gfx, SoundManager *sfx, ScoreKeeper *scoreKeeper);
  virtual void update(Field *field, Hero *hero);
  void rollLeft();
  void rollRight();
  void rollRandom();
  void stun();
  void blink();
  void die();
  Direction getRollDir() { return rollDirection; }
  Direction getMoveDir() { return moveDirection; }
  void setMoveDir(Direction dir) { moveDirection = dir; }
  bool isStunned();
  bool hit();
  void isFast();

 private:
  void attach(int posX1, int posX2, int posY1, int posY2, Direction direction);
  void attachNone();
  void startRolling();

  Direction moveDirection;
  Direction lastAttachDirection;

  bool stunned;
  bool falling;
  unsigned int blinkTime;
  int fallDistance;
  bool fastFuzz;
  bool dead;
  bool onSpikes;

  int FALL_LIMIT;
  int SPIKES_LEVEL;

  /* Keep track of the last block(s) that we we're attached to */
  int last1_x, last1_y;
  int last2_x, last2_y;

  int deathTimer;
  int stunTimer;

  int pauseTimer;

 protected:
  Direction rollDirection;
};

#endif
