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

#include "Level.h"
#include <fstream>
#include <iostream>
#include "PowerBullet.h"
#include "LaserBeam.h"
#include "GoldDrone.h"
#include "DireSpaceViper.h"
#include "DarkBountyHunter.h"
#include "ScoreSign.h"

int DEBUG = 0;

Level::Level(remar2d *gfx, SoundManager *sfx, Input *input,
	     ScoreKeeper *scoreKeeper, char *datadir)
  : GameMode(gfx, sfx, input, scoreKeeper), stageSign(0), startDelay(60)
{
  gfx->setupTileBackground(32, 32);

  paused = false;
  win = false;
  debugOutputTimer = 60;
  loadFailed = false;
  hero = 0;

  scoreKeeper->setTimer(200);
  timerTimer = 60;
  timerPaused = true;

  doWipe = false;
  wipeTimer = 70;
  wipeCounter = 0;

  hud = new HUD(gfx, scoreKeeper);
  bulletHandler = new BulletHandler(hud);
  helmet = new Helmet(gfx, sfx);
  
  scoreKeeper->resetKills();

  field = new Field(gfx, sfx, &brokenBlocks, &objects,
		    scoreKeeper->blocksTakeTwoHits(),
		    scoreKeeper->getSkillLevel(),
		    false /* normal blue background */,
		    datadir);

  char buf[1024];
  sprintf(buf, "%s/data/levels/%d.lev", datadir, scoreKeeper->getLevel());

  // TODO: Use exceptions instead of this error handling
  bool success = loadLevel(buf);

  if(!success)
    {
      printf("OMG! Failed to load level %d!\n", scoreKeeper->getLevel());
      loadFailed = true;
    }

  bulletHandler->setHero(hero);

  spawner = new Spawner(&objects,
			scoreKeeper->redFuzzes(),
			scoreKeeper->numberOfEnemy(ScoreKeeper::Fuzz));

  for(int i = 0;i < scoreKeeper->numberOfEnemy(ScoreKeeper::Drone);i++)
    enemies.push_back(new Drone(gfx, sfx, scoreKeeper));

  for(int i = 0;i < scoreKeeper->numberOfEnemy(ScoreKeeper::GoldDrone);i++)
    enemies.push_back(new GoldDrone(gfx, sfx, scoreKeeper));

  if(int length = scoreKeeper->numberOfEnemy(ScoreKeeper::SpaceViper))
    {
      SpaceViper *v = new SpaceViper(gfx, sfx, scoreKeeper, &enemies,
				     length*8);
      enemies.push_front(v);
    }

  if(int length = scoreKeeper->numberOfEnemy(ScoreKeeper::DireSpaceViper))
    {
      DireSpaceViper *v = new DireSpaceViper(gfx, sfx, scoreKeeper, &enemies,
					     length*8);
      enemies.push_front(v);
    }

  if(scoreKeeper->numberOfEnemy(ScoreKeeper::BountyHunter))
    {
      enemies.push_back(new BountyHunter(gfx, sfx, scoreKeeper, &enemies,
					 &objects));
    }

  if(scoreKeeper->numberOfEnemy(ScoreKeeper::DarkBountyHunter))
    {
      enemies.push_back(new DarkBountyHunter(gfx, sfx, scoreKeeper, &enemies,
					     &objects));
    }

  bonusSpawner = new BonusSpawner(gfx, field, &bonuses, &coins,
				  scoreKeeper->getSkillLevel() >= 4);
  
  sprintf(buf, "stage %d", scoreKeeper->getLevel());
  stageSign = gfx->print("text", buf);
  gfx->showSprite(stageSign, true);
  gfx->moveSpriteAbs(stageSign, (800-16*7)/2, (600-16)/2);

  showAllObjects(false);
}

Level::~Level()
{
  if(hud)
    delete hud;

  if(helmet)
    delete helmet;

  if(stageSign)
    {
      gfx->showSprite(stageSign, false);
      gfx->removeSpriteInstance(stageSign);
    }

  clearLevel();
}

bool
Level::loadLevel(char *lev)
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
	      {
		Nest *nest = new Nest(gfx, sfx, scoreKeeper, &enemies);
		nest->setVisible(true);
		nest->moveAbs(x*32+3, y*32+3*32+3);
		objects.push_back(nest);
	      }
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

  hero = new Hero(gfx, sfx, &bullets, bulletHandler, hud);
  hero->setVisible(true);
  hero->moveAbs(heroStartX, heroStartY);

  field->redrawAll();

  return true;
}

