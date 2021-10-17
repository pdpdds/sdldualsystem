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

#ifndef ANIMATION_H
#define ANIMATION_H

#include "Frame.h"
#include "SDL.h"
#include <list>

using namespace std;

class Animation
{
 public:
  Animation(const char *name);
  ~Animation();
  void addImage(const char *path, int size_x, int size_y,
		int orig_x, int orig_y, const char *key);
  void addSDLImage(SDL_Surface *surface, int size_x, int size_y);
  void addFrame(Frame *frame);
  void framesAdded();
  SDL_Surface *getImage();
  SDL_Rect *getRect(int frame);
  void getNextFrame(int *currentFrame, int *timeSpentInTime, int delta);
  void setLooping(bool loop);
  bool emptyFrame(int currentFrame);
  const char *getName();

  /* Centre point of animation */
  int orig_x, orig_y;

 private:
  const char *name;
  SDL_Surface *image;

  /* Size of frames */
  int size_x, size_y;

  bool looping;

  Frame **frames;
  int numFrames;
  list<Frame *> temp_frames;
};

#endif
