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

#include "SpaceViper.h"

SpaceViper::SpaceViper(remar2d *gfx, SoundManager *sfx,
		       ScoreKeeper *scoreKeeper, list<Enemy *> *enemies,
		       int length)
  : Enemy(gfx, "snake head", sfx, scoreKeeper), enemies(enemies),
    moveDirection(UP), hitPoints(6), dead(false), moved(0),
    pauseTimer(0),  aimAtHero(false)
{
  setBoundingBox(19, 19, 6, 6);

  for(int i = 0;i < length;i++)
    {
      SpaceViperBody *b = new SpaceViperBody(gfx, sfx, i%10 < 2, this);
      body.push_back(b);
      enemies->push_back(b);
    }

  gfx->removeSpriteInstance(sprite_instance);
  sprite_instance = gfx->createSpriteInstance("snake head");

  setVisible(true);
  updateAnimation();

  /* Not initialized yet */
  moveAbs(-32, -32);

  // Block used for collision detection (when checking tail location)
  block = new Object(gfx, "shot", sfx);
  block->setAnimation("normal");
  block->setBoundingBox(32, 32, 0, 0);
}

SpaceViper::~SpaceViper()
{
  delete block;
}

void
SpaceViper::update(Field *field, Hero *hero)
{
  int move_x_table[] = {0, -2, 2, 0, 0};
  int move_y_table[] = {0, 0, 0, -2, 2};

  if(dead)
    {
      if(--destroyTimer == 0)
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

  if(getX() == -32)
    {
      /* Randomize location */
      bool done = false;
      while(!done)
	{
 	  int x = (rand()%21)*32 + 96;
	  int y = (rand()%13)*32 + 128;

	  if(field->emptyBlock(x/32, y/32))
	    {
	      moveAbs(x, y);
	      list<SpaceViperBody *>::iterator it = body.begin();
	      for(;it != body.end();it++)
		{
		  (*it)->moveAbs(x, y);
		}
	      done = true;
	    }
	}

      /* Force new direction */
      moved = 32;

      return;
    }

  if(collides(hero) && !hero->isBlinking() && !hero->isDead())
    {
      hero->die();

      pause();
    }

  if(moveThisFrame == false)
    {
      moveThisFrame = true;
      return;
    }

  if(moved == 4)
    {
      int blockX, blockY;
      blockX = (getX()+4) / 32 + move_x_table[moveDirection]/2;
      blockY = (getY()+4) / 32 + move_y_table[moveDirection]/2;

      if(field->field[blockX][blockY] == Field::BREAKABLE
	 || field->field[blockX][blockY] == Field::DAMAGED)
	{
	  field->breakBlock(blockX, blockY);
	  sfx->playSound(3);
	}
    }

  if(moved == 32)
    {
      /* First, check all directions for possible ways, if none exist,
	 stop and coil up tail. */

      int surroundingBlocks = 0;
      int surroundingTail = 0;

      int blockX, blockY;

      for(int i = 1;i < 5;i++)
	{
	  getBlockAtDirection((Direction)i, &blockX, &blockY);

	  if(!field->emptyBlock(blockX, blockY))
	    surroundingBlocks++;
	  else if(blockY == 17)
	    surroundingBlocks++;
	  if(tailAtPosition(blockX, blockY))
	    surroundingTail++;	 
	}

      if(surroundingBlocks == 4)
	{
	  /* We must DIG our way out! */
	  bool done = false;
	  int blockX, blockY;
	  int solidBlocks = 0;

	  // Sanity check
	  for(int i = 1;i < 5;i++)
	    {
	      getBlockAtDirection((Direction)i, &blockX, &blockY);

	      if(field->field[blockX][blockY] == Field::SOLID)
		solidBlocks++;
	    }

	  if(solidBlocks == 4)
	    {
	      // Uh oh, Space Viper is reaaaaaally stuck. Randomize
	      // new location and wish for the best
	      moveAbs(-32, -32);
	    }

	  while(!done)
	    {
	      moveDirection = (Direction)(rand()%4 + 1);

	      getBlockAtDirection(moveDirection, &blockX, &blockY);

	      if(field->field[blockX][blockY] == Field::BREAKABLE
		 || field->field[blockX][blockY] == Field::DAMAGED)
		{
		  done = true;
		}
	    }

	  updateAnimation();
	  moved = 0;
	}
      else if(surroundingBlocks + surroundingTail >= 4)
	{
	  /* Wait for tail to coil up */
	  moveDirection = NONE;
	}
      else
	{
	  // Choose direction

	  bool done = false;
	  int blockX, blockY;

	  if(aimAtHero && !(hero->isBlinking()))
	    {
	      bool tryXDirection = (bool)(rand() % 2);

	      for(int i = 0;i < 2;i++)
		{
		  if(tryXDirection)
		    {
		      moveDirection = getX() > hero->getX() ? LEFT : RIGHT;
		    }
		  else
		    {
		      moveDirection = getY() > hero->getY() ? UP : DOWN;
		    }

		  getBlockAtDirection(moveDirection, &blockX, &blockY);

		  if(blockY < field->SPIKES_LEVEL/32
		     && field->emptyBlock(blockX, blockY)
		     && !tailAtPosition(blockX, blockY))
		    {
		      done = true;
		      break;
		    }

		  tryXDirection = !tryXDirection;
		}
	    }

	  while(!done)
	    {
	      moveDirection = (Direction)(rand()%4 + 1);

	      getBlockAtDirection(moveDirection, &blockX, &blockY);

	      if(blockY == field->SPIKES_LEVEL/32)
		{
		  continue;
		}

	      if(field->emptyBlock(blockX, blockY)
		 && !tailAtPosition(blockX, blockY))
		done = true;
	    }

	  updateAnimation();
	  moved = 0;
	}
    }

  int move_x = move_x_table[moveDirection];
  int move_y = move_y_table[moveDirection];  

  moveRel(move_x, move_y);

  int old_x, old_y;
  int new_x, new_y;

  new_x = getX();
  new_y = getY();
      
  list<SpaceViperBody *>::iterator it = body.begin();
  for(;it != body.end();it++)
    {
      old_x = (*it)->getX();
      old_y = (*it)->getY();
      
      (*it)->moveAbs(new_x, new_y);
      
      new_x = old_x;
      new_y = old_y;
    }

  if(moveDirection != NONE)
    moved+=2;

  if(getX() <= -32)
    {
      moveAbs(798, getY());
      moved = 2;
    }
  if(getX() >= 800)
    {
      moveAbs(-30, getY());
      moved = 2;
    }

  moveThisFrame = false;
}

bool
SpaceViper::hit()
{
  if(hitPoints > 0 && --hitPoints == 0)
    {
      scoreKeeper->killed(ScoreKeeper::SpaceViper);

      dead = true;
      destroyTimer = 60;

      pauseAnimation(true);

      list<SpaceViperBody *>::iterator it = body.begin();
      for(;it != body.end();it++)
	(*it)->splat();
    }

  return true;
}

void
SpaceViper::pause()
{
  pauseTimer = 60;

  pauseAnimation(true);
}

void
SpaceViper::updateAnimation()
{
  const char *dirStrings[] = {"up", "left", "right", "up", "down"};

  setAnimation((char *)dirStrings[moveDirection]);
}

bool
SpaceViper::tailAtPosition(int blockX, int blockY)
{
  /* Wrap checks */
  if(blockX == -1)
    blockX = 24;
  if(blockX == 25)
    blockX = 0;

  block->moveAbs(blockX*32, blockY*32);

  list<SpaceViperBody *>::iterator it = body.begin();
  for(;it != body.end();it++)
    {
      if(block->collides(*it))
	return true;
    }

  return false;
}

void
SpaceViper::getBlockAtDirection(Direction direction, int *blockX, int *blockY)
{
  switch(direction)
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

