/*============================================================================||
||       __  ___  _______    ___  ____  __         ___                        ||
||      /  \/  /_/__  __/_ _/ __\/_/  \/ /   _____/   \                       ||
||     / /\_/ / ___/ / __ `/ /  __/ __/  \  / ___/  `_/                       ||
||    / /  / / __// / /_/ / /__/ / / / /\ \/ __// /\ \                        ||
||   /_/  /_/____/_/\__,_/\___/_/_/ /_/ /_/____/_/ /_/                        ||
||                                                                            ||
||============================================================================||
|| MetaPacman 																  ||
|| Copyright Tim Jones. All Rights Reserved.                                  ||
||============================================================================*/
#include "Define.h"
#include "Graphics.h"
#include "Util.h"
#include "XFontMap.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_rotozoom.h>

#include <iostream>

#include "CFruit.h"
#include "CGhost.h"
#include "CScore.h"
#include "CPacman.h"
#include "CNibblet.h"
#include "CHighScore.h"

//==============================================================================

using namespace std;

//==============================================================================
void OnEvent();
bool OnInit();

bool OnLoop();
void OnGameLoop();

void OnCutSceneInit();
void OnCutSceneLoop();

void OnRender();
void OnCleanup();

bool CheckNibblets();
bool CheckGhosts();
bool LoadLevel(char* File);

void Reset();
void AddScore(int Points);

//==============================================================================
SDL_Surface*    Display     		= NULL;		//Our main display

SDL_Surface*	Surf_Background		= NULL;		//The background image

SDL_Surface*	Surf_Bar			= NULL;		//The background image

SDL_Surface*	Surf_Lives			= NULL;		//Image to represent lives

SDL_Surface*	Surf_Score			= NULL;		//Image to show score

SDL_Surface*	Surf_Difficulty		= NULL;		//Image to show difficulty

SDL_Surface*	Surf_Paused			= NULL;		//Image to show paused

SDL_Surface*	Surf_Title			= NULL;		//Image for Title Screen

//------------------------------------------------------------------------------
SDL_Event       Event;							//Our main event structure

bool            Running     		= true;		//Is our game still running?

CFruit			Fruit;

CGhost			Ghosts[MAX_GHOSTS];				//Each of our ghosts

double			SpawnTime			= 0;		//Time from last ghost spawn

double			VulnerableTime		= 0;		//How long ghosts have been vulnerable

CNibblet		NibbletList[MAP_AREA];			//Used to fill map with little nibblets

int				NumberNibblets		= 0;		//How many nibblets do we have? 
												//(Used to check if Pacman beat 
            									//the level)
            									
bool			CutScene			= false;

XFontMap		FontControl;

CHighScore		HighScore;

CScore			ScoreAnim;

//------------------------------------------------------------------------------
int				Score				= 0;		//Pacman's current score

int				OldScore			= 0;		//Pacman's old score (we 
												//compare this with Score to 
            									//see if the player earned a 
                     							//life)

int				Lives 				= DEFAULT_LIVES;		//Number of lives

int				Difficulty			= DEFAULT_DIFFICULTY;	//Difficulty of the game

bool			Fullscreen			= false;

bool			Paused				= true;

int				BackgroundX			= 0;

double			BackgroundLastTime	= 0;

bool			ScrollInLevel		= true;

int				LevelX				= 480;

double			LevelLastTime		= 0;

bool			ShowTitle			= true;

