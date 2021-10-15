#ifndef GAME_H_
#define GAME_H_

//Includes
#include 	"Globals.h"				//Use of the globals variables of the game
#include 	"Graphics.h"			//Use of the Graphics object
#include 	"Sounds.h"				//Use of the Sounds object
#include 	"Menu.h"				//Use of the Menu object
#include 	"SDLTimer.h"			//Use of the SDLTimer object
#include 	"HeroShip.h"			//Use of the HeroShip object
#include 	"Fleet.h"				//Use of the Fleet object
#include 	"Object.h"				//Use of the Object object
#include 	"Top10.h"				//Use of Top10 object
#include 	"BTree.h"				//For BTree use
#include 	"SDL.h"				//For SDL access
#include 	<list>					//For List use
#include 	<fstream> 				//To load files
#include 	<iostream>				//cout used for debug

class Game {
	
public:

	//Attributes
	
	//Constructors - Destructors
	Game();
	virtual ~Game();
	
	//Methods
	void Start();
	

private:
	
	//Attributes
	short unsigned mode;
	short unsigned level;
	
	//Game Components
	bool fullscreen;
	Graphics* graphics;
	Sounds* sounds;
	Menu* menu;
	HeroShip* heroShip;
	Fleet* fleet;
	Invader* motherShip;
	list<Object> objects;
	list<Object> kamikazes;
	BTree scenes;
	Top10* top10;
	
	//Game variables
	SDLTimer* fps;
	SDLTimer* time_timer;
	SDLTimer* slow_mode_timer;
	unsigned int seconds_counter;
	long time;
	bool level_ended;
	bool game_over;
	bool quit;
	Object* final_gem;
	
	//Methods
	void ReadConfig();
	void GotoMenu();
	void GotoLevel();
	void AnimationLevelInit();
	void CheckIfKilled(Invader* invader);
	bool LastOne();
	void WaitForAClick();
	string GetInput();
	bool CheckCollision(SDL_Rect* A, SDL_Rect* B);
	void RegisterScene(unsigned int second);
	void LoadScene(SDL_Surface* precreated_surface, Scene* scene, int index, int count, float pixels_per_second, bool generate_report);
	void GotoScore();
	
};

#endif /*GAME_H_*/
