/*============================================================================||
||       __  ___  _______    ___  ____  __         ___                        ||
||      /  \/  /_/__  __/_ _/ __\/_/  \/ /   _____/   \                       ||
||     / /\_/ / ___/ / __ `/ /  __/ __/  \  / ___/  `_/                       ||
||    / /  / / __// / /_/ / /__/ / / / /\ \/ __// /\ \                        ||
||   /_/  /_/____/_/\__,_/\___/_/_/ /_/ /_/____/_/ /_/                        ||
||                                                                            ||
||============================================================================||
|| CHighScore																  ||
|| Copyright Tim Jones. All Rights Reserved.                                  ||
||============================================================================*/


#ifndef _CHIGHSCORE_H_
	#define _CHIGHSCORE_H_
	
#include "define.h"
#include "graphics.h"

#include <SDL.h>
#include <iostream>

//==============================================================================

using namespace std;

//==============================================================================

class CHighScore {
	private:
		
		struct SEntry {
			char		Name[3];
		
			int			Score;
		
			char		Difficulty;
		};
		
	public:
		SDL_Surface*	Display;
		
		SDL_Surface*	Surf_HighScore;
		
		SEntry 			Entry[18];
		
		bool			EnterName;
		
		int				EntryID;
		
		int				CursorX;
		
		int				CursorY;
		
		int 			NameID[3];
		
		int				Pos;
		
	public:
		CHighScore();
		
		void OnEvent(SDL_Event* Event);
		
		bool OnInit();
		
		void OnRender();
		
		void OnCleanup();
		
		void CheckScore(int Score);
		
		void LoadHighScore();
		
		void WriteHighScore();
};

//==============================================================================

#endif
