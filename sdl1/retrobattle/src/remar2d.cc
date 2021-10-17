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

#include "remar2d.h"
#include "Sprite.h"
#include "SpriteInstance.h"
#include "TileSet.h"
#include "Tile.h"
#include "Font.h"
#include "SurfaceCache.h"

#include <string.h>

// +IMPLEMENTATION DETAILS

  /* The screen that remar2d renders to. */
  SDL_Surface *screen;

SurfaceCache *surfaceCache;

  int screenWidth, screenHeight, screenBPP;

  SDL_Rect *updateRects;
  SDL_Rect *allRects;

  Uint32 backgroundColor;

  map<string, Sprite *> sprites;
  map<int, SpriteInstance *> spriteInstances;
  map<string, TileSet *> tileSets;
  map<string, Font *> fonts;

  /* TODO: Class for tilemap. */

  /* Data to keep track of the background (tilemap...) */
  Tile **tiles;
  int *dirty;
  int mapWidth, mapHeight;
  int tileWidth, tileHeight;

  /* When creating a sprite instance, this variable holds the id
     number for that instance. */
  int nextSpriteInstance;

  /* Keep track of time to perform animation. */
  int lastTime;

remar2d::ErrorCode errorCode;

  int frameCounter;
  int frameTimer;

  /* Background has been set up */
  bool backgroundSetup;

  void markBackgroundDirty(SDL_Rect *rect);

  void addSpriteToTiles(SpriteInstance *spriteInstance);
  void removeSpriteFromTiles(SpriteInstance *spriteInstance);

  int ALTERNATE;

  /* Pause all animations (used for pausing in games...) */
  bool pausedAnimations;

  bool fullScreen;

void addSpriteToTiles(SpriteInstance *spriteInstance);
void removeSpriteFromTiles(SpriteInstance *spriteInstance);
void markBackgroundDirty(SDL_Rect *rect);

// -IMPLEMENTATION DETAILS


remar2d::remar2d(int width, int height, int bpp, int fullscreen,
		 const char *title)
{
  errorCode = NO_ERROR;
  nextSpriteInstance = 1;
  lastTime = 0;
  frameCounter = 0;
  frameTimer = 0;
  backgroundSetup = false;
  pausedAnimations = false;
  fullScreen = false;
  screenWidth = width;
  screenHeight = height;
  screenBPP = bpp;
  updateRects = 0;
  allRects = 0;

  /* SDL_RESIZABLE makes it possible to switch between window/fullscreen */
  //int flags = SDL_DOUBLEBUF | SDL_RESIZABLE;
  int flags = SDL_RESIZABLE;

  fullScreen = fullscreen;

  if(fullScreen)
    {
      flags |= SDL_FULLSCREEN;
    }

  screenWidth = width;
  screenHeight = height;

  screen = SDL_SetVideoMode(width, height, bpp, flags);

  if(screen == 0)
    {
      errorCode = FAILED_SET_VIDEO_MODE;
    }

  setBackgroundColor(0, 0, 0);

  SDL_ShowCursor(SDL_DISABLE);

  SDL_WM_SetCaption(title, 0);

  showCredits();

  surfaceCache = new SurfaceCache();
}

remar2d::~remar2d()
{
  delete surfaceCache;
}

void
remar2d::showCredits()
{
  printf("remar2d credits---------------------------------------------------"
	 "-------------\n");
  printf("SDL by Sam Lantinga et. al. (c) ?-? (www.libsdl.org)\n");
  printf("TinyXML by Lee Thomason (c) 2000-2006 (www.grinninglizard.com)\n");
  printf("------------------------------------------------------------------"
	 "-------------\n");
}

// TODO: Use exceptions instead
remar2d::ErrorCode
remar2d::getError()
{
  return errorCode;
}

const char *
remar2d::getErrorMessage()
{
  const char *errorToString[] = { "No error",
				  "Failed to set video mode",
				  "Failed to load sprite"};

  if(errorCode >= NO_ERROR && errorCode <= FAILED_LOAD_TILESET)
    {
      return errorToString[errorCode];
    }
  else
    {
      return "BUG! Error code has no matching string";
    }
}

