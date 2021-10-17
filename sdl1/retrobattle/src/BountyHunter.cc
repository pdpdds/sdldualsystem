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

#include "BountyHunter.h"
#include "HunterBullet.h"

BountyHunter::BountyHunter(remar2d *gfx, SoundManager *sfx,
			   ScoreKeeper *scoreKeeper, list<Enemy *> *enemies,
			   list<Object *> *objects)
  : Enemy(gfx, "hunter", sfx, scoreKeeper), hitPoints(4), dead(false),
    pauseTimer(0), objects(objects), enemies(enemies)
{
  setAnimation("left");
  setVisible(false);
  moveAbs(-32, -32);
  setBoundingBox(24, 24, 0, 0);

  frameCounter = 3*60;
  state = HIDDEN;
}

void
BountyHunter::update(Field *field, Hero *hero)
{
  if(dead)
    {
      destroyMe = true;
      return;
    }

  if(pauseTimer)
    {
      pauseTimer--;

      if(pauseTimer == 0)
	{
	  pauseAnimation(false);
	}

      return;
    }

  if(hero->isBlinking() && state == HIDDEN)
    return;

  if(collides(hero) && state == PREFIRE
     && !hero->isBlinking() && !hero->isDead())
    {
      hero->die();

      pauseTimer = 60;

      pauseAnimation(true);
    }

  switch(state)
    {
    case HIDDEN:
      if(frameCounter == 0)
	{
	  setVisible(true);
	  frameCounter = 60;
	  randomizeLocation(field, hero);
	  state = FADEIN;
	  updateAnimation();
	}
      break;

    case FADEIN:
      setFacing(hero);

      if(frameCounter == 0)
	{
	  frameCounter = 60;
	  state = PREFIRE;
	  updateAnimation();
	}
      break;

    case PREFIRE:
      setFacing(hero);

      if(frameCounter == 0)
	{
	  fire();
	  // Skip postfire
	  frameCounter = 0;
	  state = POSTFIRE;
	  updateAnimation();
	}
      break;

    case POSTFIRE:
      if(frameCounter == 0)
	{
	  frameCounter = 60;
	  state = FADEOUT;
	  updateAnimation();
	}
      break;

    case FADEOUT:
      if(frameCounter == 0)
	{
	  frameCounter = (rand()%3)*60+60; /* 1, 2, or 3 seconds */
	  state = HIDDEN;
	  setVisible(false);
	}
      break;
    }

  if(frameCounter)
    --frameCounter;
}

bool
BountyHunter::hit()
{
  if(state == PREFIRE)
    {
      if(--hitPoints == 0)
	die();
      return true;
    }

  return false;
}

void
BountyHunter::die()
{
  scoreKeeper->killed(ScoreKeeper::BountyHunter);

  dead = true;
}

void
BountyHunter::randomizeLocation(Field *field, Hero *hero)
{
  /* Randomize location */
  bool done = false;
  while(!done)
    {
      int x = (rand()%21)*32 + 96 + 4;
      int y = (rand()%13)*32 + 128 + 4;
      
      if(field->emptyBlock(x/32, y/32))
	{
	  moveAbs(x, y);

	  if(!collides(hero))
	    done = true;
	}
    }
}

void
BountyHunter::setFacing(Hero *hero)
{
  Direction oldFacing = facing;

  if((hero->getX()+7) < getX())
    facing = LEFT;
  else
    facing = RIGHT;

  if(facing != oldFacing)
    updateAnimation();
}

void
BountyHunter::updateAnimation()
{
  const char *facings[] = {"left", "right", "blink left", "blink right"};
  int anim = facing + ((state == FADEIN || state == FADEOUT) ? 2 : 0);
  setAnimation((char *)facings[anim]);
}

void
BountyHunter::fire()
{
  HunterBullet *b = new HunterBullet(gfx, sfx, objects);
  if(facing == LEFT)
    {
      b->moveAbs(getX(), getY()+12);
      b->moveLeft();
    }
  else
    {
      b->moveAbs(getX()+24, getY()+12);
      b->moveRight();      
    }

  enemies->push_back(b);
}
