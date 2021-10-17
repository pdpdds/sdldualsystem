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

#include "BonusLevel.h"
#include <fstream>
#include <iostream>

BonusLevel::BonusLevel(remar2d *gfx, SoundManager *sfx, Input *input,
		       ScoreKeeper *scoreKeeper, char *datadir)
  : GameMode(gfx, sfx, input, scoreKeeper)
{
  paused = false;
  win = false;
  debugOutputTimer = 60;
  loadFailed = false;
  hero = 0;

  scoreKeeper->setTimer(80);
  timerTimer = 60;
  timerPaused = true;

  doWipe = false;
  wipeTimer = 70;
  wipeCounter = 0;

  gfx->setupTileBackground(32, 32);

  scoreKeeper->resetKills();

  field = new Field(gfx, sfx, &brokenBlocks, &objects,
		    scoreKeeper->blocksTakeTwoHits(),
		    scoreKeeper->getSkillLevel(), true,
		    datadir);

  char buf[1024];
  sprintf(buf, "%s/data/levels/b%d.lev", datadir, scoreKeeper->getBonusLevel());

  // TODO: Use exceptions instead of this error handling
  bool success = loadLevel(buf);

  if(!success)
    {
      printf("OMG! Failed to load bonus level %d!\n",
	     scoreKeeper->getBonusLevel());
      loadFailed = true;
    }

  sfx->playMusic(2);

  hud = gfx->createSpriteInstance("timered");
  gfx->setAnimation(hud, "normal");
  gfx->showSprite(hud, true);
  gfx->moveSpriteAbs(hud, 11*32, 1*32);

  timerCounter = new Counter(gfx, 2);
  timerCounter->setPosition(11*32+42, 1*32+12);
  timerCounter->setCounter(scoreKeeper->getTimer());

  helmet = new Helmet(gfx, sfx);
}

BonusLevel::~BonusLevel()
{
  clearLevel();

  if(hud)
    gfx->removeSpriteInstance(hud);

  if(timerCounter)
    delete timerCounter;
}

bool
BonusLevel::loadLevel(char *lev)
{
  /*
    
  0: Empty
  1: Breakable
  2: Solid
  3: Captain Good
  4: Fuzz Nest
  5: Golden Coin

  */

  clearLevel();
  scoreKeeper->resetCoins();

  std::ifstream file(lev);
  if(!file)
    {
      /* Failed to open file */
      return false;
    }

  int x = 0, y = 0;
  int val;

  while(!file.eof())
    {
      file >> val;

      if(x >= 0 && x <= 24 && y >= 0 && y <= 14)
	{
	  int map[] = { Field::EMPTY, Field::BREAKABLE, Field::SOLID };
	  switch(val)
	    {
	    case 0: case 1: case 2:
	      field->field[x][y+3] = (Field::BlockType)map[val];
	      break;

	    case 3:
	      heroStartX = x*32 + 8;
	      heroStartY = y*32 + 3*32 + 8;
	      break;

	    case 4:
	      printf("This shouldn't happen...\n");
	      break;

	    case 5:
	      {
		  Coin *coin = new Coin(gfx, sfx);
		  coin->setVisible(true);
		  coin->moveAbs(x*32+8, (y+3)*32 + 4);
		  coins.push_back(coin);

		  scoreKeeper->addCoin();
	      }
	      break;
	    }
	}
      
      x++;
      if(x >= 25)
	{
	  x = 0; y++;
	}
    }

  hero = new Hero(gfx, sfx, &bullets, 0, 0);
  hero->setVisible(true);
  hero->moveAbs(heroStartX, heroStartY);

  field->redrawAll();

  return true;
}

void
BonusLevel::clearLevel()
{
  for(int y = 0;y < 19;y++)
    for(int x = 0;x < 25;x++)
      field->field[x][y] = Field::EMPTY;
  
  for(int i = 0;i < 25;i++)
    {
      field->field[i][0] = field->field[i][1] = field->field[i][2]
	= field->field[i][18] = Field::SOLID;
    }

  for(int i = 3; i < 18;i++)
    {
      field->field[0][i] = field->field[1][i] = field->field[23][i]
	= field->field[24][i] = Field::SOLID;
    }

  deleteAllObjects();
}

