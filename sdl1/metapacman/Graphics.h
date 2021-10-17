/*============================================================================||
||       __  ___  _______    ___  ____  __         ___                        ||
||      /  \/  /_/__  __/_ _/ __\/_/  \/ /   _____/   \                       ||
||     / /\_/ / ___/ / __ `/ /  __/ __/  \  / ___/  `_/                       ||
||    / /  / / __// / /_/ / /__/ / / / /\ \/ __// /\ \                        ||
||   /_/  /_/____/_/\__,_/\___/_/_/ /_/ /_/____/_/ /_/                        ||
||                                                                            ||
||============================================================================||
|| Graphics																	  ||
|| Copyright Tim Jones. All Rights Reserved.                                  ||
||============================================================================*/

#ifndef _GRAPHICS_H_
	#define _GRAPHICS_H_

#include <SDL.h>
#include <SDL_image.h>

#define 	RMASK 				0xff000000
#define 	GMASK 				0x00ff0000
#define 	BMASK 				0x0000ff00
#define 	AMASK 				0x000000ff

#define 	TTF_TYPE_SOLID		0x00
#define 	TTF_TYPE_SHADED		0x01
#define 	TTF_TYPE_BLENDED	0x02

//==============================================================================

void Blit(SDL_Surface* Display, SDL_Surface* Image, int X, int Y);

void Blit(SDL_Surface* Display, SDL_Surface* Image, int X, int Y, int X2, int Y2, int Width, int Height);

void Transparency(SDL_Surface* Surface, SDL_Color Color);

SDL_Surface* ImageLoad(char* File);

SDL_Color SetColor(int R, int G, int B);

SDL_Surface *ctoc(SDL_Surface* surf, SDL_Color from, SDL_Color to);

void ClearImage(SDL_Surface* Surface, Uint32 Color);

void SetAlpha(SDL_Surface* Surface, int Alpha);

//==============================================================================

#endif
