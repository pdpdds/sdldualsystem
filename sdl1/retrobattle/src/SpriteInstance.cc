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

#include "SpriteInstance.h"

SpriteInstance::SpriteInstance(Sprite *sprite)
  :  x(0), y(0), currentFrame(0),currentAnimationName(0), currentAnimation(0),
     currentAnimRect(0), paused(false), timeSpentInFrame(0),redraw(false),
     neverRedraw(false)
     
{
  this->sprite = sprite;

  currentRect.x = currentRect.y = currentRect.w = currentRect.h = 0;
}

void
SpriteInstance::setAnimation(const char *animation)
{
  currentAnimationName = animation;
  currentFrame = 0;
  timeSpentInFrame = 0;

  currentAnimation = sprite->getAnimation(currentAnimationName);
  currentAnimRect = currentAnimation->getRect(0);
  currentAnimOffsetX = currentAnimation->orig_x;
  currentAnimOffsetY = currentAnimation->orig_y;

  redraw = true;
}

void
SpriteInstance::setVisible(bool visible)
{
  this->visible = visible;
}

Sprite *
SpriteInstance::getSprite()
{
  return sprite;
}

const char *
SpriteInstance::getCurrentAnimation()
{
  return currentAnimationName;
}

void
SpriteInstance::animate(int delta)
{
  if(!currentAnimationName || paused)
    return;

  // Animation *animation = sprite->getAnimation(currentAnimationName);

  int oldFrame = currentFrame;
  currentAnimation->getNextFrame(&currentFrame, &timeSpentInFrame, delta);

  if(currentFrame != oldFrame)
    redraw = true;
}

void
SpriteInstance::pauseAnimation(bool yes)
{
  paused = yes;
}

bool
SpriteInstance::getVisible()
{
  return visible;
}

void
SpriteInstance::setRedraw(bool redraw)
{
  if(!neverRedraw)
    this->redraw = redraw;
}

bool
SpriteInstance::getRedraw()
{
  return redraw;
}

void
SpriteInstance::moveRel(int x, int y)
{
  old_x = this->x;
  old_y = this->y;
  this->x += x;
  this->y += y;

  redraw = true;
}

void
SpriteInstance::moveAbs(int x, int y)
{
  old_x = this->x;
  old_y = this->y;
  this->x = x;
  this->y = y;
  
  redraw = true;
}

void
SpriteInstance::setLastRect(int x, int y, int w, int h)
{
  lastRect.x = x;
  lastRect.y = y;
  lastRect.w = w;
  lastRect.h = h;
}

void
SpriteInstance::setNeverRedraw(bool on)
{
  neverRedraw = on;
}

SDL_Rect *
SpriteInstance::getLastRect()
{
  return &lastRect;
}

SDL_Rect *
SpriteInstance::getCurrentRect()
{
  // FIXME: Must take into account the animation offset as well!!
  // Animation *animation = sprite->getAnimation(currentAnimationName);
  if(!currentAnimRect)
    return &currentRect;

  currentRect.x = x - currentAnimOffsetX;
  currentRect.y = y - currentAnimOffsetY;
  currentRect.w = currentAnimRect->w;
  currentRect.h = currentAnimRect->h;

  return &currentRect;
}