//==============================================================================
void OnEvent() {
    while(SDL_PollEvent(&Event)) {		
        switch(Event.type) {
        
        	//System requests a shut down (usually by clicking the X)
            case SDL_QUIT: {
                Running = false;
                break;
            }
            
            //A key has been pressed
       		case SDL_KEYDOWN: {			
				if(CutScene) break;
					
       			switch(Event.key.keysym.sym) {					
       				//Quit
       				case SDLK_ESCAPE: Running = false; break;
       				
       				//Restart Pacman
       				case SDLK_F1: {
        				OldScore 	= 0;
  						Score 		= 0;				
        				Lives 		= DEFAULT_LIVES;
        				
 						LoadLevel(DEFAULT_LEVEL);
  						Reset();
  						
       					break;
       				}
       				
       				//Change Difficulty Level
       				case SDLK_F2: {
						
						//It's safe to assume Score == 0 means first level
						if(Score == 0) {
       						Difficulty++;
       						if(Difficulty > SKILL_HARD) Difficulty = 0;
						}

       					break;
       				}
       				
       				case SDLK_F4: {
						SDL_FreeSurface(Display);
						
						Fullscreen = !Fullscreen;
						
						int Flags = WFLAGS;
						
						if(Fullscreen) Flags |= SDL_FULLSCREEN;
						
    					//Create our main display (see Define.h)
						Display = SDL_SetVideoMode(WWIDTH, WHEIGHT, WBIT, Flags);
						if (Display == NULL) {
        					cerr << "Cannot Set Video Mode\n";
							return;
    					}						
						
						break;
					}
					
					case SDLK_F8: {
						LoadLevel(CMap::NextLevel);
						
						CPacman::Pacman.Skin++;
						if(CPacman::Pacman.Skin > 2) CPacman::Pacman.Skin = 0;
						
						break;
					}
					
					default: {
						if(ShowTitle) {
							ShowTitle = false;
							
							Paused = true;

							LevelLastTime = SDL_GetTicks();
							
						}else if(HighScore.EnterName == false) {
							if(Paused) {
								SpawnTime = SDL_GetTicks();
							}
							Paused = false;		
						}
					}
     			}
    		}
        }        
        
		if(HighScore.EnterName) {
			HighScore.OnEvent(&Event);
		}        
    }
    
    //Let our Pacman class deal with events (normally, we'd put this inside the
    //event loop, and pass the Event structure. We won't be needing the event
    //structure in this game, we'll use SDL_GetKeyState instead).
    if(CutScene == false) {
    	CPacman::Pacman.OnEvent();
	}
};

