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

#include "GameLogic.h"
#include <stdlib.h>
#include "Level.h"
#include "Menu.h"
#include "ScoreScreen.h"
#include "BonusLevel.h"
#include "BonusScoreScreen.h"
#include "FileManager.h"
#include <time.h>

GameLogic::GameLogic(Input *i, remar2d *gfx, SoundManager *sfx, char *datadir)
  : input(i), graphics(gfx), sound(sfx),
    lastFrameTime(SDL_GetTicks() / 1000.0), cyclesLeftOver(0), quitGame(false),
    fullScreen(false), datadir(datadir)
{
  char buf[1024];

  srand(time(0));

  graphics->setupTileBackground(32, 32);

  makeGfxPath(buf, datadir, "text.xml");
  graphics->loadFont(buf);

  int loading = graphics->print("text", "loading");
  graphics->showSprite(loading, true);
  graphics->moveSpriteAbs(loading, 0, 0);
  graphics->redraw();
  
  /* Read in graphics */
  const char *sprites[] = {"good.xml",
			   "fuzz.xml",
			   "pixel.xml",
			   "coin.xml",
			   "flame.xml",
			   "nest.xml",
			   "shot.xml",
			   "hud.xml",
			   "explosion.xml",
			   "smoke.xml",
			   "numbers.xml",
			   "drone.xml",
			   "clock.xml",
			   "ammo.xml",
			   "reload.xml",
			   "snakehead.xml",
			   "snakebody.xml",
			   "timered.xml",
			   "hunter.xml",
			   "hshot.xml",
			   "note.xml",
			   "bonus.xml",
			   "helmet.xml",
			   "redfuzz.xml",
			   "golddrone.xml",
			   "diresnakehead.xml",
			   "diresnakebody.xml",
			   "darkhunter.xml",
			   "dhshot.xml",
			   "darkexplo.xml",
			   "arrows.xml",
			   "pickup_scores.xml",
			   0};

  printf("Loading sprites");
  for(int i = 0;sprites[i];i++)
    {
      makeGfxPath(buf, datadir, sprites[i]);
      printf(".");
      graphics->loadSprite(buf);
    }
  printf("\n");

  const char *tilesets[] = {"background.xml",
			    "backgroundred.xml",
			    "maintiles.xml",
			    0};

  printf("Loading tilesets");
  for(int i = 0;tilesets[i];i++)
    {
      makeGfxPath(buf, datadir, tilesets[i]);
      printf(".");
      graphics->loadTileSet(buf);
    }
  printf("\n");

  scoreKeeper = new ScoreKeeper();

  graphics->showSprite(loading, false);
  graphics->removeSpriteInstance(loading);

  mode = MENU;
  gameMode = new Menu(graphics, sound, input, scoreKeeper);

  int keyConfig[4] = {SDLK_LEFT, SDLK_RIGHT, SDLK_x, SDLK_z};

  FileManager fileManager;

  fileManager.readKeyConfig(keyConfig);

  input->defineActionKey(Input::LEFT, keyConfig[0]);
  input->defineActionKey(Input::RIGHT, keyConfig[1]);
  input->defineActionKey(Input::FIRE, keyConfig[2]);
  input->defineActionKey(Input::JUMP, keyConfig[3]);
}

GameLogic::~GameLogic()
{
  delete scoreKeeper;
}

void
GameLogic::update()
{
  float currentTime = SDL_GetTicks() / 1000.0;
  float updateIterations = (currentTime - lastFrameTime) + cyclesLeftOver;

  if(updateIterations > maxCyclesPerFrame * updateInterval)
    {
      updateIterations = maxCyclesPerFrame * updateInterval;
    }

  if(input->pressed(SDLK_f))
    {
      fullScreen = !fullScreen;
      graphics->setFullScreen(fullScreen);
    }

  for(;updateIterations > updateInterval; updateIterations -= updateInterval)
    {
      Mode oldMode = mode;

      if(gameMode)
	{
	  mode = gameMode->update();
	}

      if(oldMode != mode)
	{
	  delete gameMode;
	  gameMode = 0;
	  switch(mode)
	    {
	    case MENU:	
	      gameMode = new Menu(graphics, sound, input, scoreKeeper);
	      break;

	    case GAME:
	      gameMode = new Level(graphics, sound, input, scoreKeeper, datadir);
	      break;

	    case BONUS:
	      gameMode = new BonusLevel(graphics, sound, input, scoreKeeper, datadir);
	      break;

	    case SCORE:
	      gameMode = new ScoreScreen(graphics, sound, input, scoreKeeper);
	      break;

	    case BONUSSCORE:
	      gameMode = new BonusScoreScreen(graphics, sound, input,
					      scoreKeeper);
	      break;

	    case QUIT:
	      quitGame = true;
	      break;
	    }
	}
    }

  cyclesLeftOver = updateIterations;
  lastFrameTime = currentTime;
}

bool
GameLogic::quit()
{
  /* Should the application quit? */
  return quitGame;
}

void
GameLogic::makeGfxPath(char *buf, char *datadir, const char *file)
{
  sprintf(buf, "%s/data/gfx/%s", datadir, file);
}
