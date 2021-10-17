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

#include "ScoreScreen.h"

ScoreScreen::ScoreScreen(remar2d *gfx, SoundManager *sfx, Input *input,
			 ScoreKeeper *scoreKeeper)
  : GameMode(gfx, sfx, input, scoreKeeper),
    state(TIME),
    subState(SHOW),
    delayTimer(tickDelayObjects),
    time(0),
    coinsCounted(0),
    fuzzesCounted(0),
    dronesCounted(0),
    viperSprite(0),
    vipers(0),
    vipersCounted(0),
    hunterSprite(0),
    hunters(0),
    huntersCounted(0),
    noBonus(0)
{
  gfx->setupTileBackground(32, 32);

  drawBackground();

  top = gfx->print("text", "top");
  gfx->showSprite(top, true);
  gfx->moveSpriteAbs(top, 9*32, 4*32 + 8);

  score = gfx->print("text", "score");
  gfx->showSprite(score, true);
  gfx->moveSpriteAbs(score, 8*32 + 16, 6*32 + 8);

  stage = gfx->print("text", "stage");
  gfx->showSprite(stage, true);
  gfx->moveSpriteAbs(stage, 10*32+16, 8*32+8);

  level = new Counter(gfx, 2);
  level->setPosition(13*32+16, 8*32+8);
  level->setCounter(scoreKeeper->getLevel());

  topScoreCounter = new Counter(gfx, 8);
  topScoreCounter->setPosition(12*32+16, 4*32 + 8);
  topScoreCounter->setCounter(scoreKeeper->getTopScore());

  scoreCounter = new Counter(gfx, 8);
  scoreCounter->setPosition(12*32+16, 6*32 + 8);
  scoreCounter->setCounter(scoreKeeper->getScore());

  clock = gfx->createSpriteInstance("clock");
  gfx->setAnimation(clock, "normal");
  gfx->showSprite(clock, false);
  gfx->moveSpriteAbs(clock, 10*32+16, 9*32+8);
  
  scorePerTime = gfx->print("text", "20#");
  gfx->showSprite(scorePerTime, false);
  gfx->moveSpriteAbs(scorePerTime, 11*32+16, 9*32+8);

  coinSprite = gfx->createSpriteInstance("coin");
  gfx->setAnimation(coinSprite, "blink");
  gfx->showSprite(coinSprite, false);
  gfx->pauseAnimation(coinSprite, true);
  gfx->moveSpriteAbs(coinSprite, 10*32 + 16, 10*32+4);

  char buf[5];
  snprintf(buf, 5, "%d#", 100 * scoreKeeper->getSkillLevel());

  scorePerCoin = gfx->print("text", buf);
  gfx->showSprite(scorePerCoin, false);
  gfx->moveSpriteAbs(scorePerCoin, 11*32+16, 10*32+8);


  const char *fuzz = scoreKeeper->getSkillLevel() > 4 ? "red fuzz" : "fuzz";

  fuzzSprite = gfx->createSpriteInstance(fuzz);
  gfx->setAnimation(fuzzSprite, "roll left");
  gfx->showSprite(fuzzSprite, false);
  gfx->pauseAnimation(fuzzSprite, true);
  gfx->moveSpriteAbs(fuzzSprite, 10*32+16, 11*32+4);

  scorePerFuzz = gfx->print("text", "120#");
  gfx->showSprite(scorePerFuzz, false);
  gfx->moveSpriteAbs(scorePerFuzz, 11*32+16, 11*32+8);

  const char *drone = scoreKeeper->getSkillLevel() > 5 ? "gold drone" : "drone";

  droneSprite = gfx->createSpriteInstance(drone);
  gfx->setAnimation(droneSprite, "normal");
  gfx->showSprite(droneSprite, false);
  gfx->pauseAnimation(droneSprite, true);
  gfx->moveSpriteAbs(droneSprite, 10*32+16, 12*32+4);

  scorePerDrone = gfx->print("text", "200#");
  gfx->showSprite(scorePerDrone, false);
  gfx->moveSpriteAbs(scorePerDrone, 11*32+16, 12*32+8);

  const char *viper = scoreKeeper->getSkillLevel() > 6 ? "dire snake head" : "snake head";

  viperSprite = gfx->createSpriteInstance(viper);
  gfx->setAnimation(viperSprite, "right");
  gfx->showSprite(viperSprite, false);
  gfx->pauseAnimation(viperSprite, true);
  gfx->moveSpriteAbs(viperSprite, 10*32+8, 13*32);

  scorePerViper = gfx->print("text", "1200#");
  gfx->showSprite(scorePerViper, false);
  gfx->moveSpriteAbs(scorePerViper, 11*32+16, 13*32+8);

  const char *hunter = scoreKeeper->getSkillLevel() > 7 ? "dark hunter" : "hunter";

  hunterSprite = gfx->createSpriteInstance(hunter);
  gfx->setAnimation(hunterSprite, "right");
  gfx->showSprite(hunterSprite, false);
  gfx->pauseAnimation(hunterSprite, true);
  gfx->moveSpriteAbs(hunterSprite, 10*32+16, 14*32);

  scorePerHunter = gfx->print("text", "1000#");
  gfx->showSprite(scorePerHunter, false);
  gfx->moveSpriteAbs(scorePerHunter, 11*32+16, 14*32+8);

  if(scoreKeeper->getNoBonus())
    {
      noBonus = gfx->print("text", "no bonus");
      gfx->showSprite(noBonus, false);
      gfx->moveSpriteAbs(noBonus, 10*32+16, 9*32+8);
    }
}

