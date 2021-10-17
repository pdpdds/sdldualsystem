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

#include "DarkBountyHunter.h"
#include "DarkHunterBullet.h"

DarkBountyHunter::DarkBountyHunter(remar2d *gfx, SoundManager *sfx,
				   ScoreKeeper *scoreKeeper,
				   list<Enemy *> *enemies,
				   list<Object *> *objects)
  : BountyHunter(gfx, sfx, scoreKeeper, enemies, objects)
{
  gfx->removeSpriteInstance(sprite_instance);
  sprite_instance = gfx->createSpriteInstance("dark hunter");

  setAnimation("left");
  setVisible(false);
  moveAbs(-32, -32);
}

void
DarkBountyHunter::update(Field *field, Hero *hero)
{
  // frameCounter should be set to 20, not random, for Dark Bounty Hunter,
  // so that Dark Bounty Hunter is hidden exactly 1/3 second
  bool resetFrameCounter = (state == FADEOUT && frameCounter == 0);

  BountyHunter::update(field, hero);

  if(resetFrameCounter)
    frameCounter = 20;
}

void
DarkBountyHunter::fire()
{
  // Create Dark Bounty Hunter Shot !
  DarkHunterBullet *b = new DarkHunterBullet(gfx, sfx, enemies, objects);
  if(facing == LEFT)
    {
      b->moveAbs(getX(), getY()+12);
      b->moveLeft();
    }
  else
    {
      b->moveAbs(getX()+24, getY()+12);
      b->moveRight();
    }

  enemies->push_back(b);
}
