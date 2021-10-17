/*============================================================================|
||       __  ___  _______    ___  ____  __         ___                        ||
||      /  \/  /_/__  __/_ _/ __\/_/  \/ /   _____/   \                       ||
||     / /\_/ / ___/ / __ `/ /  __/ __/  \  / ___/  `_/                       ||
||    / /  / / __// / /_/ / /__/ / / / /\ \/ __// /\ \                        ||
||   /_/  /_/____/_/\__,_/\___/_/_/ /_/ /_/____/_/ /_/                        ||
||                                                                            ||
||============================================================================||
|| CScore																	  ||
|| Copyright Tim Jones. All Rights Reserved.                                  ||
||============================================================================*/

#ifndef _CSCORE_H_
	#define _CSCORE_H_
	
#define SCORE_ALIVETIME		3000

#include "Graphics.h"

#include <iostream>
#include <vector>

//==============================================================================

class CScore {
	private:
		class CScore_Anim {
			public:
				char Str_Score[5];
				
				double SpawnTime;
				
				int X;
				int Y;
		};
		
		std::vector<CScore_Anim>		Score_Anim;
		
	public:
		SDL_Surface*			Surf_Score;
		
		SDL_Surface*			Display;
		
    public:
        CScore();
        
        bool OnInit();
        
        void OnRender();
        
        void OnCleanup();
        
        void AddScore(int Score, int X, int Y);
        
        void Reset();
};

//==============================================================================

#endif
