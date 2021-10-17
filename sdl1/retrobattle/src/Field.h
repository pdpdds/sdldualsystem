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

#ifndef FIELD_H
#define FIELD_H

#include <remar2d.h>
#include <list>
#include "SoundManager.h"
#include "BrokenBlock.h"
#include "Object.h"


class Field
{
 public:
  enum BlockType
    {
      EMPTY = 0,
      SOLID = 1,
      BREAKABLE = 2,
      DAMAGED = 3,
      BROKEN = 4
    };

  BlockType field[25][19];

  int SPIKES_LEVEL;

  Field(remar2d *gfx, SoundManager *sfx,
	list<BrokenBlock *> *brokenBlocks,
	list<Object *> *objects,
	bool blocksTakeTwoHits,
	int skill,
	bool bonusLevel,
	char *datadir);

  bool blockHit(int x, int y, bool createExplosion = true);
  void breakBlock(int x, int y, bool createExplosion = true);
  void repairBlock(int x, int y);
  void drawBlock(int x, int y);
  void drawBlockAndSurrounding(int x, int y);
  void redrawAll(bool bright = false);
  void randomBlocks();
  void moveObjectRel(Object *object, int *x, int *y);

  /* Returns true if the Object collides with the background. x and y
     is set to the background tile that the object collides with (can
     only collide with one tile...) */
  bool objectCollidesWithBackground(Object *object, int *x, int *y);

  inline bool emptyBlock(int x, int y)
  {
    /* Let checks wrap around */
    if(x <= -1)
      x = 24;
    if(x >= 25)
      x = 0;

    return field[x][y] == EMPTY || field[x][y] == BROKEN;
  }

 private:

  remar2d *gfx;
  SoundManager *sfx;
  list<BrokenBlock *> *brokenBlocks;
  list<Object *> *objects;
  bool blocksTakeTwoHits;
  int skill;

  /* Names of tilesets */
  const char *backgroundBlocks;
  const char *blocks;
  const char *solids;
  const char *dots;
  const char *spikes;

  bool bonusLevel;
};

#endif
