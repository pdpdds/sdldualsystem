//                      GILBERT AND THE DOORS V1.0
//               COPYRIGHT (c) Daniel Champagne 2016-2017
//                 Contact EMAIL: phoenixbozo@gmail.com
/*
  Gilbert and the doors is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

DEPENDENCY
----------
0-STANDARD Libc (With GCC)
1-LIBSDL1.2
2-LIBSDL_mixer1.2
3-MATH

Compile with command line on linux terminal:
gcc -o Gilbert Gilbert.c Widjet.c MoteurDeJeu.c -lm -lSDL_mixer  `sdl-config --libs --cflags`
*/

#include <SDL.h>
#include <SDL_mixer.h>
#include "Widjet.h"
#include "MoteurDeJeu.h"
//#include <windows.h> //For windows only

enum { INGAME = 1, MENU = 0 };
#undef main
int main(int argc, char* argv[]) {
	MainBoot();//initialisation des variable public au demarage
	//FreeConsole(); //efface le terminal pour windows
	if ((SDL_Init(SDL_INIT_EVERYTHING)) == -1) {
		fprintf(stderr, "Erreur SDL\n"); return -1;
	}
	//Joystick variable
	SDL_Joystick* Joyst = NULL;
	NumberofJoystick = SDL_NumJoysticks();


	SDL_WM_SetCaption("Gilbert and the Doors v1.0 (2016-2017)", NULL);

	// debut Configuration du son
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) == -1) //Mixer init music
	{
		printf("%s", Mix_GetError());
	}

	//definition du nombre de canaux et du volume
	Mix_AllocateChannels(12);
	Mix_Volume(1, MIX_MAX_VOLUME / 2);
	Mix_VolumeMusic(MUSICVOLUME);

	Mix_Music* music;
	music = Mix_LoadMUS("MUSIC/intro.wav");

	//Créer les pointeurs pour stocker les .WAV 
	Mix_Chunk* jump;
	Mix_Chunk* jump2;
	Mix_Chunk* key;
	Mix_Chunk* bump;
	Mix_Chunk* outch;
	Mix_Chunk* coin;
	Mix_Chunk* explose;
	Mix_Chunk* gameover;
	Mix_Chunk* switchon;
	Mix_Chunk* switchoff;
	Mix_Chunk* opendoor;
	Mix_Chunk* badaboum;
	Mix_Chunk* vital;

	//Charge les wav dans les pointeurs
	jump = Mix_LoadWAV("SND/jump.wav");
	jump2 = Mix_LoadWAV("SND/jump2.wav");
	key = Mix_LoadWAV("SND/key.wav");
	bump = Mix_LoadWAV("SND/bump.wav");
	outch = Mix_LoadWAV("SND/outch.wav");
	coin = Mix_LoadWAV("SND/coin.wav");
	explose = Mix_LoadWAV("SND/explose.wav");
	gameover = Mix_LoadWAV("SND/game over.wav");
	switchon = Mix_LoadWAV("SND/switchon.wav");
	switchoff = Mix_LoadWAV("SND/switchoff.wav");
	opendoor = Mix_LoadWAV("SND/opendoor.wav");
	vital = Mix_LoadWAV("SND/vital.wav");
	badaboum = Mix_LoadWAV("SND/badaboum.wav");

	//attribuer les volumes pour les wav
	Mix_VolumeChunk(jump, MIX_MAX_VOLUME);
	Mix_VolumeChunk(jump2, MIX_MAX_VOLUME);
	Mix_VolumeChunk(key, MIX_MAX_VOLUME / 2);
	Mix_VolumeChunk(bump, MIX_MAX_VOLUME / 2);
	Mix_VolumeChunk(outch, MIX_MAX_VOLUME / 4);
	Mix_VolumeChunk(coin, MIX_MAX_VOLUME);
	Mix_VolumeChunk(explose, MIX_MAX_VOLUME);
	Mix_VolumeChunk(gameover, MIX_MAX_VOLUME);
	Mix_VolumeChunk(switchon, MIX_MAX_VOLUME / 2);
	Mix_VolumeChunk(switchoff, MIX_MAX_VOLUME / 2);
	Mix_VolumeChunk(opendoor, MIX_MAX_VOLUME / 8);
	Mix_VolumeChunk(badaboum, MIX_MAX_VOLUME);
	Mix_VolumeChunk(vital, MIX_MAX_VOLUME / 2);
	// fin Configuration du son

	//initiation des variable simple
	int Game = MENU;
	char OneTime = 1;

	config Config;//Déclaration d'une varable structure de configuration
	LoadConfig(&Config);//récupération des configs déja sauvgarder
	_putenv("SDL_VIDEO_WINDOW_POS=center"); //centrer la fenêtre

	//Variable pour controle du nombre de frame par seconde
	uint32_t delay1 = 0, delay2 = 0, delay3 = 0;

	//déclaration de varable simple et initialisation
	char opdoor = 0;
	char gameend = 0;
	char SNDchannel = 2; //Channel audio a utilisé
	int keycount = 0; //pour  détecteur de clé pour les son
	int spin2 = 8;//normal8
	char ButtonClick = 0;
	SDL_Rect rect;


	//déclaration de structure widjet
	Widjet widjet = { {NULL} };

