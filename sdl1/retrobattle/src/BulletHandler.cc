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

#include "BulletHandler.h"

BulletHandler::BulletHandler(HUD *hud)
  : hud(hud)
{
  reset();
//   ammo = 8;
//   refillCounter = 0;
//   refillCounterTimer = 0;
}

void
BulletHandler::update()
{
  if(refillCounterTimer > 0)
    {
      --refillCounterTimer;
      if(refillCounterTimer == 0)
	{
	  refillCounter++;

	  if(refillCounter == 9)
	    {
	      ammo++;

	      hud->setValue(HUD::BULLET, ammo);
	      hero->showNote(ammo);

	      refillCounter = 0;
	    }
	  else
	    {
	      refillCounterTimer = 60;
	    }

	  hud->setValue(HUD::RELOAD, refillCounter);
	}
    }

  if(ammo < 8 && refillCounter == 0 && refillCounterTimer == 0)
    {
      /* Start refilling */
      refillCounterTimer = 60;
    }
}

bool
BulletHandler::fire()
{
  if(ammo == 0)
    return false;

  ammo--;

  hud->setValue(HUD::BULLET, ammo);

  hero->showNote(ammo);

  return true;
}

void
BulletHandler::reset()
{
  ammo = 8;
  refillCounter = 0;
  refillCounterTimer = 0;

  hud->setValue(HUD::BULLET, ammo);
  hud->setValue(HUD::RELOAD, refillCounter);
}

void
BulletHandler::addOne()
{
  if(ammo == 0)
    {
      ammo++;
      hud->setValue(HUD::BULLET, ammo);
      hero->showNote(ammo);
    }
}