//------------------------------------------------------------------------------
bool OnInit() {
	//Start up SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "Cannot Initialize SDL Systems: " << SDL_GetError() << "\n";
		return false;
    }

    //Create our main display (see Define.h)
	Display = SDL_SetVideoMode(WWIDTH, WHEIGHT, WBIT, WFLAGS);
	if (Display == NULL) {
        cerr << "Cannot Set Video Mode\n";
		return false;
    }
    
    TTF_Init();
    
    //Set the title for our Window (see Define.h)
	SDL_WM_SetCaption(WCAPTION, WCAPTION);
	
	SDL_ShowCursor(0);
	
	//Load the background of our game, and etc...
	if((Surf_Background = ImageLoad("media/bg.png")) == NULL) {
		cerr << "Cannot load file: media/bg.png\n";
		return false;
	}
	
	//Load the background of our game, and etc...
	if((Surf_Bar = ImageLoad("media/bar.png")) == NULL) {
		cerr << "Cannot load file: media/bar.png\n";
		return false;
	}	

	if((Surf_Lives = ImageLoad("media/lives.png")) == NULL) {
		cerr << "Cannot load file: media/lives.png\n";
		return false;
	}
	
	if((Surf_Score = ImageLoad("media/score.png")) == NULL) {
		cerr << "Cannot load file: media/score.png\n";
		return false;
	}
 
	if((Surf_Difficulty = ImageLoad("media/difficulty.png")) == NULL) {
		cerr << "Cannot load file: media/difficulty.png\n";
		return false;
	}
	
	if((Surf_Paused = ImageLoad("media/paused.png")) == NULL) {
		cerr << "Cannot load file: media/paused.png\n";
		return false;
	}	
	
	if((Surf_Title = ImageLoad("media/title.png")) == NULL) {
		cerr << "Cannot load file: media/title.png\n";
		return false;
	}		
	
	//Make these surfaces have a transparent color
	Transparency(Surf_Lives, SetColor(255, 0, 255));		
	Transparency(Surf_Score, SetColor(255, 0, 255));	
	Transparency(Surf_Paused, SetColor(255, 0, 255));	
	Transparency(Surf_Title, SetColor(255, 0, 255));	
	
	//Set the display for the Pacman class to draw to
	CPacman::Pacman.Display = Display;
	CPacman::Pacman.OnInit();			//And initialize
	
	//...
	CNibblet::Display = Display;	
	CNibblet::OnInit();
	
	CGhost::Display = Display;	
	
	CFruit::Display = Display;
	Fruit.OnInit();
	
	HighScore.OnInit();
	HighScore.Display = Display;
	
	ScoreAnim.OnInit();
	ScoreAnim.Display = Display;
	
	char GhostFile[255];
	
	//Load each of our ghosts. The sprintf will increment the filename each time
	//for how many ghosts we have.
	for(int i = 0;i < MAX_GHOSTS;i++) {
		sprintf(GhostFile, "media/ghost_%d.png", i);
		if(Ghosts[i].OnInit(GhostFile) == false) {
			return false;
		}
	}
	
	if((CGhost::Surf_Dead = ImageLoad("media/ghost_dead.png")) == NULL) {
		cerr << "Cannot load file: media/ghost_dead.png\n";
		return false;
	}	
	
	if((CGhost::Surf_Vulnerable = ImageLoad("media/ghost_vulnerable.png")) == NULL) {
		cerr << "Cannot load file: media/ghost_vulnerable.png\n";
		return false;
	}	

	//Set the map for our pathing class
	CGhost::Path.Map = CMap::MapControl.TileList;
	CFruit::Path.Map = CMap::MapControl.TileList;	
	
	Transparency(CGhost::Surf_Dead, SetColor(255, 0, 255));
	Transparency(CGhost::Surf_Vulnerable, SetColor(255, 0, 255));
	
	//Load the default level, and reset
	LoadLevel(DEFAULT_LEVEL);
	Reset();
	
	if(FontControl.Initialize("fonts/1900805.ttf", 14, WBIT) == false) return false;
	FontControl.RenderFontMap(TTF_TYPE_SOLID);
	
	Transparency(FontControl.Surf_FontMap, SetColor(255, 0, 255));
	
	HighScore.LoadHighScore();
	
    return true;
};

//------------------------------------------------------------------------------
bool OnLoop() {
    while(Running) {
        OnEvent();	//Grab events
        
        if(ShowTitle) {
		}else
        if(CutScene) {
			OnCutSceneLoop();
		}else	
		if(HighScore.EnterName) {
			
		}else{
        	OnGameLoop();
		}
		
       	//Scroll Background
       	if(BackgroundLastTime + 50 < SDL_GetTicks()) {	
			BackgroundX++;
			   
			if(BackgroundX > 480) BackgroundX = 0;			
		}		
        
        //Draw our game
        OnRender();
    }
    
    return true;
};

