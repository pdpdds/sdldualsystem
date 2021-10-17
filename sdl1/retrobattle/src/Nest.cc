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

#include "Nest.h"
#include "RedFuzz.h"

Nest::Nest(remar2d *gfx, SoundManager *sfx, ScoreKeeper *scoreKeeper,
	   list<Enemy *> *enemies)
  : Object(gfx, "nest", sfx), scoreKeeper(scoreKeeper), spawnTimer(0)
{
  this->enemies = enemies;
  this->fastFuzzes = scoreKeeper->fastFuzzes();
  this->redFuzzes = scoreKeeper->redFuzzes();

  setAnimation("normal");

  blink(false);
}

void
Nest::update()
{
  if(spawnTimer > 0)
    {
      --spawnTimer;
      if(spawnTimer == 0)
	{
	  Fuzz *fuzz;

	  if(!redFuzzes)
	    fuzz = new Fuzz(gfx, sfx, scoreKeeper);
	  else
	    fuzz = new RedFuzz(gfx, sfx, scoreKeeper);

	  fuzz->setVisible(true);
	  fuzz->moveAbs(getX(), getY());
	  fuzz->rollRandom();  

	  if(fastFuzzes)
	    fuzz->isFast();

	  enemies->push_back(fuzz);

	  sfx->playSound(7, false);  	  
	  blink(false);
	}
    }
}

void
Nest::blink(bool on)
{
  if(on)
    setAnimation("blink");
  else
    setAnimation("normal");
}

void
Nest::spawn()
{
  spawnTimer = 60;
  blink(true);
}

bool
Nest::isSpawning()
{
  return spawnTimer > 0;
}

void
Nest::lock(bool l)
{
  if(l)
    setAnimation("locked");
  else
    setAnimation("normal");
}

void
Nest::cancelSpawning()
{
  spawnTimer = 0;
  setAnimation("normal");
}
