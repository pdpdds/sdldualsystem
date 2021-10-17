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

#include "Counter.h"

Counter::Counter(remar2d *gfx, int length)
{
  this->gfx = gfx;
  this->length = length;

  sprite_instances = new int[length];

  for(int i = 0;i < length;i++)
    {
      sprite_instances[i] = gfx->createSpriteInstance("numbers");
      gfx->setAnimation(sprite_instances[i], "0");
      gfx->showSprite(sprite_instances[i], true);
    }

  setPosition(0, 0);
}

Counter::~Counter()
{
  for(int i = 0;i < length;i++)
    {
      gfx->showSprite(sprite_instances[i], false);
      gfx->removeSpriteInstance(sprite_instances[i]);
    }

  delete [] sprite_instances;
}

void
Counter::setPosition(int x, int y)
{
  for(int i = 0;i < length;i++)
    {
      gfx->moveSpriteAbs(sprite_instances[i], x+i*16, y);
    }
}

int exp10(int n)
{
  int retval = 1;

  for(int i = 0;i < n;i++)
    {
      retval *= 10;
    }

  return retval;
}

void
Counter::setCounter(int v)
{
  if(v == value || v < 0 || v >= exp10(length))
    return;

  value = v;

  int temp = value;
  const char *nums[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

  for(int i = length-1, j=0;i >= 0;i--,j++)
    {
      int num = temp / exp10(i);
      gfx->setAnimation(sprite_instances[j], nums[num]);
      temp %= exp10(i);
    }
}
