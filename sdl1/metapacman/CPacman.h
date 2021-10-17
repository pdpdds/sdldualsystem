/*============================================================================||
||       __  ___  _______    ___  ____  __         ___                        ||
||      /  \/  /_/__  __/_ _/ __\/_/  \/ /   _____/   \                       ||
||     / /\_/ / ___/ / __ `/ /  __/ __/  \  / ___/  `_/                       ||
||    / /  / / __// / /_/ / /__/ / / / /\ \/ __// /\ \                        ||
||   /_/  /_/____/_/\__,_/\___/_/_/ /_/ /_/____/_/ /_/                        ||
||                                                                            ||
||============================================================================||
|| CPacman																	  ||
|| Copyright Tim Jones. All Rights Reserved.                                  ||
||============================================================================*/

#ifndef _CPACMAN_H_
	#define _CPACMAN_H_
	
#include "define.h"
#include "graphics.h"

#include <SDL.h>
#include <iostream>

#include "CMap.h"

//==============================================================================

using namespace std;

//==============================================================================

class CPacman {
	public:
		SDL_Surface*	Display;
		
		static CPacman	Pacman;			//Singleton to Pacman
		
		bool			Dieing;			//Is pacman dieing?
	
	private:
		SDL_Surface*	Surf_Pacman;
		
	public:
		int				Dir;
		
		int				Skin;
		
	private:
		double			DieStartTime;	//When did pacman start dieing?
		
		double			MoveTime;		//Time since last move in sliding
		
	public:
		float			X;				//Coords on map
		
		float			Y;
	
		float			NewX;			//Where we want to move
		
		float			NewY;
	
		int				OffsetX;		//Coords for sliding
		
		int				OffsetY;
		
	private:
		int				Frame;			//Current frame in animation
		
	public:
		CPacman();
		
		void OnEvent();
		
		bool OnInit();
		
		void OnLoop();
		
		void OnRender();
		
		void OnCleanup();
		
		void Kill();
		
		void Reset();
};

//==============================================================================

#endif

