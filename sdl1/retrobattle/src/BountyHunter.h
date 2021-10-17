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

#ifndef BOUNTYHUNTER_H
#define BOUNTYHUNTER_H

#include "Enemy.h"

class BountyHunter : public Enemy
{
 public:
  BountyHunter(remar2d *gfx, SoundManager *sfx, ScoreKeeper *scoreKeeper,
	       list<Enemy *> *enemies, list<Object *> *objects);
  virtual void update(Field *field, Hero *hero);
  bool hit();
  enum State {HIDDEN, FADEIN, PREFIRE, POSTFIRE, FADEOUT};

 private:
  void die();
  void randomizeLocation(Field *field, Hero *hero);
  void setFacing(Hero *hero);
  void updateAnimation();

  int hitPoints;

  bool dead;
  int deathTimer;

  int pauseTimer;

 protected:
  virtual void fire();
  State state;
  int frameCounter;

  list<Object *> *objects;
  list<Enemy *> *enemies;

  enum Direction {LEFT, RIGHT};
  Direction facing;
};

#endif // BOUNTYHUNTER_H
