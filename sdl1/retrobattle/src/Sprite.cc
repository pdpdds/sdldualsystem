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

#include "Sprite.h"

Sprite::Sprite(const char *file)
  : name((char *)"")
{
  TiXmlDocument doc(file);

  if(!doc.LoadFile())
    {
      return;
    }

  TiXmlNode *sprite = 0, *animation = 0, *node = 0;
  TiXmlElement *element = 0;

  /* Locate the 'sprite' node */
  sprite = doc.FirstChild("sprite");
  if(sprite)
    {
      TiXmlElement *sprite_elm = sprite->ToElement();

      const char *n = sprite_elm->Attribute("name");
      name = new char[strlen(n) + 1];
      strncpy(name, n, strlen(n) + 1);

      while((animation = sprite->IterateChildren("animation", animation)))
	{
	  element = animation->ToElement();

	  startNewAnimation(element);

	  while((node = animation->IterateChildren(node)))
	    {
	      element = node->ToElement();
	      if(strcmp(element->Value(), "image") == 0)
		{
		  addImageToAnimation(element, file);
		}
	      else if(strcmp(element->Value(), "frame") == 0)
		{
		  addFrameToAnimation(element);
		}
	    }

	  animations[currentAnimation]->framesAdded();
	}
    }
}

Sprite::Sprite()
  : name(0)
{
  
}

Sprite::~Sprite()
{
  /* Delete animations */
  delete [] name;
  animations.clear();
}

const char *
Sprite::getName()
{  
  return (const char *)name;
}

void
Sprite::setName(const char *n)
{
  if(name)
    delete [] name;

  name = new char[strlen(n) + 1];
  strncpy(name, n, strlen(n) + 1);
}

Animation *
Sprite::getAnimation(const char *animName)
{
  return animations[animName];
}

void
Sprite::addAnimation(Animation *animation)
{
  animations[string(animation->getName())] = animation;
}

void
Sprite::startNewAnimation(TiXmlElement *element)
{
  const char *name = element->Attribute("name");
  const char *loop = element->Attribute("loop");
  Animation *anim = new Animation(name);
  
  if(loop && strcmp(loop, "false") == 0)
    {
      anim->setLooping(false);
    }

  animations[string(name)] = anim;
  currentAnimation = name;
}

void
Sprite::addImageToAnimation(TiXmlElement *element, const char *pathToXml)
{
  int pathLen;
  for(int i = strlen(pathToXml);i >= 0;i--)
    {
      if(pathToXml[i] == '/')
	{
	  pathLen = i + 1;
	  break;
	}
    }

  const char *path = element->Attribute("path");
  int size_x = atoi(element->Attribute("size_x"));
  int size_y = atoi(element->Attribute("size_y"));
  int orig_x = atoi(element->Attribute("orig_x"));
  int orig_y = atoi(element->Attribute("orig_y"));
  const char *key = element->Attribute("key");

  char *pathToImage = new char[strlen(path) + pathLen + 1];
  strncpy(pathToImage, pathToXml, pathLen);
  strncpy(&pathToImage[pathLen], path, strlen(path));
  pathToImage[strlen(path) + pathLen] = '\0';

  animations[currentAnimation]->addImage(pathToImage, size_x, size_y,
					 orig_x, orig_y, key);

  delete[] pathToImage;
}

void
Sprite::addFrameToAnimation(TiXmlElement *element)
{
  int x = 0, y = 0, dur = 0;
  bool emptyFrame = false;

  const char *empty = element->Attribute("empty");

  if(empty && strcmp(empty, "true") == 0)
    {
      emptyFrame = true;
    }
  else
    {
      x = atoi(element->Attribute("x"));
      y = atoi(element->Attribute("y"));
    }

  dur = atoi(element->Attribute("dur"));

  Frame *frame = new Frame(x, y, dur, emptyFrame);

  animations[currentAnimation]->addFrame(frame);
}
