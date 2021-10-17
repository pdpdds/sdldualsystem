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

#include "BonusScoreScreen.h"

BonusScoreScreen::BonusScoreScreen(remar2d *gfx, SoundManager *sfx,
				   Input *input, ScoreKeeper *scoreKeeper)
  : GameMode(gfx, sfx, input, scoreKeeper), 
    state(TIME), 
    subState(SHOW), 
    delayTimer(60),
    coinsCounted(0)
{
  gfx->setupTileBackground(32, 32);

  drawBackground();

  top = gfx->print("text", "top");
  gfx->showSprite(top, true);
  gfx->moveSpriteAbs(top, 9*32, 4*32 + 8);

  score = gfx->print("text", "score");
  gfx->showSprite(score, true);
  gfx->moveSpriteAbs(score, 8*32 + 16, 6*32 + 8);

  stage = gfx->print("text", "bonus");
  gfx->showSprite(stage, true);
  gfx->moveSpriteAbs(stage, 10*32+16, 8*32+8);

  level = new Counter(gfx, 2);
  level->setPosition(13*32+16, 8*32+8);
  level->setCounter(scoreKeeper->getBonusLevel());

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
  //snprintf(buf, 5, "%d#", 100 * scoreKeeper->getSkillLevel());
  snprintf(buf, 5, "%d#", 200 + 100 * scoreKeeper->getBonusLevel());

  scorePerCoin = gfx->print("text", buf);
  gfx->showSprite(scorePerCoin, false);
  gfx->moveSpriteAbs(scorePerCoin, 11*32+16, 10*32+8);

  perfect = gfx->print("text", "perfect");
  gfx->showSprite(perfect, false);
  gfx->moveSpriteAbs(perfect, 10*32+24, 12*32+8);

  twothousand = gfx->print("text", "2000");
  gfx->showSprite(twothousand, false);
  gfx->moveSpriteAbs(twothousand, 11*32+16, 13*32+8);
}

BonusScoreScreen::~BonusScoreScreen()
{
  /* Delete counters */
  delete topScoreCounter;
  delete scoreCounter;
  delete level;
  delete time;
  delete coins;

  gfx->removeSpriteInstance(scorePerTime);
  gfx->removeSpriteInstance(scorePerCoin);
  gfx->removeSpriteInstance(top);
  gfx->removeSpriteInstance(score);
  gfx->removeSpriteInstance(stage);
  gfx->removeSpriteInstance(clock);
  gfx->removeSpriteInstance(coinSprite);
}

void
BonusScoreScreen::drawBackground()
{
  for(int y = 0;y < 19;y++)
    for(int x = 0;x < 25;x++)
      {
	gfx->setTile(x, y, "red background", 0, 0);
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
	    gfx->setTile(x + offsetX, y + offsetY, "red background",
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
	  gfx->setTile(x + offsetX, y + offsetY, "red background", i, 0);
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
12 if coins != amount collected, goto 10
13 delay 1 second
14 show fuzz
15 delay 1 second
16 tick down 1 fuzz and add score
17 delay 1/2 second
18 if fuzz != fuzz killed, goto 16
19 ...


*/

Mode
BonusScoreScreen::update()
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
      return BONUSSCORE;
    }

  if(subState == TICKDELAY)
    {
      if(--tickDelayTimer == 0)
	{
	  subState = TICK;
	}
    }

  switch(state)
    {
    case TIME:
      switch(subState)
	{
	case SHOW:
	  gfx->showSprite(clock, true);
	  gfx->showSprite(scorePerTime, true);
	  
	  time = new Counter(gfx, 3);
	  time->setPosition(13*32, 9*32+8);
	  time->setCounter(scoreKeeper->getTimer());

	  tickDelayTimer = 60;
	  subState = TICKDELAY;
	  break;
	  
	case TICK:
	  if(scoreKeeper->getTimer() == 0)
	    {
	      state = COINS;
	      subState = SHOW;
	      delayTimer = 60;
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
	      
	      tickDelayTimer = 6;

	      sfx->playSound(15);
	    }
	  break;

	default:
	  break;
	}
      break;

    case COINS:
      switch(subState)
	{
	case SHOW:
	  gfx->showSprite(coinSprite, true);
	  gfx->showSprite(scorePerCoin, true);

	  coins = new Counter(gfx, 2);
	  coins->setPosition(13*32+16, 10*32+8);
	  coins->setCounter(0);

	  tickDelayTimer = 60;
	  subState = TICKDELAY;
	  break;

	case TICK:
	  if(coinsCounted == scoreKeeper->getCoinsCollected())
	    {
	      state = PERFECT;
	      subState = SHOW;
	      delayTimer = 60;
	    }
	  else
	    {
	      coinsCounted++;

	      coins->setCounter(coinsCounted);
	      scoreKeeper->addScore(200 + 100 * scoreKeeper->getBonusLevel());

	      tickDelayTimer = 12;

	      sfx->playSound(15);
	    }
	  break;

	default:
	  break;
	}
      break;

    case PERFECT:
      switch(subState)
	{
	case SHOW:
	  if(scoreKeeper->coinsLeft() == 0)
	    {
	      gfx->showSprite(perfect, true);
	      gfx->showSprite(twothousand, true);

	      sfx->playSound(16);

	      scoreKeeper->addScore(2000);
	      scoreCounter->setCounter(scoreKeeper->getScore());
	      topScoreCounter->setCounter(scoreKeeper->getTopScore());
	    }

	  state = DONE;
	  subState = SHOW;
	  delayTimer = 0;
	  break;

	default:
	  break;
	}
      break;

    case DONE:
      switch(subState)
	{
	case SHOW:
	  tickDelayTimer = 3*60;
	  subState = TICKDELAY;
	  break;

	case TICK:

	  scoreKeeper->resetKills();
	  scoreKeeper->incSkillLevel();
	  scoreKeeper->nextBonusLevel();

	  return GAME;

	  break;

	default:
	  break;
	}
      break;

    default:
      break;
    }

  if(subState == TICK)
    {
      scoreCounter->setCounter(scoreKeeper->getScore());
      topScoreCounter->setCounter(scoreKeeper->getTopScore());
      subState = TICKDELAY;
    }

  return BONUSSCORE;
}
