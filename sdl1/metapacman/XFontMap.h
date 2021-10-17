/*============================================================================||
||       __  ___  _______    ___  ____  __         ___                        ||
||      /  \/  /_/__  __/_ _/ __\/_/  \/ /   _____/   \                       ||
||     / /\_/ / ___/ / __ `/ /  __/ __/  \  / ___/  `_/                       ||
||    / /  / / __// / /_/ / /__/ / / / /\ \/ __// /\ \                        ||
||   /_/  /_/____/_/\__,_/\___/_/_/ /_/ /_/____/_/ /_/                        ||
||                                                                            ||
||============================================================================||
|| XFontMap																	  ||
|| Copyright Tim Jones. All Rights Reserved.                                  ||
||============================================================================*/

#ifndef _XFONTMAP_H_
	#define _XFONTMAP_H_

#include <SDL.h>
#include <SDL_ttf.h>

#include <iostream>

#include "graphics.h"

//==============================================================================

using namespace std;

//==============================================================================
class XFontMap {
	public:
		TTF_Font* 		FontControl;

		bool			Crop;

		SDL_Color		FontColor;

		SDL_Color		FontBGColor;

		SDL_Surface*	Surf_FontMap;

	private:
		int 			CharWidth;	//Largest Characters Width

		int 			CharHeight; //Height of Characters

	public:
		XFontMap();
		~XFontMap();

		void OnCleanup();

		bool Initialize(char* File, int Pointsize, int Bit);

		void Print(SDL_Surface* Display, int X, int Y, char* Text, ...);

		void Print(SDL_Surface* Display, int X, int Y, int Pos, int MaxWidth, const char* Text, ...);

		void PrintClip(SDL_Surface* Display, int X, int Y, int MaxWidth, int MaxHeight, const char* Text, ...);

		void RenderFontMap(int Type);
		
		int GetWidth(char* Text);
};

//==============================================================================

#endif