void
Level::clearLevel()
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
Level::respawn(int x, int y)
{
  Object block(gfx, "shot", sfx);
  block.setAnimation("normal");
  block.setBoundingBox(32, 32, 0, 0);
  block.moveAbs(x*32, y*32);

  /* Check for collision against Captain Good, enemies, and shots */
  if(block.collides(hero))
    return false;

  for(list<Enemy *>::iterator it = enemies.begin();it != enemies.end();it++)
    {
      if(block.collides((*it)))
	return false;
    }

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
Level::update()
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

      return GAME;
    }

  if(input->pressed(SDLK_ESCAPE))
    {
      if(paused)
	{
	  helmet->setVisible(false);
	}
      performWipe(MENU);
      return GAME;
    }

  if(startDelay)
    {
      if(--startDelay == 0)
	{
	  gfx->showSprite(stageSign, false);
	  showAllObjects(true);
	  sfx->playMusic(0);
	}

      return GAME;
    }

  if(win)
    {
      --winTimer;
      if(!(winTimer % 10))
	{
	  if((winTimer / 10) % 2)
	    {
	      /* Set bright blue background */
	      field->redrawAll(true);
	    }
	  else
	    {
	      /* Set normal background */
	      field->redrawAll(false);
	    }
	}

      /* When done, go to score tally screen */
      if(winTimer == 0)
	{
	  performWipe(SCORE);
	}

      return GAME;
    }

  if(input->pressed(SDLK_p)) pause();

  if(paused)
    return GAME;


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
	
	  hud->setValue(HUD::TIME, scoreKeeper->getTimer());

	  if(scoreKeeper->getTimer() == 0)
	    {
	      hero->die();
	      scoreKeeper->setTimer(100);
	      timerPaused = true;
	    }
	}
    }

  if(DEBUG && --debugOutputTimer == 0)
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
  for(objIt = objects.begin();objIt != objects.end();)
    {
      (*objIt)->update();
      if((*objIt)->destroy())
	{
	  delete (*objIt);
	  objIt = objects.erase(objIt);
	  continue;
	}
      objIt++;
    }

  bulletHandler->update();

  // DONE: Move movement code for Captain Good to Hero class
  hero->update(input, field);
  if(hero->destroy())
    {
      scoreKeeper->heroKilled();
      hud->setValue(HUD::LIVES, scoreKeeper->getLives());
      hud->setValue(HUD::TIME, scoreKeeper->getTimer());

      timerPaused = true;

      if(scoreKeeper->getLives() <= 0)
	{
	  /* GAME OVER */
	  performWipe(SCORE);
	  return GAME;
	}

      hero->reset();
      hero->moveAbs(heroStartX, heroStartY);
      bulletHandler->reset();
    }

  spawner->update();
  bonusSpawner->update(hero);

  for(list<Bonus *>::iterator it = bonuses.begin();it != bonuses.end();)
    {
      (*it)->update();
      if((*it)->destroy())
	{
	  delete (*it);
	  it = bonuses.erase(it);
	}
      else
	{
	  if((*it)->collides(hero) && !(*it)->getCollected())
	    {
	      (*it)->setCollected();

	      Bonus::BonusType type = (*it)->getType();
	      bool updateHUD = false;

	      switch(type)
		{
		case Bonus::SMALL_COINS:
		  sfx->playSound(17);
		  scoreKeeper->addScore(200);
		  objects.push_back(new ScoreSign(gfx, ScoreSign::SCORE_200, (*it)->getX(), (*it)->getY()));
		  updateHUD = true;
		  break;

		case Bonus::DIAMOND:
		  sfx->playSound(17);
		  scoreKeeper->addScore(250);
		  objects.push_back(new ScoreSign(gfx, ScoreSign::SCORE_250, (*it)->getX(), (*it)->getY()));
		  updateHUD = true;
		  break;

		case Bonus::GOLD_BAR:
		  sfx->playSound(17);
		  scoreKeeper->addScore(300);
		  objects.push_back(new ScoreSign(gfx, ScoreSign::SCORE_300, (*it)->getX(), (*it)->getY()));
		  updateHUD = true;
		  break;

		case Bonus::BLUE_PEARL:
		  sfx->playSound(17);
		  scoreKeeper->addScore(400);
		  objects.push_back(new ScoreSign(gfx, ScoreSign::SCORE_400, (*it)->getX(), (*it)->getY()));
		  updateHUD = true;
		  break;

		case Bonus::WHITE_BALL:
		  sfx->playSound(17);
		  bulletHandler->reset();
		  break;

		case Bonus::LOCK:
		  sfx->playSound(17);
		  spawner->lockNests();
		  break;

		case Bonus::POWER_GUN:
		  // inform Hero object that next shot is a power shot
		  hero->powerShot();
		  bulletHandler->addOne();

		  sfx->playSound(17);
		  break;

		case Bonus::LASER_GUN:
		  // inform Hero object that next shot is a laser shot
		  hero->laserShot();
		  bulletHandler->addOne();

		  sfx->playSound(17);
		  break;

		case Bonus::CLOCK:
		  {
		  int newtime = scoreKeeper->getTimer() + 50;
		  scoreKeeper->setTimer((newtime > 200 ? 200 : newtime));
		  hud->setValue(HUD::TIME, scoreKeeper->getTimer());
		  sfx->playSound(17);
		  }
		  break;

		case Bonus::BOMB:
		  for(list<Enemy *>::iterator nmy = enemies.begin();nmy != enemies.end();nmy++)
		    {
		      if(Fuzz *f = dynamic_cast<Fuzz *>(*nmy))
			{
			  f->stun();
			}
		      else
			{
			  (*nmy)->hit();
			  (*nmy)->hit();
			}
		    }

		  spawner->cancelSpawningNests();
		  sfx->playSound(0);

		  break;

		case Bonus::QUAKE:
		  // damage all undamaged blocks in the Field
		  for(int y = 0;y < 19;y++)
		    for(int x = 0;x < 25;x++)
		      {
			if(field->field[x][y] == Field::BREAKABLE)
			  {
			    field->field[x][y] = Field::DAMAGED;
			    field->drawBlockAndSurrounding(x, y);
			  }
		      }
		  sfx->playSound(18);

		  break;
		}

	      if(updateHUD)
		{
		  hud->setValue(HUD::SCORE, scoreKeeper->getScore());
		  hud->setValue(HUD::TOP, scoreKeeper->getTopScore());
		  hud->setValue(HUD::LIVES, scoreKeeper->getLives());
		}
	    }

	  it++;
	}
    }

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
	      
	      return GAME;
	    }
	}

      it++;
    }

  for(list<Bullet *>::iterator b = bullets.begin();b != bullets.end();)
    {
      /* check collision between bullet and various stuff... */
      int x = (*b)->getX(), y = (*b)->getY();
      int blockX, blockY;

      if(LaserBeam *lb = dynamic_cast<LaserBeam *>(*b))
	{
	  fireLaserBeam(lb->getX()/32, lb->getY()/32, lb->getDirection());

	  b = bullets.erase(b);
	  continue;
	}

      if(x < -5 || x > 799)
	{
	  delete (*b);
	  b = bullets.erase(b);
	  continue;
	}
      else if(field->objectCollidesWithBackground((*b), &blockX, &blockY))
	{
	  if(dynamic_cast<PowerBullet *>(*b))
	    {
	      powerBulletHit(blockX, blockY);
	    }
	  else
	    {
	      if(!field->blockHit(blockX, blockY))
		{
		  objects.push_back(new Explosion(gfx, sfx, x-9, y-9));
		}
	    }

	  sfx->playSound(3, false);

	  delete (*b);
	  b = bullets.erase(b);

	  continue;
	}

      bool bulletRemoved = false;
      bool removeBullet = false;

      for(list<Enemy *>::iterator it = enemies.begin();it != enemies.end();
	  it++)
	{
	  if((*b)->collides(*it))
	    {
	      if((*it)->hit())
		{
		  if(dynamic_cast<PowerBullet *>(*b))
		    {
		      powerBulletHit((*b)->getX()/32, (*b)->getY()/32);
		    }
		  else if(!removeBullet)
		    {
		      objects.push_back(new Explosion(gfx, sfx, x-9, y-9));
		      sfx->playSound(3, false);
		    }

		  removeBullet = true;

		  break;
		}
	    }
	}

      if(removeBullet)
	{
	  delete (*b);
	  b = bullets.erase(b);
	  bulletRemoved = true;
	}

      if(!bulletRemoved)
	(*b)->update();

      b++;
    }


  for(list<DamagingExplosion *>::iterator it = damagingExplosions.begin();
      it != damagingExplosions.end();)
    {
      (*it)->update();

      if((*it)->destroy())
	{
	  delete (*it);
	  it = damagingExplosions.erase(it);
	}
      else
	{
	  if((*it)->checkCollision())
	    {
	      // Check for collision with blocks
	      int blockX, blockY;
	      if(field->objectCollidesWithBackground((*it), &blockX, &blockY))
		{
		  field->blockHit(blockX, blockY);
		}
	      else
		{
		  // Check for collision against enemies
		  for(list<Enemy *>::iterator nmy = enemies.begin();
		      nmy != enemies.end(); nmy++)
		    {
		      if((*nmy)->collides(*it))
			{
			  // Two HP damage
			  (*nmy)->hit();
			  (*nmy)->hit();
			}
		    }
		}
	    }

	  it++;
	}
    }

  for(list<Enemy *>::iterator it = enemies.begin();it != enemies.end();)
    {
      (*it)->update(field, hero);
      if((*it)->destroy())
	{
	  if(dynamic_cast<Fuzz *>(*it))
	    {
	      /* If this is a Fuzz, make sure that Spawner respawns a
		 Fuzz later on */
	      spawner->addFuzz();
	    }
	  else if(dynamic_cast<BountyHunter *>(*it))
	    {
	      /* If this is a Bounty Hunter, create 3x3 explosions
		 that damages other enemies */
	      int hunter_x = (*it)->getX();
	      int hunter_y = (*it)->getY();

	      for(int y = -1;y < 2;y++)
		for(int x = -1;x < 2;x++)
		  {
		    damagingExplosions
		      .push_back(new DamagingExplosion(gfx, sfx, 
						       hunter_x+x*32,
						       hunter_y+y*32));
		  }
	    }

	  delete (*it);
	  it = enemies.erase(it);
	}
      else
	{
	  it++;
	}
    }

  return GAME;
}

