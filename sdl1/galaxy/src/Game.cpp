//Includes
#include 	"Game.h"				//Own funtions definition

Game::Game() {
	
	//Init the game
	
	//Start SDL
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
    }
	
	//Read the config file
	ReadConfig();
	
	//Init the screen and the graphics
	graphics = new Graphics(fullscreen); 
	
	//Init the sound
	//sounds = new Sounds();
	
	//Init the menu
	menu = new Menu();
	
	//Load the score
	top10 = new Top10();
	
	//Init the game needed variables
	//The frame rate regulator timer
	fps = new SDLTimer();
	
	//Init the time counter Timer and related variable
	time_timer = new SDLTimer();
	time = 0;
	
	//Init the scene creation regulator counter
	seconds_counter = 0;
	
	//Init the slow mode timer
	slow_mode_timer = new SDLTimer();
	
	//Init the Hero ship object
	heroShip = new HeroShip();
	
	//Init the fleet object 
	fleet = new Fleet();
	
	//Init the fleet object 
	motherShip = new Invader(INVADER_MOTHERSHIP);
	
	//Set the initial level
	level = 1;
	
}

Game::~Game() {
	//Return the multimedia resources.
	//delete menu // TODO : Verify bug;
	//delete sounds;
	delete graphics;
	
	//Return the resources from SDL.
	SDL_Quit();
	
}

void Game::ReadConfig() {
	
	//Open a file for reading 
	ifstream config_file(FILE_CONFIG.c_str());
	
	if(config_file.good()) {
		
		//The current line 
		string line;
		
		//Get the next line 
		getline(config_file, line);
		
		//If an error occured while trying to load data 
		if(!config_file.fail()) { 
			
			if(line == "Fullscreen = TRUE;") {
				fullscreen = true;
			} else if(line == "Fullscreen = FALSE;") {
				fullscreen = false;
			} else {
				fprintf(stderr, "Error, corrupt configuration file.\n");
			}
			
			config_file.close();
			
			//If everything goes fine.
			return;
		}   
		
	}
	
	//The default configuration
	fprintf(stderr, "Error, unable to load the configuration file.\n");
	fullscreen = false;
	
}

void Game::Start() {
	
	//Grab the mouse
	if(fullscreen) {
		SDL_WM_GrabInput(SDL_GRAB_ON);
	}
	
	//Intro
	SDL_ShowCursor(SDL_DISABLE);
	SDL_Delay(1000);
	graphics->FadeIn(graphics->intro, 2);
	SDL_Delay(1000);
	graphics->FadeOut(graphics->intro, 2);
	
	SDL_Surface* temporal_surface;
	temporal_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, WIDTH, HEIGHT, BPP, 0x00, 0x00, 0x00, 0x00);
	if(temporal_surface == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
		exit(1);
	}
	
	//Apply the graphics, the program menu.
	graphics->ApplySurface(0, 0, graphics->menu_background, temporal_surface, NULL);

	//Apply the buttons graphics
	for(int n = 0; n < menu->NUMBER_BUTTONS; n++) {
		graphics->ApplySurface(menu->buttons[n].x, menu->buttons[n].y, graphics->menu_buttons, temporal_surface, menu->buttons[n].image);
	}
	
	//FadeIn the new screen
	graphics->FadeIn(temporal_surface, 1);
	
	SDL_ShowCursor(SDL_ENABLE);
	GotoMenu();
}

void Game::GotoMenu() {
	
	//Set the mode
	mode = MODE_MENU;
	
	//Apply the graphics, the program menu.
	graphics->ApplySurface(0, 0, graphics->menu_background, NULL);
	
	//Apply the buttons graphics
	for(int n = 0; n < menu->NUMBER_BUTTONS; n++) {
		graphics->ApplySurface(menu->buttons[n].x, menu->buttons[n].y, graphics->menu_buttons, menu->buttons[n].image);
	}
		
	//Update the screen 
	graphics->UpdateScreen();
	
	//Play the menu music
	//sounds->PlayMusic(MODE_MENU);
	
	//::Wait for events::
	//Variables needed.
	SDL_Event event;
	bool enable = true;
	//The mouse offsets 
	int changed, x = 0, y = 0;
	
	while(enable) {
		
		SDL_WaitEvent(&event);
		
		switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				//printf("Mouse button %d pressed at (%d,%d)\n",
				//		event.button.button, event.button.x, event.button.y);
				
				//If the left mouse button was pressed 
				if( event.button.button == SDL_BUTTON_LEFT ) { 
					
					//Get the mouse offsets 
					x = event.button.x; 
					y = event.button.y;
					
					//Search which button is pressed 
					int button_selected = menu->Over(x, y);
					if(button_selected != -1) {
						switch (button_selected) {
							case 0:
							
								//Reset previous game states
								heroShip->Reset();
								
								//Start the game
								GotoLevel();
								
								SDL_Surface* temporal_surface;
								temporal_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, WIDTH, HEIGHT, BPP, 0x00, 0x00, 0x00, 0x00);
    							if(temporal_surface == NULL) {
        							fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
       								exit(1);
								}
								//BackUp the screen
								graphics->ApplySurface(0, 0, graphics->screen, temporal_surface, NULL);
								
								//FadeOut the current screen
								graphics->FadeOut(temporal_surface, 1);
								
								//Apply the graphics, the program menu.
								graphics->ApplySurface(0, 0, graphics->menu_background, temporal_surface, NULL);
	
								//Apply the buttons graphics
								for(int n = 0; n < menu->NUMBER_BUTTONS; n++) {
									graphics->ApplySurface(menu->buttons[n].x, menu->buttons[n].y, graphics->menu_buttons, temporal_surface, menu->buttons[n].image);
								}
								
								//FadeIn the new screen
								graphics->FadeIn(temporal_surface, 1);
								
								//Correct the non-exact fade problem
								graphics->ApplySurface(0, 0, graphics->menu_background, NULL);
								
								//Show the cursor
								SDL_WarpMouse((WIDTH / 2), (HEIGHT / 2));
								SDL_ShowCursor(SDL_ENABLE);
	
								//Free the resource
								SDL_FreeSurface(temporal_surface);
								
								break;
							case 1://Scores
								GotoScore();
								break;
							case 2://Credits
								break;
							case 3://Exit
								
								//sounds->PlayMusic(-1);
								
								SDL_Surface* what_in_screen_surface;
								what_in_screen_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, WIDTH, HEIGHT, BPP, 0x00, 0x00, 0x00, 0x00);
    							if(what_in_screen_surface == NULL) {
        							fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
       								exit(1);
								}
								//BackUp the screen
								graphics->ApplySurface(0, 0, graphics->screen, what_in_screen_surface, NULL);
								
								//FadeOut the current screen
								graphics->FadeOut(what_in_screen_surface, 1);
	
								//Free the resource
								SDL_FreeSurface(what_in_screen_surface);
								
								enable = false;
								break;
						}	
					}
				} 
				break;

            case SDL_MOUSEMOTION:
                //printf("Mouse moved by %d,%d to (%d,%d)\n", 
                //       event.motion.xrel, event.motion.yrel,
                //       event.motion.x, event.motion.y);
				
				x = event.motion.x;
				y = event.motion.y;
				changed = menu->Changed(x, y);
				
				if(changed != -1) {
					changed--;						//Decrement variable changed to correspond array index.
					for(int n = 0; n < menu->NUMBER_BUTTONS; n++) {
						if(n == changed) {			//If this button is the selected one
							//Draw the selected button sprite
							graphics->ApplySurface(menu->buttons[n].x, menu->buttons[n].y, graphics->menu_buttons, menu->buttons[n].image_over);
						} else {					//This button is not selected 
							//Draw the non selected button sprite
							graphics->ApplySurface(menu->buttons[n].x, menu->buttons[n].y, graphics->menu_buttons, menu->buttons[n].image);
						}
					}
					//Update the screen
					graphics->UpdateScreen();
				}
                break;
                
	        case SDL_KEYDOWN:
	        //If music key was pressed
	        	if(event.key.keysym.sym == SDLK_m) {
	        		//sounds->ToggleMusic();
	        	} else if(event.key.keysym.sym == SDLK_ESCAPE) {
	        		//sounds->PlayMusic(-1);
					enable = false;
	        	}
	            
	            //printf("The %s key was pressed!\n",
	            //       SDL_GetKeyName(event.key.keysym.sym));
	            
	            break;
	            
	        case SDL_QUIT:
	            enable = false;
	            break;
		}
	}
}

