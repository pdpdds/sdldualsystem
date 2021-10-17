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

#ifndef BONUS_H
#define BONUS_H

#include "Object.h"
#include "ScoreKeeper.h"

class Bonus : public Object
{
 public:
  Bonus(remar2d *gfx, bool spawnQuake);
  bool getCollected() { return isCollected; }
  void setCollected()
  { 
    isCollected = true; 
    destroyTimer = 60; 
    setVisible(false);
  }

  enum BonusType { SMALL_COINS,
		   DIAMOND,
		   GOLD_BAR,
		   BLUE_PEARL,
		   WHITE_BALL,
		   LOCK,
		   POWER_GUN,
		   LASER_GUN,
		   CLOCK,
		   BOMB,
		   QUAKE };

  BonusType getType() { return type; }


 private:
  void randomize(bool spawnQuake);

  BonusType type;
  bool isCollected;
};

#endif // BONUS_H
