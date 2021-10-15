#ifndef FLEET_H_
#define FLEET_H_

//Includes
#include 	"Matrix.h"				//Use of the matrix datatype
#include 	"Invader.h"				//Use of the Invader object
#include 	"Globals.h"				//For use of the global variables
#include 	"SDLTimer.h"			//For SDLTimer object use
#include 	"SDL.h"				//For SDL access

class Fleet {
	
public:

	//Attributes
	
	//Constructor - destructor
	Fleet();
	virtual ~Fleet();
	
	//Methods
	void Move();
	Invader* Get(short unsigned row, short unsigned column);
	SDL_Rect* GetSpaceDefinition();
	void LoadLevel(short unsigned level);
	bool StillAlive();
	void KillAnInvader();
	string GetHash();
	
private:

	//Attributes
	Matrix<Invader*>* fleet_matrix;
	SDL_Rect space_definition;
	int horizontal_speed;
	int vertical_speed;
	unsigned int total_invaders;
	
	//Methods
	void GenerateRandomLevel();
};

#endif /*FLEET_H_*/