void
remar2d::redraw()
{
  int delta;

  if(lastTime == 0)
    {
      delta = 0;
      lastTime = SDL_GetTicks();
    }
  else
    {
      delta = SDL_GetTicks() - lastTime;
      lastTime = SDL_GetTicks();
    }

  /* Animate sprites to see where we need to redraw */
  if(!pausedAnimations)
    {
      for(int i = 1;i < nextSpriteInstance;i++)
	{
	  SpriteInstance *spriteInstance = spriteInstances[i];
	  
	  if(spriteInstance && spriteInstance->getVisible())
	    {
	      spriteInstance->animate(delta);
	      
	      if(spriteInstance->getRedraw() == true)
		{
		  SDL_Rect *rect = spriteInstance->getLastRect();
		  
		  markBackgroundDirty(rect);
		}
	    }
	}
    }

  /* Redraw background tiles */
  SDL_Rect *source;
  SDL_Rect dest;
  SDL_Surface *image;
  for(int i = 0;i < mapWidth*mapHeight;i++)
    {
      if(dirty[i])
	{
	  dest.x = allRects[i].x;
	  dest.y = allRects[i].y;
	  dest.w = tileWidth;
	  dest.h = tileHeight;

	  SDL_FillRect(screen, &dest, backgroundColor);

	  if(tiles[i] != 0)
	    {
	      tiles[i]->markSpritesDirty();

	      if(tiles[i]->empty == false)
		{
		  /* TODO: Store this info in the tile instead (optimize) */
		  TileSet *set = tileSets[tiles[i]->tileSet];

		  source = set->getRect(tiles[i]->x, tiles[i]->y);
		  image = set->getImage();

		  SDL_BlitSurface(image, source, screen, &dest);
		}
	    }
	}
    }

  map<int, SpriteInstance *>::iterator it = spriteInstances.begin();

  for(;it != spriteInstances.end();it++)
    {
      SpriteInstance *spriteInstance = (*it).second;

      if(spriteInstance
	 && spriteInstance->getVisible()
	 && spriteInstance->getRedraw())
	{
	  spriteInstance->setRedraw(false);

	  Sprite *sprite = spriteInstance->getSprite();
	  SDL_Rect *source;
	  SDL_Rect dest;

	  const char *anim = spriteInstance->getCurrentAnimation();

	  if(!anim)
	    {
	      /* If animation isn't set, just continue */
	      continue;
	    }

	  Animation *animation = sprite->getAnimation(anim);
	  int currentFrame = spriteInstance->currentFrame;

	  if(!animation->emptyFrame(currentFrame))
	    {

	      SDL_Surface *animSurface = animation->getImage();
	      source = animation->getRect(currentFrame);

	      dest.x = spriteInstance->x - animation->orig_x;
	      dest.y = spriteInstance->y - animation->orig_y;

	      // printf("Redraw %s\n", sprite->getName());
	      
	      SDL_BlitSurface(animSurface,
			      source,
			      screen,
			      &dest);

	      /* Save where we painted so we can paint over later */
	      spriteInstance->setLastRect(dest.x, dest.y,
					  source->w, source->h);
	      
	      delete source;
	    }
	}
    }

  int dirtyRects = 0;

  for(int i = 0;i < mapWidth*mapHeight;i++)
    if(dirty[i])
      {
	updateRects[dirtyRects].x = allRects[i].x;
	updateRects[dirtyRects].y = allRects[i].y;
	updateRects[dirtyRects].w = allRects[i].w;
	updateRects[dirtyRects].h = allRects[i].h;
	
	dirtyRects++;
	dirty[i] = 0;
      }

  SDL_UpdateRects(screen, dirtyRects, updateRects);

  frameCounter++;
  if(frameTimer == 0)
    {
      frameTimer = SDL_GetTicks();
    }

  if(SDL_GetTicks() - frameTimer > 1000)
    {
      //printf("FPS: %d\n", frameCounter);
      frameTimer = SDL_GetTicks();
      frameCounter = 0;
    }
}

