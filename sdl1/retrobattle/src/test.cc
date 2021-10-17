/*
    remar2d - a 2D graphics engine using SDL
    Copyright (C) 2007 Andreas Remar, andreas.remar@gmail.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Test the remar2d Graphics Engine
 */

#define IJIS 10

#include "remar2d.h"
#include <unistd.h>
#include <stdlib.h>

const int WIDTH = 640;
const int HEIGHT = 480;
const int BPP = 32;
const int FS = 0;

remar2d *gfx;
int is[IJIS];

void setAnimation(const char *anim)
{
  for(int i = 0;i < IJIS;i++)
    gfx->setAnimation(is[i], anim);
}

int main(int argc, char *argv[])
{
  SDL_Init(SDL_INIT_VIDEO);

  gfx = new remar2d(WIDTH, HEIGHT, BPP, FS, "test");

  gfx->setupTileBackground(32, 32);
  const char *fnt = gfx->loadFont("../gfx/text.xml");

  int loading = gfx->print("text", "loading");
  gfx->showSprite(loading, true);
  gfx->moveSpriteAbs(loading, 0, 0);
  gfx->redraw();

  if(gfx->getError() == remar2d::FAILED_SET_VIDEO_MODE)
    {
      printf("Failed to set video mode!! %s!!\n", gfx->getErrorMessage());

      exit(-1);
    }

  const char *SPRITES[] = { "../gfx/iji.xml", "../gfx/yellow_iji/iji.xml" };
  int num_sprites = 2;

  const char *t1 = gfx->loadTileSet("../gfx/tileset.xml");
  printf("Tileset: %s\n", t1);

  for(int y = 0;y < 15;y++)
    for(int x = 0;x < 20;x++)
      {
	gfx->setTile(x, y, t1, (x*y)%4, 0);
      }

  for(int i = 0;i < num_sprites;i++)
    {
      gfx->loadSprite(SPRITES[i]);
    }

  for(int i = 0;i < IJIS;i++)
    {
      is[i] = gfx->createSpriteInstance("iji");
      gfx->setAnimation(is[i], "look right");
      gfx->showSprite(is[i], true);
      gfx->moveSpriteAbs(is[i], 320, (480-60)-48*i);
    }

  int dir = 0;
  int quit = 0;
  int left_pressed = 0, right_pressed = 0;
  int left_checked = 0, right_checked = 0;
  int txt = 0;

  while(!quit)
    {
      SDL_Event event;

      while(SDL_PollEvent(&event))
	{
	  switch(event.type)
	    {
	    case SDL_KEYDOWN:
	      switch(event.key.keysym.sym)
		{
		case SDLK_LEFT:
		  left_pressed = 1;
		  left_checked = 0;
		  break;
		case SDLK_RIGHT:
		  right_pressed = 1;
		  right_checked = 0;
		  break;

		case SDLK_p:
		  if(!txt)
		    {
		      txt = gfx->print(fnt, "titta det funkar 123");

		      printf("txt sprite instance: %d\n", txt);

		      gfx->showSprite(txt, true);
		      gfx->moveSpriteAbs(txt, 100, 100);
  		    }
		  else
		    gfx->moveSpriteRel(txt, 1, 0);

		  break;

		case SDLK_ESCAPE: quit = 1;          break;
		default:
		  break;
		}
	      break;

	    case SDL_KEYUP:
	      switch(event.key.keysym.sym)
		{
		case SDLK_LEFT:
		  left_pressed = 0;
		  left_checked = 0;
		  break;
		case SDLK_RIGHT:
		  right_pressed = 0;
		  right_checked = 0;
		  break;
		default:
		  break;
		}
	      break;
	    }
	}

      int old_dir = dir;

      if(!left_checked)
	{
	  if(left_pressed)
	    {
	      dir -= 1;
	    }
	  else
	    {
	      dir += 1;
	    }
	  left_checked = 1;
	}
      if(!right_checked)
	{
	  if(right_pressed)
	    {
	      dir += 1;
	    }
	  else
	    {
	      dir -= 1;
	    }
	  right_checked = 1;
	}

      if(old_dir != dir)
	{
	  if(dir < 0)
	    setAnimation("run left");
	  else if(dir == 0)
	    setAnimation("look right");
	  else
	    setAnimation("run right");
	}
      
      if(dir)
	{
	  for(int i = 0;i < IJIS;i++)
	    gfx->moveSpriteRel(is[i], dir*3, 0);
	}

      gfx->redraw();
      SDL_Delay(10);
    }

  SDL_Quit();
}