ScoreScreen::~ScoreScreen()
{
  /* Delete counters */
  delete topScoreCounter;
  delete scoreCounter;
  delete level;
  delete time;
  delete coins;
  delete fuzzes;
  delete drones;
  if(vipers)  delete vipers;
  if(hunters) delete hunters;

  gfx->removeSpriteInstance(scorePerTime);
  gfx->removeSpriteInstance(scorePerCoin);
  gfx->removeSpriteInstance(scorePerFuzz);
  gfx->removeSpriteInstance(scorePerDrone);
  gfx->removeSpriteInstance(scorePerViper);
  gfx->removeSpriteInstance(scorePerHunter);
  gfx->removeSpriteInstance(top);
  gfx->removeSpriteInstance(score);
  gfx->removeSpriteInstance(stage);
  gfx->removeSpriteInstance(clock);
  gfx->removeSpriteInstance(coinSprite);
  gfx->removeSpriteInstance(fuzzSprite);
  gfx->removeSpriteInstance(droneSprite);
  gfx->removeSpriteInstance(viperSprite);
  gfx->removeSpriteInstance(hunterSprite);
  if(noBonus) gfx->removeSpriteInstance(noBonus);
}

void
ScoreScreen::drawBackground()
{
  for(int y = 0;y < 19;y++)
    for(int x = 0;x < 25;x++)
      {
	gfx->setTile(x, y, "background", 0, 0);
      }

  int offsetX = 7, offsetY = 3;

  int smallWindows[] =
    {11, 4, 4, 4, 4, 4, 4, 4, 4, 4, 12, 
      1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
     27, 26, 26, 26, 26, 26, 26, 26, 26, 26, 28,
     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
     9, 2, 29, 26, 26, 26, 26, 26, 30, 2, 10, };

  for(int y = 0;y < 5;y++)
    for(int x = 0;x < 11;x++)
      {
	if(smallWindows[y*11 + x])
	  {
	    gfx->setTile(x + offsetX, y + offsetY, "background",
			 smallWindows[y*11 + x], 0);
	  }
	else
	  {
	    gfx->setTile(x + offsetX, y + offsetY, 0, 0, 0);
	  }
      }

  offsetX = 9; offsetY = 8;

  for(int y = 0;y < 8;y++)
    for(int x = 0;x < 7;x++)
      {
	int i;
	if(y != 7)
	  {
	    if(x == 0)      i = 1;
	    else if(x == 6) i = 3;
	    else            i = -1;
	  }
	else
	  {
	    if(x == 0)      i = 9;
	    else if(x == 6) i = 10;
	    else            i = 2;
	  }

	if(i == -1)
	  gfx->setTile(x + offsetX, y + offsetY, 0, 0, 0);
	else
	  gfx->setTile(x + offsetX, y + offsetY, "background", i, 0);
      }
}

/*

SCRIPT

1. delay 1 second
2. show clock and time left
3. delay 1 second
4. tick down 1 second and add score
5. delay 1/4 second
6. if time != 0, goto 4
7. delay 1 second
8. show coin and coins collected
9. delay 1 second
10 tick down 1 coin and add score
11 delay 1/2 second
12 if coins != 8, goto 10
13 delay 1 second
14 show fuzz
15 delay 1 second
16 tick down 1 fuzz and add score
17 delay 1/2 second
18 if fuzz != fuzz killed, goto 16
19 ...


*/

