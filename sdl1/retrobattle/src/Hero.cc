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

#include "Hero.h"
#include "PowerBullet.h"
#include "LaserBeam.h"

Hero::Hero(remar2d *gfx, SoundManager *sfx, list<Bullet *> *bullets,
	   BulletHandler *bulletHandler, HUD *hud)
  : Object(gfx, "good", sfx), channel(-1), bullets(bullets),
    bulletHandler(bulletHandler), hud(hud)
{
  setBoundingBox(6, 20, 5, 4);

  flame = gfx->createSpriteInstance("flame");
  gfx->setAnimation(flame, "right");
  gfx->showSprite(flame, false);

  note = gfx->createSpriteInstance("note");
  gfx->setAnimation(note, "8");
  gfx->showSprite(note, false);

  reset();  
}

Hero::~Hero()
{
  /* Stop walking sound */
  if(channel != -1)
    sfx->stopSound(channel);

  gfx->showSprite(flame, false);
  gfx->removeSpriteInstance(flame);

  gfx->showSprite(note, false);
  gfx->removeSpriteInstance(note);
}

int getDir(float dir)
{
  if(dir < 0.0)
    return -1;
  else if(dir == 0.0)
    return 0;
  else
    return 1;
}

void
Hero::setDirection(float dir)
{
  if(dir < 0.0)
    direction = LEFT;
  else if(dir > 0.0)
    direction = RIGHT;
}

void
Hero::moveRel(float xDir, float yDir)
{
  if(dead)
    return;

  bool inAir = true;

  /* When on the ground, yDir == 0 and jumping == false*/
  if(yDir == 0 && jumping == false)
    {
      jumpCounter = 80;
      inAir = false;
    }
  else if(yDir >= 0.0)
    {
      /* We've either hit the roof or let go of the jump button, don't
	 allow more jumping */
      jumpCounter = -1;
    }

  if(blinking)
    {
      if(xDir != 0.0 || yDir != 0.0)
	blinking = false;
      else
	return;
    }

  setDirection(xDir);

  if((xDir != oldXDir || yDir != oldYDir) && blinking == false)
    updateAnimation(xDir, yDir);

  oldXDir = xDir;
  oldYDir = yDir;

  Object::moveRel(xDir, yDir);

//   if(noteShown)
    {
      gfx->moveSpriteAbs(note, getX()+4, getY()-13);
    }

  if(inAir)
    {
      gfx->showSprite(flame, true);
      if(direction == LEFT)
	{
	  if(flameShown == false || flameDirection == RIGHT)
	    {
	      gfx->setAnimation(flame, "left");
	      flameDirection = LEFT;
	    }
	  gfx->moveSpriteAbs(flame, getX()+13, getY()+13);
	}
      else
	{
	  if(flameShown == false || flameDirection == LEFT)
	    {
	     gfx->setAnimation(flame, "right");
	     flameDirection = RIGHT;
	    }
	  gfx->moveSpriteAbs(flame, getX()-4, getY()+13);
	}
      flameShown = true;
    }
  else
    {
      gfx->showSprite(flame, false);
      flameShown = false;
    }

}

void
Hero::setVisible(bool visible)
{
  Object::setVisible(visible);

  if(flameShown)
    gfx->showSprite(flame, visible);

  if(noteShown)
    gfx->showSprite(note, visible);

  if(!visible && channel != -1)
    {
      sfx->stopSound(channel);
      channel = -1;
      hasStoppedSound = true;
    }
  else if(visible && hasStoppedSound)
    {
      channel = sfx->playSound(11, true);
      hasStoppedSound = false;
    }
}

void
Hero::stop()
{
  if(direction == LEFT)
    setAnimation("stand left");
  else
    setAnimation("stand right");
}

void
Hero::jump(bool on)
{
  if(on == false)
    jumpCounter = -1;
  jumping = on;

  if(on && jumpCounter == 80)
    sfx->playSound(4, false);
}

bool
Hero::jumps(int decrease)
{
  jumpCounter -= decrease;

  if(jumpCounter < 0)
    {
      jumping = false;
    }

  return jumping;
}

void
Hero::updateAnimation(float xDir, float yDir)
{
  /* getY()%32 == 8 assures us that we're on the ground */
  if(xDir == 0 && yDir == 0 && (getY()%32)==8)
    {
      if(direction == LEFT)
	setAnimation("stand left");
      else
	setAnimation("stand right");

      if(channel != -1)
	{
	  sfx->stopSound(channel);
	  channel = -1;
	}

      currentAnimation = STANDING;
    }
  else if(yDir == 0 && (getY()%32)==8)
    {
      if(direction == LEFT)
	setAnimation("run left");
      else
	setAnimation("run right");

      if(channel == -1)
	{
	  channel = sfx->playSound(11, true);
	}

      currentAnimation = WALKING;
    }
  else
    {
      if(direction == LEFT)
	setAnimation("fly left");
      else
	setAnimation("fly right");

      if(channel != -1)
	{
	  sfx->stopSound(channel);
	  channel = -1;
	}

      currentAnimation = FLYING;
    }
}

