//==============================================================================
#include "CGhost.h"

//==============================================================================
SDL_Surface* 	CGhost::Display 			= NULL;

SDL_Surface* 	CGhost::Surf_Dead		 	= NULL;

SDL_Surface* 	CGhost::Surf_Vulnerable 	= NULL;

CPath 			CGhost::Path;

//==============================================================================		
CGhost::CGhost() {
	Dir = DIR_LEFT;
	
	X = 7;
	Y = 7;
	
	Frame = 0;
	
	Surf_Ghost = NULL;
	
	OldTime = 0;
	
	WaitTime = 100;
	
	Vulnerable = false;
	
	OffsetX = 0;
	OffsetY = 0;
	
	MoveTime = 0;	
};
		
//------------------------------------------------------------------------------
bool CGhost::OnInit(char* File) {
	if((Surf_Ghost = ImageLoad(File)) == NULL) {
		cerr << "Cannot load file: " << File << "\n";
		return false;
	}	
	
	Transparency(Surf_Ghost, SetColor(255, 0, 255));
	
	return true;
};

//------------------------------------------------------------------------------
void CGhost::OnLoop() {
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
		
		if(Dead) {
			ID = Path.FindPath(X, Y, 7, 9);
		}else
		
		if(CMap::MapControl.CutScene) {
			ID = X + (Y * MAP_WIDTH);
			if(Vulnerable) {
				ID--;
			}else{
				ID++;
			}
		}else{
  			//We need to run away!
  			if(Vulnerable) {
	  			//Check where pacman is, so we can run away from him
	  			int pX = CPacman::Pacman.X;
	  			int pY = CPacman::Pacman.Y;
  			
  				//Pacman is at the Top Left
  				if(pX < 7 && pY < 7) {
	  				//Go to Bottom Right
					ID = Path.FindPath(X, Y, MAP_WIDTH - 2, MAP_HEIGHT - 2);				
	  			}else
	  			
	  			//Pacman is at the Top Right
  				if(pX >= 7 && pY < 7) {
		  			//Go to Bottom Left
					ID = Path.FindPath(X, Y, 1, MAP_HEIGHT - 2);				
	  			}else
	     
	  			//Pacman is at the Bottom Left
	  			if(pX < 7 && pY >= 7) {
	  				//Go to Top Right
					ID = Path.FindPath(X, Y, MAP_WIDTH - 2, 1);				
	  			}else
	     
	  			//Pacman is at the Bottom Right
  				if(pX >= 7 && pY >= 7) {
					//Go to Top Left
					ID = Path.FindPath(X, Y, 1, 1);				
  				}                    			
	  			
			//Otherwise, lets get Pacman!
	  		}else{  			
	     		ID = Path.FindPath(X, Y, CPacman::Pacman.X, CPacman::Pacman.Y);
			}
		}
   		
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
void CGhost::OnRender() {
	int Dif = ((SDL_GetTicks() - MoveTime) / 35) * 4;

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

	//Lets do the sliding animation
	/*if(MoveTime + 10 < SDL_GetTicks()) {
		if(OffsetX != 0) {
			if(OffsetX > 0) OffsetX -= 4;
			else if(OffsetX < 0) OffsetX += 4;
		}
		if(OffsetY != 0) {
			if(OffsetY > 0) OffsetY -= 4;
			else if(OffsetY < 0) OffsetY += 4;
		}			
		MoveTime = SDL_GetTicks();
	}*/
		
	//Draw the correct ghost (dead, vulnerable, or just regular)
	if(Dead) {
		Blit(Display, Surf_Dead, (X * TILE_SIZE) + OffsetX, (Y * TILE_SIZE) + OffsetY, Dir * 32, Frame * 32, 32, 32);
	}else
	if(Vulnerable) {
		Blit(Display, Surf_Vulnerable, (X * TILE_SIZE) + OffsetX, (Y * TILE_SIZE) + OffsetY, Dir * 32, Frame * 32, 32, 32);
	}else{
		Blit(Display, Surf_Ghost, (X * TILE_SIZE) + OffsetX, (Y * TILE_SIZE) + OffsetY, Dir * 32, Frame * 32, 32, 32);
	}
};

//------------------------------------------------------------------------------
void CGhost::OnCleanup() {
	if(Surf_Ghost)	SDL_FreeSurface(Surf_Ghost);
};

//------------------------------------------------------------------------------
void CGhost::Reset() {
	Vulnerable = false;

	X = 7;
	Y = 7;
	
	OldTime = SDL_GetTicks();
	
	OffsetX = 0;
	OffsetY = 0;
};

//------------------------------------------------------------------------------
bool CGhost::HasCollision(int X, int Y) {
	if(this->X == X && this->Y == Y) return true;
	
	//This is prevent the ghost from just going through pacman
	if(OffsetX > 0) { if(this->X + 1 == X && this->Y == Y) return true;	}
	if(OffsetX < 0) { if(this->X - 1 == X && this->Y == Y) return true;	}
	if(OffsetY > 0) { if(this->X == X && this->Y + 1 == Y) return true;	}
	if(OffsetY < 0) { if(this->X == X && this->Y - 1 == Y) return true;	}  	
	 
 	return false;
};

//------------------------------------------------------------------------------
void CGhost::SetCoords(int newX, int newY) {
	X = newX;
	Y = newY;
};

//==============================================================================

