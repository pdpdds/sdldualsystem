#ifndef HEROSHIP_H_
#define HEROSHIP_H_

//Includes
#include 	"Globals.h"					//Use of the globals variables of the game
#include 	"SDL.h"					//For SDL access
#include 	"SDLTimer.h"				//For use of the SDLTimer object
#include 	"Object.h"					//For use of the Object object

class HeroShip {
	
public:

	//Attributes
	//Score related
	unsigned int score;
	unsigned int invaders_killed;
	
	unsigned int invader_low_killed;
	unsigned int invader_medium_killed;
	unsigned int invader_high_killed;
	unsigned int kamikazes_killed;
	unsigned int mothership_killed;
	unsigned int amount_shields;
	unsigned int amount_invis;
	unsigned int amount_misiles;
	unsigned int amount_plasmas;
	unsigned int amount_times;
	
	//Constructor - Destructor
	HeroShip();
	virtual ~HeroShip();
	
	//Methods
	void Reset();
	void ChangeLevel();
	void Move(int xAbsolute);
	void Move(Object* final_gem);
	bool Fire();
	void ToggleWeapon();
	void Reallocate();
	int GetLocation();
	int GetGameLine();
	int GetWeapon();
	void SetWeapon(unsigned short weapon);
	void AddWeapon(unsigned short weapon);
	SDL_Rect* GetStatus();
	SDL_Rect* GetSpaceDefinition();
	SDL_Rect* GetSensibleArea();
	int Hit(unsigned short type);			//NOT_HIT if not hit (shield), HIT if hit but not dead and LIVES_LEFT if hit and dead, GAME_OVER if dead and no more lives left
	void SetVisibility(bool status);
	bool IsInvisible();
	void SetShield(bool status);
	unsigned int GetLives();
	void AddLive();
	unsigned int GetHP();
	unsigned int GetScore();
	unsigned int GetMisiles();
	unsigned int GetPlasmas();
	void UpdateScore(unsigned int type);
	string GetHash();
	
private:

	//Attributes
	unsigned int lives;
	unsigned int health_points;
	unsigned int weapon;
	unsigned int misiles;
	unsigned int plasmas;
	bool shield;
	bool invisibility;
	SDL_Rect space_definition;
	SDL_Rect sensible_area;
	//Time Control
	SDLTimer* invisibility_counter;
	SDLTimer* fire_regulator;
	//Graphics
	SDL_Rect graphics;

	//Methods
	
};

#endif /*HEROSHIP_H_*/
