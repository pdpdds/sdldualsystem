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

#include "SpaceViperBody.h"
#include "SpaceViper.h"

SpaceViperBody::SpaceViperBody(remar2d *gfx, SoundManager *sfx, bool bright,
			       SpaceViper *head)
  : Enemy(gfx, "snake body", sfx, 0), head(head)
{
  if(bright)
    setAnimation("light green");
  else
    setAnimation("dark green");
  setVisible(true);

  setBoundingBox(14, 14, 9, 9);
}

void
SpaceViperBody::update(Field *field, Hero *hero)
{
  Object::update();

  if(destroyTimer > 0 && splatAnimationStart > 0)
    {
      splatAnimationStart--;
      if(splatAnimationStart == 0)
	{
	  setAnimation("splat");
	}
    }

  if(collides(hero) && destroyTimer == 0 && !destroyMe
     && !hero->isBlinking() && !hero->isDead())
    {
      hero->die();

      head->pause();
    }
}

bool
SpaceViperBody::hit()
{
  /* Remove bullet, but no effect on the Spaaaaace Viiiiipeeeeeer! */
  return true;
}

void
SpaceViperBody::splat()
{
  splatAnimationStart = rand()%15;
  destroyTimer = 60;
}
