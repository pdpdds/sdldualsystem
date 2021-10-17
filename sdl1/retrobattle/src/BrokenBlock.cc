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

#include "BrokenBlock.h"

BrokenBlock::BrokenBlock(int x, int y)
{
  this->x = x;
  this->y = y;

  respawnTimer = 8*60; /* 8 seconds respawn time */
}

void
BrokenBlock::update()
{
  if(respawnTimer > 0)
    respawnTimer--;
}

bool
BrokenBlock::respawn()
{
  return respawnTimer == 0;
}

int
BrokenBlock::getX()
{
  return x;
}

int
BrokenBlock::getY()
{
  return y;
}

void
BrokenBlock::resetRespawnTimer(int value)
{
  respawnTimer = value;
}
