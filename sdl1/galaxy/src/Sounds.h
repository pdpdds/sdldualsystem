#ifndef SOUNDS_H_
#define SOUNDS_H_

//Includes
#include 	"SDL.h"				//For SDL access
#include 	"SDL_mixer.h"		//For SDL_mixer access

class Sounds {
	
public:
	
	//Attributes
	
	//Constructors - Destructors
	Sounds();
	virtual ~Sounds();
	
	//Methods
	void PlayMusic(short mode);
	void ToggleMusic();

private:
	
	//Attributes
	Mix_Music* music_menu;
	//Mix_Music* music_level1;
	bool enabled;
	
	//Methods

};

#endif /*SOUNDS_H_*/
