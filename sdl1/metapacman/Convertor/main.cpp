/*============================================================================||
||       __  ___  _______    ___  ____  __         ___                        ||
||      /  \/  /_/__  __/_ _/ __\/_/  \/ /   _____/   \                       ||
||     / /\_/ / ___/ / __ `/ /  __/ __/  \  / ___/  `_/                       ||
||    / /  / / __// / /_/ / /__/ / / / /\ \/ __// /\ \                        ||
||   /_/  /_/____/_/\__,_/\___/_/_/ /_/ /_/____/_/ /_/                        ||
||                                                                            ||
||============================================================================||
|| Convertor																  ||
|| Copyright Tim Jones. All Rights Reserved.                                  ||
||============================================================================*/
#include "Define.h"
#include "Graphics.h"

#include <SDL.h>
#include <iostream>

//==============================================================================

using namespace std;

//==============================================================================

bool OnInit();

//==============================================================================

int Map[MAP_AREA];

//==============================================================================
bool OnInit() {
	//===  Start SDL Routines  ===//
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "Cannot Initialize SDL Systems: " << SDL_GetError() << "\n";
		return false;
    }
	
    return true;
};

//------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
	atexit(SDL_Quit);

    if(OnInit() == false) {
        cerr << "Unable to Init Pacman!\n";
        return -1;
    }
    
    if(argc <= 1) {
		cerr << "Map BMP not given\n";
		return -1;
	}
    
    SDL_Surface* Surf = SDL_LoadBMP(argv[1]);
    
    if(Surf == NULL) {
		cerr << "BMP not found or errors produced\n";
		return -1;
	}
	
	char newFile[255];
	strcpy(newFile, argv[1]);
	
	newFile[strlen(argv[1]) - 3] = 'm';
	newFile[strlen(argv[1]) - 2] = 'a';
	newFile[strlen(argv[1]) - 1] = 'p';
    
    FILE* FileHandle = fopen(newFile, "w");
    
	fprintf(FileHandle, "%s\n", argv[1]);
	
	char nextMap[255];
	sprintf(nextMap, "%d.map", atoi(newFile) + 1);
	fprintf(FileHandle, "%s\n", nextMap);
    
    for(int Y = 0;Y < MAP_HEIGHT;Y++) {
    	for(int X = 0;X < MAP_WIDTH;X++) {
    		int ID = X + (Y * MAP_WIDTH);
    		
    		if(GetPixel(Surf, X * TILE_SIZE, Y * TILE_SIZE) == 255) {
    			fprintf(FileHandle, "0 ");
    		}else{
    			fprintf(FileHandle, "1 ");
    		}
    	}
    	fprintf(FileHandle, "\n");
    }
    
    SDL_FreeSurface(Surf);
    
    fclose(FileHandle);

    return 0;
};

//==============================================================================

