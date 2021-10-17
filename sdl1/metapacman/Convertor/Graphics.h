/*============================================================================||
||       __  ___  _______    ____      _____  __ _______					  || 
||      /  \/  /_/__  __/_ _/ /_/___  /_/   \/ //__  __/                      ||
||     / /\_/ / ___/ / __ `/ _-/ __ \__/ () /  \  / /                         ||
||    / /  / / __// / /_/ /  \/ / / / /\_  / /\ \/ /                          ||
||   /_/  /_/____/_/\__,_/_/_/_/ /_/_/__/ /_/ /_/_/                           ||
||                                /______/                                    ||
||                                                                            ||
||============================================================================||
|| Graphics																	  ||
|| Copyright Jeff Harlan. All Rights Reserved.                                ||
||============================================================================*/        
//To Do
/*
||	Rotation
||	Zoom
*/

#ifndef _GRAPHICS_H
	#define _GRAPHICS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <SDL.h>
#include <SDL_image.h>

#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff

#define TTF_TYPE_SOLID			0x00
#define TTF_TYPE_SHADED			0x01
#define TTF_TYPE_BLENDED		0x02

//-----------------------------------------------------------------------------
void Blit(SDL_Surface *Dest, int DestX, int DestY, SDL_Surface *Src);

void Blit(SDL_Surface *Dest, int DestX, int DestY,
					SDL_Surface *Src, int SrcX, int SrcY, int SrcW, int SrcH);

void DrawPixel(SDL_Surface *Dest, int DestX, int DestY, Uint8 R, Uint8 G, Uint8 B);

void DrawPixel(SDL_Surface *Dest, int DestX, int DestY, SDL_Color Color);

Uint32 GetPixel(SDL_Surface *Dest, int DestX, int DestY);

void DrawLine(SDL_Surface *Dest, double OriginX, double OriginY, double TargetX, double TargetY, Uint8 R, Uint8 G, Uint8 B);

void SurfaceLock(SDL_Surface *Dest);

void SurfaceUnlock(SDL_Surface *Dest);

void Transparent(SDL_Surface *Dest, int R, int G, int B);

void Transparent(SDL_Surface *Dest, SDL_Color Color);

SDL_Surface* ImageLoad(char *cFile);

SDL_Color SetColor(int R, int G, int B);

SDL_Surface* CreateRGBSurface(int Flags, int Width, int Height, int Bits);

//==============================================================================

#endif