void
Hero::shoot()
{
  if(dead)
    return;

  if(blinking)
    {
      blinking = false;
      updateAnimation(0, 0);
    }

  Bullet *b = 0;

  switch(weapon)
    {
    case STANDARD:
      b = new Bullet(gfx, sfx); 
      b->setVisible(true);
      break;

    case POWER:
      b = new PowerBullet(gfx, sfx);
      b->setVisible(true);
      weapon = STANDARD;
      hud->setValue(HUD::WEAPON, 0 /* nothing  */);
      break;

    case LASER:
      b = new LaserBeam(gfx, sfx);
      weapon = STANDARD;
      hud->setValue(HUD::WEAPON, 0 /* nothing  */);
      break;
    }

  if(direction == LEFT)
    {
      b->moveAbs(getX()-4, getY()+8);
      b->moveLeft();
    }
  else if(direction == RIGHT)
    {
      b->moveAbs(getX()+16, getY()+8);
      b->moveRight();
    }

  bullets->push_back(b);

  sfx->playSound(6, false);
}

void
Hero::die()
{
  if(dead || blinking)
    return;

  dead = true;

  gfx->showSprite(flame, false); /* just in case... */
  gfx->showSprite(note, false);

  noteTimer = 0;
  noteShown = false;

  /* Stop walking sound */
  if(channel != -1)
    sfx->stopSound(channel);

  sfx->playSound(2, false);

  if(direction == LEFT)
    setAnimation("death left");
  else
    setAnimation("death right");

  deathTimer = 150;
}

void
Hero::update(Input *input, Field *field)
{
  if(noteTimer)
    {
      if(--noteTimer == 0)
	{
	  noteShown = false;
	  gfx->showSprite(note, false);
	}
    }

  if(dead)
    {
      deathTimer--;

      if(deathTimer == 0)
	destroyMe = true;

      return;
    }

  int move_x = 0;
  int move_y = 1; /* Constantly fall.. :-) */

  //if(input->pressed(SDLK_LEFT))
  if(input->actionPressed(Input::LEFT))
    {
      if(primaryDirection == NONE)
	primaryDirection = LEFT;
      else if(primaryDirection == RIGHT && secondaryDirection == NONE)
	secondaryDirection = LEFT;
    }

  if(input->actionPressed(Input::RIGHT))
    {
      if(primaryDirection == NONE)
	primaryDirection = RIGHT;
      else if(primaryDirection == LEFT && secondaryDirection == NONE)
	secondaryDirection = RIGHT;
    }

  if(input->actionReleased(Input::LEFT))
    {
      if(secondaryDirection == LEFT)
	secondaryDirection = NONE;
      else if(secondaryDirection == RIGHT)
	{
	  primaryDirection = RIGHT;
	  secondaryDirection = NONE;
	}
      else if(primaryDirection == LEFT)
	primaryDirection = NONE;
    }

  if(input->actionReleased(Input::RIGHT))
    {
      if(secondaryDirection == RIGHT)
	secondaryDirection = NONE;
      else if(secondaryDirection == LEFT)
	{
	  primaryDirection = LEFT;
	  secondaryDirection = NONE;
	}
      else if(primaryDirection == RIGHT)
	primaryDirection = NONE;
    }

  int x_dir[3] = {0, -1, 1};

  if(secondaryDirection != NONE)
    move_x = x_dir[secondaryDirection];
  else if(primaryDirection != NONE)
    move_x = x_dir[primaryDirection];

  if(input->actionPressed(Input::JUMP))  jump(true);
  if(input->actionReleased(Input::JUMP)) jump(false);
  if(input->actionPressed(Input::FIRE)
     && (!bulletHandler || bulletHandler->fire()))
    {
      shoot();
    }

  if(jumps(1))
    move_y = -1;

  int heroX = getX();
  int heroY = getY();

  if(heroY + 24 == field->SPIKES_LEVEL)
    die();

  if(heroX < -24-5)
    {
      moveAbs(800+16-5, heroY);
    }
  else if(heroX > 800+16-5)
    {
      moveAbs(-24-5, heroY);
    }
  else
    {
      field->moveObjectRel(this, &move_x, &move_y);
      moveRel(move_x, move_y);
    }
}

void
Hero::blink()
{
  blinking = true;
  setAnimation("blink right");
}

void
Hero::showNote(int ammo)
{
  if(dead)
    return;

  const char *ammoString[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};

  gfx->showSprite(note, true);
  gfx->setAnimation(note, ammoString[ammo]);

  noteShown = true;
  noteTimer = 60;
}

void
Hero::powerShot()
{
  weapon = POWER;
  hud->setValue(HUD::WEAPON, 1 /* POWER */);
}

void
Hero::laserShot()
{
  weapon = LASER;
  hud->setValue(HUD::WEAPON, 2 /* LASER */);
}

void
Hero::reset()
{
  blink();
  setVisible(true);

  destroyMe = false;
  destroyTimer = 0;

  gfx->showSprite(flame, false);
  gfx->showSprite(note, false);

  if(channel != -1)
    sfx->stopSound(channel);

  jumping = false;
  dead = false;
  direction = RIGHT;
  weapon = STANDARD;
  jumpCounter = 80;
  currentAnimation = STANDING;
  channel = -1;
  oldXDir = 0;
  oldYDir = 0;
  noteShown = false;
  hasStoppedSound = false;
  primaryDirection = NONE;
  secondaryDirection = NONE;
}