void
remar2d::setBackgroundColor(int r, int g, int b)
{
  backgroundColor = SDL_MapRGB(screen->format, r, g, b);
}

const char *
remar2d::loadTileSet(const char *file)
{
  TileSet *tileSet = new TileSet(file);
  if(tileSet)
    {
      if(strcmp(tileSet->getName(), "") == 0)
	{
	  errorCode = FAILED_LOAD_TILESET;
	  return 0;
	}
    }
  else
    {
      errorCode = FAILED_LOAD_TILESET;
      return 0;
    }

  /* Store tileset */
  tileSets[string(tileSet->getName())] = tileSet;

  return tileSet->getName();
}

void
remar2d::removeTileSet(const char *tileset)
{

}

void
remar2d::loadTileMap(const char *file)
{

}

void
remar2d::setupTileBackground(int size_x, int size_y)
{
  if(backgroundSetup)
    {
      delete [] tiles;
      delete [] dirty;
      delete [] updateRects;
      delete [] allRects;
    }

  int odd_width, odd_height;

  odd_width = !!(screenWidth % size_x);
  odd_height = !!(screenHeight % size_y);

  mapWidth = screenWidth/size_x + odd_width;
  mapHeight = screenHeight/size_y + odd_height;

  tileWidth = size_x;
  tileHeight = size_y;

  updateRects = new SDL_Rect[mapWidth * mapHeight];
  allRects = new SDL_Rect[mapWidth * mapHeight];

  tiles = new Tile*[mapWidth * mapHeight];
  dirty = new int[mapWidth * mapHeight];

  for(int i = 0;i < mapWidth * mapHeight;i++)
    {
      tiles[i] = new Tile();
      dirty[i] = 1;

      allRects[i].x = (i % mapWidth)*tileWidth;
      allRects[i].y = (i / mapWidth)*tileHeight;
      allRects[i].w = tileWidth;
      allRects[i].h = tileHeight;
    }

  /* The last column and the last row of Update Rects are *NOT*
     clipped to the screen, so we must make sure that they're not
     outside of the screen. */
  if(odd_width)
    {
      int lastRectWidth = screenWidth % size_x;
      for(int y = mapWidth - 1;y < mapWidth * mapHeight;y += mapWidth)
	allRects[y].w = lastRectWidth;
    }
  if(odd_height)
    {
      int lastRectHeight = screenHeight % size_y;
      for(int x = mapWidth * (mapHeight - 1);x < mapWidth * mapHeight; x++)
	allRects[x].h = lastRectHeight;
    }

  backgroundSetup = true;
}

void
remar2d::setTile(int x, int y, const char *tileSet, int t_x, int t_y)
{
  /* Save the old tile for a while, might want to get list of sprites
     from it */
  // TODO: Maybe we should just modify the tile directly instead...?
  Tile *oldTile = tiles[y*mapWidth + x];

  Tile *tile;

  if(tileSet == 0)
    {
      tile = new Tile();
    }
  else
    {
      tile = new Tile(tileSet, t_x, t_y);
    }

  if(oldTile)
    {
      tile->setListOfSprites(oldTile->getListOfSprites());
      delete oldTile;
    }

  tiles[y*mapWidth + x] = tile;
  dirty[y*mapWidth + x] = 1;
}

const char *
remar2d::loadSprite(const char *file)
{
  Sprite *sprite = new Sprite(file);
  if(sprite)
    {
      if(strcmp(sprite->getName(), "") == 0)
	{
	  errorCode = FAILED_LOAD_SPRITE;
	  return 0;
	}
    }
  else
    {
      errorCode = FAILED_LOAD_SPRITE;
      return 0;
    }

  /* Store sprite */
  sprites[string(sprite->getName())] = sprite;

  /* Use this name when you want to create instances of this sprite */
  return sprite->getName();
}

