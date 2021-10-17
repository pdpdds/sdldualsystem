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

#ifndef OBJECT_H
#define OBJECT_H

#include "SDL.h"
#include <remar2d.h>
#include "SoundManager.h"

using namespace std;

/* Animated object in the game */
class Object
{
 public:
  Object(remar2d *gfx, const char *sprite, SoundManager *sfx);
  virtual ~Object();
  void moveAbs(float x, float y);
  void moveRel(float x, float y);
  virtual void setVisible(bool visible);
  void setAnimation(const char *animation);
  void pauseAnimation(bool on);
  void setBoundingBox(int width, int height, int offset_x, int offset_y);
  int getX();
  int getY();
  SDL_Rect *getBoundingBox();
  bool collides(Object *other);
  bool destroy();

  /* Update position and actions in the level. */
  virtual void update();

 protected:
  remar2d *gfx;
  SoundManager *sfx;
  int sprite_instance;
  float pos_x, pos_y;
  SDL_Rect bounding_box;
  bool animationPaused;
  const char *name;

  /* Countdown to destruction (GRRRRRR) */
  int destroyTimer;

  /* Destroy this object */
  bool destroyMe;
};

#endif
