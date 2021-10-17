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

#ifndef DRONE_H
#define DRONE_H

#include "Enemy.h"

class Drone : public Enemy
{
 public:
  Drone(remar2d *gfx, SoundManager *sfx, ScoreKeeper *scoreKeeper);
  virtual void update(Field *field, Hero *hero);
  bool hit();

 protected:
  bool aimAtHero;

 private:
  enum Direction {NONE, LEFT, RIGHT, UP, DOWN};

  void die();
  void getBlockInDirection(Direction dir, int *blockX, int *blockY);

  Direction moveDirection;
  int moved;
  bool willDig;
  bool moveThisUpdate;
  int hitPoints;

  bool dead;
  int deathTimer;

  int pauseTimer;
};

#endif
