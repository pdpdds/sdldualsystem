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

#ifndef TILE_H
#define TILE_H

#include "SpriteInstance.h"
#include <list>

using namespace std;

class Tile
{
 public:
  Tile();
  Tile(const char *tileSet, int x, int y);

  /** Add sprite instance to tile to indicate that this sprite should
      be redrawn if this tile is redrawn. */
  void addSpriteInstance(SpriteInstance *spriteInstance);

  /** Remove sprite instance from tile to indicate that this sprite
      has left the tile so redraw isn't needed when tile is redrawn. */
  void removeSpriteInstance(SpriteInstance *spriteInstance);

  list<SpriteInstance *> *getListOfSprites();
  void setListOfSprites(list<SpriteInstance *> *oldSpriteList);

  void markSpritesDirty();

  const char *tileSet;

  /* Tile position in the tileset */
  int x, y;

  bool redraw;
  list<SpriteInstance *> sprites;

  /* If empty, this tile contains no graphic, just overdraw with
     background color when necessary */
  bool empty;
};

#endif
