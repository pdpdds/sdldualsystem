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

#include "Bonus.h"
#include <stdlib.h>

Bonus::Bonus(remar2d *gfx, bool spawnQuake)
  : Object(gfx, "bonus", 0), isCollected(false)
{
  randomize(spawnQuake);

  const char *animations[] = 
    {"small coins", "diamond", "gold bar", "blue pearl",
     "white ball", "lock", "power gun", "laser gun",
     "clock", "bomb", "quake"};
  setAnimation(animations[type]);
  setVisible(true);

  setBoundingBox(20, 20, 0, 0);
}

void
Bonus::randomize(bool spawnQuake)
{
  int selected;

  if(spawnQuake)
    selected = rand() % 23;
  else
    selected = rand() % 22;

  BonusType weighted[] = {SMALL_COINS, SMALL_COINS, SMALL_COINS,
			  DIAMOND, DIAMOND, DIAMOND,
			  GOLD_BAR, GOLD_BAR, GOLD_BAR,
			  BLUE_PEARL, BLUE_PEARL, BLUE_PEARL,
			  WHITE_BALL, WHITE_BALL,
			  LOCK, LOCK,
			  POWER_GUN, POWER_GUN,
			  LASER_GUN, LASER_GUN,
			  CLOCK,
			  BOMB,
			  QUAKE};

  type = weighted[selected];
}
