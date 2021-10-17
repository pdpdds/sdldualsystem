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

#include "HUD.h"

HUD::HUD(remar2d *gfx, ScoreKeeper *scoreKeeper)
{
  this->scoreKeeper = scoreKeeper;
  this->gfx = gfx;

  hudSprite = gfx->createSpriteInstance("hud");
  gfx->setAnimation(hudSprite, "normal");
  gfx->showSprite(hudSprite, true);
  gfx->moveSpriteAbs(hudSprite, 32, 8);
  gfx->neverRedraw(hudSprite, true);

  int xOffset = 32 + 8 + 8;
  int yOffset = 32 + 16 + 5;

  for(int i = 0;i < 8;i++)
    {
      ammoSprites[i] = gfx->createSpriteInstance("ammo");
      gfx->setAnimation(ammoSprites[i], "show");
      gfx->showSprite(ammoSprites[i], true);
      gfx->moveSpriteAbs(ammoSprites[i], xOffset + i*15, yOffset);

      reloadSprites[i] = gfx->createSpriteInstance("reload");
      gfx->setAnimation(reloadSprites[i], "hide");
      gfx->showSprite(reloadSprites[i], true);
      gfx->moveSpriteAbs(reloadSprites[i], xOffset + i*15 - 1,
			 yOffset - 32 + 1);
    }

  int lengths[6] = { 8, 8, 3, 2, 2, 1 };
  int pos_x[6] = {298, 522, 714, 298, 458, 618};
  int pos_y[6] = {20, 20, 20, 50, 50, 50};

  for(int i = 0;i < 6;i++)
    {
      counters[i] = new Counter(gfx, lengths[i]);
      counters[i]->setPosition(pos_x[i], pos_y[i]);
    }

  powerText = gfx->print("text", "power");
  laserText = gfx->print("text", "laser");
  gfx->showSprite(powerText, false);
  gfx->moveSpriteAbs(powerText, 32*21+10, 32*1+16+2);
  gfx->showSprite(laserText, false);
  gfx->moveSpriteAbs(laserText, 32*21+10, 32*1+16+2);

  setValue(SCORE, scoreKeeper->getScore());
  setValue(TOP, scoreKeeper->getTopScore());
  setValue(TIME, 200);
  setValue(LIVES, scoreKeeper->getLives());
  setValue(STAGE, scoreKeeper->getLevel());
  setValue(SKILL, scoreKeeper->getSkillLevel());
}

HUD::~HUD()
{
  gfx->showSprite(hudSprite, false);

  for(int i = 0;i < 6;i++)
    delete counters[i];

  for(int i = 0;i < 8;i++)
    {
      gfx->removeSpriteInstance(ammoSprites[i]);
      gfx->removeSpriteInstance(reloadSprites[i]);
    }

  gfx->removeSpriteInstance(powerText);
  gfx->removeSpriteInstance(laserText);
}

void
HUD::update()
{

}

void
HUD::setValue(Display display, int value)
{
  if(display < RELOAD)
    counters[display]->setCounter(value);
  else if(display == BULLET)
    setAmmo(value);
  else if(display == RELOAD)
    setReload(value);
  else if(display == WEAPON)
    setWeapon(value);
}

void
HUD::setAmmo(int value)
{
  for(int i = 0;i < 8;i++)
    {
      if(i < value)
	gfx->setAnimation(ammoSprites[i], "show");
      else
	gfx->setAnimation(ammoSprites[i], "hide");
    }
}

void
HUD::setReload(int value)
{
  for(int i = 0;i < 8;i++)
    {
      if(i < value)
	gfx->setAnimation(reloadSprites[i], "show");
      else
	gfx->setAnimation(reloadSprites[i], "hide");
    }
}

void
HUD::setWeapon(int value)
{
  switch(value)
    {
    case 0: // No special weapon
      gfx->showSprite(powerText, false);
      gfx->showSprite(laserText, false);
      break;

    case 1: // POWER gun
      gfx->showSprite(powerText, true);
      gfx->showSprite(laserText, false);
      break;

    case 2: // LASER gun
      gfx->showSprite(powerText, false);
      gfx->showSprite(laserText, true);
      break;
    }
}