void Game::GotoLevel() {
	
	game_over = false;

	while(!game_over) {
		
		//Reset all the possible uncleaned variables from previous games
		heroShip->Reallocate();
		heroShip->ChangeLevel();
		objects.clear();
		kamikazes.clear();
		scenes.Clear();
			
		//Set it to the correct location and hide it
		SDL_WarpMouse((WIDTH / 2), 0);
		SDL_ShowCursor(SDL_DISABLE);
		
		//Load the current level
		motherShip->Revive();
		fleet->LoadLevel(level);
		
		//Animation
		AnimationLevelInit();
		
		//Start the game timer
		time_timer->Start();
		time = 0;
		
		//Start the scene recorder regulator timer
		seconds_counter = 0;
		
		//Variables needed
		SDL_Event event;
		quit = false;
		level_ended = false;
		final_gem = NULL;
		
		while(!quit) {
			
			//Start the frame rate regulator timer 
			fps->Start();
			
			if(!level_ended) {
				//While there's an event to handle 
				while(SDL_PollEvent(&event)) { 
					
					switch (event.type) {
						case SDL_KEYDOWN:
							if(event.key.keysym.sym == SDLK_p) {
								//Pause all the timers
			        			time_timer->Pause();
			        			fps->Pause();
			        			slow_mode_timer->Pause();
			        			
			        			//Pause the game
			        			graphics->ApplySurface(0, 0, graphics->level_pause, NULL);
			        			graphics->ApplySurface(0, 16, graphics->level_slider_selection, NULL);
								//Draw the Numbers
								graphics->DrawText(12, 48, FONT_MEDIUM, "1");
								graphics->DrawText(950, 48, FONT_MEDIUM, IntegerToString(scenes.GetCount()));
			        			graphics->UpdateScreen();
			        			
			        			//Pause variable
			        			bool paused = true;
			        			
			        			//Generate report variable
			        			bool generate_report = false;
			        			
			        			//Grafication variables
			        			int x = 0;
			        			int index = 0;
			        			int index_monitor = 0;
			        			int count = scenes.GetCount();
			        			float pixels_per_second;
			        			
			        			//Avoid division by 0
			        			if((count == 0) || (count == 1)) {
			        				pixels_per_second = 1000.0;
			        			} else {
			        				pixels_per_second = (1000.0 / (count - 1));
			        			}
			        			
			        			while(paused) {
			        				
			        				//Wait for an event
			        				SDL_WaitEvent(&event);
			
									if(event.type == SDL_MOUSEMOTION) {
										
										//Set the x
										x = event.motion.x;
										
										//Verify it still in the offset
										if(x < 12) {
											x = 0;
										} else if(x > 1012) {
											x = 1000;
										} else {
											x -= 12;
										}
										
										//Set index
										index = (x / pixels_per_second);
										//Debug
										//printf("Index:%i\n", index);
										
									} else if(event.type == SDL_KEYDOWN) {
										
										if(event.key.keysym.sym == SDLK_p) {
		        							paused = false;
										} else if(event.key.keysym.sym == SDLK_m) {
	        								//sounds->ToggleMusic();
										} else if(event.key.keysym.sym == SDLK_g) {
														
											Scene* loaded_scene = scenes.Find(index);
											if(loaded_scene != NULL) {
												generate_report = true;
											} else {
												//Debug
												//cout << "Null was retorned :S" << endl;
											}
										}
									}
									
									//Paint the specified scene if the index has changed
									if((index != index_monitor) || generate_report) {
										
										//Set the variable to the current index
										index_monitor = index;
										
										//Load the scene
										Scene* loaded_scene = scenes.Find(index);
										if(loaded_scene != NULL) {
											
											SDL_Surface* temporal = SDL_CreateRGBSurface(SDL_SWSURFACE, WIDTH, HEIGHT, BPP, 0x00, 0x00, 0x00, 0x00);
											if(temporal != NULL) {
												//Reload the interface
												LoadScene(temporal, loaded_scene, index, count, pixels_per_second, generate_report);
												graphics->ApplySurface(0, 0, temporal, NULL);
												graphics->UpdateScreen();
												SDL_FreeSurface(temporal);
												//Reset the generate_report variable
												if(generate_report) {
													generate_report = false;
												}
											} else {
												//Debug
												fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
											}
										}
									}
			        			}
			        			
			        			//Unpause all the timers
			        			time_timer->Unpause();
			        			fps->Unpause();
			        			slow_mode_timer->Unpause();
			        			
							}else if(event.key.keysym.sym == SDLK_m) {
								//Toogle ON/OFF the music
			        			//sounds->ToggleMusic();
			        		} else if(event.key.keysym.sym == SDLK_ESCAPE) {
			        			
			        			//Pause all the timers
			        			time_timer->Pause();
			        			fps->Pause();
			        			slow_mode_timer->Pause();
			        			
			        			//Close the game
				        		bool selection = graphics->SelectionYESNO();
			        			if(selection) {
			        				quit = true;
			        			} else {
			        				//Unpause all the timers
			        				time_timer->Unpause();
			        				fps->Unpause();
			        				slow_mode_timer->Unpause();
			        			}
				        	}
							break;
						case SDL_MOUSEMOTION:
							if(event.motion.xrel != 0)
								heroShip->Move(event.motion.x);//rel);
							break;
						case SDL_MOUSEBUTTONDOWN:
							//If the left mouse button was pressed 
							if(event.button.button == SDL_BUTTON_LEFT) {
								//Shoot
								if(heroShip->Fire()) {
									if(heroShip->GetWeapon() != TYPE_MISILE) {
										Object* new_object = new Object(SHIP, heroShip->GetWeapon(), (heroShip->GetLocation() + (HERO_SPRITE_DIMENSION / 2) - (OBJECT_SPRITE_DIMENSION / 2)), heroShip->GetGameLine());
										objects.push_back(*new_object);
									} else {
										for(int n = 0, x = (heroShip->GetLocation() + (HERO_SPRITE_DIMENSION / 2) - (OBJECT_SPRITE_DIMENSION / 2) -40); n < 5; n++) {
											Object* new_object = new Object(SHIP, TYPE_MISILE, x, heroShip->GetGameLine());
											objects.push_back(*new_object);
											x += 20;
										}
									}
								}
							} else if(event.button.button == SDL_BUTTON_RIGHT) {
								heroShip->ToggleWeapon();
							}
							break;
					}
				}
			} else {		//The level has ended
				heroShip->Move(final_gem);
				
				if(CheckCollision(heroShip->GetSensibleArea(), final_gem->GetSpaceDefinition())) {
					heroShip->AddLive();
					level++;
					quit = true;
				}
			}
			
			if(!slow_mode_timer->Is_Started()) {
				//Move the fleet
				fleet->Move();
				//Move the mothership
				motherShip->Move();
			}
			
			//**GRAPHICS**//
			//Apply the new background
			graphics->ApplySurface(0, 0, graphics->level_background, graphics->AnimationBackground()); 
			//Relocate the hero ship
			graphics->ApplySurface(heroShip->GetLocation(), heroShip->GetGameLine(), graphics->level_hero, heroShip->GetStatus());
			//Relocate the final gem - level ended
			if(level_ended) {
				graphics->ApplySurface(final_gem->GetSpaceDefinition()->x, final_gem->GetSpaceDefinition()->y, graphics->level_objects, final_gem->Next());
			}
			//Relocate the fleet
			if(fleet->StillAlive()) {
				for(short row = 0, column = 0; row != FLEET_ROWS; column++) {
					
					//Reset the columns when reached the limit of the matrix
					if(column == FLEET_COLUMNS) {
						column = -1;
						row++;
					} else {
						
						Invader* current_invader = fleet->Get(row, column);
						
						//If the invader still alive
						if(current_invader->StillAlive()) {
							
							//Compute the position
							int x = (fleet->GetSpaceDefinition()->x + (column * INVADER_SPRITE_DIMENSION) + (column * INVADERS_SEPARATION));
							int y = (fleet->GetSpaceDefinition()->y + (row * INVADER_SPRITE_DIMENSION));
							//Update de position
							current_invader->GetSpaceDefinition()->x = x;
							current_invader->GetSpaceDefinition()->y = y;
							//Paint the invader
							graphics->ApplySurface(x, y, graphics->level_invaders, current_invader->Next());
							
							//Fire!!
							if(current_invader->Fire()) {
								Object* new_object = new Object(current_invader->GetType(), TYPE_LASER, (x + (OBJECT_SPRITE_DIMENSION / 2)), (y + INVADER_SPRITE_DIMENSION - 20));
								objects.push_back(*new_object);
							}
						}
					}
				}
			}
			
			//Relocate the mothership
			if(motherShip->StillAlive()) {
				
				graphics->ApplySurface(motherShip->GetSpaceDefinition()->x, motherShip->GetSpaceDefinition()->y, graphics->level_invaders, motherShip->Next());
				
				if(motherShip->Fire()) {
					Object* new_object = new Object(INVADER_MOTHERSHIP, TYPE_KAMIKAZE, (motherShip->GetSpaceDefinition()->x + OBJECT_SPRITE_DIMENSION), (motherShip->GetSpaceDefinition()->y + INVADER_SPRITE_DIMENSION));
					kamikazes.push_back(*new_object);
				}
			}
			
			//For each object in list objects
			list<Object>::iterator object = objects.begin();
			while(object != objects.end()) {
				
				if(object->StillAlive()) {					//Just in case
					//Move the object
					object->Move();
					
					//Check collision
					if(object->from == SHIP) {	//Object released by the hero ship
						
						//Check if the bullet collide with the mothership
						if(motherShip->StillAlive() && CheckCollision(object->GetSpaceDefinition(), motherShip->GetSpaceDefinition())) {
							
							//Create a small explosion
							Object* new_object = new Object(NOBODY, TYPE_SMALL_EXPLOSION, object->GetSpaceDefinition()->x, object->GetSpaceDefinition()->y);
							objects.push_back(*new_object);
							
							//Damage the mothership
							if(object->type == TYPE_PLASMA) {
								motherShip->Kill();
							} else {	
								motherShip->Hit();
							}
							
							//If the invaders is killed then update score and free an item
							CheckIfKilled(motherShip);
							
							//Kill the object and stop searching
							object->Kill();
						}
						
						//Check if the bullet collide with an invader
						if(object->StillAlive() && CheckCollision(object->GetSpaceDefinition(), fleet->GetSpaceDefinition())) {
							for(short row = 0, column = 0; row < FLEET_ROWS; column++) {
								
								//Reset the columns when reached the limit of the matrix
								if(column == FLEET_COLUMNS) {
									column = -1;
									row++;
								} else {
									
									Invader* current_invader = fleet->Get(row, column);
									
									//If the invader still alive
									if(current_invader->StillAlive()) {
										
										//Compute the position
										int x = (fleet->GetSpaceDefinition()->x + (column * INVADER_SPRITE_DIMENSION) + (column * INVADERS_SEPARATION));
										int y = (fleet->GetSpaceDefinition()->y + (row * INVADER_SPRITE_DIMENSION));
										//Update de position
										current_invader->GetSpaceDefinition()->x = x;
										current_invader->GetSpaceDefinition()->y = y;
										
										//Check collision
										if(CheckCollision(object->GetSpaceDefinition(), current_invader->GetSpaceDefinition())) {
											
											//Create a small explosion
											Object* new_object = new Object(NOBODY, TYPE_SMALL_EXPLOSION, object->GetSpaceDefinition()->x, object->GetSpaceDefinition()->y);
											objects.push_back(*new_object);
							
											//Damage the invader
											if(object->type == TYPE_PLASMA) {
												
												current_invader->Kill();
												
												int new_row;
												int new_column;
												Invader* colateral_invader;
												//Destroy all the invaders nearby in X formation
												
												//X*  X
												//  X
												//X   X
												new_row = row - 1;
												new_column = column - 1;
												if((new_row >= 0) && (new_column >= 0)) {
													
													colateral_invader = fleet->Get(new_row, new_column);
													
													if(colateral_invader->StillAlive()) {
														
														colateral_invader->Kill();
														//If the invaders is killed then update score and free an item
														CheckIfKilled(colateral_invader);
														
													}
												}
												
												//X   X*
												//  X
												//X   X
												new_row = row - 1;
												new_column = column + 1;
												if((new_row >= 0) && (new_column < FLEET_COLUMNS)) {
													
													colateral_invader = fleet->Get(new_row, new_column);
													
													if(colateral_invader->StillAlive()) {
														colateral_invader->Kill();
														//If the invaders is killed then update score and free an item
														CheckIfKilled(colateral_invader);
														
													}
												}
												
												//X   X
												//  X
												//X*  X
												new_row = row + 1;
												new_column = column - 1;
												if((new_row < FLEET_ROWS) && (new_column >= 0)) {
													
													colateral_invader = fleet->Get(new_row, new_column);
													
													if(colateral_invader->StillAlive()) {
														
														colateral_invader->Kill();
														//If the invaders is killed then update score and free an item
														CheckIfKilled(colateral_invader);
														
													}
												}
												
												//X   X
												//  X
												//X   X*
												new_row = row + 1;
												new_column = column + 1;
												if((new_row < FLEET_ROWS) && (new_column < FLEET_COLUMNS)) {
													
													colateral_invader = fleet->Get(new_row, new_column);
													
													if(colateral_invader->StillAlive()) {
														
														colateral_invader->Kill();
														//If the invaders is killed then update score and free an item
														CheckIfKilled(colateral_invader);
														
													}
												}
												
											} else {	
												current_invader->Hit();
											}
											
											//If the invaders is killed then update score and free an item
											CheckIfKilled(current_invader);
											
											
											//Kill the object and stop searching
											object->Kill();
											row = FLEET_ROWS;
										}
									}
								}
							}
						}
							
						//Check if the bullet collide with a kamikaze
						if(object->StillAlive()) {
							list<Object>::iterator kamikaze = kamikazes.begin();
							while(kamikaze != kamikazes.end()) {
								if(kamikaze->StillAlive()) {					//Just in case
									//If the bullet collide and kamikaze
									if(CheckCollision(object->GetSpaceDefinition(), kamikaze->GetSpaceDefinition())) {
										
										//Create a small explosion
										Object* new_object = new Object(NOBODY, TYPE_SMALL_EXPLOSION, object->GetSpaceDefinition()->x, object->GetSpaceDefinition()->y);
										objects.push_back(*new_object);
											
										//Damage the kamikaze
										if(object->type == TYPE_PLASMA) {
											kamikaze->Kill();
										} else {	
											kamikaze->Hit();
										}
										
										if(!kamikaze->StillAlive()) {
											
											if(!LastOne()) {
												//Update the score
												heroShip->UpdateScore(INVADER_KAMIKAZE);
												//Free an item
												int item = Invader::FreeItem();
												if(item != TYPE_NONE) {
													Object* new_object = new Object(INVADER_MOTHERSHIP, item, kamikaze->GetSpaceDefinition()->x, kamikaze->GetSpaceDefinition()->y);
													objects.push_back(*new_object);
												}
												//The Kamikaze don't create a big explosion
												//Object* new_object = new Object(NOBODY, TYPE_BIG_EXPLOSION, kamikaze->GetSpaceDefinition()->x, kamikaze->GetSpaceDefinition()->y);
												//objects.push_back(*new_object);
												
											} else {
												level_ended = true;
												final_gem = new Object(kamikaze->type, TYPE_GEM, kamikaze->GetSpaceDefinition()->x, kamikaze->GetSpaceDefinition()->y);
											}
										}
										//Kill the object and stop searching
										object->Kill();
										kamikaze = kamikazes.end();
									} else {
										kamikaze++;
									}
								} else {
									kamikaze++;
								}
							}
						}
						
					} else if((object->from != NOBODY) && (!level_ended)) {						//If the object was released by and invader and is not an explosion and if the lavel hasn't ended
						//If the object collide with the hero ship
						if(CheckCollision(object->GetSpaceDefinition(), heroShip->GetSensibleArea())) {
							//Check the nature of the object
							if(object->type == TYPE_LASER){
								
								//Create a small explosion
								Object* new_object = new Object(NOBODY, TYPE_SMALL_EXPLOSION, object->GetSpaceDefinition()->x, object->GetSpaceDefinition()->y);
								objects.push_back(*new_object);
							
								//Damage the hero ship 
								int result = heroShip->Hit(TYPE_LASER);
								
								//NOT_HIT if not hit (shield), 
								//HIT if hit but not dead and 
								//LIVES_LEFT if hit and dead, 
								//GAME_OVER if dead and no more lives left
													
								//Handle the result
								if(result == HIT) {
									//Create a small explosion
									Object* new_object = new Object(NOBODY, TYPE_SMALL_EXPLOSION, object->GetSpaceDefinition()->x, object->GetSpaceDefinition()->y);
									objects.push_back(*new_object);
									
								} else if(result == LIVES_LEFT) {
									//Create a big explosion and set invisibility
									Object* new_object = new Object(NOBODY, TYPE_BIG_EXPLOSION, heroShip->GetSpaceDefinition()->x, heroShip->GetSpaceDefinition()->y);
									objects.push_back(*new_object);
									heroShip->SetVisibility(false);
									
								} else if(result == GAME_OVER) {
									//Create a big explosion animation and lose
									quit = true;
									game_over = true;
								}	
								
							} else if(object->type == TYPE_POWERUP_SHIELD) {
								heroShip->SetShield(true);
								heroShip->UpdateScore(TYPE_POWERUP_SHIELD);
								
							} else if(object->type == TYPE_POWERUP_INVI) {
								heroShip->SetVisibility(false);
								heroShip->UpdateScore(TYPE_POWERUP_INVI);
								
							} else if(object->type == TYPE_POWERUP_MISILES) {
								heroShip->AddWeapon(TYPE_MISILE);
								heroShip->UpdateScore(TYPE_POWERUP_MISILES);
								
							} else if(object->type == TYPE_POWERUP_PLASMA) {
								heroShip->UpdateScore(TYPE_POWERUP_PLASMA);
								heroShip->AddWeapon(TYPE_PLASMA);
								
							} else if(object->type == TYPE_POWERUP_TIME) {
								slow_mode_timer->Start();
								heroShip->UpdateScore(TYPE_POWERUP_TIME);
								
							}
							//Kill the object
							object->Kill();
						}
					}
					
					//Check if the object is out of the screen
					int y = object->GetSpaceDefinition()->y;
					if((y < -OBJECT_SPRITE_DIMENSION) || (y > HEIGHT)) {
						object->Kill();
					}
				}
				
				//If object still alive
				if(object->StillAlive()) {
					//Paint the object or remove it from the list
					if(object->type == TYPE_BIG_EXPLOSION) {
						graphics->ApplySurface(object->GetSpaceDefinition()->x, object->GetSpaceDefinition()->y, graphics->level_explosion, object->BigNext());
					} else {
						graphics->ApplySurface(object->GetSpaceDefinition()->x, object->GetSpaceDefinition()->y, graphics->level_objects, object->Next());
					}
					object++;
				} else {
					object = objects.erase(object);
				}
			}
			
			//For each kamikaze in list kamikaze
			list<Object>::iterator kamikaze = kamikazes.begin();
			while(kamikaze != kamikazes.end()) {

				if(kamikaze->StillAlive()) {					//Just in case
					//Move the object
					if(	!heroShip->IsInvisible() && 
						(motherShip->StillAlive() || fleet->StillAlive()) && 
						(!slow_mode_timer->Is_Started())) {
							
						kamikaze->Move(heroShip->GetSensibleArea());
					}
					
					//If the object collide with the hero ship
					if(CheckCollision(kamikaze->GetSpaceDefinition(), heroShip->GetSensibleArea())) {
							
						//Damage the hero ship
						int result = heroShip->Hit(TYPE_KAMIKAZE);
						
						//NOT_HIT if not hit (shield), 
						//HIT if hit but not dead and 
						//LIVES_LEFT if hit and dead, 
						//GAME_OVER if dead and no more lives left
											
						//Handle the result
						if(result == HIT) {
							//Create a small explosion
							Object* new_object = new Object(NOBODY, TYPE_SMALL_EXPLOSION, kamikaze->GetSpaceDefinition()->x, kamikaze->GetSpaceDefinition()->y);
							objects.push_back(*new_object);
							
						} else if(result == LIVES_LEFT) {
							//Create a big explosion and set invisibility
							Object* new_object = new Object(NOBODY, TYPE_BIG_EXPLOSION, heroShip->GetSpaceDefinition()->x, heroShip->GetSpaceDefinition()->y);
							objects.push_back(*new_object);
							heroShip->SetVisibility(false);
							
						} else if(result == GAME_OVER) {
							//Create a big explosion animation and lose
							quit = true;
							game_over = true;
						}	
						
						//Kill kamikaze
						kamikaze->Kill();
					}
				}
				
				//If kamikaze still alive / Not  hitted the hero ship
				if(kamikaze->StillAlive()) {
					//Paint the object or revome it from the list
					graphics->ApplySurface(kamikaze->GetSpaceDefinition()->x, kamikaze->GetSpaceDefinition()->y, graphics->level_objects, kamikaze->Next());
					
					//Paint the extra images
					if((!motherShip->StillAlive()) && (!fleet->StillAlive())) {
						graphics->ApplySurface((kamikaze->GetSpaceDefinition()->x - 24), (kamikaze->GetSpaceDefinition()->y - 24), graphics->level_exclamation, NULL);
					} else if(heroShip->IsInvisible()) {
						graphics->ApplySurface((kamikaze->GetSpaceDefinition()->x - 24), (kamikaze->GetSpaceDefinition()->y - 24), graphics->level_interrogation, NULL);
					}
					
					kamikaze++;
					
					
				} else {
					kamikaze = kamikazes.erase(kamikaze);
				}
			}
			
			//Relocate the menu and the status variables
			graphics->ApplySurface(0, (HEIGHT - graphics->level_menu->h), graphics->level_menu, NULL);
			//Score
			graphics->DrawText(60, 733, FONT_MEDIUM, IntegerToString(heroShip->GetScore()));
			//Lives
			graphics->DrawText(360, 733, FONT_SMALL, IntegerToString(heroShip->GetLives()));
			//HP
			graphics->DrawText(440, 733, FONT_SMALL, IntegerToString(heroShip->GetHP()).append("%"));
			//Time
			time += time_timer->Get_Ticks();
			time_timer->Start();
			
			unsigned int minutes = (time / 60000);
			unsigned int seconds = (time % 60000) / 1000;
			unsigned int total_seconds = (time / 1000);
			
			string string_time = "";
			
			if(minutes < 10) {
				string_time.append("0");
			}
			string_time.append(IntegerToString(minutes));
			string_time.append(":");
			
			if(seconds < 10) {
				string_time.append("0");
			}
			string_time.append(IntegerToString(seconds));
			graphics->DrawText(870, 733, FONT_MEDIUM, string_time);
			//Laser
			graphics->DrawText(630, 750, FONT_SMALL, "*");
			//Misiles
			graphics->DrawText(660, 750, FONT_SMALL, IntegerToString(heroShip->GetMisiles()));
			//Plasmas
			graphics->DrawText(690, 750, FONT_SMALL, IntegerToString(heroShip->GetPlasmas()));
			//Draw the selection
			int selection = 0;
			if(heroShip->GetWeapon() == TYPE_LASER) {
				selection = 0;
			} else if(heroShip->GetWeapon() == TYPE_MISILE) {
				selection = 1;
			} else if(heroShip->GetWeapon() == TYPE_PLASMA) {
				selection = 2;
			}
			graphics->ApplySurface((620 + (30 * selection)), 716, graphics->level_menu_selection, NULL);
			
			//Check if one second has passed and record the whole scene
			if(total_seconds != seconds_counter) {
				
				//Record this scene
				RegisterScene(total_seconds);
				
				//Reset the timer
				seconds_counter = total_seconds;
			}
			
			//Check if the slow mode time was ended
			if(slow_mode_timer->Is_Started()) {
				if(slow_mode_timer->Get_Ticks() >= 5000) {
					slow_mode_timer->Stop();
				} else {
					int seconds = (5 - ((slow_mode_timer->Get_Ticks() % 60000) / 1000));
					graphics->DrawText((heroShip->GetSpaceDefinition()->x + heroShip->GetSpaceDefinition()->w - 10), (heroShip->GetSpaceDefinition()->y + heroShip->GetSpaceDefinition()->h - 10), FONT_SMALL, IntegerToString(seconds));
				}
			}
			
			//Update the screen
			graphics->UpdateScreen();
			
			//Cap the frame rate 
			if(fps->Get_Ticks() < (1000 / FRAMES_PER_SECOND)) { 
				SDL_Delay( (1000 / FRAMES_PER_SECOND) - fps->Get_Ticks()); 
			}
		}
		
		if(level_ended) {		//The player win the level
			
			int delay = 200;
			
			graphics->ApplySurface(0, 0, graphics->level_statistics, NULL);
			SDL_Delay(delay);
			graphics->UpdateScreen();
			
			graphics->DrawText(452, 173, FONT_MEDIUM, IntegerToString(heroShip->invader_low_killed));
			SDL_Delay(delay);
			graphics->UpdateScreen();
			graphics->DrawText(452, 213, FONT_MEDIUM, IntegerToString(heroShip->invader_medium_killed));
			SDL_Delay(delay);
			graphics->UpdateScreen();
			graphics->DrawText(452, 253, FONT_MEDIUM, IntegerToString(heroShip->invader_high_killed));
			SDL_Delay(delay);
			graphics->UpdateScreen();
			graphics->DrawText(452, 293, FONT_MEDIUM, IntegerToString(heroShip->mothership_killed));
			SDL_Delay(delay);
			graphics->UpdateScreen();
			graphics->DrawText(452, 333, FONT_MEDIUM, IntegerToString(heroShip->kamikazes_killed));
			SDL_Delay(delay);
			graphics->UpdateScreen();
			
			graphics->DrawText(452, 389, FONT_MEDIUM, IntegerToString(heroShip->invader_low_killed + heroShip->invader_medium_killed + heroShip->invader_high_killed + heroShip->mothership_killed + heroShip->kamikazes_killed));
			SDL_Delay(delay);
			graphics->UpdateScreen();
			
			graphics->DrawText(692, 173, FONT_MEDIUM, IntegerToString(heroShip->amount_shields));
			SDL_Delay(delay);
			graphics->UpdateScreen();
			graphics->DrawText(692, 213, FONT_MEDIUM, IntegerToString(heroShip->amount_invis));
			SDL_Delay(delay);
			graphics->UpdateScreen();
			graphics->DrawText(692, 253, FONT_MEDIUM, IntegerToString(heroShip->amount_misiles));
			SDL_Delay(delay);
			graphics->UpdateScreen();
			graphics->DrawText(692, 293, FONT_MEDIUM, IntegerToString(heroShip->amount_plasmas));
			SDL_Delay(delay);
			graphics->UpdateScreen();
			graphics->DrawText(692, 333, FONT_MEDIUM, IntegerToString(heroShip->amount_times));
			SDL_Delay(delay);
			graphics->UpdateScreen();
			
			graphics->DrawText(692, 389, FONT_MEDIUM, IntegerToString(heroShip->amount_shields + heroShip->amount_invis + heroShip->amount_misiles + heroShip->amount_plasmas + heroShip->amount_times));
			SDL_Delay(delay);
			graphics->UpdateScreen();
			
			graphics->DrawText(452, 450, FONT_MEDIUM, IntegerToString(heroShip->score));
			SDL_Delay(delay);
			graphics->UpdateScreen();
			
			
			int minutes = (time / 60000);
			int seconds = (time % 60000) / 1000;
			string string_time = "";
			if(minutes < 10) {
				string_time.append("0");
			}
			string_time.append(IntegerToString(minutes));
			string_time.append(":");
			if(seconds < 10) {
				string_time.append("0");
			}
			string_time.append(IntegerToString(seconds));
			graphics->DrawText(452, 508, FONT_MEDIUM, string_time);
			SDL_Delay(delay);
			graphics->UpdateScreen();
			graphics->DrawText(452, 558, FONT_MEDIUM, IntegerToString(heroShip->GetLives()));
			SDL_Delay(delay);
			graphics->UpdateScreen();
			
			//Wait for a left click
			WaitForAClick();
			
		} else if(game_over) {		//The player lose the level
			//Animation
			SDL_Surface* backup_surface;
			backup_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, WIDTH, HEIGHT, BPP, 0x00, 0x00, 0x00, 0x00);
			if(backup_surface == NULL) {
				fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
				exit(1);
			}
			//BackUp the current screen
			graphics->ApplySurface(0, 0, graphics->screen, backup_surface, NULL);
			//Fade out the backuped screen
			graphics->FadeOut(backup_surface, 1);
			//Free the backup surface
			SDL_FreeSurface(backup_surface);
			
			//Apply the game over screen
			graphics->FadeIn(graphics->level_game_over,1);
			
			//Wait for a click
			WaitForAClick();
			
			if(top10->NewScore(heroShip->score)) {
				//Ask for a name and put it to the score
				string name = GetInput();
				top10->AddScore(name, heroShip->score, heroShip->invaders_killed);
			}
			
			
		} else {					//The player just want to exit
			game_over = true;
		}
	}
}

