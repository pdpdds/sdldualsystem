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

#include "Character.h"

Character::Character(SDL_Surface *img, SDL_Rect *rect)
{
  image = img;
  character.w = rect->w;
  character.h = rect->h;
  character.x = rect->x;
  character.y = rect->y;
}

Character::~Character()
{

}

SDL_Surface *
Character::getImage()
{
  return image;
}

SDL_Rect *
Character::getRect()
{
  return &character;
}