//------------------------------------------------------------------------------
void OnGameLoop() {
        
        if(!Paused) {
        	//Animate and Move Pacman and Ghosts
       		CPacman::Pacman.OnLoop();
       	
        	for(int i = 0;i < 4;i++) {
	           	Ghosts[i].OnLoop();
	        }	
	        
	        Fruit.OnLoop();
	       	if((rand() % 2500) == 0 && Fruit.Alive == false) {
				Fruit.Spawn();
			}	        
		}
		
		if(Fruit.HasCollision(CPacman::Pacman.X, CPacman::Pacman.Y) == true) {
			Fruit.Alive = false;
			AddScore(50);
		}
       	
       	//Check collisions
       	CheckNibblets();
       	CheckGhosts();
       	
       	//Spawn Ghost
       	//If 2 seconds have passed, spawn a ghost (if there are any to spawn)
       	if(SpawnTime + 2000 < SDL_GetTicks() && !Paused) {
       		SpawnTime = SDL_GetTicks();
       		
        	for(int i = 0;i < 4;i++) {
        	
				//If the ghost is in the center, move him out so he can run around
				if(Ghosts[i].HasCollision(7, 7)) {
               		Ghosts[i].SetCoords(7, 9);
               		break;
				}
           	}
       	}
       	
       	//Pacman Died!
       	if(CPacman::Pacman.Dieing == true && CPacman::Pacman.Dir > 3) {
       		Lives--;
       		
       		//If Pacman's lives are less than 0, restart
 			if(Lives < 0) {
				HighScore.CheckScore(Score);
				
				if(HighScore.EnterName) {
					char t = 'E';
					
					if(Difficulty == 0) t = 'E';
					if(Difficulty == 1) t = 'M';
					if(Difficulty == 2) t = 'H';
					
					HighScore.Entry[HighScore.EntryID].Difficulty  = t;
				}
				
 				Lives = DEFAULT_LIVES;
 				LoadLevel(DEFAULT_LEVEL);
 				OldScore = 0;
 				Score = 0;
 				
 			//If not, restart the level
 			}else{
				//LoadLevel(CMap::CurrentLevel);
 			}
 			//Put everyone back into position
  			Reset();
       	}
       	
       	//Pacman beat the level!
       	if(NumberNibblets <= 0) {       		
       		OnCutSceneInit();
       	}
       	
       	bool VulnerableOn = false;
       	
       	//Is Vulnerability on?
       	for(int i = 0;i < 4;i++) {
			
       		//We have a ghost who is vulnerable, so vulnerability must be on
       		if(Ghosts[i].Vulnerable == true) {
         		VulnerableOn = true;
         		break;
 			}
       	}
       	
       	//If Vulnerability is on, and it's been 5 secs, turn it off
       	if(VulnerableOn && VulnerableTime + (5000 - (Difficulty * 1000)) < SDL_GetTicks()) {
        	for(int i = 0;i < 4;i++) {
        		Ghosts[i].Vulnerable = false;
            }
       	} 	
		
       	//Scroll Level
		if(ScrollInLevel) {
//       	if(LevelLastTime + 10 < SDL_GetTicks() && ScrollInLevel) {	
//			LevelX -= 4;

			LevelX = 480 - (SDL_GetTicks() - LevelLastTime) / 4;
			   
			if(LevelX <= 0) {
				ScrollInLevel = false;
				LevelX = 480;
			}
		}			
};

//------------------------------------------------------------------------------
void OnCutSceneInit() {
	CutScene = true;
	
	CPacman::Pacman.Reset();	
	CPacman::Pacman.X = 14;
	CPacman::Pacman.Y = 7;
	CPacman::Pacman.Dir =  DIR_LEFT;	
	
	Ghosts[0].Reset();	
	Ghosts[0].X = 4;
	Ghosts[0].Y = 7;
	Ghosts[0].Dir = DIR_RIGHT;
	Ghosts[0].WaitTime = 25;
	
	CMap::MapControl.CutScene = true;
}

//------------------------------------------------------------------------------
void OnCutSceneLoop() {
	CPacman::Pacman.NewX = -1;
	CPacman::Pacman.NewY =  0;
	
	CPacman::Pacman.OnLoop();
	
	if(Ghosts[0].X != -1) {
		Ghosts[0].OnLoop();
	}
	
	if(CPacman::Pacman.X == 9) {
		Ghosts[0].Vulnerable = true;
		Ghosts[0].WaitTime = 25;
	}
	
	if(Ghosts[0].X == CPacman::Pacman.X) {
		Ghosts[0].X = -1;
	}
	
	if(CPacman::Pacman.X <= 0) {
		CMap::MapControl.CutScene = false;
		CutScene = false;
		
       	//Next level
       	LoadLevel(CMap::NextLevel);
       	Reset();		
	}
};