void Game::AnimationLevelInit() {
	
	//Animation
	SDL_Surface* backup_surface;
	backup_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, WIDTH, HEIGHT, BPP, 0x00, 0x00, 0x00, 0x00);
    if(backup_surface == NULL) {
        fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
       	exit(1);
	}
	//BackUp the current screen
	graphics->ApplySurface(0, 0, graphics->screen, backup_surface, NULL);
	//Fade out the backuped screen
	graphics->FadeOut(backup_surface, 1);
	//Free the backup surface
	SDL_FreeSurface(backup_surface);
	//Custom FadeIn
	
	int alpha = SDL_ALPHA_OPAQUE;
	int reduction = (SDL_ALPHA_OPAQUE / FRAMES_PER_SECOND);
	
	while(alpha > SDL_ALPHA_TRANSPARENT) {
		fps->Start();
		
		//Set surface alpha 
		SDL_SetAlpha(graphics->black_mask, SDL_SRCALPHA, alpha);
		
		//Apply the background
		graphics->ApplySurface(0, 0, graphics->level_background, graphics->AnimationBackground());
		//Apply the front 
		graphics->ApplySurface(0, 0, graphics->black_mask, NULL);
		
		//Delay for frame per seconds
		if(fps->Get_Ticks() < (1000 / FRAMES_PER_SECOND)) { 
			SDL_Delay( (1000 / FRAMES_PER_SECOND) - fps->Get_Ticks()); 
		}
		
		//Update the screen
		graphics->UpdateScreen();
		
		//Reduce the alpha of the black mask
		alpha -= reduction;
	}
	
	//Animate the fleet and the hero ship
	for(int n = 0, 
		y_hero = (heroShip->GetGameLine() + 150),
		y_fleet = (fleet->GetSpaceDefinition()->y - 450),
		x_mothership = -150;
		n != (FRAMES_PER_SECOND * 5); n++) {
		
		//Start counting the compute time
		fps->Start();
		
		//Apply the background and the number
		graphics->ApplySurface(0, 0, graphics->level_background, graphics->AnimationBackground());
		graphics->ApplySurface(heroShip->GetLocation(), y_hero, graphics->level_hero, heroShip->GetStatus());
		graphics->ApplySurface(x_mothership, motherShip->GetSpaceDefinition()->y, graphics->level_invaders, motherShip->Next());
		for(short row = 0, column = 0; row != FLEET_ROWS; column++) {
				
			//Reset the columns when reached the limit of the matrix
			if(column == FLEET_COLUMNS) {
				column = -1;
				row++;
			} else {
					
				//Paint the invader
				int x = (fleet->GetSpaceDefinition()->x + (column * INVADER_SPRITE_DIMENSION) + (column * INVADERS_SEPARATION));
				int y = (y_fleet + (row * INVADER_SPRITE_DIMENSION));
				graphics->ApplySurface(x, y, graphics->level_invaders, fleet->Get(row, column)->Next());

			}
		}
		
		//Delay for frame per seconds
		if(fps->Get_Ticks() < (1000 / FRAMES_PER_SECOND)) { 
			SDL_Delay( (1000 / FRAMES_PER_SECOND) - fps->Get_Ticks()); 
		}
	
		//Update the screen
		graphics->UpdateScreen();
		
		y_hero--;
		y_fleet += 3;
		x_mothership++;
	}
	
	//Show the 3,2,1 sequence
	//Variables needed to paint the 3,2,1 sequence
	//********************
	//Cannot combine per-pixel alpha and per-surface alpha. 
	//Because of that, alpha code was commented.
	//********************
	//alpha = SDL_ALPHA_OPAQUE;
	
	SDL_Rect numbers_offset;
	numbers_offset.x = (NUMBERS_SPRITE_DIMENSION * 2);
	numbers_offset.y = 0;
	numbers_offset.w = NUMBERS_SPRITE_DIMENSION;
	numbers_offset.h = ((NUMBERS_SPRITE_DIMENSION / 4) * 3);
	
	for(int n = 0, m = 1, x = ((WIDTH - numbers_offset.w) / 2), y = ((HEIGHT - numbers_offset.h) / 2); n != (3 * FRAMES_PER_SECOND); n++) {
		
		//Start counting the compute time
		fps->Start();
		
		//Set surface alpha 
		//SDL_SetAlpha(graphics->level_numbers, SDL_SRCALPHA, alpha);
		
		//Apply the background, the hero, the invaders and the number
		graphics->ApplySurface(0, 0, graphics->level_background, graphics->AnimationBackground());
		graphics->ApplySurface(heroShip->GetLocation(), heroShip->GetGameLine(), graphics->level_hero, heroShip->GetStatus());
		graphics->ApplySurface(motherShip->GetSpaceDefinition()->x, motherShip->GetSpaceDefinition()->y, graphics->level_invaders, motherShip->Next());
		for(short row = 0, column = 0; row != FLEET_ROWS; column++) {
				
			//Reset the columns when reached the limit of the matrix
			if(column == FLEET_COLUMNS) {
				column = -1;
				row++;
			} else {
					
				//Paint the invader
				int x = (fleet->GetSpaceDefinition()->x + (column * INVADER_SPRITE_DIMENSION) + (column * INVADERS_SEPARATION));
				int y = (fleet->GetSpaceDefinition()->y + (row * INVADER_SPRITE_DIMENSION));
				graphics->ApplySurface(x, y, graphics->level_invaders, fleet->Get(row, column)->Next());

			}
		}
		//Apply the numbers surface
		graphics->ApplySurface(x, y, graphics->level_numbers, &numbers_offset);
		
		//Change the numbers
		if(	(n == (FRAMES_PER_SECOND * m))	) {
			//Change the offset graphics
			numbers_offset.x -= NUMBERS_SPRITE_DIMENSION;
			//Reset the alpha of the image
			//alpha = SDL_ALPHA_OPAQUE;
			m++;
		}
		
		//Delay for frame per seconds
		if(fps->Get_Ticks() < (1000 / FRAMES_PER_SECOND)) { 
			SDL_Delay( (1000 / FRAMES_PER_SECOND) - fps->Get_Ticks()); 
		}
	
		//Update the screen
		graphics->UpdateScreen();
		
		//Reduce the alpha of the black mask
		//alpha -= reduction;
	}
	
}

