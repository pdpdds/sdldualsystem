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

#ifndef SPACE_VIPER_H
#define SPACE_VIPER_H

#include "Enemy.h"
#include "SpaceViperBody.h"

class SpaceViper : public Enemy
{
 public:
  SpaceViper(remar2d *gfx, SoundManager *sfx, ScoreKeeper *scoreKeeper,
	     list<Enemy *> *enemies, int length);
  ~SpaceViper();
  virtual void update(Field *field, Hero *hero);
  bool hit();
  void pause();

 private:
  enum Direction {NONE, LEFT, RIGHT, UP, DOWN};

  void die();
  bool tailAtPosition(int blockX, int blockY);
  void getBlockAtDirection(Direction direction, int *blockX, int *blockY);
  
  Object *block;

  list<Enemy *> *enemies;

  /* Head sprite */
  int head;

  /* Body objects */
  Direction moveDirection;
  int hitPoints;
  bool dead;
  int length;

  bool moveThisFrame;

  /* Keep track of how long we've moved, randomize direction every 32 pixels */
  int moved;

  int pauseTimer;

protected:
  void updateAnimation();
  list<SpaceViperBody *> body;
  bool aimAtHero;
};

#endif
