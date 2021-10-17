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

#include "Spawner.h"

Spawner::Spawner(list<Object *> *objects, int type, int maxNumber)
  : numberOfNests(0), nestLockTimer(0)
{
  this->objects = objects;
  this->type = type;
  this->maxNumber = maxNumber;

  startSpawn = true;
  inQueue = maxNumber;
  countDown = 60; /* 60 frames to next spawn */

  list<Object *>::iterator it;
  for(it = objects->begin();it != objects->end();it++)
    {
      if(Nest *nest = dynamic_cast<Nest *>(*it))
	{
	  nests.push_back(nest);
	  numberOfNests++;
	}
    }
}

void
Spawner::update()
{
  if(nestLockTimer)
    {
      if(--nestLockTimer == 0)
	{
	  // Unlock nests
	  for(list<Nest *>::iterator it = nests.begin();it != nests.end();it++)
	    {
	      (*it)->lock(false);
	    }
	}

      return;
    }

  list<Nest *>::iterator it;
  int i;

  if(inQueue == 0)
    return;

  countDown--;
  if(countDown <= 0)
    {
      bool nestFound = false;

      while(nestFound == false)
	{
	  int n = rand() % numberOfNests;

	  for(it = nests.begin(), i = 0;it != nests.end() && i != n;it++, i++);
	  if((*it)->isSpawning() == false)
	    {
	      (*it)->spawn();
	      nestFound = true;
	    }
	}

      inQueue--;

      if(startSpawn && inQueue == 0)
	startSpawn = false;

      if(startSpawn)
	countDown = 60;
      else
	countDown = 60 * 8; /* 8 seconds */
    }
}

void
Spawner::addFuzz()
{
  inQueue++;
}

void
Spawner::lockNests()
{
  nestLockTimer = 50*60; // 50 seconds

  cancelSpawningNests();

  for(list<Nest *>::iterator it = nests.begin();it != nests.end();it++)
    {
      (*it)->lock(true);
    }
}

void
Spawner::cancelSpawningNests()
{
  for(list<Nest *>::iterator it = nests.begin();it != nests.end();it++)
    {
      if((*it)->isSpawning())
	{
	  (*it)->cancelSpawning();

	  // Spawn fuzz a little later
	  addFuzz();
	}
    }
}
