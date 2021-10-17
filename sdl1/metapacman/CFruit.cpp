//==============================================================================
#include "CFruit.h"

//==============================================================================
SDL_Surface* 	CFruit::Display 			= NULL;

SDL_Surface* 	CFruit::Surf_Fruit		 	= NULL;

CPath 			CFruit::Path;

//==============================================================================		
CFruit::CFruit() {
	Type = 0;
	
	X = 7;
	Y = 7;
	
	Frame = 0;
	
	Surf_Fruit = NULL;
	
	OldTime = 0;
	
	WaitTime = 100;
	
	OffsetX = 0;
	OffsetY = 0;
	
	MoveTime = 0;	
	
	SpawnTime = 0;
	AliveTime = 10000;
	
	Alive = false;
};
		
//------------------------------------------------------------------------------
bool CFruit::OnInit() {
	if((Surf_Fruit = ImageLoad("media/fruit.png")) == NULL) {
		cerr << "Cannot load file: media/fruit.png\n";
		return false;
	}	
	
	Transparency(Surf_Fruit, SetColor(255, 0, 255));
	
	return true;
};

//------------------------------------------------------------------------------
void CFruit::OnLoop() {
	if(SpawnTime + AliveTime < SDL_GetTicks()) {
		Alive = false;
		return;
	}
	
	Frame = (SDL_GetTicks() % 1000) / 500;	//Every 1/2 second, iterate animation
	
	//Used to find new coords to move to on map
	int NewX = 0;
	int NewY = 0;	
	
	if(Dead) {
		if(HasCollision(7, 9)) {
			X = 7;
			Y = 7;
			
			Dead = false;
		}
	}

	//Has enough time passed so we can move again? (And make sure pacman isn't
	//dead already)
	if(OffsetX == 0 && OffsetY == 0 && CPacman::Pacman.Dieing == false) {	
		//ID on the map
		int ID = -1;
			
   		ID = Path.FindPath(X, Y, CPacman::Pacman.X, CPacman::Pacman.Y);
   		
   		//Make sure the ID is valid (and invalid ID means there is no path!)
		if(ID >= 0 && ID < MAP_AREA) {		
   			NewX = (ID % MAP_WIDTH);
      		NewY = (ID / MAP_WIDTH);
      		
      		//Figure out which direction we are going, so we can slide correctly
      		if(NewX > X) OffsetX = -TILE_SIZE; //Moving Right
      		if(NewX < X) OffsetX =  TILE_SIZE; //Moving Left
      		if(NewY > Y) OffsetY = -TILE_SIZE; //Moving Up
      		if(NewY < Y) OffsetY =  TILE_SIZE; //Moving Down          		
   		
   			//Move the ghost
      		X = NewX;
         	Y = NewY;
		}

		MoveTime = SDL_GetTicks();
	}
};

//------------------------------------------------------------------------------
void CFruit::OnRender() {
	if(Alive == false) return;
		
	int Dif = ((SDL_GetTicks() - MoveTime) / WaitTime) * 4;

	if(Dif >= TILE_SIZE) {
		OffsetX = 0;
		OffsetY = 0;
	}

	if(OffsetX != 0) {
		if(OffsetX > 0) OffsetX = TILE_SIZE - Dif;
		if(OffsetX < 0) OffsetX = -(TILE_SIZE - Dif);
	}

	if(OffsetY != 0) {
		if(OffsetY > 0) OffsetY = TILE_SIZE - Dif;
		if(OffsetY < 0) OffsetY = -(TILE_SIZE - Dif);
	}
		
	Blit(Display, Surf_Fruit, (X * TILE_SIZE) + OffsetX, (Y * TILE_SIZE) + OffsetY, Type * 32, 0, 32, 32);
};

//------------------------------------------------------------------------------
void CFruit::OnCleanup() {
	if(Surf_Fruit)	SDL_FreeSurface(Surf_Fruit);
};

//------------------------------------------------------------------------------
void CFruit::Reset() {
	X = 7;
	Y = 7;
	
	Alive = false;
	
	OffsetX = 0;
	OffsetY = 0;
};

//------------------------------------------------------------------------------
bool CFruit::HasCollision(int X, int Y) {
	if(Alive == false) return false;
	if(this->X == X && this->Y == Y) return true;
	
	//This is prevent the ghost from just going through pacman
	if(OffsetX > 0) { if(this->X + TILE_SIZE == X && this->Y == Y) return true;	}
	if(OffsetX < 0) { if(this->X - TILE_SIZE == X && this->Y == Y) return true;	}
	if(OffsetY > 0) { if(this->X == X && this->Y + TILE_SIZE == Y) return true;	}
	if(OffsetY < 0) { if(this->X == X && this->Y - TILE_SIZE == Y) return true;	}  	
	 
 	return false;
};

//------------------------------------------------------------------------------
void CFruit::SetCoords(int newX, int newY) {
	X = newX;
	Y = newY;
};

//------------------------------------------------------------------------------
void CFruit::Spawn() {
	SpawnTime = SDL_GetTicks();
	
	X = 7;
	Y = 9;
	
	Type = (rand() % 4);
	
	Alive = true;
};

//==============================================================================