//------------------------------------------------------------------------------
void OnRender() {
	Blit(Display, Surf_Background, BackgroundX, 0, 0, 0, 480 - BackgroundX, 480);
	
	Blit(Display, Surf_Background, 0, 0, 480 - BackgroundX, 0, BackgroundX, 480);	
		
	if(ShowTitle) {
		Blit(Display, Surf_Title, 0, 0);
	}else
	if(CutScene) {
		if(CPacman::Pacman.X > 9) {
			Blit(Display, CNibblet::Surf_Nibblet, 9 * TILE_SIZE, 7 * TILE_SIZE, 0, 32, 32, 32);
		}
				
		CPacman::Pacman.OnRender();
		Ghosts[0].OnRender();
		
	}else
	if(HighScore.EnterName) {	
		HighScore.OnRender();		
	}else{
		//Draw the current level
		if(ScrollInLevel) {
			Blit(Display, CMap::Surf_Level, LevelX, 0);
		}else{
			Blit(Display, CMap::Surf_Level, 0, 0);
						
			//Draw all the little nibblets
			for(int i = 0;i < MAP_AREA;i++) {
				NibbletList[i].OnRender();
			}
		
			//Draw the ghosts
		   	for(int i = 0;i < MAX_GHOSTS;i++) {
				Ghosts[i].OnRender();
			}
			
			Fruit.OnRender();
		
			//Draw Pacman
			CPacman::Pacman.OnRender();	
			
			ScoreAnim.OnRender();
			
			if(Paused) {
				Blit(Display, Surf_Paused, (WWIDTH / 2) - (320 / 2), (WHEIGHT / 2) - (240 / 2));
			}
		}		
	}
	
	Blit(Display, Surf_Bar, 480, 0);
	
	//Draw the current difficulty
	Blit(Display, Surf_Difficulty, 480, 80, 0, (Difficulty * 48), 160, 48);
	
	//Draw the highscores
	for(int i = 0;i < 18;i++) {
		FontControl.Print(Display, 496 + 4, (14 * i) + 160, HighScore.Entry[i].Name);
		
		char Temp[255];
		sprintf(Temp, "%d", HighScore.Entry[i].Score);		
		int W = FontControl.GetWidth(Temp);
		
		FontControl.Print(Display, 496 + (96 - W), (14 * i) + 160, Temp);
		
		sprintf(Temp, "%c", HighScore.Entry[i].Difficulty);
		
		FontControl.Print(Display, 496 + 112, (14 * i) + 160, Temp);
	}
	
	//Draw how many lives we have
	for(int i = 0;i < Lives;i++) {
	
		//Do we have more than 3 lives? If so, draw the little 'plus'
		//instead of another pacman, and then stop
		if(i >= 3) {
			Blit(Display, Surf_Lives, 496 + (i * 32), 432, 32, 0, 16, 16);
			break;
		}else{
			Blit(Display, Surf_Lives, 496 + (i * 32), 432, 0, 0, 32, 32);
		}
	}	
	
    //Display Score
    int HunThou, TenThou, Thou, Hun, Ten, One;

	//Figure out how many ones we have, how many tens we have, etc...
    HunThou = (Score / 100000);
    TenThou = (Score - (HunThou * 100000)) / 10000;
    Thou    = (Score - (HunThou * 100000) - (TenThou * 10000)) / 1000;
    Hun     = (Score - (HunThou * 100000) - (TenThou * 10000) - (Thou * 1000)) / 100;
    Ten     = (Score - (HunThou * 100000) - (TenThou * 10000) - (Thou * 1000) - (Hun * 100)) / 10;
    One     = (Score - (HunThou * 100000) - (TenThou * 10000) - (Thou * 1000) - (Hun * 100) - (Ten * 10));

	Blit(Display, Surf_Score, 496, 		 48, HunThou * 20, 0, 20, 20);
    Blit(Display, Surf_Score, 496 + 20,  48, TenThou * 20, 0, 20, 20);
    Blit(Display, Surf_Score, 496 + 40,  48, Thou    * 20, 0, 20, 20);
	Blit(Display, Surf_Score, 496 + 60,  48, Hun     * 20, 0, 20, 20);
    Blit(Display, Surf_Score, 496 + 80,  48, Ten     * 20, 0, 20, 20);
    Blit(Display, Surf_Score, 496 + 100, 48, One     * 20, 0, 20, 20);  

	SDL_Flip(Display);
};