bool
BonusLevel::respawn(int x, int y)
{
  Object block(gfx, "shot", sfx);
  block.setAnimation("normal");
  block.setBoundingBox(32, 32, 0, 0);
  block.moveAbs(x*32, y*32);

  /* Check for collision against Captain Good and bullets */
  if(block.collides(hero))
    return false;

  for(list<Bullet *>::iterator it = bullets.begin();it != bullets.end();it++)
    {
      if(block.collides(*it))
	return false;
    }

  field->field[x][y] = Field::BREAKABLE;
  field->drawBlockAndSurrounding(x, y);

  objects.push_back(new Smoke(gfx, sfx, x*32, y*32));
  sfx->playSound(12);

  return true;
}

Mode
BonusLevel::update()
{
  if(loadFailed)
    return QUIT;

  if(doWipe)
    {
      --wipeTimer;

      if(wipeTimer == 0)
	{
	  return returnMode;
	}

       if(wipeTimer % 2 && wipeCounter < 19)
 	{
	  for(int x = 0;x < 25;x++)
	    {
	      gfx->setTile(x, wipeCounter, 0, 0, 0);
	    }

	  wipeCounter++;
 	}

      return BONUS;
    }

  if(input->pressed(SDLK_ESCAPE))
    {
      performWipe(MENU);
      return BONUS;
    }

  if(win)
    {
      --winTimer;
      if(!(winTimer % 10))
	{
	  if((winTimer / 10) % 2)
	    {
	      /* Set bright red background */
	      field->redrawAll(true);
	    }
	  else
	    {
	      /* Set normal red background */
	      field->redrawAll(false);
	    }
	}

      /* When done, go to score tally screen */
      if(winTimer == 0)
	{
	  performWipe(BONUSSCORE);
	}

      return BONUS;
    }

  if(input->pressed(SDLK_p)) pause();

  if(paused)
    return BONUS;


  if(hero->isDead())
    {
      timerPaused = true;
    }

  if(timerPaused && !hero->isBlinking() && !hero->isDead())
    {
      timerPaused = false;
    }

  if(!timerPaused && --timerTimer == 0)
    {
      timerTimer = 60;

      if(scoreKeeper->getTimer() > 0)
	{
	  scoreKeeper->decTimer();
	  timerCounter->setCounter(scoreKeeper->getTimer());

	  if(scoreKeeper->getTimer() == 0)
	    {
	      performWipe(BONUSSCORE);
	      return BONUS;
	    }
	}
    }

  if(0 && --debugOutputTimer == 0)
    {
      printf("----------------------------------------\n");
      printf("Broken blocks\n");

      debugOutputTimer = 60;

      list<BrokenBlock *>::iterator it = brokenBlocks.begin();
      for(;it != brokenBlocks.end();it++)
	{
	  BrokenBlock *b = *it;
	  printf("%d, %d, respawn: %d\n",
		 b->getX(), b->getY(), b->respawn());
	  
	}
    }

  list<BrokenBlock *>::iterator it;
  for(it = brokenBlocks.begin();it != brokenBlocks.end();it++)
    {
      (*it)->update();
      if((*it)->respawn())
	{
	  int x = (*it)->getX();
	  int y = (*it)->getY();

	  if(respawn(x, y))
	    {
	      delete (*it);
	      it = brokenBlocks.erase(it);
	    }
	  else
	    {
	      (*it)->resetRespawnTimer(2*60);
	    }
	}
    }

  list<Object *>::iterator objIt;
  for(objIt = objects.begin();objIt != objects.end();objIt++)
    {
      (*objIt)->update();
      if((*objIt)->destroy())
	{
	  delete (*objIt);
	  objIt = objects.erase(objIt);
	}
    }

  hero->update(input, field);
  if(hero->destroy())
    {
      scoreKeeper->heroKilled();

      timerPaused = true;

      if(scoreKeeper->getLives() <= 0)
	{
	  /* GAME OVER */
	  performWipe(BONUSSCORE);
	  return BONUS;
	}

      delete hero;
      hero = new Hero(gfx, sfx, &bullets, 0, 0);
      hero->setVisible(true);
      hero->moveAbs(heroStartX, heroStartY);      
    }

  // TODO: Move movement code for Captain Good to Hero class

//   int move_x = 0;
//   int move_y = 1; /* Constantly fall.. :-) */

//   if(input->held(SDLK_LEFT))   move_x--;
//   if(input->held(SDLK_RIGHT))  move_x++;
//   if(input->pressed(SDLK_UP))  hero->jump(true);
//   if(input->released(SDLK_UP)) hero->jump(false);
//   if(input->pressed(SDLK_z))
//     hero->shoot();
//   if(input->pressed(SDLK_d))   hero->die();

//   if(hero->jumps(1))
//     move_y = -1;

//   int heroX = hero->getX();
//   int heroY = hero->getY();

//   if(heroY + 24 == field->SPIKES_LEVEL)
//     hero->die();

//   if(heroX < -25)
//     {
//       hero->moveAbs(801, heroY);
//     }
//   else if(heroX > 802)
//     {
//       hero->moveAbs(-24, heroY);
//     }
//   else
//     {
//       field->moveObjectRel(hero, &move_x, &move_y);
//       hero->moveRel(move_x, move_y);
//     }

  for(list<Coin *>::iterator it = coins.begin();it != coins.end();)
    {
      (*it)->update();
      if((*it)->destroy())
	{
	  delete (*it);
	  it = coins.erase(it);

	  continue;
	}

      if((*it)->collides(hero) && (*it)->collect())
	{
	  scoreKeeper->collectCoin();

	  if(scoreKeeper->coinsLeft() == 0)
	    {
	      win = true;
	      winTimer = 3*60; /* Blink background for 3 seconds */
	      
	      sfx->playMusic(1, false);
	      deleteAllObjects();
	      
	      return BONUS;
	    }
	}

      it++;
    }

  for(list<Bullet *>::iterator it = bullets.begin();it != bullets.end();)
    {
      /* check collision between bullet and various stuff... */
      int x = (*it)->getX(), y = (*it)->getY();
      int blockX, blockY;
      if(x < -5 || x > 799)
	{
	  delete (*it);
	  it = bullets.erase(it);

	  continue;
	}
      else if(field->objectCollidesWithBackground((*it), &blockX, &blockY))
	{
	  if(!field->blockHit(blockX, blockY))
	    {
	      objects.push_back(new Explosion(gfx, sfx, x-9, y-9));
	    }

	  delete (*it);
	  it = bullets.erase(it);
	  
	  sfx->playSound(3, false);
	  
	  continue;
	}

      (*it)->update();

      it++;
    }

  return BONUS;
}

