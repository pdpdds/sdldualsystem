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

#ifndef SPAWNER_H
#define SPAWNER_H

#include "Nest.h"
#include "Fuzz.h"
#include "Enemy.h"
#include <list>

/* Spawn fuzzies when needed and with correct interval */

class Spawner
{
 public:
  //Spawner(Nest **nest, Fuzz **fuzz, int type, int maxNumber);
  Spawner(list<Object *> *objects, int type, int maxNumber);
  void update();
  void addFuzz();
  void lockNests();
  void cancelSpawningNests();

 private:
  // Fuzz **findEmptyFuzz();

  // Nest **nest;
  // Fuzz **fuzz;
  list<Object *> *objects;
  list<Nest *> nests;
  int type; /* 0 = Blue Fuzz, 1 = Red Fuzz */
  int maxNumber;
  int numberOfNests;

  int countDown; /* Countdown to next spawn */

  int inQueue; /* Number of fuzzes waiting to spawn */

  bool startSpawn; /* Indicates if this is the fast "start spawn" */

  int nestLockTimer;
};

#endif
