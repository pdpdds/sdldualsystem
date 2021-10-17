/*
    remar2d - a 2D graphics engine using SDL
    Copyright (C) 2007 Andreas Remar, andreas.remar@gmail.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef SPRITE_H
#define SPRITE_H

#include "Animation.h"
#include "tinyxml.h"
#include <map>
#include <string>

using namespace std;

class Sprite
{
 public:
  Sprite(const char *file);
  Sprite();
  ~Sprite();

  const char *getName();
  void setName(const char *n);
  Animation *getAnimation(const char *animName);
  void addAnimation(Animation *anim);

 private:
  char *name;

  const char *currentAnimation;
  map<string, Animation *> animations;

  void startNewAnimation(TiXmlElement *element);
  void addImageToAnimation(TiXmlElement *element, const char *pathToXml);
  void addFrameToAnimation(TiXmlElement *element);
};

#endif
