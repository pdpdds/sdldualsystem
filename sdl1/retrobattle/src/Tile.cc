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

#include "Tile.h"
#include <algorithm>

Tile::Tile()
  : empty(true)
{
}

Tile::Tile(const char *tileSet, int x, int y)
  : empty(false)
{
  this->tileSet = tileSet;
  this->x = x;
  this->y = y;
}

void
Tile::addSpriteInstance(SpriteInstance *spriteInstance)
{
  sprites.push_back(spriteInstance);
}

void
Tile::removeSpriteInstance(SpriteInstance *spriteInstance)
{
  sprites.remove(spriteInstance);
}

list<SpriteInstance *> *
Tile::getListOfSprites()
{
  return &sprites;
}

void
Tile::setListOfSprites(list<SpriteInstance *> *oldSpriteList)
{
  /* Is this too ugly? It destroys the old sprite list... */
  sprites.swap(*oldSpriteList);
}

/* This should have been a lambda... */
template<class T> struct setRedraw : public unary_function<T, void>
{
  void operator() (T x)
  {
//     Sprite *sprite = x->getSprite();
//     printf("Mark %s as dirty!!\n", sprite->getName());
    x->setRedraw(true);
  }
};

void
Tile::markSpritesDirty()
{
  for_each(sprites.begin(), sprites.end(), setRedraw<SpriteInstance *>());
}