#define RENDERX MainRender
#define XSCALE 640 //render out
#define YSCALE 480 //render out
#define XSCALEbuffer 640 //memoire
#define YSCALEbuffer 480 //memoire

	//initialisation de la surface en fonction de la configuration récupéré
	if (Config.FULLSCREEN) {
		if (Config.SOFTWARERENDER) {
			if (Config.DOUBLEBUF) { widjet.render = SDL_SetVideoMode(XSCALE, YSCALE, Config.BIT, SDL_SWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN); }
			else { widjet.render = SDL_SetVideoMode(XSCALE, YSCALE, Config.BIT, SDL_SWSURFACE | SDL_FULLSCREEN); }
		}
		else { if (Config.DOUBLEBUF) { widjet.render = SDL_SetVideoMode(XSCALE, YSCALE, Config.BIT, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN); } else { widjet.render = SDL_SetVideoMode(XSCALE, YSCALE, Config.BIT, SDL_HWSURFACE | SDL_FULLSCREEN); } }
	}
	else {
		if (Config.SOFTWARERENDER) {
			if (Config.DOUBLEBUF) { widjet.render = SDL_SetVideoMode(XSCALE, YSCALE, Config.BIT, SDL_SWSURFACE | SDL_DOUBLEBUF); }
			else { widjet.render = SDL_SetVideoMode(XSCALE, YSCALE, Config.BIT, SDL_SWSURFACE); }
		}
		else {
			if (Config.DOUBLEBUF) { widjet.render = SDL_SetVideoMode(XSCALE, YSCALE, Config.BIT, SDL_HWSURFACE | SDL_DOUBLEBUF); }
			else { widjet.render = SDL_SetVideoMode(XSCALE, YSCALE, Config.BIT, SDL_HWSURFACE); }
		}
	}

	LevelTotal = NumberOfLevel();
	SpriteTotal = NumberOfSprite();

	WidjetInit(&widjet);

	//picture init
	ImagePKG imagePKG = { NULL };
	ImagePKGInit(&Config, &imagePKG);
	SDL_WM_SetIcon(imagePKG.spriteBMP[45], NULL);//icone du program

	//sprite level init
	LevelGrid levelgrid = { 0 };
	LevelGridInit(&levelgrid, &imagePKG);

	levelgrid.DoubleJumpCancel = 0;

	//vérification si joystick viable
	if (NumberofJoystick >= 1) {
		Joyst = SDL_JoystickOpen(0);
		if (Joyst == NULL) {
			NumberofJoystick = 0; fprintf(stderr, "Erreur joystick\n");
		}
		else {
			if (JoystickUpdateEvents(&Joyst, &widjet.Event, &levelgrid)) { NumberofJoystick = 0; }
		}
	}
	//utilisation direct du joystick non par eventstate
	SDL_JoystickEventState(SDL_DISABLE);


	//retire le curseur de la souris
	SDL_ShowCursor(SDL_DISABLE);

	//splash screen
	SplashScreen(&imagePKG, &widjet);

	//initialise la fonts selon nos parametre
	WidjetGetFont(12, WidjetRGB(255, 255, 0), WidjetRGB(0, 0, 0), &widjet);
	//fade du splashscreen
	FadeInFadeOut(NULL, 16, 33, imagePKG.spriteBMP[44], &widjet);

	SDL_ShowCursor(SDL_ENABLE);

	//boucle principale MENU
	while ((!widjet.Event.key[SDLK_ESCAPE]) && (!widjet.Event.quit)) { //bouble principal
		if (OneTime == 1) {
			if (Config.MusicConfig == 1) {
				Mix_FreeMusic(music);
				music = Mix_LoadMUS("MUSIC/intro.wav");
				Mix_PlayMusic(music, -1);
			}
			OneTime = 0;
		}
		UpdateEvents(&widjet.Event); //récupération des evenement souris clavier
		JoystickUpdateEvents(&Joyst, &widjet.Event, &levelgrid); //récupération des evenement joystick

		rect.x = 0;
		rect.y = 0;
		SDL_BlitSurface(imagePKG.spriteBMP[44], 0, widjet.render, &rect); //effacement de la surface widjet.render

		//section boutton menu principal
		if (WidjetButtonBMP(20, 240, 168, 50, "New Game", &imagePKG.spriteBMP[12], &widjet) || (widjet.Event.key[SDLK_RETURN]) || widjet.Event.JoyButton1) { ButtonClick = 1; }

		rect.x = 208;
		rect.y = 250;
		if (WidjetZoneToutch(18, 238, 172, 54, &widjet)) { SDL_BlitSurface(imagePKG.spriteBMP[77], 0, widjet.render, &rect); }

		if (WidjetButtonBMP(20, 310, 120, 50, "Setup", &imagePKG.spriteBMP[12], &widjet)) { ButtonClick = 2; }

		rect.x = 160;
		rect.y = 320;
		if (WidjetZoneToutch(18, 308, 124, 54, &widjet)) { SDL_BlitSurface(imagePKG.spriteBMP[77], 0, widjet.render, &rect); }

		if (WidjetButtonBMP(20, 380, 100, 50, "Quit", &imagePKG.spriteBMP[12], &widjet)) { ButtonClick = 3; }

		WidjetGetFont(0, 16777215, 0, &widjet);

#define centrex -5
#define centrey 460
		if (WidjetButtonBMP(234 + centrex, 0 + centrey, 43, 20, "info", &imagePKG.spriteBMP[12], &widjet)) { ButtonClick = 4; }

		if (WidjetButtonBMP(277 + centrex, 0 + centrey, 68, 20, "Dev.Note", &imagePKG.spriteBMP[12], &widjet)) { ButtonClick = 5; }

		if (WidjetButtonBMP(345 + centrex, 0 + centrey, 80, 20, "license", &imagePKG.spriteBMP[12], &widjet)) { ButtonClick = 6; }

		WidjetGetFont(12, WidjetRGB(255, 255, 0), WidjetRGB(0, 0, 0), &widjet);
		rect.x = 140;
		rect.y = 390;
		if (WidjetZoneToutch(18, 378, 104, 54, &widjet)) { SDL_BlitSurface(imagePKG.spriteBMP[77], 0, widjet.render, &rect); }

		if (ButtonClick == 1) {
			ResetAllControl(&levelgrid, &widjet);
			OneTime = 1;
			SDL_ShowCursor(SDL_DISABLE);
			Game = INGAME;
			SDL_Flip(widjet.render);
			FadeInFadeOut(widjet.render, 16, 33, NULL, &widjet);
			RestartGame(&levelgrid, &imagePKG, &widjet); PlaySND(PLAY, MUSIC1);
			if (music == NULL) { Mix_LoadMUS(levelgrid.music); }
			else {
				Mix_FreeMusic(music);
				Mix_LoadMUS(levelgrid.music);
			}
		}

		if (ButtonClick == 2) {
			SDL_Flip(widjet.render); SDL_Delay(150); SetupConfig(&Config, &levelgrid, &imagePKG, &widjet);
		}

		if (ButtonClick == 3) {
			widjet.Event.quit = 1; SDL_Flip(widjet.render); SDL_Delay(150);
		}

		if (ButtonClick == 4) {
			WidjetGetFont(0, 16777215, 0, &widjet);
			INFO(&imagePKG, &widjet);
		}

		if (ButtonClick == 5) {
			WidjetGetFont(0, 16777215, 0, &widjet);
			DEVNOTE(&imagePKG, &widjet);
		}

		if (ButtonClick == 6) {
			WidjetGetFont(0, 16777215, 0, &widjet);
			LicenceGPL(&imagePKG, &widjet);
		}

		ButtonClick = 0;//reinisitalise les button

		if (widjet.Event.mousebuttons[SDL_BUTTON_MIDDLE])
		{
			SDL_WM_ToggleFullScreen(widjet.render);
			widjet.Event.mousebuttons[SDL_BUTTON_MIDDLE] = 0;
		}

		if (Game == INGAME) {
			SDL_ShowCursor(SDL_DISABLE);

			//boucle INGAME secondaire
			while (Game == INGAME) {
				UpdateEvents(&widjet.Event);
				JoystickUpdateEvents(&Joyst, &widjet.Event, &levelgrid);
				if ((widjet.Event.key[SDLK_ESCAPE] == 1) || (widjet.Event.quit == 1)) {
					Game = MENU;
					widjet.Event.key[SDLK_ESCAPE] = 0;
					widjet.Event.quit = 0;
					FadeInFadeOut(widjet.render, 32, 33, imagePKG.spriteBMP[44], &widjet);

				}
				else {
					if (Config.BACKGROUND) { RenduDeBackGround(&levelgrid, &imagePKG, &widjet); }
					else {
						SDL_BlitSurface(imagePKG.spriteBMP[levelgrid.background], 0, widjet.render, 0);
					}

					if (Config.BACK) { RenduDeFond(&levelgrid, &imagePKG, &widjet); }

					RenduDeCouche(DEFAULT, &levelgrid, &imagePKG, &widjet);
					if (Config.FRONT) { RenduDeFront(&levelgrid, &imagePKG, &widjet); }

					UpdateScroll(&levelgrid, &widjet);
					if (levelgrid.DoorOpen == 0) { ControlUpdate(&levelgrid, &imagePKG, &widjet); }

					//option FULLSCREEN WINDOWED mode (linux only)
					if (widjet.Event.mousebuttons[SDL_BUTTON_MIDDLE])
					{
						SDL_WM_ToggleFullScreen(widjet.render);
						widjet.Event.mousebuttons[SDL_BUTTON_MIDDLE] = 0;
					}

					if (Config.MODESANSECHEC == 1) { WidjetText(200, 10, "MODE SANS ECHEC", &widjet); }

#define centx 2
					rect.x = centx;
					rect.y = 2;
					SDL_BlitSurface(imagePKG.spriteBMP[139], 0, widjet.render, &rect);
					if (Config.BACKGROUND == 1) { WidjetBlitAndStretch(&widjet.render, DEFAULT, centx + 1, 3, 38, 35, &widjet.render); }

					if (imagePKG.leveluse + 1 >= 10) {
						WidjetNumericText(centx, 9, imagePKG.leveluse + 1, &widjet);
					}
					else {
						WidjetNumericText(centx + 10, 9, imagePKG.leveluse + 1, &widjet);
					}

					//affichage bomb
					if (bomberCount > 0) {
						WidjetText(308, 6, "x", &widjet);

						rect.x = 270;
						rect.y = 1;
						SDL_BlitSurface(imagePKG.spriteBMP[199], 0, widjet.render, &rect);
						WidjetNumericText(328, 8, bomberCount, &widjet);
					}

					//affiche vie
					WidjetText(429, 6, "x", &widjet);

					rect.x = 400;
					rect.y = 1;

					SDL_BlitSurface(imagePKG.spriteBMP[22], 0, widjet.render, &rect);
					WidjetNumericText(449, 8, levelgrid.NombreDeVie, &widjet);

					if (levelgrid.CleTotaleAmasser > 0) {
						//affiche cle
						WidjetText(520, 6, "x", &widjet);
						rect.x = 500;
						if (levelgrid.CleTotaleAmasser == levelgrid.CleTotalInLevel) { if (levelgrid.ActualFrame > 60) { rect.y = 4; SDL_BlitSurface(imagePKG.spriteBMP[79], 0, widjet.render, &rect); } else { rect.y = 1; SDL_BlitSurface(imagePKG.spriteBMP[95], 0, widjet.render, &rect); } }
						else { rect.y = 1; SDL_BlitSurface(imagePKG.spriteBMP[95], 0, widjet.render, &rect); }
						WidjetNumericText(540, 8, levelgrid.CleTotaleAmasser, &widjet);
						if (keycount != levelgrid.CleTotaleAmasser) {
							keycount = levelgrid.CleTotaleAmasser; //detecteur de clé pour son
						}
					}
					else {
						WidjetText(520, 6, "x", &widjet);
						rect.x = 500;
						rect.y = 4; SDL_BlitSurface(imagePKG.spriteBMP[95], 0, widjet.render, &rect);
						if (levelgrid.ActualFrame > 60) { WidjetNumericText(540, 8, levelgrid.CleTotalInLevel, &widjet); }
					}
					if ((sndmatrice[1] == PLAY) && (Config.MusicConfig == 1)) { Mix_PauseMusic(); Mix_RewindMusic(); Mix_PlayMusic(music, -1); PlaySND(STOP, MUSIC1); }
					if (sndtrig == 1) { PlaySND(PLAY, JUMP); sndtrig = 2; }
					if (sndmatrice[0] == PLAY) {
						if (rand() % 100 > 75) { if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, jump, 0); } }
						else { if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, jump2, 0); } }
						if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; }
						else { SNDchannel = 2; }PlaySND(STOP, JUMP);
					}
					// vital
					if (sndmatrice[17] == PLAY) {
						if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, vital, 0); }
						Mix_PauseMusic();
						Mix_RewindMusic();
						FadeInFadeOut(widjet.render, 32, 33, NULL, &widjet);
						PlaySND(STOP, VITAL);
						if (levelgrid.NombreDeVie != 0) {
							ResetLevel(&levelgrid, &imagePKG, &widjet);
							if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; }
							else { SNDchannel = 2; }
						}
						else {
							PlaySND(PLAY, GAMEOVER);
							gameend = 1;
							if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; }
							else { SNDchannel = 1; }
						}
					}//vital fin

					if (sndmatrice[2] == PLAY) { if (Config.FXConfig == 1) { Mix_PlayMusic(music, -1); }Game = TheEnd(&Joyst, &levelgrid, &imagePKG, &widjet); break; }

					if (sndmatrice[5] == PLAY) { if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, key, 0); }  if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; } else { SNDchannel = 2; }PlaySND(STOP, KEY); }
					if (sndmatrice[6] == PLAY) { if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, bump, 0); }  if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; } else { SNDchannel = 2; } PlaySND(STOP, BUMP); }
					if (sndmatrice[7] == PLAY) { if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, key, 0); } if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; } else { SNDchannel = 2; }PlaySND(STOP, DOOR1); }
					if (sndmatrice[8] == PLAY) { if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, key, 0); }  if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; } else { SNDchannel = 2; } PlaySND(STOP, DOOR2); }
					if (sndmatrice[9] == PLAY) { if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, outch, 0); }  if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; } else { SNDchannel = 2; } PlaySND(STOP, OUTCH); }
					if (sndmatrice[10] == PLAY) { if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, coin, 0); }  if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; } else { SNDchannel = 2; } PlaySND(STOP, COIN); }
					if (sndmatrice[11] == PLAY) { if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, explose, 0); }  if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; } else { SNDchannel = 2; } PlaySND(STOP, EXPLOSE); }
					if (sndmatrice[12] == PLAY) { if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, gameover, 0); }  if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; } else { SNDchannel = 2; } PlaySND(STOP, GAMEOVER); }
					if (sndmatrice[13] == PLAY) { if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, switchon, 0); }  if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; } else { SNDchannel = 2; } PlaySND(STOP, SWITCHON); }
					if (sndmatrice[14] == PLAY) { if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, switchoff, 0); }  if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; } else { SNDchannel = 2; } PlaySND(STOP, SWITCHOFF); }
					if (sndmatrice[15] == PLAY) { if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, opendoor, 0); }  if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; } else { SNDchannel = 2; } PlaySND(STOP, OPENDOOR); }
					if (sndmatrice[16] == PLAY) { if (Config.FXConfig == 1) { Mix_PlayChannel(SNDchannel, badaboum, 0); }  if (SNDchannel < 11) { SNDchannel = SNDchannel + 1; } else { SNDchannel = 2; } PlaySND(STOP, BADABOUM); }
					//fin matrice de son


					//Full frame Procedure routine
					FullFrameRoutine(&levelgrid, &imagePKG, &widjet);
					opdoor = OpenDoorProcedure(&levelgrid, &imagePKG, &widjet);
					if (opdoor == 1) {
						Mix_FreeMusic(music);
						music = Mix_LoadMUS(levelgrid.music);
					}

					if (opdoor == 2) {
						Mix_FreeMusic(music);
						music = Mix_LoadMUS("MUSIC/theend.wav");
						PlaySND(PLAY, THEEND);
					}
					opdoor = 0;
					rect.x = 0;
					rect.y = 0;

					//game over
					if (gameend == 1) {
						WidjetGetFont(50, WidjetRGB(0, 255, 255), WidjetRGB(0, 0, 0), &widjet);
						FadeInFadeOut(NULL, 16, 33, imagePKG.spriteBMP[20], &widjet);

#define AL -20
						int t = 0;
						float down = 0;
						char flip = 0;
						//boucle game over
						while (t < 1000) {
							if (flip == 0) { down = down + .3; }
							else { down = down - .3; }
							if (down >= 32) { flip = 1; }
							if (down <= 0) { flip = 0; }
							SDL_BlitSurface(imagePKG.spriteBMP[20], 0, widjet.render, &rect);
							WidjetText(50 + AL + (rand() % 4), 180 + down - 16 + (rand() % 4), "G", &widjet);
							WidjetText(114 + AL + (rand() % 4), 180 - down + 16 + (rand() % 4), "A", &widjet);
							WidjetText(178 + AL + (rand() % 4), 180 + down - 16 + (rand() % 4), "M", &widjet);
							WidjetText(242 + AL + (rand() % 4), 180 - down + 16 + (rand() % 4), "E", &widjet);
							WidjetText(370 + AL + (rand() % 4), 180 + down - 16 + (rand() % 4), "O", &widjet);
							WidjetText(434 + AL + (rand() % 4), 180 - down + 16 + (rand() % 4), "V", &widjet);
							WidjetText(498 + AL + (rand() % 4), 180 + down - 16 + (rand() % 4), "E", &widjet);
							WidjetText(562 + AL + (rand() % 4), 180 - down + 16 + (rand() % 4), "R", &widjet);


							delay2 = SDL_GetTicks();
							delay3 = delay2 - delay1;
							if ((delay3 < spin2)) { SDL_Delay(spin2 - delay3); }
							delay1 = SDL_GetTicks();
							SDL_Flip(widjet.render);
							t++;
						}

						FadeInFadeOut(imagePKG.spriteBMP[20], 16, 33, NULL, &widjet);
						WidjetGetFont(12, WidjetRGB(255, 255, 0), WidjetRGB(0, 0, 0), &widjet);

						if (GameOver(&levelgrid, &imagePKG, &widjet)) { Game = MENU; gameend = 0; }
						ResetAllControl(&levelgrid, &widjet);
					}

					//framerate
					delay2 = SDL_GetTicks();
					delay3 = delay2 - delay1;
					if ((delay3 < spin2)) { SDL_Delay(spin2 - delay3); }
					delay1 = SDL_GetTicks();

					SDL_Flip(widjet.render);

					if (levelgrid.ActualFrame < 124) { levelgrid.ActualFrame = levelgrid.ActualFrame + 1; }
					else { levelgrid.ActualFrame = 0; }

				}
			}
		}
		else { SDL_ShowCursor(SDL_ENABLE); }//fin boucle INGAME



		delay2 = SDL_GetTicks();
		delay3 = delay2 - delay1;
		if ((delay3 < spin2)) { SDL_Delay(spin2 - delay3); }
		delay1 = SDL_GetTicks();
		SDL_Flip(widjet.render);
	}//boucle principale

	FadeInFadeOut(widjet.render, 32, 33, NULL, &widjet);
	//Libération de tous les pointer
	LevelGridFree(&levelgrid);
	WidjetFree(&widjet);
	ImagePKGFree(&imagePKG);
	SDL_JoystickClose(Joyst);
	Mix_FreeChunk(vital);
	Mix_FreeChunk(badaboum);
	Mix_FreeChunk(opendoor);
	Mix_FreeChunk(switchon);
	Mix_FreeChunk(switchoff);
	Mix_FreeChunk(gameover);
	Mix_FreeChunk(explose);
	Mix_FreeChunk(coin);
	Mix_FreeChunk(bump);
	Mix_FreeChunk(key);
	Mix_FreeMusic(music);
	Mix_FreeChunk(jump);
	Mix_FreeChunk(jump2);
	Mix_CloseAudio();
	SDL_Quit();
	return EXIT_SUCCESS;
}



