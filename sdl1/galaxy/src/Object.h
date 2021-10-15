#ifndef OBJECT_H_
#define OBJECT_H_

//Includes
#include 	"Globals.h"					//For use of the global variables
#include 	"SDL.h"					//For SDL access

class Object {

public:

	//Attributes
	int unsigned from;
	int unsigned type;
	
	//Constructor - destructor
	Object(int param_from, int param_type, int x, int y);
	virtual ~Object();
	
	//Methods
	void Move();
	void Move(SDL_Rect* hero);
	SDL_Rect* GetSpaceDefinition();
	SDL_Rect* Next();
	SDL_Rect* BigNext();
	bool StillAlive();
	void Hit();
	void Kill();
	string GetHash();
	

private:

	//Attributes
	int frames_counter;
	bool alive;
	int resistance;
	int horizontal_speed;
	int vertical_speed;
	SDL_Rect graphics;
	SDL_Rect space_definition;
	
	//Methods
	
};

#endif /*OBJECT_H_*/
