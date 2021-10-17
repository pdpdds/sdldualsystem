/*============================================================================||
||       __  ___  _______    ___  ____  __         ___                        ||
||      /  \/  /_/__  __/_ _/ __\/_/  \/ /   _____/   \                       ||
||     / /\_/ / ___/ / __ `/ /  __/ __/  \  / ___/  `_/                       ||
||    / /  / / __// / /_/ / /__/ / / / /\ \/ __// /\ \                        ||
||   /_/  /_/____/_/\__,_/\___/_/_/ /_/ /_/____/_/ /_/                        ||
||                                                                            ||
||============================================================================||
|| CNibblet																	  ||
|| Copyright Tim Jones. All Rights Reserved.                                  ||
||============================================================================*/


#ifndef _CNIBBLET_H_
	#define _CNIBBLET_H_
	
#include "define.h"
#include "graphics.h"

#include <SDL.h>
#include <iostream>

//==============================================================================

using namespace std;

//==============================================================================

class CNibblet {
	public:
		static SDL_Surface*		Display;
	
		static SDL_Surface*		Surf_Nibblet;	//All nibblets use the same surface
	
		int		Type;	//Big or Small nibblet?
		
	private:
		int		X;				//Coords on map
		
		int		Y;
		
		int 	Frame;			//Current frame
		
		double	FrameTime;		//Last time since animation
		
	public:
		CNibblet();
		
		static bool OnInit();
		
		void OnRender();
		
		void SetCoords(int newX, int newY);
		
		bool HasCollision(int X, int Y);
};

//==============================================================================

#endif

