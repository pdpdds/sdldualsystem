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

#ifndef DARK_HUNTER_BULLET_H
#define DARK_HUNTER_BULLET_H

#include "HunterBullet.h"

class DarkHunterBullet : public HunterBullet
{
 public:
  DarkHunterBullet(remar2d *gfx, SoundManager *sfx,
		   list<Enemy *> *enemies, list<Object *> *objects);
  virtual void update(Field *field, Hero *hero);

 private:
  list<Enemy *> *enemies;

 protected:
  virtual void blockHit(Field *field, int blockX, int blockY);
};

#endif
