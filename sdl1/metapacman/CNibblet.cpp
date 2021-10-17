//==============================================================================
#include "CNibblet.h"

//==============================================================================
SDL_Surface* CNibblet::Surf_Nibblet = NULL;

SDL_Surface* CNibblet::Display		= NULL;

//==============================================================================
CNibblet::CNibblet() {
	Type = NIBBLET_NONE;
	
	X = 0;
	Y = 0;
	
	Frame = 0;
	FrameTime = 0;
};

//------------------------------------------------------------------------------
bool CNibblet::OnInit() {
	if((Surf_Nibblet = ImageLoad("media/nibblet.png")) == NULL) {
		cerr << "Cannot load file: media/nibblet.png\n";
		return false;
	}	
	
	Transparency(Surf_Nibblet, SetColor(255, 0, 255));
	
	return true;
};

//------------------------------------------------------------------------------
void CNibblet::OnRender() {
	//Animate, but choose a random frame. Giving it that 'jittery' effect.
	if(FrameTime + 50 < SDL_GetTicks()) {
		Frame = (rand() % 4);
		FrameTime = SDL_GetTicks();
	}
	
	//Draw the nibblet
	if(Type != NIBBLET_NONE) {
		Blit(Display, Surf_Nibblet, X * TILE_SIZE, Y * TILE_SIZE, Frame * 32, Type * 32, 32, 32);
	}
};

//------------------------------------------------------------------------------
void CNibblet::SetCoords(int newX, int newY) {
	X = newX;
	Y = newY;
};

//------------------------------------------------------------------------------
bool CNibblet::HasCollision(int X, int Y) {
	if(Type == NIBBLET_NONE) return false;

	if(this->X == X && this->Y == Y) return true;
	
	return false;
};

//==============================================================================