void Game::CheckIfKilled(Invader* invader) {
	if(!invader->StillAlive()) {
		//Update the score
		heroShip->UpdateScore(invader->GetType());
		
		if(!LastOne()) {
			 
			//Free an item if it wasn't the last invader
			int item = Invader::FreeItem();
			
			if(invader->GetType() == INVADER_MOTHERSHIP) {
				//Load that item
				if(item != TYPE_NONE) {
					Object* new_object = new Object(invader->GetType(), item, (motherShip->GetSpaceDefinition()->x + INVADER_SPRITE_DIMENSION), (motherShip->GetSpaceDefinition()->y + (INVADER_SPRITE_DIMENSION / 2)));
					objects.push_back(*new_object);
				}
				
				//Create a big explosion
				Object* new_object = new Object(NOBODY, TYPE_BIG_EXPLOSION, (motherShip->GetSpaceDefinition()->x + (INVADER_SPRITE_DIMENSION / 2)), motherShip->GetSpaceDefinition()->y);
				objects.push_back(*new_object);
				
			} else {
				//Register the kill in the fleet;
				fleet->KillAnInvader();
				if(LastOne()) {
					level_ended = true;
					final_gem = new Object(invader->GetType(), TYPE_GEM, (invader->GetSpaceDefinition()->x + (INVADER_SPRITE_DIMENSION / 2)), (invader->GetSpaceDefinition()->y  + (INVADER_SPRITE_DIMENSION / 2)));
				} else {
					//Load that item
					if(item != TYPE_NONE) {
						Object* new_object = new Object(invader->GetType(), item, (invader->GetSpaceDefinition()->x + (INVADER_SPRITE_DIMENSION / 2)), (invader->GetSpaceDefinition()->y  + (INVADER_SPRITE_DIMENSION / 2)));
						objects.push_back(*new_object);
					}
				}
				
				//Create a big explosion
				Object* new_object = new Object(NOBODY, TYPE_BIG_EXPLOSION, invader->GetSpaceDefinition()->x, invader->GetSpaceDefinition()->y);
				objects.push_back(*new_object);
			}
		} else {
			level_ended = true;
			final_gem = new Object(invader->GetType(), TYPE_GEM, (invader->GetSpaceDefinition()->x + (INVADER_SPRITE_DIMENSION / 2)), (invader->GetSpaceDefinition()->y  + (INVADER_SPRITE_DIMENSION / 2)));
		}
	}
}

