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

#ifndef REMAR2D_H
#define REMAR2D_H

#include "SDL.h"

class remar2d
{
 public:
  
  enum ErrorCode
    {
      NO_ERROR,
      FAILED_SET_VIDEO_MODE,
      FAILED_LOAD_SPRITE,
      FAILED_LOAD_TILESET
    };

  remar2d(int width, int height, int bpp, int fullscreen, const char *title);
  ~remar2d();

  void showCredits();

  ErrorCode getError();
  const char *getErrorMessage();

  void redraw();

  void setBackgroundColor(int r, int g, int b);

  const char *loadTileSet(const char *file);
  void  removeTileSet(const char *tileset);
  void  loadTileMap(const char *file);

  void setupTileBackground(int size_x, int size_y);
  void setTile(int x, int y, const char *tileSet, int t_x, int t_y);

  const char *loadSprite(const char *file);
  void  removeSprite(const char *sprite);
  int   inFrontOf(int sprite1, int sprite2);

  int  createSpriteInstance(const char *sprite);
  void setAnimation(int sprite, const char *animation);
  void moveSpriteRel(int sprite, int x, int y);
  void moveSpriteAbs(int sprite, int x, int y);
  void showSprite(int sprite, bool show);
  void removeSpriteInstance(int sprite);
  void pauseAnimation(int sprite, bool on);
  void neverRedraw(int sprite, bool on);

  void pauseAnimations(bool on);

  const char *loadFont(const char *file);
  void removeFont(const char *font);
  int print(const char *font, const char *text);

  void showPointer(bool on);

  void printResources();

  void setFullScreen(bool on);
};

#endif
