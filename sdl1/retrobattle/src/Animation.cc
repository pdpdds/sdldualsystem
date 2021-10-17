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

#include "Animation.h"

#include "SurfaceCache.h"
extern SurfaceCache *surfaceCache;

Animation::Animation(const char *name)
{
  this->name = name;
  looping = true;
  orig_x = 0;
  orig_y = 0;
}

Animation::~Animation()
{
  /* Delete frames */
  delete [] frames;
}

void
Animation::addImage(const char *path, int size_x, int size_y,
		    int orig_x, int orig_y, const char *key)
{
  image = surfaceCache->get(path);
  if(image == 0)
    {
      printf("FAILED TO LOAD IMAGE \"%s\"\n", path);
    }
  else if(key)
    {
      if(strlen(key) == 6)
	{
	  int r, g, b;
	  sscanf(key, "%2x", &r);
	  sscanf(&key[2], "%2x", &g);
	  sscanf(&key[4], "%2x", &b);

	  SDL_SetColorKey(image, SDL_SRCCOLORKEY | SDL_RLEACCEL,
			  SDL_MapRGB(image->format, r, g, b));
	}
    }

  this->size_x = size_x;
  this->size_y = size_y;
  this->orig_x = orig_x;
  this->orig_y = orig_y;
}

void
Animation::addSDLImage(SDL_Surface *surface, int size_x, int size_y)
{
  image = surface;
  this->size_x = size_x;
  this->size_y = size_y;
  orig_x = 0;
  orig_y = 0;
}

void
Animation::addFrame(Frame *frame)
{
  temp_frames.push_back(frame);
}

void
Animation::framesAdded()
{
  numFrames = temp_frames.size();

  frames = new Frame*[numFrames];

  int i = 0;
  while(temp_frames.empty() == 0)
    {
      frames[i++] = temp_frames.front();
      temp_frames.pop_front();
    }

  temp_frames.clear();
}

SDL_Surface *
Animation::getImage()
{
  return image;
}

SDL_Rect *
Animation::getRect(int frame)
{
  SDL_Rect *rect = new SDL_Rect;
  Frame *f = frames[frame];
  rect->w = size_x;
  rect->h = size_y;
  rect->x = f->x * size_x;
  rect->y = f->y * size_y;

  return rect;
}

void
Animation::getNextFrame(int *currentFrame, int *timeSpentInFrame, int delta)
{
  if(*currentFrame >= numFrames)
    {
      printf("BUG! Frame was set to nonexistant frame no!\n");

      *currentFrame = 0;
      *timeSpentInFrame = 0;
      
      return;
    }
  else if(*currentFrame == numFrames - 1 && !looping)
    {
      /* Animation is at last frame */
      return;
    }

  *timeSpentInFrame += delta;

  while(*timeSpentInFrame >= frames[*currentFrame]->duration)
    {
      *timeSpentInFrame -= frames[*currentFrame]->duration;

      (*currentFrame)++;
      if(*currentFrame >= numFrames)
	{
	  *currentFrame = 0;
	}
    }
}

void
Animation::setLooping(bool loop)
{
  looping = loop;
}

bool
Animation::emptyFrame(int currentFrame)
{
  return frames[currentFrame]->empty;
}

const char *
Animation::getName()
{
  return name;
}