//Check if the killed invaders was the last one
bool Game::LastOne() {
	
	bool kamikazeStillAlive = false;
	list<Object>::iterator kamikaze = kamikazes.begin();
	while(kamikaze != kamikazes.end()) {
		
		if(kamikaze->StillAlive()) {
			kamikazeStillAlive = true;
			kamikaze = kamikazes.end();
		} else {
			kamikaze++;
		}
	}
	
	return (!(fleet->StillAlive()) &&
			!(motherShip->StillAlive()) &&
			!kamikazeStillAlive);
}

void Game::WaitForAClick() {
	
	//Wait for a click
	SDL_Event event;
	bool wait = true;
	while(wait) {
		SDL_WaitEvent(&event);

		if(event.type == SDL_MOUSEBUTTONDOWN) {
			if(event.button.button == SDL_BUTTON_RIGHT) {
				wait = false;
			}
		}
	}
}

string Game::GetInput() {
	
	//Redraw the templat graphic
	graphics->FadeOut(graphics->level_game_over, 1);
	graphics->FadeIn(graphics->level_new_score, 1);
					
	SDL_EnableUNICODE(SDL_ENABLE);
	
	string name = "";
	bool name_entered = false;
	bool something_change = false;
	
	SDL_Event event;
	
	while(!name_entered) {
		
		SDL_WaitEvent(&event);
		
		//If a key was pressed
	    if(event.type == SDL_KEYDOWN) {
	    	
	    	something_change = false;
		        
			//If backspace was pressed and the string isn't blank
	        if((event.key.keysym.sym == SDLK_BACKSPACE) && (name.length() != 0)) {
	            
	            //Remove a character from the end
	            name.erase(name.length() - 1);
	            //A change has been made to the string
	            something_change = true;
	            
	        } else if(event.key.keysym.sym == SDLK_RETURN) {
	        	
	        	if(name.length() != 0) {
	        		//The name was entered and is not null
	        		name_entered = true;
	        	}
	        	
	        } else {
		        
		        //If the string less than maximum size
		        if(name.length() <= 7) {
		        	
		            if(event.key.keysym.unicode == (Uint16)' ') {															//If the key is a space
		            	
		                //Don't append the character
		                //name += (char)event.key.keysym.unicode;
		                    
		            } else if((event.key.keysym.unicode >= (Uint16)'0') && (event.key.keysym.unicode <= (Uint16)'9')) { 	//If the key is a number
		                
		                //Append the character
		                name += (char)event.key.keysym.unicode;
		                //A change has been made to the string
	            		something_change = true;
		                
		            } else if((event.key.keysym.unicode >= (Uint16)'A') && (event.key.keysym.unicode <= (Uint16)'Z')) {		//If the key is a uppercase letter
		            	
		                //Append the character
		                name += (char)event.key.keysym.unicode;
		                //A change has been made to the string
	            		something_change = true;
		            } else if((event.key.keysym.unicode >= (Uint16)'a') && (event.key.keysym.unicode <= (Uint16)'z')) {		//If the key is a lowercase letter
		            	
						//Append the character
						name += (char)event.key.keysym.unicode;
						//A change has been made to the string
	            		something_change = true;
					}
		        }
	        }
			
			if(something_change) {
			        //Redraw the templat graphic
			        graphics->ApplySurface(0,0, graphics->level_new_score, NULL);
			        //Draw it to the window
					graphics->DrawText(472, 379, FONT_MEDIUM, name);
			        //Update the screen
					graphics->UpdateScreen();
			}
	    }	
	}
	
	SDL_EnableUNICODE( SDL_DISABLE );
	
	return name;
}

