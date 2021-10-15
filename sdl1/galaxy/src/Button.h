#ifndef BUTTON_H_
#define BUTTON_H_

//Includes
#include 	"SDL.h"				//For SDL access

class Button {
	
public:

	//Attributes
	int x, y;
	SDL_Rect* image;
	SDL_Rect* image_over;
	
	//Constructors - Destructor
	Button();
	virtual ~Button();
	
	//Methods
	bool Over(int param_x, int param_y);
	
private:

	//Attributes
	
	//Methods

};

#endif /*BUTTON_H_*/