//------------------------------------------------------------------------------
void OnCleanup() {
   	for(int i = 0;i < 4;i++) {
		Ghosts[i].OnCleanup();
	}	
	
    if(Display) 			SDL_FreeSurface(Display);
    
   	if(Surf_Background)		SDL_FreeSurface(Surf_Background);
   	
   	if(Surf_Bar)			SDL_FreeSurface(Surf_Bar);
   	
   	if(Surf_Lives)			SDL_FreeSurface(Surf_Lives);
   	
   	if(Surf_Score)			SDL_FreeSurface(Surf_Score);
   	
   	if(Surf_Difficulty)		SDL_FreeSurface(Surf_Difficulty);
   	
   	if(Surf_Paused) 		SDL_FreeSurface(Surf_Paused);
   	
   	if(Surf_Title) 			SDL_FreeSurface(Surf_Title);
   	
   	HighScore.WriteHighScore();
   	HighScore.OnCleanup();
   	
   	CPacman::Pacman.OnCleanup();
   	
   	if(CNibblet::Surf_Nibblet)	SDL_FreeSurface(CNibblet::Surf_Nibblet);
   	
   	if(CGhost::Surf_Dead)		SDL_FreeSurface(CGhost::Surf_Dead);
   	
   	if(CGhost::Surf_Vulnerable)	SDL_FreeSurface(CGhost::Surf_Vulnerable);
   	
   	if(CMap::Surf_Level) SDL_FreeSurface(CMap::Surf_Level);
   	
   	FontControl.OnCleanup();
   	
   	ScoreAnim.OnCleanup();
   	
   	TTF_Quit();
   	SDL_Quit();
};

//------------------------------------------------------------------------------
bool CheckNibblets() {
	//For each nibblet in the map
	for(int i = 0;i < MAP_AREA;i++) {
		//Is pacman on top of one?
		if(NibbletList[i].HasCollision(CPacman::Pacman.X, CPacman::Pacman.Y)) {
			//It's just a regular nibblet, so add to the score, and deincrement
			//the amount of nibblets we have
			if(NibbletList[i].Type == NIBBLET_SMALL) {
				AddScore(1);
				NumberNibblets--;
			}
			//Same as above, but is this a large nibblet?
			if(NibbletList[i].Type == NIBBLET_LARGE) {
			
				//We just ate a big nibblet, so turn vulnerability on
			 	VulnerableTime = SDL_GetTicks();
			 	for(int i = 0;i < 4;i++) {
			 		Ghosts[i].Vulnerable = true;
		 		}
		 		
		 		//Add 5 points
		 		AddScore(5);
			 	
			 	NumberNibblets--;
			}
			
			//Now remove this nibblet from the list
			NibbletList[i].Type = NIBBLET_NONE;			
		}
	}

	return true;
};