bool Game::CheckCollision(SDL_Rect* A, SDL_Rect* B) {
	
	//If any of the sides from A are outside of B 
	
	if((A->y + A->h) <= B->y) { 		//bottomA <= topB
		return false; 
	} if(A->y >= (B->y + B->h)) {		//topA >= bottomB 
		return false; 
	} if((A->x + A->w) <= B->x) {		//rightA <= leftB 
		return false; 
	} if(A->x >= (B->x + B->w)) {		//leftA >= rightB 
		return false; 
	} 
	
	//If none of the sides from A are outside B 
	return true; 
	
}

void Game::RegisterScene(unsigned int second) {
	
	//Add the ID, the hash for the hero ship, the hash for the fleet and the hash for the mothership
	Scene* new_scene = new Scene(second, heroShip->GetHash(), fleet->GetHash(), motherShip->GetHash());
	//Add the hash of each invader
	for(short row = 0, column = 0; row != FLEET_ROWS; column++) {
				
		//Reset the columns when reached the limit of the matrix
		if(column == FLEET_COLUMNS) {
			column = -1;
			row++;
		} else {
			
			//Add the hash of the invader
			new_scene->AddInvader(fleet->Get(row, column)->GetHash());
			
		}
	}
	//Add the hash of each kamikaze
	for(list<Object>::iterator kamikaze = kamikazes.begin(); kamikaze != kamikazes.end(); kamikaze++) {
		//Add the hash of the invader
		new_scene->AddKamikaze(kamikaze->GetHash());
	}
	//Add the hash of each object
	for(list<Object>::iterator object = objects.begin(); object != objects.end(); object++) {
		//Add the hash of the object
		new_scene->AddObject(object->GetHash());
	}
	
	//Insert the new scene
	new_scene->Show();
	scenes.Insert(new_scene);
	
}

