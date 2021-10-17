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

#ifndef HUD_H
#define HUD_H

#include <remar2d.h>
#include "ScoreKeeper.h"
#include "Counter.h"

class HUD
{
 public:
  enum Display { SCORE, TOP, TIME, LIVES, STAGE, SKILL, RELOAD, BULLET,
		 WEAPON };

  HUD(remar2d *gfx, ScoreKeeper *scoreKeeper);
  ~HUD();
  void update();  
  void setValue(Display display, int value);

 private:
  void setAmmo(int value);
  void setReload(int value);
  void setWeapon(int value); // 0 = none, 1 = POWER, 2 = LASER

  remar2d *gfx;
  Counter *counters[6];
  ScoreKeeper *scoreKeeper;

  int hudSprite;

  int ammoSprites[8];
  int reloadSprites[8];

  int powerText;
  int laserText;
};

#endif
