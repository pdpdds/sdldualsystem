#ifndef SDLTIMER_H_
#define SDLTIMER_H_

//Includes
#include 	"SDL.h"				//For SDL access

class SDLTimer {
	
public:
	
	//Attributes
	
	//Constructors - Destructors
	SDLTimer();
	virtual ~SDLTimer();	//Not need
	
	//Methods
	void Start();
	void Stop();
	void Pause();
	void Unpause();
    
    //Gets the timer's time
    int Get_Ticks();
    
    //Checks the status of the timer
    bool Is_Started();
    bool Is_Paused();
	
private:
	
	//Attributes
	int startTicks;
	int pausedTicks;
	bool paused;
	bool started;
	 	
	//Methods
	
};

#endif /*SDLTIMER_H_*/
