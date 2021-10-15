#ifndef INVADER_H_
#define INVADER_H_

//Includes
#include 	"Globals.h"					//Use of the globals variables of the game
#include 	"SDL.h"					//For SDL access

class Invader {
	
public:
	
	//Attributes
	
	//Constructor - destructor
	Invader();
	Invader(const unsigned short param_type);
	virtual ~Invader();
	
	//Methods
	void SetType(const unsigned short param_type);
	int GetType();
	bool StillAlive();
	void Hit();
	void Kill();
	void Revive();
	bool Fire();
	void Move();
	static int FreeItem();
	SDL_Rect* Next();
	SDL_Rect* GetSpaceDefinition();
	string GetHash();
	
private:

	//Attributes
	int frames_counter;
	int type;
	int fire_probability;
	bool alive;
	int resistance;
	int speed;
	SDL_Rect graphics;
	SDL_Rect space_definition;
	
	//Methods
	
};

#endif /*INVADER_H_*/