bool
Level::collides(Object *obj1, Object *obj2)
{
  return obj1->collides(obj2);
}

void
Level::pause()
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
}

void
Level::showAllObjects(bool show)
{
  /* Show/hide all enemies */
  for(list<Enemy *>::iterator it = enemies.begin();it != enemies.end();it++)
    {
      (*it)->setVisible(show);
    }

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

  for(list<DamagingExplosion *>::iterator it = damagingExplosions.begin();
      it != damagingExplosions.end();it++)
    {
      (*it)->setVisible(show);
    }

  for(list<Bonus *>::iterator it = bonuses.begin();it != bonuses.end();it++)
    {
      (*it)->setVisible(show);
    }
}

void
Level::deleteAllObjects()
{
  /* Remove all enemies */
  for(list<Enemy *>::iterator it = enemies.begin();it != enemies.end();)
    {
      delete (*it);
      it = enemies.erase(it);
    }

  /* Remove other objects */
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

  for(list<DamagingExplosion *>::iterator it = damagingExplosions.begin();
      it != damagingExplosions.end();)
    {
      delete (*it);
      it = damagingExplosions.erase(it);
    }

  for(list<Bonus *>::iterator it = bonuses.begin();it != bonuses.end();)
    {
      delete (*it);
      it = bonuses.erase(it);
    }

  gfx->showSprite(stageSign, false);
}

