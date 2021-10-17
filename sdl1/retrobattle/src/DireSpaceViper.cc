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

#include "DireSpaceViper.h"
#include "DireSpaceViperBody.h"

DireSpaceViper::DireSpaceViper(remar2d *gfx, SoundManager *sfx,
			       ScoreKeeper *scoreKeeper,
			       list<Enemy *> *enemies, int length)
  : SpaceViper(gfx, sfx, scoreKeeper, enemies, length)
{
  // Remove old sprite and body parts
  gfx->removeSpriteInstance(sprite_instance);

  // Go through list of body parts and remove from the "enemies" list

  list<SpaceViperBody *>::iterator it = body.begin();
  for(;it != body.end();it++)
    {
      enemies->remove(*it);
    }
  body.clear();

  // Create new sprite and body parts
  for(int i = 0;i < length;i++)
    {
      DireSpaceViperBody *b = new DireSpaceViperBody(gfx, sfx, i%10 == 0, (SpaceViper *)this);
      body.push_back(b);
      enemies->push_back(b);
    }

  sprite_instance = gfx->createSpriteInstance("dire snake head");
  setVisible(true);
  updateAnimation();

  /* Not initialized yet */
  moveAbs(-32, -32);
}

DireSpaceViper::~DireSpaceViper()
{

}

void DireSpaceViper::update(Field *field, Hero *hero)
{
  // 50% chance that Dire Space Viper will home on Captain Good when
  // it's time to choose direction
  aimAtHero = (bool)(rand()%2);

  SpaceViper::update(field, hero);
}
