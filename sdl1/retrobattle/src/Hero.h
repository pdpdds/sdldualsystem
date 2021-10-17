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

#ifndef HERO_H
#define HERO_H

#include <remar2d.h>
#include <list>
#include "Object.h"
#include "Bullet.h"
#include "BulletHandler.h"
#include "HUD.h"
#include "Field.h"
#include "Input.h"

class BulletHandler; // defined in BulletHandler.h (FIXME: Circular dependency)

class Hero : public Object
{
 public:
  Hero(remar2d *gfx, SoundManager *sfx, list<Bullet *> *bullets,
       BulletHandler *bulletHandler, HUD *hud);
  ~Hero();
  void moveRel(float xDir, float yDir); //, bool onGround);
  void setVisible(bool visible);
  void stop();
  void jump(bool on);
  bool jumps(int decrease);
  void shoot(); //int *bullets, Bullet **bullet);
  void die();
  void update(Input *input, Field *field);
  void blink();
  bool isBlinking() { return blinking; }
  bool isDead() { return dead; }
  void showNote(int ammo);
  void powerShot(); // Fire a POWER bullet on next shoot()
  void laserShot(); // Fire a LASER beam on next shoot()
  void reset();

 private:
  void updateAnimation(float xDir, float yDir);
  void setDirection(float dir);

  enum Direction
    {
      NONE,
      LEFT,
      RIGHT
    };

  enum Weapon
    {
      STANDARD,
      POWER,
      LASER
    };

  Direction direction;
  Weapon weapon;

  int jumpCounter;
  bool jumping;
  bool allowJumping;
  bool falling;

  bool blinking;
  bool dead;

  /* The flame sprite (displayed when jumping) */
  int flame;
  bool flameShown;
  Direction flameDirection;

  /* The "note" sprite (showing how many bullets are currently available) */
  int note;
  bool noteShown;
  int noteTimer; // Countdown how long note should be shown

  float oldXDir;
  float oldYDir;

  enum Animation
    {
      FLYING,
      WALKING,
      STANDING
    };

  Animation currentAnimation;
  
  /* channel that the walk sound is played on */
  int channel;

  int deathTimer;

  list<Bullet *> *bullets;
  BulletHandler *bulletHandler;

  HUD *hud;

  bool hasStoppedSound;

  // These are used to allow twitch-turn (if player presses first left
  // then right, Hero should go right until right is released, then go
  // left).
  Direction primaryDirection;
  Direction secondaryDirection;
};

#endif