void
Level::performWipe(Mode modeToReturn)
{
  sfx->stopMusic();

  deleteAllObjects();
  
  delete hud;
  hud = 0;

  returnMode = modeToReturn;
  doWipe = true;
}

void
Level::powerBulletHit(int blockX, int blockY)
{
  // Add 5x5 diamond of explosions
  //
  //     x
  //   x x x
  // x x X x x
  //   x x x
  //     x
  
  int x_off[] = {0, -1, 0, 1, -2, -1, 0, 1, 2, -1, 0, 1, 0};
  int y_off[] = {-2, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 1, 2};
  
  for(int i = 0;i < 13;i++)
    {
      damagingExplosions
	.push_back(new DamagingExplosion(gfx, sfx, 
					 (blockX+x_off[i])*32+4,
					 (blockY+y_off[i])*32+4));
    }
}

void
Level::fireLaserBeam(int x, int y, Bullet::Direction direction)
{
  int endX, increment;
  
  if(direction == Bullet::LEFT)
    {
      endX = -1;
      increment = -1;
    }
  else
    {
      endX = 25;
      increment = 1;
    }
  
  for(int i = x;i != endX;i += increment)
    {
      damagingExplosions.push_back(new DamagingExplosion(gfx, sfx,
							 i*32+4,
							 y*32+4));
      
    }
}
