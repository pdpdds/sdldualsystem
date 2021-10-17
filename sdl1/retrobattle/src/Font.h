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

#ifndef FONT_H
#define FONT_H

#include "SDL.h"
#include "tinyxml.h"
#include <string>
#include <map>
#include "Character.h"

class Font
{
 public:
  Font(const char *file);
  ~Font();

  const char *getName();

  int getWidth();
  int getHeight();

  Character *getCharacter(char ch);
  
 private:
  char *addImageToFont(TiXmlElement *element, const char *pathToXml);
  void addCharToFont(TiXmlElement *element, char *currentImage);

  char *name;
  int size_x, size_y;

  /* The character set of this font */
  std::map<char, Character *> characters;

  /* The images that this font consists of */
  std::map<std::string, SDL_Surface *> images;
};

#endif
