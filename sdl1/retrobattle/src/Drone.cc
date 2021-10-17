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

#include "Drone.h"

Drone::Drone(remar2d *gfx, SoundManager *sfx, ScoreKeeper *scoreKeeper)
  : Enemy(gfx, "drone", sfx, scoreKeeper), aimAtHero(false), moved(0), 
    moveThisUpdate(true), hitPoints(2), dead(false), pauseTimer(0)
{
  setAnimation("normal");
  setVisible(true);
  moveAbs(-32, -32);

  setBoundingBox(24, 24, 0, 0);
}

void
Drone::getBlockInDirection(Direction dir, int *blockX, int *blockY)
{

  switch(dir)
    {
    case LEFT:
      *blockX = getX() / 32 - 1;
      *blockY = getY() / 32;
      break;

    case RIGHT:
      *blockX = getX() / 32 + 1;
      *blockY = getY() / 32;
      break;

    case UP:
      *blockX = getX() / 32;
      *blockY = getY() / 32 - 1;
      break;

    case DOWN:
      *blockX = getX() / 32;
      *blockY = getY() / 32 + 1;
      break;

    default:
      break;
    }
}

void
Drone::update(Field *field, Hero *hero)
{
  if(dead)
    {
      if(--deathTimer == 0)
	{
	  destroyMe = true;
	}

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

  if(collides(hero) && !hero->isBlinking() && !hero->isDead())
    {
      hero->die();

      pauseTimer = 60;

      pauseAnimation(true);
    }

  if(!moveThisUpdate)
    {
      moveThisUpdate = true;
      return;
    }

  if(getX() == -32)
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
	      done = true;
	    }
	}

      /* Force recalibration of move direction, sir! */
      moved = 32;
    }

  int move_x_table[] = {0, -2, 2, 0, 0};
  int move_y_table[] = {0, 0, 0, -2, 2};

  if(moved == 4 && willDig)
    {
      int blockX, blockY;
      blockX = getX() / 32 + move_x_table[moveDirection]/2;
      blockY = getY() / 32 + move_y_table[moveDirection]/2;

      field->breakBlock(blockX, blockY);
      sfx->playSound(3);
    }
  else if(moved == 32)
    {
      Direction newDirection;
      bool done = false;
      willDig = false;

      if(aimAtHero && !(hero->isBlinking()))
	{
	  bool tryXDirection = (bool)(rand() % 2);
	  int blockX, blockY;

	  for(int i = 0;i < 2;i++)
	    {
	      if(tryXDirection)
		{
		  newDirection = getX() > hero->getX() ? LEFT : RIGHT;
		}
	      else
		{
		  newDirection = getY() > hero->getY() ? UP : DOWN;
		}

	      getBlockInDirection(newDirection, &blockX, &blockY);

	      if(blockY < field->SPIKES_LEVEL/32
		 && field->field[blockX][blockY] != Field::SOLID)
		{
		  done = true;

		  if(field->field[blockX][blockY] == Field::BREAKABLE
		      || field->field[blockX][blockY] == Field::DAMAGED)
		    {
		      willDig = true;
		    }

		  break;
		}

	      tryXDirection = !tryXDirection;
	    }
	}

      while(!done)
	{
	  newDirection = (Direction)(rand()%4 + 1);
	  bool dig = ((rand()%100) > 91) && !(hero->isBlinking());
	  int blockX, blockY;

	  getBlockInDirection(newDirection, &blockX, &blockY);

	  if(blockY == field->SPIKES_LEVEL/32)
	    {
	      continue;
	    }

	  if(field->emptyBlock(blockX, blockY))
	    done = true;
	  else if((field->field[blockX][blockY] == Field::BREAKABLE
		   || field->field[blockX][blockY] == Field::DAMAGED)
		  && dig)
	    {
	      done = true;
	      willDig = true;
	    }
	}

      moveDirection = newDirection;
      moved = 0;
    }

  int move_x = move_x_table[moveDirection];
  int move_y = move_y_table[moveDirection];

  moveRel(move_x, move_y);
  moved+=2;

  if(getX() <= -25)
    {
      moveAbs(800, getY());
      moved = 4;
    }
  if(getX() >= 801)
    {
      moveAbs(-24, getY());
      moved = 4;
    }

  moveThisUpdate = false;
}

bool
Drone::hit()
{
  if(dead)
    return false;

  if(hitPoints > 0)
    {
      hitPoints--;
      if(hitPoints == 0)
	{
	  die();
	}
    }
  return true;
}

void
Drone::die()
{
  scoreKeeper->killed(ScoreKeeper::Drone);

  setAnimation("crash");
  deathTimer = 60;

  dead = true;
}
