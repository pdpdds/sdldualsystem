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

#include "ScoreKeeper.h"
#include <stdio.h>
#include "FileManager.h"

ScoreKeeper::ScoreKeeper()
  : skillLevel(1), 
    level(1), 
    bonusLevel(1), 
    levelsCompleted(0), 
    lives(3), 
    score(0),
    top(0), 
    timer(0),
    noBonus(false),
    nextExtraLife(30000)
{
  resetKills();

  FileManager fileManager;

  top = fileManager.readTopScore();

  resetCoins();
}

ScoreKeeper::~ScoreKeeper()
{
  FileManager fileManager;

  fileManager.writeTopScore(top);
}

bool
ScoreKeeper::fastFuzzes()
{
  return skillLevel > 1;
}

bool
ScoreKeeper::redFuzzes()
{
  return skillLevel >= 5;
}

int
ScoreKeeper::numberOfEnemy(EnemyType type)
{
  int amounts[] = 

  /*  0  1  2  3  4  5  6  7  8 */

    { 0, 3, 4, 5, 6, 6, 6, 6, 6, /* Fuzz (Blue or Red) */
      0, 2, 3, 4, 4, 4, 0, 0, 0, /* Drone */
      0, 0, 4, 5, 6, 6, 6, 0, 0, /* Space Viper (length) */
      0, 0, 0, 1, 1, 1, 1, 1, 0, /* Bounty Hunter */
      /*0, 0, 0, 0, 0, 6, 6, 6, 6,*/ /* Red Fuzz */
      0, 0, 0, 0, 0, 0, 4, 4, 4, /* Gold Drone */
      0, 0, 0, 0, 0, 0, 0, 6, 6, /* Dire Space Viper (length) */
      0, 0, 0, 0, 0, 0, 0, 0, 1  /* Dark Bounty Hunter */
    };

  return amounts[type * 9 + skillLevel];
}

void
ScoreKeeper::setSkillLevel(int level)
{
  skillLevel = level;
}

int
ScoreKeeper::getSkillLevel()
{
  return skillLevel;
}

void
ScoreKeeper::incSkillLevel()
{
  if(skillLevel < 8)
    skillLevel++;
}

int
ScoreKeeper::getLevel()
{
  return level;
}

void
ScoreKeeper::setLevel(int lev)
{
  level = lev;
}

void
ScoreKeeper::nextLevel()
{
  level++;
  if(level > 32)
    level = 1;
}

bool
ScoreKeeper::nextLevelIsBonus()
{
  if(levelsCompleted == 4)
    {
      levelsCompleted = 0;
      return true;
    }
  return false;
}
			  
bool
ScoreKeeper::blocksTakeTwoHits()
{
  return skillLevel >= 4;
}

void
ScoreKeeper::resetKills()
{
  for(int i = 0;i < 7;i++)
    kills[i] = 0;
}

void
ScoreKeeper::killed(EnemyType type)
{
  kills[type]++;
}

int
ScoreKeeper::howManyKilled(EnemyType type)
{
  return kills[type];
}

void
ScoreKeeper::heroKilled()
{
  setNoBonus(true);

  if(lives)
    lives--;
}

int
ScoreKeeper::getLives()
{
  return lives;
}

void
ScoreKeeper::setLives(int l)
{
  lives = l;
}

void
ScoreKeeper::addScore(int s)
{
  setScore(score + s);
}

void
ScoreKeeper::setScore(int s)
{
  if(score < nextExtraLife && s >= nextExtraLife && nextExtraLife <= 8*30000)
    {
      lives++;
      nextExtraLife += 30000;
    }

  score = s;

  if(score > top)
    top = score;
}

int
ScoreKeeper::getScore()
{
  return score;
}

void
ScoreKeeper::setTopScore(int s)
{
  top = s;
}

int
ScoreKeeper::getTopScore()
{
  return top;
}

void
ScoreKeeper::calculateScore()
{
  int scores[] = {120, 200, 1200, 1000, 200, 1200, 1000};

  for(int i = 0;i < 7;i++)
    {
      score += kills[i] * scores[i];
    }

  score += 100*skillLevel;
}

void
ScoreKeeper::reset()
{
  setScore(0);
  setLives(3);
  resetKills();
  levelsCompleted = 0;
  nextExtraLife = 30000;
}