void Game::LoadScene(SDL_Surface* precreated_surface, Scene* scene, int index, int count, float pixels_per_second, bool generate_report) {
	
	//*****************************************************
	//Load the graphic template
	graphics->ApplySurface(0, 0, graphics->level_background, precreated_surface, graphics->AnimationBackground());
	graphics->ApplySurface(0, (HEIGHT - graphics->level_menu->h), graphics->level_menu, precreated_surface, NULL);
	//*****************************************************
	
	
	
	//*****************************************************
	//Seconds hash
	unsigned int second = (scene->GetSecond() % 60);
	unsigned int minutes = (scene->GetSecond() / 60);
	
	string string_time = "";
	
	if(minutes < 10) {
		string_time.append("0");
	}
	string_time.append(IntegerToString(minutes));
	string_time.append(":");
	
	if(second < 10) {
		string_time.append("0");
	}
	string_time.append(IntegerToString(second));
	graphics->DrawText(870, 733, FONT_MEDIUM, string_time, precreated_surface);
	
	//Generate report
	if(generate_report) {
		scene->Show();
		
		cout << "***********************************************************" << endl;
		cout << "Individual object report for " << second << ", " << string_time << "." << endl;
		cout << endl;
	}
	//*****************************************************
	
	
	
	//*****************************************************
	//Hero ship hash
	//Example 	:	0|3|100|12|0|0|0|0|488|604|64|64|504|620|32|32|0|0|64|64
	//Data		: 	score|lives|hp|weapon|misiles|plasmas|shield|invisibility|space_definition.x|space_definition.y|space_definition.h|space_definition.w|sensibility_area.x|sensibility_area.y|sensibility_area.h|sensibility_area.w|graphics.x|graphics.y|graphics.h|graphics.w
	
	//Hash
	string hero_ship_hash = scene->GetHeroShipHash();
	
	//Variables
	unsigned int hero_ship_score;
	unsigned int hero_ship_lives;
	unsigned int hero_ship_health_points;
	unsigned int hero_ship_weapon;
	unsigned int hero_ship_misiles;
	unsigned int hero_ship_plasmas;
	bool hero_ship_shield;
	bool hero_ship_invisibility;
	SDL_Rect hero_ship_space_definition;
	SDL_Rect hero_ship_sensible_area;
	SDL_Rect hero_ship_graphics;
	
	//Scanner
	istringstream hero_ship_stream(hero_ship_hash);
	
	//Load the variables
	hero_ship_stream >> hero_ship_score;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_lives;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_health_points;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_weapon;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_misiles;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_plasmas;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_shield;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_invisibility;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_space_definition.x;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_space_definition.y;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_space_definition.h;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_space_definition.w;
	hero_ship_stream.ignore(); // Ignore the '|'	
	hero_ship_stream >> hero_ship_sensible_area.x;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_sensible_area.y;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_sensible_area.h;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_sensible_area.w;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_graphics.x;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_graphics.y;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_graphics.h;
	hero_ship_stream.ignore(); // Ignore the '|'
	hero_ship_stream >> hero_ship_graphics.w;
	hero_ship_stream.ignore(); // Ignore the '|'
	
	//Use the parsed variables
	//Score
	graphics->DrawText(60, 733, FONT_MEDIUM, IntegerToString(hero_ship_score), precreated_surface);
	//Lives
	graphics->DrawText(360, 733, FONT_SMALL, IntegerToString(hero_ship_lives), precreated_surface);
	//HP
	graphics->DrawText(440, 733, FONT_SMALL, IntegerToString(hero_ship_health_points).append("%"), precreated_surface);
	//Laser
	graphics->DrawText(630, 750, FONT_SMALL, "*", precreated_surface);
	//Misiles
	graphics->DrawText(660, 750, FONT_SMALL, IntegerToString(hero_ship_misiles), precreated_surface);
	//Plasmas
	graphics->DrawText(690, 750, FONT_SMALL, IntegerToString(hero_ship_plasmas), precreated_surface);
	//Draw the selection
	int selection = 0;
	if(hero_ship_weapon == TYPE_LASER) {
		selection = 0;
	} else if(hero_ship_weapon == TYPE_MISILE) {
		selection = 1;
	} else if(hero_ship_weapon == TYPE_PLASMA) {
		selection = 2;
	}
	graphics->ApplySurface((620 + (30 * selection)), 716, graphics->level_menu_selection, precreated_surface, NULL);
	//Paint the Hero
	graphics->ApplySurface(hero_ship_space_definition.x, hero_ship_space_definition.y, graphics->level_hero, precreated_surface, &hero_ship_graphics);
	
	//Report
	if(generate_report) {
		cout << "========================================================================================" << endl;
		cout << "Object type:\tHeroShip" << endl;
		cout << "Hash:\t\t" << hero_ship_hash << endl;
		cout << "Attributes:" << endl;
		cout << "\t Score:\t\t\t\t\t" << hero_ship_score << endl;
		cout << "\t Lives left:\t\t\t\t" << hero_ship_lives << endl;
		cout << "\t Health points:\t\t\t\t" << hero_ship_health_points << endl;
		cout << "\t Weapon Number:\t\t\t\t" << hero_ship_weapon << endl;
		cout << "\t Number Misiles:\t\t\t" << hero_ship_misiles << endl;
		cout << "\t Number Plasmas:\t\t\t" << hero_ship_plasmas << endl;
		cout << "\t Shield activated?:\t\t\t" << (hero_ship_shield?"Yes":"No") << endl;
		cout << "\t Invisibility activated?:\t\t" << (hero_ship_invisibility?"Yes":"No") << endl;
		cout << "\t In space definiton (x,y,h,w):\t\t";
		cout << hero_ship_space_definition.x << ",";
		cout << hero_ship_space_definition.y << ",";
		cout << hero_ship_space_definition.h << ",";
		cout << hero_ship_space_definition.w << endl;		
		cout << "\t Sensible area (x,y,h,w)?:\t\t";
		cout << hero_ship_sensible_area.x << ",";
		cout << hero_ship_sensible_area.y << ",";
		cout << hero_ship_sensible_area.h << ",";
		cout << hero_ship_sensible_area.w << endl;
		cout << "\t In image definition (x,y,h,w)?:\t" ;
		cout << hero_ship_graphics.x << ",";
		cout << hero_ship_graphics.y << ",";
		cout << hero_ship_graphics.h << ",";
		cout << hero_ship_graphics.w << endl;
		cout << "========================================================================================" << endl << endl;
	}
	//*****************************************************
	
	
	
	//*****************************************************
	//Mothership hash
	//Example 	:	11|2|1|4|-4|256|192|64|128|360|10|64|128
	//Data		: 	type|fire_probability|alive|resistance|speed|graphics.x|graphics.y|graphics.h|graphics.w|space_definition.x|space_definition.y|space_definition.h|space_definition.w
	
	//Hash
	string mothership_hash = scene->GetMothershipHash();
	
	//Variables
	int mothership_type;
	int mothership_fire_probability;
	bool mothership_alive;
	int mothership_resistance;
	int mothership_speed;
	SDL_Rect mothership_graphics;
	SDL_Rect mothership_space_definition;
	
	//Scanner
	istringstream mothership_stream(mothership_hash);
	
	//Load the variables
	mothership_stream >> mothership_type;
	mothership_stream.ignore(); // Ignore the '|'
	mothership_stream >> mothership_fire_probability;
	mothership_stream.ignore(); // Ignore the '|'
	mothership_stream >> mothership_alive;
	mothership_stream.ignore(); // Ignore the '|'
	mothership_stream >> mothership_resistance;
	mothership_stream.ignore(); // Ignore the '|'
	mothership_stream >> mothership_speed;
	mothership_stream.ignore(); // Ignore the '|'
	mothership_stream >> mothership_graphics.x;
	mothership_stream.ignore(); // Ignore the '|'
	mothership_stream >> mothership_graphics.y;
	mothership_stream.ignore(); // Ignore the '|'
	mothership_stream >> mothership_graphics.h;
	mothership_stream.ignore(); // Ignore the '|'
	mothership_stream >> mothership_graphics.w;
	mothership_stream.ignore(); // Ignore the '|'
	mothership_stream >> mothership_space_definition.x;
	mothership_stream.ignore(); // Ignore the '|'
	mothership_stream >> mothership_space_definition.y;
	mothership_stream.ignore(); // Ignore the '|'
	mothership_stream >> mothership_space_definition.h;
	mothership_stream.ignore(); // Ignore the '|'
	mothership_stream >> mothership_space_definition.w;
	mothership_stream.ignore(); // Ignore the '|'
	
	//Use the parsed variables
	if(mothership_alive) {
		graphics->ApplySurface(mothership_space_definition.x, mothership_space_definition.y, graphics->level_invaders, precreated_surface, &mothership_graphics);
	}
	
	//Report
	if(generate_report) {
		cout << "========================================================================================" << endl;
		cout << "Object type:\tMothership" << endl;
		cout << "Hash:\t\t" << mothership_hash << endl;
		cout << "Attributes:" << endl;
		cout << "\t Fire probability:\t\t\t" << mothership_fire_probability << endl;
		cout << "\t Still alive?:\t\t\t\t" << (mothership_alive?"Yes":"No") << endl;
		cout << "\t Resistance left:\t\t\t" << mothership_resistance << endl;
		cout << "\t Actual speed:\t\t\t\t" << mothership_speed << endl;
		cout << "\t In space definiton (x,y,h,w):\t\t";
		cout << mothership_space_definition.x << ",";
		cout << mothership_space_definition.y << ",";
		cout << mothership_space_definition.h << ",";
		cout << mothership_space_definition.w << endl;
		cout << "\t In image definition (x,y,h,w)?:\t" ;
		cout << mothership_graphics.x << ",";
		cout << mothership_graphics.y << ",";
		cout << mothership_graphics.h << ",";
		cout << mothership_graphics.w << endl;
		cout << "========================================================================================" << endl << endl;
	}
	//*****************************************************
	
	
	
	//*****************************************************
	//Fleet hash
	//Example 	:	402|64|320|582|3|10|40|
	//Data		: 	space_definition.x|space_definition.y|space_definition.h|space_definition.w|horizontal_speed|vertical_speed|total_invaders
	
	//Hash
	string fleet_hash = scene->GetFleetHash();
	
	//Variables
	SDL_Rect fleet_space_definition;
	int fleet_horizontal_speed;
	int fleet_vertical_speed;
	unsigned int fleet_total_invaders;
	
	//Scanner
	istringstream fleet_stream(fleet_hash);
	
	//Load the variables
	fleet_stream >> fleet_space_definition.x;
	fleet_stream.ignore(); // Ignore the '|'
	fleet_stream >> fleet_space_definition.y;
	fleet_stream.ignore(); // Ignore the '|'
	fleet_stream >> fleet_space_definition.h;
	fleet_stream.ignore(); // Ignore the '|'
	fleet_stream >> fleet_space_definition.w;
	fleet_stream.ignore(); // Ignore the '|'
	fleet_stream >> fleet_horizontal_speed;
	fleet_stream.ignore(); // Ignore the '|'
	fleet_stream >> fleet_vertical_speed;
	fleet_stream.ignore(); // Ignore the '|'
	fleet_stream >> fleet_total_invaders;
	fleet_stream.ignore(); // Ignore the '|'
	
	//Generate report
	//Report
	if(generate_report) {
		cout << "========================================================================================" << endl;
		cout << "Object type:\tFleet" << endl;
		cout << "Hash:\t\t" << fleet_hash << endl;
		cout << "Attributes:" << endl;
		cout << "\t Still alive?:\t\t\t\t" << ((fleet_total_invaders > 0)?"Yes":"No") << endl;
		cout << "\t Invaders left:\t\t\t\t" << fleet_total_invaders << endl;
		cout << "\t Actual horizontal speed:\t\t" << fleet_horizontal_speed << endl;
		cout << "\t Actual vertical speed:\t\t\t" << fleet_vertical_speed << endl;
		cout << "\t In space definiton (x,y,h,w):\t\t";
		cout << fleet_space_definition.x << ",";
		cout << fleet_space_definition.y << ",";
		cout << fleet_space_definition.h << ",";
		cout << fleet_space_definition.w << endl;
		cout << "========================================================================================" << endl << endl;
	}
	
	
	//Use the parsed variables
	if(fleet_total_invaders > 0) {
		
		list<string>* invaders = scene->GetInvaders(); 
		
		list<string>::iterator invader_iterator = invaders->begin();
		
		for(short x = fleet_space_definition.x, 
			y = fleet_space_definition.y, 
			row = 0, 
			column = 0; 
			invader_iterator != invaders->end(); 
			column++) {
					
			//Reset the columns when reached the limit of the matrix
			if(column == FLEET_COLUMNS) {
				column = -1;
				row++;
			} else {
				
				//Hash
				string invader_hash = (*invader_iterator);
				
				//Variables
				int invader_type;
				int invader_fire_probability;
				bool invader_alive;
				int invader_resistance;
				int invader_speed;
				SDL_Rect invader_graphics;
				SDL_Rect invader_space_definition;
				
				//Scanner
				istringstream invader_stream(invader_hash);
				
				//Load the variables
				invader_stream >> invader_type;
				invader_stream.ignore(); // Ignore the '|'
				invader_stream >> invader_fire_probability;
				invader_stream.ignore(); // Ignore the '|'
				invader_stream >> invader_alive;
				invader_stream.ignore(); // Ignore the '|'
				invader_stream >> invader_resistance;
				invader_stream.ignore(); // Ignore the '|'
				invader_stream >> invader_speed;
				invader_stream.ignore(); // Ignore the '|'
				invader_stream >> invader_graphics.x;
				invader_stream.ignore(); // Ignore the '|'
				invader_stream >> invader_graphics.y;
				invader_stream.ignore(); // Ignore the '|'
				invader_stream >> invader_graphics.h;
				invader_stream.ignore(); // Ignore the '|'
				invader_stream >> invader_graphics.w;
				invader_stream.ignore(); // Ignore the '|'
				invader_stream >> invader_space_definition.x;
				invader_stream.ignore(); // Ignore the '|'
				invader_stream >> invader_space_definition.y;
				invader_stream.ignore(); // Ignore the '|'
				invader_stream >> invader_space_definition.h;
				invader_stream.ignore(); // Ignore the '|'
				invader_stream >> invader_space_definition.w;
				invader_stream.ignore(); // Ignore the '|'
				
				//Use the parsed variables
				if(invader_alive) {
					
					//Compute the position
					int invader_x = (x + (column * INVADER_SPRITE_DIMENSION) + (column * INVADERS_SEPARATION));
					int invader_y = (y + (row * INVADER_SPRITE_DIMENSION));
					//Paint the invader
					graphics->ApplySurface(invader_x, invader_y, graphics->level_invaders, precreated_surface, &invader_graphics);
				}
				
				//Report
				if(generate_report) {
					cout << "========================================================================================" << endl;
					cout << "Object type:\tInvader" << endl;
					cout << "Hash:\t\t" << invader_hash << endl;
					cout << "Attributes:" << endl;
					cout << "\t Invader class number:\t\t\t" << invader_type << endl;
					cout << "\t Fire probability:\t\t\t" << invader_fire_probability << endl;
					cout << "\t Still alive?:\t\t\t\t" << (invader_alive?"Yes":"No") << endl;
					cout << "\t Resistance left:\t\t\t" << invader_resistance << endl;
					cout << "\t Actual speed:\t\t\t\t" << invader_speed << endl;
					cout << "\t In space definiton (x,y,h,w):\t\t";
					cout << invader_space_definition.x << ",";
					cout << invader_space_definition.y << ",";
					cout << invader_space_definition.h << ",";
					cout << invader_space_definition.w << endl;
					cout << "\t In image definition (x,y,h,w)?:\t" ;
					cout << invader_graphics.x << ",";
					cout << invader_graphics.y << ",";
					cout << invader_graphics.h << ",";
					cout << invader_graphics.w << endl;
					cout << "========================================================================================" << endl << endl;
				}
				//Step the iterator
				invader_iterator++;
			}
		}
	}
	//*****************************************************
	
	
	
	//*****************************************************
	//Objects container
	list<string>* objects = scene->GetObjects();
	
	for(list<string>::iterator object_iterator = objects->begin(); object_iterator != objects->end(); object_iterator++) {
		
		//Object Hash
		//Example 	:	7|12|1|0|0|3|192|96|32|32|345|724|32|32
		//Data		: 	from|type|alive|resistance|horizontal_speed|vertical_speed|graphics.x|graphics.y|graphics.h|graphics.w|space_definition.x|space_definition.y|space_definition.h|space_definition.w
		
		//Hash
		string object_hash = (*object_iterator);
		
		//Variables
		int unsigned object_from;
		int unsigned object_type;
		bool object_alive;
		int object_resistance;
		int object_horizontal_speed;
		int object_vertical_speed;
		SDL_Rect object_graphics;
		SDL_Rect object_space_definition;
		
		//Scanner
		istringstream object_stream(object_hash);
		
		//Load the variables
		object_stream >> object_from;
		object_stream.ignore(); // Ignore the '|'
		object_stream >> object_type;
		object_stream.ignore(); // Ignore the '|'
		object_stream >> object_alive;
		object_stream.ignore(); // Ignore the '|'
		object_stream >> object_resistance;
		object_stream.ignore(); // Ignore the '|'
		object_stream >> object_horizontal_speed;
		object_stream.ignore(); // Ignore the '|'
		object_stream >> object_vertical_speed;
		object_stream.ignore(); // Ignore the '|'
		object_stream >> object_graphics.x;
		object_stream.ignore(); // Ignore the '|'
		object_stream >> object_graphics.y;
		object_stream.ignore(); // Ignore the '|'
		object_stream >> object_graphics.h;
		object_stream.ignore(); // Ignore the '|'
		object_stream >> object_graphics.w;
		object_stream.ignore(); // Ignore the '|'
		object_stream >> object_space_definition.x;
		object_stream.ignore(); // Ignore the '|'
		object_stream >> object_space_definition.y;
		object_stream.ignore(); // Ignore the '|'
		object_stream >> object_space_definition.h;
		object_stream.ignore(); // Ignore the '|'
		object_stream >> object_space_definition.w;
		object_stream.ignore(); // Ignore the '|'
		
		//Use the parse variables
		if(object_alive) {
			if(object_type == TYPE_BIG_EXPLOSION) {
				graphics->ApplySurface(object_space_definition.x, object_space_definition.y, graphics->level_explosion, precreated_surface, &object_graphics);
			} else {
				graphics->ApplySurface(object_space_definition.x, object_space_definition.y, graphics->level_objects, precreated_surface, &object_graphics);
			}
		}
		
		//Report
		if(generate_report) {
			cout << "========================================================================================" << endl;
			cout << "Object type:\tObject" << endl;
			cout << "Hash:\t\t" << object_hash << endl;
			cout << "Attributes:" << endl;
			cout << "\t Who release it (code):\t\t\t" << object_from << endl;
			cout << "\t Object type number:\t\t\t" << object_type << endl;
			cout << "\t Still alive?:\t\t\t\t" << (object_alive?"Yes":"No") << endl;
			cout << "\t Resistance left:\t\t\t" << object_resistance << endl;
			cout << "\t Actual horizontal speed:\t\t" << object_horizontal_speed << endl;
			cout << "\t Actual vertical speed:\t\t\t" << object_vertical_speed << endl;
			cout << "\t In space definiton (x,y,h,w):\t\t";
			cout << object_space_definition.x << ",";
			cout << object_space_definition.y << ",";
			cout << object_space_definition.h << ",";
			cout << object_space_definition.w << endl;
			cout << "\t In image definition (x,y,h,w)?:\t" ;
			cout << object_graphics.x << ",";
			cout << object_graphics.y << ",";
			cout << object_graphics.h << ",";
			cout << object_graphics.w << endl;
			cout << "========================================================================================" << endl << endl;
		}
		
	}
	//*****************************************************
	
	
	
	//*****************************************************
	//Kamikazes container
	list<string>* kamikazes = scene->GetKamikazes();
	
	for(list<string>::iterator kamikaze_iterator = kamikazes->begin(); kamikaze_iterator != kamikazes->end(); kamikaze_iterator++) {
		
		//Object Hash
		//Example 	:	7|12|1|0|0|3|192|96|32|32|345|724|32|32
		//Data		: 	from|type|alive|resistance|horizontal_speed|vertical_speed|graphics.x|graphics.y|graphics.h|graphics.w|space_definition.x|space_definition.y|space_definition.h|space_definition.w
		
		//Hash
		string kamikaze_hash = (*kamikaze_iterator);
		
		//Variables
		int unsigned kamikaze_from;
		int unsigned kamikaze_type;
		bool kamikaze_alive;
		int kamikaze_resistance;
		int kamikaze_horizontal_speed;
		int kamikaze_vertical_speed;
		SDL_Rect kamikaze_graphics;
		SDL_Rect kamikaze_space_definition;
		
		//Scanner
		istringstream kamikaze_stream(kamikaze_hash);
		
		//Load the variables
		kamikaze_stream >> kamikaze_from;
		kamikaze_stream.ignore(); // Ignore the '|'
		kamikaze_stream >> kamikaze_type;
		kamikaze_stream.ignore(); // Ignore the '|'
		kamikaze_stream >> kamikaze_alive;
		kamikaze_stream.ignore(); // Ignore the '|'
		kamikaze_stream >> kamikaze_resistance;
		kamikaze_stream.ignore(); // Ignore the '|'
		kamikaze_stream >> kamikaze_horizontal_speed;
		kamikaze_stream.ignore(); // Ignore the '|'
		kamikaze_stream >> kamikaze_vertical_speed;
		kamikaze_stream.ignore(); // Ignore the '|'
		kamikaze_stream >> kamikaze_graphics.x;
		kamikaze_stream.ignore(); // Ignore the '|'
		kamikaze_stream >> kamikaze_graphics.y;
		kamikaze_stream.ignore(); // Ignore the '|'
		kamikaze_stream >> kamikaze_graphics.h;
		kamikaze_stream.ignore(); // Ignore the '|'
		kamikaze_stream >> kamikaze_graphics.w;
		kamikaze_stream.ignore(); // Ignore the '|'
		kamikaze_stream >> kamikaze_space_definition.x;
		kamikaze_stream.ignore(); // Ignore the '|'
		kamikaze_stream >> kamikaze_space_definition.y;
		kamikaze_stream.ignore(); // Ignore the '|'
		kamikaze_stream >> kamikaze_space_definition.h;
		kamikaze_stream.ignore(); // Ignore the '|'
		kamikaze_stream >> kamikaze_space_definition.w;
		kamikaze_stream.ignore(); // Ignore the '|'
		
		//Use the parse variables
		if(kamikaze_alive) {
			graphics->ApplySurface(kamikaze_space_definition.x, kamikaze_space_definition.y, graphics->level_objects, precreated_surface, &kamikaze_graphics);
		}
		
		//Report
		if(generate_report) {
			cout << "========================================================================================" << endl;
			cout << "Object type:\tKamikaze" << endl;
			cout << "Hash:\t\t" << kamikaze_hash << endl;
			cout << "Attributes:" << endl;
			cout << "\t Who release it :\t\t\tMothership" << endl;
			cout << "\t Object type number:\t\t\t" << kamikaze_type << endl;
			cout << "\t Still alive?:\t\t\t\t" << (kamikaze_alive?"Yes":"No") << endl;
			cout << "\t Resistance left:\t\t\t" << kamikaze_resistance << endl;
			cout << "\t Actual horizontal speed:\t\t" << kamikaze_horizontal_speed << endl;
			cout << "\t Actual vertical speed:\t\t\t" << kamikaze_vertical_speed << endl;
			cout << "\t In space definiton (x,y,h,w):\t\t";
			cout << kamikaze_space_definition.x << ",";
			cout << kamikaze_space_definition.y << ",";
			cout << kamikaze_space_definition.h << ",";
			cout << kamikaze_space_definition.w << endl;
			cout << "\t In image definition (x,y,h,w)?:\t" ;
			cout << kamikaze_graphics.x << ",";
			cout << kamikaze_graphics.y << ",";
			cout << kamikaze_graphics.h << ",";
			cout << kamikaze_graphics.w << endl;
			cout << "========================================================================================" << endl << endl;
			cout << "***********************************************************" << endl;
		}
	}
	//*****************************************************
	
	
	
	//*****************************************************
	//Extras
	//Set the pause mode mask
	graphics->ApplySurface(0, 0, graphics->level_pause, precreated_surface, NULL);
	//Apply the selection
	graphics->ApplySurface((index * pixels_per_second), 16, graphics->level_slider_selection, precreated_surface, NULL);
	//Draw the Numbers
	graphics->DrawText(12, 48, FONT_MEDIUM, "1", precreated_surface);
	graphics->DrawText(950, 48, FONT_MEDIUM, IntegerToString(count), precreated_surface);
	//*****************************************************
}

