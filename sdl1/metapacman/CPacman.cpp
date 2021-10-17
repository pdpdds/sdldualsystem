//==============================================================================
#include "CPacman.h"

//==============================================================================
CPacman	CPacman::Pacman;

//==============================================================================
CPacman::CPacman() {
	Surf_Pacman = NULL;
	
	Dir = DIR_LEFT;
	
	X = 7;
	Y = 9;
	
	Frame = 0;
	
	Dieing = false;
	
	DieStartTime = 0;
	
	OffsetX = 0;
	OffsetY = 0;
	
	MoveTime = 0;
	
	NewX = 0;
	NewY = 0;	
	
	Skin = 0;
};

//------------------------------------------------------------------------------
void CPacman::OnEvent() {
	//Pacman can't move when dieing
	if(Dieing) return;

	//We don't care if the key was "pressed" or "unpressed"
	//We just need to know if the key is being held down
	Uint8* Keys = SDL_GetKeyState(NULL);
	
	//Reset
	NewX = 0;
	NewY = 0;
	
	//Now lets move pacman according to the appropriate key
	if(Keys[SDLK_LEFT]) 	{ Dir = DIR_LEFT; 	NewX = -1; } else
	if(Keys[SDLK_UP]) 		{ Dir = DIR_UP; 	NewY = -1; } else
	if(Keys[SDLK_RIGHT]) 	{ Dir = DIR_RIGHT; 	NewX =  1; } else
	if(Keys[SDLK_DOWN]) 	{ Dir = DIR_DOWN; 	NewY =  1; }
};

//------------------------------------------------------------------------------
bool CPacman::OnInit() {
	if((Surf_Pacman = ImageLoad("media/pacman.png")) == NULL) {
		cerr << "Cannot load file: media/pacman.png\n";
		return false;
	}	
	
	Transparency(Surf_Pacman, SetColor(255, 0, 255));
	
	return true;
};

//------------------------------------------------------------------------------
void CPacman::OnLoop() {
	Frame = (SDL_GetTicks() % 400) / 100; //Animate every 1/10 of a second
	
	if(NewX == 0 && NewY == 0) 			return;	//We have no where to move to, so return
	if(OffsetX != 0 || OffsetY != 0) 	return; //We are currently sliding into place
												//So we need to wait
	
	//Are we trying to move off the map? If so, warp to the other side
	if(CMap::MapControl.GetTile(X + NewX, Y + NewY) == TILE_TYPE_EDGE) {
		if(Dir == DIR_LEFT) 	X = MAP_WIDTH - 1;
		if(Dir == DIR_UP) 		Y = MAP_HEIGHT - 1;
		if(Dir == DIR_RIGHT) 	X = 0;
		if(Dir == DIR_DOWN) 	Y = 0;
	}else	
	
	//Check to see if the tile we want to move to is ground
	if(CMap::MapControl.GetTile(X + NewX, Y + NewY) == TILE_TYPE_GROUND) {
		//Move pacman
		X += NewX;
		Y += NewY;
		
		//Figure out which direction we are going, so we can slide correctly
		if(Dir == DIR_LEFT) 	OffsetX = TILE_SIZE;
		if(Dir == DIR_UP) 		OffsetY = TILE_SIZE;
		if(Dir == DIR_RIGHT) 	OffsetX = -TILE_SIZE;
		if(Dir == DIR_DOWN) 	OffsetY = -TILE_SIZE;
		
		//Reset sliding move time
		MoveTime = SDL_GetTicks();
	}	
};

//------------------------------------------------------------------------------
void CPacman::OnRender() {
	//If pacman is dieing
	if(Dieing) {
		//Every 1/2 second, rotate pacman by changing his direction (dieing animation)
		if(DieStartTime + 500 < SDL_GetTicks()) {
			Dir++;
			DieStartTime = SDL_GetTicks();
		}
		
		//End of dieing animation
		if(Dir > 3) return;
		
		Blit(Display, Surf_Pacman, X * TILE_SIZE, Y * TILE_SIZE, Dir * 32, (Skin * 128), 32, 32);	
	}else{	
		Blit(Display, Surf_Pacman, (X * TILE_SIZE) + OffsetX, (Y * TILE_SIZE) + OffsetY, Dir * 32, Frame * 32 + (Skin * 128), 32, 32);

		int Dif = ((SDL_GetTicks() - MoveTime) / 30) * 4;

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
	}
};

//------------------------------------------------------------------------------
void CPacman::OnCleanup() {
	if(Surf_Pacman)		SDL_FreeSurface(Surf_Pacman);
};

//------------------------------------------------------------------------------
void CPacman::Kill() {
	Dieing = true;
	
	DieStartTime = SDL_GetTicks();	//Time that pacman started dieing
	
	Dir = 0;
}

//------------------------------------------------------------------------------
void CPacman::Reset() {
	Dieing = false;
	
	Dir = 0;
	
	X = 7;
	Y = 9;
	
	Frame = 0;
	
	OffsetX = 0;
	OffsetY = 0;
	
	NewX = 0;
	NewY = 0;
}

//==============================================================================

