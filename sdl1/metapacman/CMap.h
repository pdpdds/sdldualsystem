/*============================================================================||
||       __  ___  _______    ___  ____  __         ___                        ||
||      /  \/  /_/__  __/_ _/ __\/_/  \/ /   _____/   \                       ||
||     / /\_/ / ___/ / __ `/ /  __/ __/  \  / ___/  `_/                       ||
||    / /  / / __// / /_/ / /__/ / / / /\ \/ __// /\ \                        ||
||   /_/  /_/____/_/\__,_/\___/_/_/ /_/ /_/____/_/ /_/                        ||
||                                                                            ||
||============================================================================||
|| CMap																		  ||
|| Copyright Tim Jones. All Rights Reserved.                                  ||
||============================================================================*/


#ifndef _CMAP_H_
	#define _CMAP_H_
	
#include "define.h"
#include "graphics.h"

#include <SDL.h>
#include <iostream>

#include "CNibblet.h"

//==============================================================================

using namespace std;

//==============================================================================
class CTile {
	public:
		CTile() {
			TypeID = 0;

			F = 0;
			G = 0;
			H = 0;

			ParentID = -1;
		};

		int TypeID;		//The type of tile

		int F, G, H;	//Used for pathing
						//F = G + H
						//G = Distance from start to current tile (actual)
						//H = Distance from current tile to end (guess)

		int ParentID;	//Used for pathing
						//The connecting tile
};

//------------------------------------------------------------------------------
class CMap {
	public:
		static CMap	MapControl;
		
		static CTile TileList[MAP_AREA];	//Our tiles
		
		static SDL_Surface* Surf_Level;
		
		static char CurrentLevel[255];		//The current level
		
		static char NextLevel[255];			//Next level to load
		
		static bool CutScene;
		
	public:
		CMap();
	
		bool LoadMap(char* File);
	
		int GetTile(int X, int Y);
		
		int GetTile(int ID);		
};

//==============================================================================

#endif

