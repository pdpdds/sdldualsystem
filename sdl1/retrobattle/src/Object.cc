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

#include "Object.h"

Object::Object(remar2d *gfx, const char *sprite, SoundManager *sfx)
  : destroyTimer(0), destroyMe(false)
{
  this->gfx = gfx;
  this->sfx = sfx;
  sprite_instance = gfx->createSpriteInstance(sprite);
  name = sprite;
}

Object::~Object()
{
  setVisible(false);
  gfx->removeSpriteInstance(sprite_instance);
}

void
Object::moveAbs(float x, float y)
{
  pos_x = x;
  pos_y = y;

  gfx->moveSpriteAbs(sprite_instance, getX(), getY());
}


void
Object::moveRel(float x, float y)
{
  pos_x += x;
  pos_y += y;

  gfx->moveSpriteAbs(sprite_instance, getX(), getY());
}

void
Object::setVisible(bool visible)
{
  gfx->showSprite(sprite_instance, visible);
}

void
Object::setAnimation(const char *animation)
{
  gfx->setAnimation(sprite_instance, animation); 
}

void
Object::pauseAnimation(bool on)
{
  gfx->pauseAnimation(sprite_instance, on);
}

void
Object::setBoundingBox(int width, int height, int offset_x, int offset_y)
{
  bounding_box.w = width - 1;
  bounding_box.h = height - 1;
  bounding_box.x = offset_x;
  bounding_box.y = offset_y;
}

int
Object::getX()
{
  float q = pos_x - (int)pos_x;
  if(q > 0.5)
    return (int)pos_x+1;
  else
    return (int)pos_x;
}

int
Object::getY()
{
  float q = pos_y - (int)pos_y;
  if(q > 0.5)
    return (int)pos_y+1;
  else
    return (int)pos_y;
}

SDL_Rect *
Object::getBoundingBox()
{
  return &bounding_box;
}

inline int abs(int x)
{
  return x < 0 ? -x : x;
}

/*
  TODO: Consider when an object fits within another object completely... :P

  o         o

    *  *

    *  *

  o         o

  We must check collision in both "directions", no ``o'' is within the ``*'':s

  Also consider:

      aoa
      ooo
      ooo
   b*******b
   *********
   b*******b
      ooo
      ooo
      aoa

  None of the points are inside the other rectangle... but this sure
  is a collision... maybe check with a line intersection:

      a-a
      a |
      a |
   bbbbbbbbb
   |  a |  |
   b--a-+--b
      a |
      a |
      a-a


*/
bool
Object::collides(Object *other)
{
  // printf("Collides? (%s, %s)\n", name, other->name);

  SDL_Rect *bb1 = getBoundingBox();
  SDL_Rect *bb2 = other->getBoundingBox();

  int bb1_x1 = getX() + bb1->x;
  int bb1_y1 = getY() + bb1->y;
  int bb1_x2 = bb1_x1 + bb1->w;
  int bb1_y2 = bb1_y1 + bb1->h;

  int bb2_x1 = other->getX() + bb2->x;
  int bb2_y1 = other->getY() + bb2->y;
  int bb2_x2 = bb2_x1 + bb2->w;
  int bb2_y2 = bb2_y1 + bb2->h;

//   printf("(%d, %d) - (%d, %d) <=> (%d, %d) - (%d, %d)\n",
// 	 bb1_x1, bb1_y1, bb1_x2, bb1_y2,
// 	 bb2_x1, bb2_y1, bb2_x2, bb2_y2);

  /* Assume all objects are smaller than 32x32 (small optimization) */
  if(abs(bb1_x1 - bb2_x1) > 32 || abs(bb1_y1 - bb2_y1) > 32)
    return false;

  /* Check if any of Bounding Box 2's points are inside Bounding Box 1 */
  /* x1, y1 */
  if(bb2_x1 >= bb1_x1 && bb2_x1 <= bb1_x2
     && bb2_y1 >= bb1_y1 && bb2_y1 <= bb1_y2)
    {
      return true;
    }
  /* x2, y1 */
  if(bb2_x2 >= bb1_x1 && bb2_x2 <= bb1_x2
     && bb2_y1 >= bb1_y1 && bb2_y1 <= bb1_y2)
    {
      return true;
    }
  /* x1, y2 */
  if(bb2_x1 >= bb1_x1 && bb2_x1 <= bb1_x2
     && bb2_y2 >= bb1_y1 && bb2_y2 <= bb1_y2)
    {
      return true;
    }
  /* x2, y2 */
  if(bb2_x2 >= bb1_x1 && bb2_x2 <= bb1_x2
     && bb2_y2 >= bb1_y1 && bb2_y2 <= bb1_y2)
    {
      return true;
    }

  /* Check if any of Bounding Box 1's points are inside Bounding Box 2 */
  /* x1, y1 */
  if(bb1_x1 >= bb2_x1 && bb1_x1 <= bb2_x2
     && bb1_y1 >= bb2_y1 && bb1_y1 <= bb2_y2)
    {
      return true;
    }
  /* x2, y1 */
  if(bb1_x2 >= bb2_x1 && bb1_x2 <= bb2_x2
     && bb1_y1 >= bb2_y1 && bb1_y1 <= bb2_y2)
    {
      return true;
    }
  /* x1, y2 */
  if(bb1_x1 >= bb2_x1 && bb1_x1 <= bb2_x2
     && bb1_y2 >= bb2_y1 && bb1_y2 <= bb2_y2)
    {
      return true;
    }
  /* x2, y2 */
  if(bb1_x2 >= bb2_x1 && bb1_x2 <= bb2_x2
     && bb1_y2 >= bb2_y1 && bb1_y2 <= bb2_y2)
    {
      return true;
    }

  /* No collision */
  return false;
}

bool
Object::destroy()
{
  return destroyMe;
}

void
Object::update()
{
  if(destroyTimer > 0)
    {
      --destroyTimer;
      if(destroyTimer == 0)
	{
	  destroyMe = true;
	}
    }
}
