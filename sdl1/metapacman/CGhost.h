/*============================================================================||
||       __  ___  _______    ___  ____  __         ___                        ||
||      /  \/  /_/__  __/_ _/ __\/_/  \/ /   _____/   \                       ||
||     / /\_/ / ___/ / __ `/ /  __/ __/  \  / ___/  `_/                       ||
||    / /  / / __// / /_/ / /__/ / / / /\ \/ __// /\ \                        ||
||   /_/  /_/____/_/\__,_/\___/_/_/ /_/ /_/____/_/ /_/                        ||
||                                                                            ||
||============================================================================||
|| CGhost																	  ||
|| Copyright Tim Jones. All Rights Reserved.                                  ||
||============================================================================*/


#ifndef _CGHOST_H_
	#define _CGHOST_H_
	
#include "define.h"
#include "graphics.h"

#include <SDL.h>
#include <iostream>

#include "Path.h"

#include "CMap.h"
#include "CPacman.h"

//==============================================================================

using namespace std;

//==============================================================================

class CGhost {

	//Surfaces
	public:
		static SDL_Surface*	Display;
		
		static SDL_Surface* Surf_Dead;
		
		static SDL_Surface* Surf_Vulnerable;	//All ghosts will use the same 
												//Surface for when they are 
												//vulnerable
	
		SDL_Surface*	Surf_Ghost;
		
		static CPath 	Path;		//Used to calculate a path
		
	public:
		bool 			Vulnerable;	//Is this ghost vulnerable to be eatten?
		
		int				WaitTime;	//Time to wait before ghost can move again
  
    private:		
		double			MoveTime;	//Time since last move in sliding
		
		int				OffsetX;	//Offset coords when sliding to the next tile
		
		int				OffsetY;		
		
	public:		
		int				Dir;		//Direction of ghost
		
		int				X;			//Coords on map
		
		int				Y;
		
		bool			Dead;
		
	private:
		int				Frame;		//Current frame in animation
		
		double			OldTime;	//Time since last movement
		
	public:
		CGhost();
		
		bool OnInit(char* File);
		
		void OnLoop();
		
		void OnRender();
		
		void OnCleanup();
		
		void Reset();
		
		bool HasCollision(int X, int Y);
		
		void SetCoords(int newX, int newY);
};

//==============================================================================

#endif

