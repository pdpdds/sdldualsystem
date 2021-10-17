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

#ifndef BONUSLEVEL_H
#define BONUSLEVEL_H

#include <remar2d.h>
#include "SoundManager.h"
#include "Input.h"
#include "Object.h"
#include "Hero.h"
#include "Coin.h"
#include "ScoreKeeper.h"
#include "Field.h"
#include <map>
#include <string>
#include <list>
#include "GameMode.h"
#include "BrokenBlock.h"
#include "Explosion.h"
#include "Smoke.h"
#include "Collectible.h"
#include "Counter.h"
#include "HUD.h"
#include "BulletHandler.h"
#include "Helmet.h"

class BonusLevel : public GameMode
{
 public:
  BonusLevel(remar2d *gfx, SoundManager *sfx, Input *input,
	     ScoreKeeper *scoreKeeper, char *datadir);
  ~BonusLevel();
  Mode update();

 private:
  bool loadLevel(char *lev);
  bool respawn(int x, int y);
  void clearLevel();
  bool collides(Object *obj1, Object *obj2);
  void pause();
  void removeBullet(int i);
  void showAllObjects(bool show);
  void deleteAllObjects();
  void performWipe(Mode modeToReturn);

  Field *field;

  bool paused;

  /* Da player. */
  Hero *hero;

  /* Bullets flying through the air! (KERPOWW!!) */
/*   int bullets; */
/*   Bullet *bullet[8]; */

  list<BrokenBlock *> brokenBlocks;
  list<Object *> objects;
  list<Collectible *> collectibles;
  list<Coin *> coins;
  list<Bullet *> bullets;

  bool doWipe;
  int wipeTimer;
  int wipeCounter;

  bool win;
  int winTimer;

  /* Start position for the player */
  int heroStartX;
  int heroStartY;

  int debugOutputTimer;

  bool loadFailed;

  /* Timer counts down from 200 to 0, timerTimer keeps track of when
     timer should be decremented... (every 60 frames I guess) */
  int timer;
  int timerTimer;
  bool timerPaused;
  Counter *timerCounter;

  BulletHandler *bulletHandler;

  Mode returnMode;

  int hud;

  Helmet *helmet;
};

#endif