Mode
ScoreScreen::update()
{
  if(input->held(SDLK_RETURN) && state != DONE)
    {
      if(delayTimer > 0)
	delayTimer = 0;

      if(subState == TICKDELAY)
	{
	  tickDelayTimer = 0;
	  subState = TICK;
	}	
    }

  if(input->pressed(SDLK_RETURN) && state == DONE)
    {
      if(subState == TICKDELAY)
	{
	  tickDelayTimer = 0;
	  subState = TICK;
	}
    }

  if(delayTimer > 0)
    {
      --delayTimer;
      return SCORE;
    }

  if(subState == TICKDELAY)
    {
      if(--tickDelayTimer == 0)
	{
	  subState = TICK;
	}
    }

//   enum State { TIME = 0, COINS, FUZZ, DRONES, VIPERS, HUNTERS };
//   enum SubState { SHOW, TICK, TICKDELAY };

//char *stateChar[] = {"TIME", "COINS", "FUZZ", "DRONES", "VIPERS", "HUNTERS"};
//char *subStateChar[] = {"SHOW", "TICK", "TICKDELAY"};

  switch(state)
    {
    case TIME:
      switch(subState)
	{
	case SHOW:
	  if(scoreKeeper->getNoBonus())
	    {
	      gfx->showSprite(noBonus, true);
	      scoreKeeper->setTimer(0);

	      // Reset "No Bonus"
	      scoreKeeper->setNoBonus(false);
	    }
	  else
	    {
	      gfx->showSprite(clock, true);
	      gfx->showSprite(scorePerTime, true);

	      time = new Counter(gfx, 3);
	      time->setPosition(13*32, 9*32+8);
	      time->setCounter(scoreKeeper->getTimer());
	    }

	  tickDelayTimer = tickDelayObjects;
	  subState = TICKDELAY;

	  break;
	  
	case TICK:
	  if(scoreKeeper->getTimer() == 0)
	    {
	      state = COINS;
	      subState = SHOW;
	      delayTimer = tickDelayObjects;
	    }
	  else
	    {
	      int t = scoreKeeper->getTimer();
	      if(t > 100)
		{
		  scoreKeeper->setTimer(t - 100);
		  scoreKeeper->addScore(100 * 20);
		}
	      else if(t > 10)
		{
		  scoreKeeper->setTimer(t - 10);
		  scoreKeeper->addScore(10 * 20);
		}
	      else
		{
		  scoreKeeper->setTimer(t - 1);
		  scoreKeeper->addScore(1 * 20);
		}

	      time->setCounter(scoreKeeper->getTimer());
	      
	      tickDelayTimer = tickDelayTime;

	      sfx->playSound(15);
	    }
	  break;

	case TICKDELAY:
	  break;
	}
      break;

    case COINS:
      switch(subState)
	{
	case SHOW:
	  gfx->showSprite(coinSprite, true);
	  gfx->showSprite(scorePerCoin, true);

	  coins = new Counter(gfx, 1);
	  coins->setPosition(14*32, 10*32+8);
	  coins->setCounter(0);

	  tickDelayTimer = tickDelayObjects;
	  subState = TICKDELAY;
	  break;

	case TICK:
	  if(coinsCounted == scoreKeeper->getCoinsCollected())
	    {
	      state = FUZZ;
	      subState = SHOW;
	      delayTimer = tickDelayObjects;
	    }
	  else
	    {
	      coinsCounted++;

	      coins->setCounter(coinsCounted);
	      scoreKeeper->addScore(100 * scoreKeeper->getSkillLevel());

	      tickDelayTimer = tickDelayCoins;

	      sfx->playSound(15);
	    }
	  break;

	case TICKDELAY:
	  break;
	}
      break;

    case FUZZ:
      switch(subState)
	{
	case SHOW:
	  gfx->showSprite(fuzzSprite, true);
	  gfx->showSprite(scorePerFuzz, true);

	  fuzzes = new Counter(gfx, 2);
	  fuzzes->setPosition(13*32+16, 11*32+8);
	  fuzzes->setCounter(0);

	  tickDelayTimer = tickDelayObjects;
	  subState = TICKDELAY;
	  break;

	case TICK:
	  if(fuzzesCounted == scoreKeeper->howManyKilled(ScoreKeeper::Fuzz))
	    {
	      state = DRONES;
	      subState = SHOW;
	      delayTimer = tickDelayObjects;
	    }
	  else
	    {
	      fuzzesCounted++;

	      fuzzes->setCounter(fuzzesCounted);
	      scoreKeeper->addScore(120);

	      tickDelayTimer = tickDelayEnemies;
	      sfx->playSound(15);
	    }
	  break;

	case TICKDELAY:
	  break;
	}
      break;

    case DRONES:
      switch(subState)
	{
	case SHOW:
	  gfx->showSprite(droneSprite, true);
	  gfx->showSprite(scorePerDrone, true);

	  drones = new Counter(gfx, 2);
	  drones->setPosition(13*32+16, 12*32+8);
	  drones->setCounter(0);

	  tickDelayTimer = tickDelayObjects;
	  subState = TICKDELAY;
	  break;

	case TICK:
	  if(dronesCounted == scoreKeeper->howManyKilled(ScoreKeeper::Drone))
	    {
	      if(scoreKeeper->getSkillLevel() >= 2)
		state = VIPERS;
	      else
		state = DONE;

	      subState = SHOW;
	      delayTimer = tickDelayObjects;
	    }
	  else
	    {
	      dronesCounted++;

	      drones->setCounter(dronesCounted);
	      scoreKeeper->addScore(200);

	      tickDelayTimer = tickDelayEnemies;
	      sfx->playSound(15);
	    }
	  break;

	case TICKDELAY:
	  break;
	}
      break;

    case VIPERS:
      switch(subState)
	{
	case SHOW:
	  gfx->showSprite(viperSprite, true);
	  gfx->showSprite(scorePerViper, true);

	  vipers = new Counter(gfx, 1);
	  vipers->setPosition(14*32, 13*32+8);
	  vipers->setCounter(0);

	  tickDelayTimer = tickDelayObjects;
	  subState = TICKDELAY;
	  break;

	case TICK:
	  if(vipersCounted
	     == scoreKeeper->howManyKilled(ScoreKeeper::SpaceViper))
	    {
	      if(scoreKeeper->getSkillLevel() >= 3)
		state = HUNTERS;
	      else
		state = DONE;
	      subState = SHOW;
	      delayTimer = tickDelayObjects;
	    }
	  else
	    {
	      vipersCounted++;

	      vipers->setCounter(vipersCounted);
	      scoreKeeper->addScore(1200);

	      tickDelayTimer = tickDelayEnemies;;
	      sfx->playSound(15);
	    }
	  break;

	case TICKDELAY:
	  break;
	}
      break;

    case HUNTERS:
      switch(subState)
	{
	case SHOW:
	  gfx->showSprite(hunterSprite, true);
	  gfx->showSprite(scorePerHunter, true);

	  hunters = new Counter(gfx, 1);
	  hunters->setPosition(14*32, 14*32+8);
	  hunters->setCounter(0);

	  tickDelayTimer = tickDelayObjects;
	  subState = TICKDELAY;
	  break;

	case TICK:
	  if(huntersCounted
	     == scoreKeeper->howManyKilled(ScoreKeeper::BountyHunter))
	    {
	      state = DONE;
	      subState = SHOW;
	      delayTimer = tickDelayObjects;
	    }
	  else
	    {
	      huntersCounted++;

	      hunters->setCounter(huntersCounted);
	      scoreKeeper->addScore(1000);

	      tickDelayTimer = tickDelayEnemies;
	      sfx->playSound(15);
	    }
	  break;

	case TICKDELAY:
	  break;
	}
      break;

    case DONE:
      switch(subState)
	{
	case SHOW:
	  tickDelayTimer = 3*tickDelayObjects;
	  subState = TICKDELAY;
	  break;

	case TICK:

	  scoreKeeper->nextLevel();
	  scoreKeeper->resetKills();

	  if(scoreKeeper->getLives() > 0)
	    {
	      scoreKeeper->levelCompleted();
	      if(scoreKeeper->nextLevelIsBonus())
		return BONUS;
	      else
		return GAME;
	    }
	  else
	    return MENU;

	  break;

	case TICKDELAY:
	  break;
	}
      break;
    }

  if(subState == TICK)
    {
      scoreCounter->setCounter(scoreKeeper->getScore());
      topScoreCounter->setCounter(scoreKeeper->getTopScore());
      subState = TICKDELAY;
    }

  return SCORE;
}