void
remar2d::removeSprite(const char *sprite)
{
  Sprite *spr = sprites[string(sprite)];

  sprites.erase(string(sprite));

  delete spr;
}

int
remar2d::createSpriteInstance(const char *sprite)
{
  Sprite *spr = sprites[string(sprite)];

  spriteInstances[nextSpriteInstance] = new SpriteInstance(spr);

  //printf("Create sprite instance %d (%s)\n", nextSpriteInstance, sprite);

  return nextSpriteInstance++;
}

void
remar2d::setAnimation(int sprite, const char *animation)
{
  spriteInstances[sprite]->setAnimation(animation);
}

void
remar2d::moveSpriteRel(int sprite, int x, int y)
{
  /* Remove sprite from tiles */
  removeSpriteFromTiles(spriteInstances[sprite]);

  spriteInstances[sprite]->moveRel(x, y);

  /* Add sprite to tiles */
  addSpriteToTiles(spriteInstances[sprite]);
}

inline void clip(int *v, int limit)
{
  if(*v < 0)
    *v = 0;
  else if(*v >= limit)
    *v = limit - 1;
}

void addSpriteToTiles(SpriteInstance *spriteInstance)
{
  SDL_Rect *rect = spriteInstance->getCurrentRect();

  int x1 = rect->x;
  int x2 = rect->x + rect->w;
  int y1 = rect->y;
  int y2 = rect->y + rect->h;

  x1 /= tileWidth;
  x2 /= tileWidth;
  y1 /= tileHeight;
  y2 /= tileHeight;

  clip(&x1, mapWidth);
  clip(&x2, mapWidth);
  clip(&y1, mapHeight);
  clip(&y2, mapHeight);

  for(int y = y1;y <= y2;y++)
    for(int x = x1;x <= x2;x++)
      {
	if(tiles[x + y * mapWidth])
	  tiles[x + y * mapWidth]->addSpriteInstance(spriteInstance);
	dirty[x + y * mapWidth] = 1;
	tiles[x + y * mapWidth]->markSpritesDirty();
      }
}

void removeSpriteFromTiles(SpriteInstance *spriteInstance)
{
  SDL_Rect *rect = spriteInstance->getCurrentRect();

  int x1 = rect->x;
  int x2 = rect->x + rect->w;
  int y1 = rect->y;
  int y2 = rect->y + rect->h;

  x1 /= tileWidth;
  x2 /= tileWidth;
  y1 /= tileHeight;
  y2 /= tileHeight;

  clip(&x1, mapWidth);
  clip(&x2, mapWidth);
  clip(&y1, mapHeight);
  clip(&y2, mapHeight);

  for(int y = y1;y <= y2;y++)
    for(int x = x1;x <= x2;x++)
      {
	if(tiles[x + y * mapWidth])
	  tiles[x + y * mapWidth]->removeSpriteInstance(spriteInstance);
	dirty[x + y * mapWidth] = 1;
	tiles[x + y * mapWidth]->markSpritesDirty();
      }
}

void
remar2d::moveSpriteAbs(int sprite, int x, int y)
{
  /* Remove sprite from tiles */
  removeSpriteFromTiles(spriteInstances[sprite]);

  spriteInstances[sprite]->moveAbs(x, y);

  /* Add sprite to tiles */
  addSpriteToTiles(spriteInstances[sprite]);
}

void
remar2d::showSprite(int sprite, bool show)
{
  if(spriteInstances[sprite])
    {
      spriteInstances[sprite]->setVisible(show);

      //SDL_Rect *rect = spriteInstances[sprite]->getLastRect();
      SDL_Rect *rect = spriteInstances[sprite]->getCurrentRect();
      markBackgroundDirty(rect);
    }
}

void
remar2d::removeSpriteInstance(int sprite)
{
  //printf("Remove sprite instance %d (%s)\n", sprite, spriteInstances[sprite]->getSprite()->getName());
  showSprite(sprite, false);
}

