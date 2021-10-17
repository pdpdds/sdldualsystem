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

#include "HunterBullet.h"
#include "Explosion.h"

HunterBullet::HunterBullet(remar2d *gfx, SoundManager *sfx, 
			   list<Object *> *objects)
  : Enemy(gfx, "hshot", sfx, 0), objects(objects)
{
  setBoundingBox(4, 4, 1, 1);
  setAnimation("normal");
  setVisible(true);
  direction = LEFT;
}

void
HunterBullet::moveLeft()
{
  direction = LEFT;
}

void
HunterBullet::moveRight()
{
  direction = RIGHT;
}

void
HunterBullet::blockHit(Field *field, int blockX, int blockY)
{
  if(!field->blockHit(blockX, blockY))
    {
      objects->push_back(new Explosion(gfx, sfx, getX()-9, getY()-9));
    }
}

void
HunterBullet::update(Field *field, Hero *hero)
{
  if(destroyMe)
    return;

  if(collides(hero) && !hero->isBlinking() && !hero->isDead())
    {
      hero->die();
      destroyMe = true;

      blockHit(field, getX()/32, getY()/32);

      return;
    }

  if(getX() < -6 || getX() > 800)
    {
      destroyMe = true;
      return;
    }

  int blockX, blockY;
  if(field->objectCollidesWithBackground(this, &blockX, &blockY))
    {
      blockHit(field, blockX, blockY);

      sfx->playSound(3);
      destroyMe = true;
      return;
    }

  if(direction == LEFT)
    moveRel(-3, 0);
  else
    moveRel(3, 0);
}
