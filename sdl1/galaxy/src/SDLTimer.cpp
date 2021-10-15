//Includes
#include "SDLTimer.h"


SDLTimer::SDLTimer() {
	
    //Initialize the variables
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;    
}

SDLTimer::~SDLTimer() {
	
}

void SDLTimer::Start() {
	
    //Start the timer
    started = true;
    
    //Unpause the timer
    paused = false;
    
    //Get the current clock time
    startTicks = SDL_GetTicks();
}

void SDLTimer::Stop() {
	
    //Stop the timer
    started = false;
    
    //Unpause the timer
    paused = false;    
}

void SDLTimer::Pause() {
	
    //If the timer is running and isn't already paused
    if( ( started == true ) && ( paused == false ) )
    {
        //Pause the timer
        paused = true;
    
        //Calculate the paused ticks
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void SDLTimer::Unpause() {
	
    //If the timer is paused
    if( paused == true )
    {
        //Unpause the timer
        paused = false;
    
        //Reset the starting ticks
        startTicks = SDL_GetTicks() - pausedTicks;
        
        //Reset the paused ticks
        pausedTicks = 0;
    }
}

int SDLTimer::Get_Ticks() {
	
    //If the timer is running
    if( started == true )
    {
        //If the timer is paused
        if( paused == true )
        {
            //Return the number of ticks when the the timer was paused
            return pausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            return SDL_GetTicks() - startTicks;
        }    
    }
    
    //If the timer isn't running
    return 0;    
}

bool SDLTimer::Is_Started() {
	
    return started;    
}

bool SDLTimer::Is_Paused() {
	
    return paused;    
}