void Game::GotoScore() {
	
	SDL_Surface* temporal_surface;
	temporal_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, WIDTH, HEIGHT, BPP, 0x00, 0x00, 0x00, 0x00);
	if(temporal_surface == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
		exit(1);
	}
	//BackUp the screen
	graphics->ApplySurface(0, 0, graphics->screen, temporal_surface, NULL);
	
	//FadeOut the current screen
	graphics->FadeOut(temporal_surface, 1);
	
	//FadeIn the graphic template
	graphics->FadeIn(graphics->menu_scores, 1);
	
	//Paint the data of the Top10 object
	int x_name = 232;
	int x_score = 472;
	int x_kills = 712;
	int y = 247;
	
	list<Score>* scores = top10->GetScores();
	for(list<Score>::const_iterator iterator = scores->begin(); iterator != scores->end(); iterator++) {
		graphics->DrawText(x_name, y, FONT_MEDIUM, iterator->name);
		graphics->DrawText(x_score, y, FONT_MEDIUM, IntegerToString(iterator->score));
		graphics->DrawText(x_kills, y, FONT_MEDIUM, IntegerToString(iterator->kills));
		
		y += 36;
	
		//Update the screen
		graphics->UpdateScreen();	
		//Progressive charge
		SDL_Delay(300);
	}
	
	//Wait for a left click
	WaitForAClick();
	
	//BackUp the screen
	graphics->ApplySurface(0, 0, graphics->screen, temporal_surface, NULL);
	
	//FadeOut the current screen
	graphics->FadeOut(temporal_surface, 1);
	
	//Apply the graphics, the program menu.
	graphics->ApplySurface(0, 0, graphics->menu_background, temporal_surface, NULL);

	//Apply the buttons graphics
	for(int n = 0; n < menu->NUMBER_BUTTONS; n++) {
		graphics->ApplySurface(menu->buttons[n].x, menu->buttons[n].y, graphics->menu_buttons, temporal_surface, menu->buttons[n].image);
	}
	
	//FadeIn the new screen
	graphics->FadeIn(temporal_surface, 1);
	
	//Correct the non-exact fade problem
	graphics->ApplySurface(0, 0, graphics->menu_background, NULL);
	
	//Update the screen
	graphics->UpdateScreen();
	
}
