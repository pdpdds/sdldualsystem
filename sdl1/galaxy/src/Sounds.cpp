//Includes
#include 	"Sounds.h"				//Own funtions definition

Sounds::Sounds() {
	//Initialize SDL_mixer
    if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1 )  {
    	fprintf(stderr, "Unable to init Mixer: %s\n", Mix_GetError());
		//Set the music disabled
		enabled = false;

    } else {
    	//Set the music enabled
    	enabled = true;

    	//Loads the music files
    	music_menu = Mix_LoadMUS("data/music/endlessly.xm");
		//music_menu = Mix_LoadMUS("data/music/game_theme.ogg");
		
		//If there was a problem loading the music
    	if(music_menu == NULL) {
    		fprintf(stderr, "Unable to load music file: %s\n", Mix_GetError());
    		exit(1);
    	}
	}
}

Sounds::~Sounds() {
	if(enabled) {
		//Free resources
		//Free the sound effects
		//TODO
		//Free the music files
		Mix_FreeMusic(music_menu);
		//Quit SDL_mixer
		Mix_CloseAudio();
	}
}

void Sounds::PlayMusic(short mode) {

	//Be sure that the sound device was enabled
	if(!enabled) {
		return;
	}

	//Stop any music.
	//Mix_HaltMusic();

	// fade out music to finish 3 seconds from now :: Much better :D
	while(!Mix_FadeOutMusic(3000) && Mix_PlayingMusic()) {
    	// wait for any fades to complete
    	SDL_Delay(100);
	}

	Mix_Music* music = NULL;

	//Choose the music
	switch (mode) {
	    case 0:		//MODE_MENU
	    	music = music_menu;
	    	break;
	    case 1:		//MODE_SCORE
	    	//music = menu_music;
	    	break;
		case 2:		//MODE_CREDITS
			//music = menu_music;
	    	break;
	    case 3:		//MODE_LEVEL
	    	//music = menu_music;
	    	break;
		case 4:		//MODE_GAME_OVER
			//music = menu_music;
	    	break;
	}

	//Play the music
	if((Mix_PlayMusic(music, -1) == -1) && (mode != -1)) {
			fprintf(stderr, "Unable to play music: %s\n", Mix_GetError());
	}
}

void Sounds::ToggleMusic() {

	//Be sure that the sound device was enabled
	if(!enabled) {
		return;
	}

	if(Mix_PausedMusic() == 1) {	//If the music is paused/halted
		//Resume the music
		Mix_ResumeMusic();
	} else {						//If the music is playing
		//Pause the music
		Mix_PauseMusic();
	}
}



