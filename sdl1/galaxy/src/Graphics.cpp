//Includes
#include 	"Graphics.h"			//Own functions definition

Graphics::Graphics(bool fullscreen) {

	//Load the icon
	icon = IMG_Load("data/graphics/icon.png");
	if(icon == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}
	//Set the icon
	SDL_WM_SetIcon(icon, NULL);

	//Configure the window
	SDL_WM_SetCaption("Galaxy", NULL);

	//Set the environment variables to center the screen
	//Commented 29 Ago due to a bug in release mode
	//putenv("SDL_VIDEO_WINDOW_POS=center");
	//putenv("SDL_VIDEO_CENTERED=1");

	//Init the screen
	//Creates the screen
	if(fullscreen) {
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, BPP, SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
	} else {
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, BPP, SDL_HWSURFACE|SDL_DOUBLEBUF);
	}
	if(screen == NULL) {
		fprintf(stderr, "Unable to init screen: %s\n", SDL_GetError());
		exit(1);
	}

	//Init all the graphics needed by the game

	//Init the intro image
	intro = Load_Image("data/graphics/intro.png");
	if(intro == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init the menu background
	menu_background = Load_Image("data/graphics/menu_background.png");
	if(menu_background == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init menu buttons sprites
	menu_buttons = Load_Image("data/graphics/menu_buttons.png");
		if(menu_buttons == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init menu buttons sprites
	menu_scores = Load_Image("data/graphics/menu_scores.png");
		if(menu_scores == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init level background
	level_background = Load_Image("data/graphics/level_background.png");
		if(level_background == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init hero sprites
	level_hero = Load_Image("data/graphics/level_hero.png");
		if(level_hero == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init invaders sprites
	level_invaders = Load_Image("data/graphics/level_invaders.png");
		if(level_invaders == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init objects sprites
	level_objects = Load_Image("data/graphics/level_objects.png");
		if(level_objects == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init kamikaze extra images - exclamation
	level_exclamation = Load_Image("data/graphics/level_exclamation.png");
		if(level_exclamation == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init kamikaze extra images - interrogation
	level_interrogation = Load_Image("data/graphics/level_interrogation.png");
		if(level_interrogation == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init big explosion sprites
	level_explosion = Load_Image("data/graphics/level_explosion.png");
		if(level_explosion == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init numbers sprites
	level_numbers = Load_Image("data/graphics/level_numbers.png");
		if(level_numbers == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init the game menu sprite
	level_menu = Load_Image("data/graphics/level_menu.png");
		if(level_menu == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init menu selection graphic
	level_menu_selection = Load_Image("data/graphics/level_menu_selection.png");
		if(level_menu_selection == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init numbers sprites
	level_pause = Load_Image("data/graphics/level_pause.png");
		if(level_pause == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init slider selection graphic
	level_slider_selection = Load_Image("data/graphics/level_slider_selection.png");
		if(level_slider_selection == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init exit menu sprite
	level_exit = Load_Image("data/graphics/level_exit.png");
		if(level_exit == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init the statistics template graphic
	level_statistics = Load_Image("data/graphics/level_statistics.png");
		if(level_statistics == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init the game over screen
	level_game_over = Load_Image("data/graphics/level_game_over.png");
		if(level_game_over == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init the new score graphic template
	level_new_score = Load_Image("data/graphics/level_new_score.png");
		if(level_new_score == NULL) {
		fprintf(stderr, "Image not found: %s\n", IMG_GetError());
		exit(1);
	}

	//Init the black mask for fade
	black_mask = SDL_CreateRGBSurface(SDL_SWSURFACE, WIDTH, HEIGHT, BPP, 0x00, 0x00, 0x00, 0x00);
    if(black_mask == NULL) {
        fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
       	exit(1);
	}

	//Initialize SDL_ttf
	if(TTF_Init() == -1 ) {
		fprintf(stderr, "Failed to init SDL_ttf: %s\n", SDL_GetError());
		exit(1);
	}

	//Open the small font
	font_16 = TTF_OpenFont("data/fonts/space_age.ttf", 16);
	if(font_16 == NULL) {
		fprintf(stderr, "Font load failed: %s\n", TTF_GetError());
       	exit(1);
	}

	//Open the font
	font_28 = TTF_OpenFont("data/fonts/space_age.ttf", 28);
	if(font_28 == NULL) {
		fprintf(stderr, "Font load failed: %s\n", TTF_GetError());
       	exit(1);
	}

	//Configure the font color
	text_color.b = 255;
	text_color.g = 255;
	text_color.r = 255;

	//Init all the animation variables
	level_background_offset.h = HEIGHT;
	level_background_offset.w = WIDTH;
	level_background_offset.x = 0;
	level_background_offset.y = (level_background->h - HEIGHT);

}

Graphics::~Graphics() {
	//Free resources
	SDL_FreeSurface(intro);
	SDL_FreeSurface(icon);
	SDL_FreeSurface(menu_background);
	SDL_FreeSurface(menu_buttons);
	SDL_FreeSurface(menu_scores);
	SDL_FreeSurface(level_background);
	SDL_FreeSurface(level_hero);
	SDL_FreeSurface(level_invaders);
	SDL_FreeSurface(level_objects);
	SDL_FreeSurface(level_exclamation);
	SDL_FreeSurface(level_interrogation);
	SDL_FreeSurface(level_explosion);
	SDL_FreeSurface(level_numbers);
	SDL_FreeSurface(level_menu);
	SDL_FreeSurface(level_menu_selection);
	SDL_FreeSurface(level_pause);
	SDL_FreeSurface(level_slider_selection);
	SDL_FreeSurface(level_exit);
	SDL_FreeSurface(level_statistics);
	SDL_FreeSurface(level_game_over);
	SDL_FreeSurface(level_new_score);
	SDL_FreeSurface(black_mask);

	//Close the font that was used
	TTF_CloseFont(font_16);
	TTF_CloseFont(font_28);

	//Quit SDL_ttf
	TTF_Quit();
}

void Graphics::UpdateScreen() {
	if(SDL_Flip(screen) == -1 ) {
		fprintf(stderr, "Double buffer error: %s\n", SDL_GetError());
		exit(1);
	}
}

void Graphics::ApplySurface(int x, int y, SDL_Surface* source, SDL_Rect* clip) {
	ApplySurface(x, y, source, screen, clip);
}

void Graphics::ApplySurface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip) {

	//Make a temporary rectangle to hold the offsets
	SDL_Rect offset;
	//Give the offsets to the rectangle
	offset.x = x;
	offset.y = y;

	//Blit the surface
	SDL_BlitSurface(source, clip, destination, &offset);
}

//Loading optimazed images
SDL_Surface* Graphics::Load_Image(string filename) {

	//Temporary storage for the image that's loaded
	SDL_Surface* loadedImage = NULL; 			//The optimized image that will be used
	SDL_Surface* optimizedImage = NULL;

	//Load the image
	loadedImage = IMG_Load(filename.c_str());

	if(loadedImage != NULL) {
		//If nothing went wrong in loading the image
		//Create an optimized image
		optimizedImage = SDL_DisplayFormatAlpha(loadedImage);

		//Free the old image
		SDL_FreeSurface(loadedImage);
	}

	//Return the optimized image
	return optimizedImage;

}

void Graphics::DrawText(int x, int y, int size, string text) {

	SDL_Surface* message = NULL;
	//Render the message
	if(size == FONT_SMALL) {
		message = TTF_RenderText_Blended(font_16, text.c_str(), text_color);
	} else if(size == FONT_MEDIUM) {
		message = TTF_RenderText_Blended(font_28, text.c_str(), text_color);
	} else {

		//Create a personalized font
		TTF_Font* font = NULL;
		font = TTF_OpenFont("data/fonts/space_age.ttf", size);
		if(font == NULL) {
			fprintf(stderr, "Font load failed: %s\n", TTF_GetError());
		} else {
			message = TTF_RenderText_Blended(font, text.c_str(), text_color);
		}
		//Close the font
		TTF_CloseFont(font);

	}
	//Apply the new surface
	if(message != NULL) {
		ApplySurface(x, y, message, NULL);
	}

	//Free the resources
	SDL_FreeSurface(message);

}

void Graphics::DrawText(int x, int y, int size, string text, SDL_Surface* destination) {

	SDL_Surface* message = NULL;
	//Render the message
	if(size == FONT_SMALL) {
		message = TTF_RenderText_Blended(font_16, text.c_str(), text_color);
	} else if(size == FONT_MEDIUM) {
		message = TTF_RenderText_Blended(font_28, text.c_str(), text_color);
	} else {

		//Create a personalized font
		TTF_Font* font = NULL;
		font = TTF_OpenFont("Fonts/space_age.ttf", 16);
		if(font == NULL) {
			fprintf(stderr, "Font load failed: %s\n", TTF_GetError());
		} else {
			message = TTF_RenderText_Blended(font, text.c_str(), text_color);
		}
		//Close the font
		TTF_CloseFont(font);

	}
	//Apply the new surface
	if(message != NULL) {
		ApplySurface(x, y, message, destination, NULL);
	}

	//Free the resources
	SDL_FreeSurface(message);

}

SDL_Rect* Graphics::AnimationBackground() {

	//Scroll background
	level_background_offset.y -= 1;

	//If the background has gone too far
	if(level_background_offset.y == 0) {
		//Reset the offset
		level_background_offset.y = (level_background->h - HEIGHT);
	}

	return &level_background_offset;
}

void Graphics::FadeIn(SDL_Surface* toShow, int unsigned seconds) {

	//Avoid division by 0
	if(seconds == 0) {
		seconds = 1;
	}

	SDLTimer fps_timer;

	int alpha = SDL_ALPHA_OPAQUE;
	int reduction = (SDL_ALPHA_OPAQUE / (FRAMES_PER_SECOND * seconds));

	while(alpha > SDL_ALPHA_TRANSPARENT) {
		fps_timer.Start();

		//Set surface alpha
		SDL_SetAlpha(black_mask, SDL_SRCALPHA, alpha);

		//Apply the background
		ApplySurface(0, 0, toShow, NULL);
		//Apply the mask
		ApplySurface(0, 0, black_mask, NULL);

		//Delay for frame per seconds
		if(fps_timer.Get_Ticks() < (1000 / FRAMES_PER_SECOND)) {
			SDL_Delay( (1000 / FRAMES_PER_SECOND) - fps_timer.Get_Ticks());
		}

		//Update the screen
		UpdateScreen();

		//Reduce the alpha of the black mask
		alpha -= reduction;
	}
}

void Graphics::FadeOut(SDL_Surface* toHide, int unsigned seconds) {

	//Avoid division by 0
	if(seconds == 0) {
		seconds = 1;
	}

	SDLTimer fps_timer;

	int alpha = SDL_ALPHA_TRANSPARENT;
	int reduction = (SDL_ALPHA_OPAQUE / (FRAMES_PER_SECOND * seconds));

	while(alpha < SDL_ALPHA_OPAQUE) {
		fps_timer.Start();

		//Set surface alpha
		SDL_SetAlpha(black_mask, SDL_SRCALPHA, alpha);

		//Apply the background
		ApplySurface(0, 0, toHide, NULL);
		//Apply the mask
		ApplySurface(0, 0, black_mask, NULL);

		//Delay for frame per seconds
		if(fps_timer.Get_Ticks() < (1000 / FRAMES_PER_SECOND)) {
			SDL_Delay( (1000 / FRAMES_PER_SECOND) - fps_timer.Get_Ticks());
		}

		//Update the screen
		UpdateScreen();

		//Reduce the alpha of the black mask
		alpha += reduction;
	}
}

void Graphics::Fade(SDL_Surface* toShow, SDL_Surface* toHide, int unsigned seconds) {

	//Avoid division by 0
	if(seconds == 0) {
		seconds = 1;
	}

	SDLTimer fps_timer;

	int alpha = SDL_ALPHA_TRANSPARENT;
	int reduction = (SDL_ALPHA_OPAQUE / (FRAMES_PER_SECOND * seconds));

	while(alpha < SDL_ALPHA_OPAQUE) {
		fps_timer.Start();

		//Set surface alpha
		SDL_SetAlpha(toShow, SDL_SRCALPHA, alpha);

		//Apply the background
		ApplySurface(0, 0, toHide, NULL);
		//Apply the mask
		ApplySurface(0, 0, toShow, NULL);

		//Delay for frame per seconds
		if(fps_timer.Get_Ticks() < (1000 / FRAMES_PER_SECOND)) {
			SDL_Delay( (1000 / FRAMES_PER_SECOND) - fps_timer.Get_Ticks());
		}

		//Update the screen
		UpdateScreen();

		//Reduce the alpha of the black mask
		alpha += reduction;
	}

}

bool Graphics::SelectionYESNO() {

	//Variables needed
	SDL_Event event;
	bool result;
	bool somethingSelected = false;

	ApplySurface(((WIDTH - level_exit->w) / 2), ((HEIGHT - level_exit->h) / 2), level_exit, NULL);

	//Update the screen;
	UpdateScreen();

	while(!somethingSelected) {

		SDL_WaitEvent(&event);

		if(event.type == SDL_KEYDOWN) {
			if(event.key.keysym.sym == SDLK_y) {
	        	somethingSelected = true;
	        	result = true;
			}else if((event.key.keysym.sym == SDLK_n) || (event.key.keysym.sym == SDLK_ESCAPE)) {
				somethingSelected = true;
				result = false;
	        }
		}

	}

	return result;

}