//------------------------------------------------------------------------------
bool CheckGhosts() {

	//For each ghost 
	for(int i = 0;i < MAX_GHOSTS;i++) {
		if(Ghosts[i].Dead) {
			Ghosts[i].WaitTime = 20;
			continue;
		}
		
		//Reset how fast the ghosts should be
		if(Ghosts[i].Vulnerable) {
			Ghosts[i].WaitTime = 25 - (Difficulty * 5);
		}else{
			Ghosts[i].WaitTime = 25 - (Difficulty * 5);
		}
		
		//Ghosts are stuck together in a group, so move them at a different speed
		for(int j = 0;j < 4;j++) {
			if(i != j && Ghosts[i].HasCollision(Ghosts[j].X, Ghosts[j].Y)) {
				Ghosts[i].WaitTime = (rand() % 15) + 10;
			}
		} 	
		
		//Ghost is on Pacman
		if(Ghosts[i].HasCollision(CPacman::Pacman.X, CPacman::Pacman.Y)) {
			//If the ghost is vulnerable, send it back to the center,
			//add a score to pacman
			if(Ghosts[i].Vulnerable) {
				Ghosts[i].Dead = true;
				AddScore(10);
				Ghosts[i].Vulnerable = false;
				SpawnTime = SDL_GetTicks();
				
			//Pacman has died!
			}else if(CPacman::Pacman.Dieing == false) {
				CPacman::Pacman.Kill();
			}
		}
	}
	
	return true;
};

//------------------------------------------------------------------------------
bool LoadLevel(char* File) {
	//Reset the number of nibblets
	NumberNibblets = 0;
	
	//Load our map
  	CMap::MapControl.LoadMap(File);	
	
	//Fill the map with nibblets (where the tile is ground, and not an edge)
	for(int Y = 1;Y < MAP_HEIGHT - 1;Y++) {
		for(int X = 1; X < MAP_WIDTH - 1;X++) {
			if(CMap::MapControl.GetTile(X, Y) == TILE_TYPE_GROUND) {
				NibbletList[GetID(X, Y)].Type = NIBBLET_SMALL;
				NibbletList[GetID(X, Y)].SetCoords(X, Y);
				NumberNibblets++;
			}else{
				NibbletList[GetID(X, Y)].Type = NIBBLET_NONE;
			}
		}
	}
	
	//Make the corners have big nibblets
	NibbletList[GetID(1, 				1)].Type 				= NIBBLET_LARGE;
	NibbletList[GetID(MAP_WIDTH - 2, 	1)].Type 				= NIBBLET_LARGE;
	NibbletList[GetID(1, 				MAP_HEIGHT - 2)].Type 	= NIBBLET_LARGE;
	NibbletList[GetID(MAP_WIDTH - 2, 	MAP_HEIGHT - 2)].Type 	= NIBBLET_LARGE;
	
	//The center, and where Pacman starts don't have nibblets
	NibbletList[GetID(7, 9)].Type 	= NIBBLET_NONE; NumberNibblets--;
	NibbletList[GetID(7, 7)].Type 	= NIBBLET_NONE; NumberNibblets--;
	
	return true;
};

//------------------------------------------------------------------------------
void Reset() {	
	CPacman::Pacman.Reset();	//Send pacman back to his starting position
	
	for(int i = 0;i < 4;i++) {
		Ghosts[i].Reset();		//Send ghosts back to their starting positions
	}
	
	SpawnTime = SDL_GetTicks();	//Reset spawn time
	
	Paused = true;
	
	ScrollInLevel = true;
	LevelX = 480;
	LevelLastTime = SDL_GetTicks();
	
	Fruit.Reset();
}

//------------------------------------------------------------------------------
void AddScore(int Points) {
	OldScore = Score;
	Score += Points;
	
	if(Points > 1) {
		ScoreAnim.AddScore(Points, CPacman::Pacman.X, CPacman::Pacman.Y);
	}
	
	//Every SCORE_FOR_LIFE (see Define.h), we'll add a life
	//The default would be, every 500 points add a life
	if((Score / SCORE_FOR_LIFE) > (OldScore / SCORE_FOR_LIFE)) Lives++;
}

//------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
	atexit(OnCleanup);

    if(OnInit() == false) {
        cerr << "Unable to Init Pacman!\n";
        return -1;
    }
    
    if(OnLoop() == false) {
        cerr << "Fatel Error\n";
        return -1;
    }

    return 0;
};

//==============================================================================