bool
BonusLevel::collides(Object *obj1, Object *obj2)
{
  return obj1->collides(obj2);
}

void
BonusLevel::pause()
{
  if(paused)
    {
      paused = false;
      showAllObjects(true);
      sfx->playSound(13);

      helmet->setVisible(false);
    }
  else
    {
      paused = true;
      showAllObjects(false);
      sfx->playSound(13);

      int helmet_x = hero->getX();
      int helmet_y = hero->getY();

      if(helmet_x < 1)
	helmet_x = 1;
      else if(helmet_x > 800-20)
	helmet_x = 800-20;

      helmet->moveAbs(helmet_x-1, helmet_y);
      helmet->setVisible(true);
    }

  gfx->pauseAnimations(paused);
}

void
BonusLevel::showAllObjects(bool show)
{
  /* Show/hide other objects */
  for(list<Object *>::iterator it = objects.begin();it != objects.end();it++)
    {
      (*it)->setVisible(show);
    }

  for(list<Coin *>::iterator it = coins.begin();it != coins.end();it++)
    {
      (*it)->setVisible(show);
    }

  hero->setVisible(show);

  for(list<Bullet *>::iterator it = bullets.begin();it != bullets.end();it++)
    {
      (*it)->setVisible(show);
    }
}

void
BonusLevel::deleteAllObjects()
{
  /* Remove objects */
  for(list<Object *>::iterator it = objects.begin();it != objects.end();)
    {
      delete (*it);
      it = objects.erase(it);
    }

  for(list<Coin *>::iterator it = coins.begin();it != coins.end();)
    {
      delete (*it);
      it = coins.erase(it);
    }

  if(hero)
    {
      delete hero;
      hero = 0;
    }

  for(list<Bullet *>::iterator it = bullets.begin();it != bullets.end();)
    {
      delete (*it);
      it = bullets.erase(it);
    }
}

void
BonusLevel::performWipe(Mode modeToReturn)
{
  sfx->stopMusic();

  deleteAllObjects();

  gfx->removeSpriteInstance(hud);
  hud = 0;

  delete timerCounter;
  timerCounter = 0;

  returnMode = modeToReturn;
  doWipe = true;
}
