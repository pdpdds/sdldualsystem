#ifndef GRAPHICS_H_
#define GRAPHICS_H_

//Includes
#include 	"Globals.h"				//Use of the globals variables of the game
#include 	"SDLTimer.h"			//Use of the SDLTimer object
#include 	"SDL.h"				//For SDL access
#include 	"SDL_image.h"		//For SDL_image access
#include 	"SDL_ttf.h"			//For SDL_ttf True Type Fonts access

class Graphics {
	
public:

	//Attributes
	SDL_Surface* screen;
	SDL_Surface* icon;
	SDL_Surface* intro;
	SDL_Surface* menu_background;
	SDL_Surface* menu_buttons;
	SDL_Surface* menu_scores;
	SDL_Surface* level_background;
	SDL_Surface* level_hero;
	SDL_Surface* level_invaders;
	SDL_Surface* level_objects;
	SDL_Surface* level_exclamation;
	SDL_Surface* level_interrogation;
	SDL_Surface* level_explosion;
	SDL_Surface* level_numbers;
	SDL_Surface* level_menu;
	SDL_Surface* level_menu_selection;
	SDL_Surface* level_pause;
	SDL_Surface* level_slider_selection;
	SDL_Surface* level_exit;
	SDL_Surface* level_statistics;
	SDL_Surface* level_game_over;
	SDL_Surface* level_new_score;
	SDL_Surface* black_mask;
	
	//Constructors - Destructors
	Graphics(bool fullscreen);
	virtual ~Graphics();
	
	//Methods
	void UpdateScreen();
	void ApplySurface(int x, int y, SDL_Surface* source, SDL_Rect* clip);
	void ApplySurface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip);
	//Animation methods
	void DrawText(int x, int y, int size, string text);
	void DrawText(int x, int y, int size, string text, SDL_Surface* destination);
	SDL_Rect* AnimationBackground();
	void FadeIn(SDL_Surface* toShow, int unsigned seconds);
	void FadeOut(SDL_Surface* toHide, int unsigned seconds);
	void Fade(SDL_Surface* toShow, SDL_Surface* toHide, int unsigned seconds);
	bool SelectionYESNO();
	
	
private:

	//Attributes
	SDL_Rect level_background_offset;
	//The font configuration 
	TTF_Font* font_16;
	TTF_Font* font_28;
	SDL_Color text_color;
	
	//Methods
	SDL_Surface* Load_Image(string filename);

};

#endif /*GRAPHICS_H_*/
