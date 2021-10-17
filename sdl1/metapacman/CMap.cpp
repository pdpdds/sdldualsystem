//==============================================================================
#include "CMap.h"

//==============================================================================
CMap CMap::MapControl;

CTile CMap::TileList[MAP_AREA];

SDL_Surface* CMap::Surf_Level = 0;

char CMap::CurrentLevel[255];

char CMap::NextLevel[255];

bool CMap::CutScene = false;

//==============================================================================
CMap::CMap() {
	for(int i = MAP_AREA - 1;i >= 0;--i) {
		TileList[i].TypeID = TILE_TYPE_GROUND;
	}
};

//------------------------------------------------------------------------------
bool CMap::LoadMap(char* File) {
	char Buffer[255];
	
	FILE* FileHandle = fopen(File, "r");
	
	if(FileHandle == NULL) {
		cerr << "CPacman::LoadMap > Unable to load file '" << File << "'\n";
		return false;
	}
	
	sprintf(CurrentLevel, "maps/%s", File);
	
	char LevelImage[255];
	fscanf(FileHandle, "%s\n", Buffer);
	
	sprintf(LevelImage, "maps/%s", Buffer);
	
	SDL_FreeSurface(Surf_Level);
	
	Surf_Level = ImageLoad(LevelImage);
	
	Transparency(Surf_Level, SetColor(255, 0, 255));
	
	fscanf(FileHandle, "%s\n", Buffer);
	sprintf(NextLevel, "maps/%s", Buffer);
	
	int ID = 0;
	
	for(int Y = 0;Y < MAP_HEIGHT;Y++) {
		for(int X = 0;X < MAP_WIDTH;X++) {
			ID = X + (Y * MAP_WIDTH);
				
			fscanf(FileHandle, "%d ", &TileList[ID].TypeID);
		}
		fscanf(FileHandle, "\n");
	}
	
	fclose(FileHandle);
	
	return true;
};

//------------------------------------------------------------------------------
int CMap::GetTile(int X, int Y) {
	if(CutScene) return TILE_TYPE_GROUND;
	
	int ID = X + (Y * MAP_WIDTH);
	
	if(X < 0 || X >= MAP_WIDTH) 	return TILE_TYPE_EDGE;
	if(Y < 0 || Y >= MAP_HEIGHT) 	return TILE_TYPE_EDGE;	
	
	if(ID < 0 || ID >= MAP_AREA) 	return TILE_TYPE_WALL;
	
	return TileList[ID].TypeID;
};

//------------------------------------------------------------------------------
int CMap::GetTile(int ID) {
	if(CutScene) return TILE_TYPE_GROUND;
	
	if(ID < 0 || ID >= MAP_AREA) 	return TILE_TYPE_WALL;
	
	return TileList[ID].TypeID;
};

//==============================================================================

