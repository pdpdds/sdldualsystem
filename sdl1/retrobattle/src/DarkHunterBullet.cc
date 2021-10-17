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

#include "DarkHunterBullet.h"
#include "DarkExplosion.h"

DarkHunterBullet::DarkHunterBullet(remar2d *gfx, SoundManager *sfx,
				   list<Enemy *> *enemies,
				   list<Object *> *objects)
  : HunterBullet(gfx, sfx, objects), enemies(enemies)
{
  // Change gfx
  gfx->removeSpriteInstance(sprite_instance);
  sprite_instance = gfx->createSpriteInstance("dhshot");
  setAnimation("normal");
  setVisible(true);
}

void
DarkHunterBullet::update(Field *field, Hero *hero)
{
  HunterBullet::update(field, hero);
}

inline bool insideLevel(int x, int y)
{
  return x >= 0 && x <= 24 && y >= 0 && y <= 18;
}

void
DarkHunterBullet::blockHit(Field *field, int blockX, int blockY)
{
  // Spawn 5 Dark Explosions!
  if(insideLevel(blockX, blockY))
     enemies->push_back(new DarkExplosion(gfx, sfx, blockX*32, blockY*32));
  if(insideLevel(blockX-1, blockY))
     enemies->push_back(new DarkExplosion(gfx, sfx, (blockX-1)*32, blockY*32));
  if(insideLevel(blockX+1, blockY))
     enemies->push_back(new DarkExplosion(gfx, sfx, (blockX+1)*32, blockY*32));
  if(insideLevel(blockX, blockY-1))
     enemies->push_back(new DarkExplosion(gfx, sfx, blockX*32, (blockY-1)*32));
  if(insideLevel(blockX, blockY+1))
     enemies->push_back(new DarkExplosion(gfx, sfx, blockX*32, (blockY+1)*32));
}