void
remar2d::pauseAnimation(int sprite, bool on)
{
  spriteInstances[sprite]->pauseAnimation(on);
}

void
remar2d::neverRedraw(int sprite, bool on)
{
  if(SpriteInstance *spr = spriteInstances[sprite])
    spr->setNeverRedraw(on);
}

void
remar2d::pauseAnimations(bool on)
{
  pausedAnimations = on;
}

const char *
remar2d::loadFont(const char *file)
{
  Font *font = new Font(file);

  if(!font || strcmp(font->getName(), "") == 0)
    {
      return 0;
    }

  /* Store sprite */
  fonts[string(font->getName())] = font;

  /* Use this name when you want to print with this font */
  return font->getName();
}

void
remar2d::removeFont(const char *font)
{
  Font *fnt = fonts[string(font)];

  fonts.erase(string(font));

  delete fnt;
}

int
remar2d::print(const char *fontName, const char *text)
{
  /* Create a new sprite, new animation, new frame containing the
     text. 
     Add sprite instance to normal map of sprite instances.
  */

  /*
BRILLANT
  */

  SDL_Rect destRect;

  Font *font = fonts[string(fontName)];

  int width  = font->getWidth();
  int height = font->getHeight();

  int len = strlen(text);

  int rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

  SDL_Surface *image;
  image = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCCOLORKEY,
			       width * len,
			       height,
			       32,
			       rmask, gmask, bmask, amask);

  Character *character;
  SDL_Surface *charset;
  SDL_Rect *source;

  for(int i = 0;i < len;i++)
    {
      if(text[i] == ' ')
 	continue;

      destRect.x = i * width;
      destRect.y = 0;
      destRect.w = width;
      destRect.h = height;

      character = font->getCharacter(text[i]);

      if(character)
	{
	  charset = character->getImage();
	  source = character->getRect();

	  SDL_BlitSurface(charset, source, image, &destRect);
	}
    }

  Animation *animation = new Animation("normal");
  animation->addSDLImage(image, width * len, height);

  Frame *frame = new Frame(0, 0, 1000, false);

  animation->addFrame(frame);
  animation->framesAdded();
  animation->setLooping(false);
  
  Sprite *sprite = new Sprite();
  sprite->addAnimation(animation);

  char buf[1024];
  sprintf(buf, "text%d", nextSpriteInstance);
  sprite->setName(buf);

  sprites[string(sprite->getName())] = sprite;

  int instance = createSpriteInstance(sprite->getName());
  setAnimation(instance, "normal");

  return instance;
}

void
remar2d::showPointer(bool on)
{
  if(on)
    SDL_ShowCursor(SDL_ENABLE);
  else
    SDL_ShowCursor(SDL_DISABLE);
}

void
remar2d::setFullScreen(bool on)
{
  if(fullScreen == on)
    return;

  int flags = SDL_DOUBLEBUF | SDL_RESIZABLE;

  fullScreen = on;
  if(fullScreen)
    {
      flags |= SDL_FULLSCREEN;
    }

  screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBPP, flags);

  SDL_Rect dirtyRect;

  dirtyRect.x = 0;
  dirtyRect.y = 0;
  dirtyRect.w = screenWidth;
  dirtyRect.h = screenHeight;

  markBackgroundDirty(&dirtyRect);
}

void markBackgroundDirty(SDL_Rect *rect)
{
  int x1 = rect->x;
  int x2 = rect->x + rect->w;
  int y1 = rect->y;
  int y2 = rect->y + rect->h;

  x1 /= tileWidth;
  x2 /= tileWidth;
  y1 /= tileHeight;
  y2 /= tileHeight;

  clip(&x1, mapWidth);
  clip(&x2, mapWidth);
  clip(&y1, mapHeight);
  clip(&y2, mapHeight);

  for(int y = y1;y <= y2;y++)
    for(int x = x1;x <= x2;x++)
      {
	dirty[x + y * mapWidth] = 1;
	tiles[x + y * mapWidth]->markSpritesDirty();
      }
}
