//                      GILBERT AND THE DOORS V1.0
//               COPYRIGHT (c) Daniel Champagne 2016-2017
//                 Contact EMAIL: phoenixbozo@gmail.com
/*
  MoteurDeJeu.c is part of Gilbert and the doors.
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
*/
#include <SDL.h>
#include <SDL_mixer.h>
#include <math.h>
#include "Widjet.h"
#include "MoteurDeJeu.h"

void DEVNOTE(ImagePKG *PKG, Widjet *widjet);
void InfoTextGPL(ImagePKG *PKG, Widjet *widjet);
void SplashScreenGPL(ImagePKG *PKG, Widjet *widjet);
void InfoText(ImagePKG *PKG, Widjet *widjet);
void INFO(ImagePKG *PKG, Widjet *widjet);
void LicenceGPL(ImagePKG *PKG, Widjet *widjet);
void ResetAllControl(LevelGrid *levelgrid, Widjet *widjet);
void FadeInFadeOutNoSoundModif(SDL_Surface *INSurface, int FadeStep, int FrameRate, SDL_Surface *OUTSurface, Widjet *widjet);
void GreenScreen(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
char TheEnd(SDL_Joystick** joyst, LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
void ParticularityOfLevel(LevelGrid *levelgrid, ImagePKG *PKG);
int RestartGame(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
void SplashScreen(ImagePKG *PKG, Widjet *widjet);
void FadeInFadeOut(SDL_Surface *INSurface, int FadeStep, int FrameRate, SDL_Surface *OUTSurface, Widjet *widjet);
int JoystickUpdateEvents(SDL_Joystick** joyst, Input* in, LevelGrid *levelgrid);
void AnimLamp(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
void SetupConfig(config *Config, LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
void ResetGridInit(LevelGrid *levelgrid, ImagePKG *PKG);
void FullFrameRoutine(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
void VitalInjury(LevelGrid *levelgrid, Widjet *widjet);
void PlaySND(PLAYSTOP playstop, SND snd);
int NextLevel(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
int OpenDoorProcedure(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
int MoveEnemi(int ScanXX, int ScanYY, LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
void ResetLevel(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
void MainBoot();
int FichierPresent(const char* NomDuFichier);
int LoadConfig(config *cfg);
int RenduDeCouche(int Couche, LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
void RenduDeFond(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
void RenduDeFront(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
void RenduDeBackGround(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
void LevelGridInit(LevelGrid *levelgrid, ImagePKG *PKG);
void PartielLevelGridFree(LevelGrid *levelgrid);
void LevelGridFree(LevelGrid *levelgrid);
void ImagePKGInit(config *cfg, ImagePKG *PKG);
void ImagePKGFree(ImagePKG *PKG);
void UpdateScroll(LevelGrid *levelgrid, Widjet *widjet);
void ControlUpdate(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
void ReafectSprite(int gridpositionx, int gridpositiony, uint32_t sprite, int Decalage32x, int Decalage32Y, LevelGrid *levelgrid);
unsigned char colisiontestmaster(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
void MainSpriteAnim(LevelGrid *lg, Widjet *widjet);
int colisiondetect(uint32_t spritetester, int x1, int  y1, int w1, int h1, int x2, int  y2, int w2, int h2, LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
int colisiononeVSone(uint32_t sprite1, float x1, float  y1, float w1, float h1, uint32_t sprite2, float x2, float  y2, float w2, float h2, LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
int GameOver(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet);
int NumberOfLevel();
int NumberOfSprite();
int StandardSpriteEnemiConvertion(TypeReturn DIFERENCE_TYPE_ADD, int SpriteCheck, LevelGrid *levelgrid, ImagePKG *PKG);

void DEVNOTE(ImagePKG *PKG, Widjet *widjet) {
	FadeInFadeOutNoSoundModif(widjet->render, 16, 33, NULL, widjet);
	int t = 0, i = 0, l = 0, k = 0;
	int y = 0;
	char once = 0;
	int value = 0;
	char suite[700][255] = { {'0'} };
	char c = 'o';
	if (FichierPresent("README.txt")) {
		FILE *fichier;
		fichier = fopen("README.txt", "r");

		//nombre d'élément
		while (c != EOF) {
			c = fgetc(fichier);
			if (c == '\n') { t++; }
		}

		fseek(fichier, 0, SEEK_SET);
		c = ' ';
		l = 0; k = 0;
#define vert_note +100
#define hor_note +22
		while ((c != EOF)) {
			c = fgetc(fichier);
			suite[l][k] = c;
			if ((c != '\n') && (c != EOF)) { k++; }
			else { l++; k = 0; }
		}
		i = 0;
		SDL_Rect rect;

		while ((!widjet->Event.key[SDLK_ESCAPE]) && (!widjet->Event.quit)) {
			UpdateEvents(&widjet->Event);
			if (((widjet->Event.key[SDLK_RETURN] == 1))) { widjet->Event.quit = 1; }
			WidjetColorSurface(WidjetRGB(65, 20, 20), &widjet->render);
			rect.x = 43 + hor_note;
			rect.y = 8 + vert_note;
			rect.w = 509;
			rect.h = 233;
			SDL_FillRect(widjet->render, &rect, SDL_MapRGB(widjet->render[0].format, 0, 0, 0));
			WidjetFlatPanel(40 + hor_note, 5 + vert_note, 506, 230, widjet);
			rect.x = 44 + hor_note;
			rect.y = 8 + vert_note;
			rect.w = 500;
			rect.h = 224;
			SDL_FillRect(widjet->render, &rect, SDL_MapRGB(widjet->render[0].format, 35, 10, 10));
			rect.x = 43 + hor_note;
			rect.y = 8 + vert_note;
			rect.w = 500;
			rect.h = 224;
			WidjetGetFont(0, DEFAULT, DEFAULT, widjet);
			if (WidjetButton(4, 10, 33, 16, "BACK", widjet)) { widjet->Event.quit = 1; }
			i = value;
			WidjetGetFont(DEFAULT, WidjetRGB(255, 255, 0), 0, widjet);
			for (y = 1; y < 41; y++) {
				WidjetText(54 + hor_note, y * 11 + vert_note, suite[i], widjet);
				i++;
			}
			SDL_Delay(33);
			if (once == 0) {
				FadeInFadeOutNoSoundModif(NULL, 16, 33, widjet->render, widjet);
				once = 1;
			}
			else {
				SDL_Flip(widjet->render);
			}
			i = value;
		}
		widjet->Event.quit = 0;
		widjet->Event.key[SDLK_ESCAPE] = 0;
		fclose(fichier);
		FadeInFadeOutNoSoundModif(widjet->render, 16, 33, PKG->spriteBMP[44], widjet);
	}
}

void InfoText(ImagePKG *PKG, Widjet *widjet) {
	SDL_Rect rec;
	rec.x = 0;
	rec.y = 0;
	rec.w = 640;
	rec.h = 480;
	SDL_FillRect(widjet->render, &rec, 0);
	rec.x = 170;
	rec.y = 140;
	WidjetGetFont(2, WidjetRGB(0, 255, 255), WidjetRGB(255, 255, 255), widjet);
#define concep -10
#define concepx 250
#define ALLS 60
	WidjetBlitAndStretch(&PKG->spriteBMP[30], 0, 60 + concepx - 200, 220 + concep, 128, 100, &widjet->render);
	WidjetText(concepx, 160 + concep + ALLS, "Programed in C/SDL 1.2", widjet);
	WidjetText(concepx, 180 + concep + ALLS, "By Daniel C -Electro L.I.B", widjet);
	WidjetText(concepx, 200 + concep + ALLS, "Graphics and Sprites - Daniel C", widjet);
	WidjetText(concepx, 220 + concep + ALLS, "Musics and Sounds - Daniel C", widjet);
	WidjetText(concepx, 240 + concep + ALLS, "GamePlay Conceptor - Daniel C", widjet);
	WidjetGetFont(2, WidjetRGB(255, 255, 1), WidjetRGB(255, 255, 255), widjet);
	WidjetBlitAndStretch(&PKG->spriteBMP[254], DEFAULT, 170, 10, 300, 200, &widjet->render);
	WidjetBlitAndStretch(&PKG->spriteBMP[253], 0, 276, 448, 88, 31, &widjet->render);
	WidjetText(50, 260 + ALLS, "Tools of Conception -GIMP -KOLOURPAINT -BLENDER -LMMS -GEANY", widjet);
	WidjetGetFont(2, WidjetRGB(255, 1, 1), WidjetRGB(255, 255, 255), widjet);
	WidjetText(100, 280 + ALLS, "Gilbert And the Doors on Youtube Chanel -Daniel C", widjet);
	WidjetGetFont(3, WidjetRGB(255, 1, 255), WidjetRGB(255, 255, 255), widjet);
	WidjetText(204 + 24, 320 + ALLS, "COPYRIGHT 2016-2017", widjet);
	WidjetText(220 + 24, 340 + ALLS, "Daniel Champagne", widjet);
	WidjetText(204 + 24, 360 + ALLS, "All Rights Reserved.", widjet);
}

void INFO(ImagePKG *PKG, Widjet *widjet) {
	FadeInFadeOutNoSoundModif(widjet->render, 16, 33, NULL, widjet);
	InfoText(PKG, widjet);
	FadeInFadeOutNoSoundModif(NULL, 16, 33, widjet->render, widjet);
	WidjetGetFont(0, 16777215, 0, widjet);
	while ((!widjet->Event.key[SDLK_ESCAPE]) && (!widjet->Event.quit)) {
		UpdateEvents(&widjet->Event);
		if (((widjet->Event.key[SDLK_RETURN] == 1))) { widjet->Event.quit = 1; }
		if (WidjetButton(4, 10, 33, 16, "BACK", widjet)) { widjet->Event.quit = 1; }
		SDL_Flip(widjet->render);
		SDL_Delay(33);
	}
	widjet->Event.quit = 0;
	widjet->Event.key[SDLK_ESCAPE] = 0;
	FadeInFadeOutNoSoundModif(widjet->render, 16, 33, PKG->spriteBMP[44], widjet);
}

void LicenceGPL(ImagePKG *PKG, Widjet *widjet) {
	FadeInFadeOutNoSoundModif(widjet->render, 16, 33, NULL, widjet);
	int t = 0, i = 0, l = 0, k = 0;
	int y = 0;
	char once = 0;
	int value = 0;
	char suite[700][255] = { {'o'} };
	char c = 'o';
	if (FichierPresent("COPYING.txt")) {
		FILE *fichier;
		fichier = fopen("COPYING.txt", "r");
		//nombre d'élément
		while (c != EOF) {
			c = fgetc(fichier);
			if (c == '\n') { t++; }
		}
		fseek(fichier, 0, SEEK_SET);
		c = ' ';
		l = 0; k = 0;
		while ((c != EOF)) {
			c = fgetc(fichier);
			suite[l][k] = c;
			if ((c != '\n') && (c != EOF)) { k++; }
			else { l++; k = 0; }
		}
		i = 0;
		SDL_Rect rect;
		while ((!widjet->Event.key[SDLK_ESCAPE]) && (!widjet->Event.quit)) {
			UpdateEvents(&widjet->Event);
			if (((widjet->Event.key[SDLK_RETURN] == 1))) { widjet->Event.quit = 1; }
			WidjetColorSurface(13250, &widjet->render);
			rect.x = 43;
			rect.y = 8;
			rect.w = 569;
			rect.h = 453;
			SDL_FillRect(widjet->render, &rect, SDL_MapRGB(widjet->render[0].format, 0, 0, 0));
			WidjetFlatPanel(40, 5, 566, 450, widjet);
			rect.x = 43;
			rect.y = 8;
			rect.w = 560;
			rect.h = 444;
			SDL_FillRect(widjet->render, &rect, SDL_MapRGB(widjet->render[0].format, 0, 100, 125));
			WidjetGetFont(0, 0, 16777215, widjet);
			if (WidjetButton(4, 10, 33, 16, "BACK", widjet)) { widjet->Event.quit = 1; }
			WidjetTrackBar(40, 464, 566, 0, t - 40, &value, widjet);
			if (WidjetButton(16, 462, 20, 15, "-", widjet)) { if (value - 1 >= 0) { value--; } }
			if (WidjetButton(610, 462, 20, 15, "+", widjet)) { if (value + 1 <= t - 40) { value++; } }
			WidjetGetFont(0, 16777215, 0, widjet);
			i = value;
			for (y = 1; y < 41; y++) {
				WidjetText(54, y * 11, suite[i], widjet);
				i++;
			}
			SDL_Delay(33);
			if (once == 0) {
				FadeInFadeOutNoSoundModif(NULL, 16, 33, widjet->render, widjet);
				once = 1;
			}
			else {
				SDL_Flip(widjet->render);
			}
			i = value;
		}
		widjet->Event.quit = 0;
		widjet->Event.key[SDLK_ESCAPE] = 0;
		fclose(fichier);
		FadeInFadeOutNoSoundModif(widjet->render, 16, 33, PKG->spriteBMP[44], widjet);
	}
}

void ResetAllControl(LevelGrid *levelgrid, Widjet *widjet) {
	memset(&widjet->Event, 0, sizeof(widjet->Event));
	levelgrid->DoubleJumpCancel = 0;
	widjet->Event.JoyButton0 = 0;
	widjet->Event.JoyButton1 = 0;
	widjet->Event.JoyButton2 = 0;
	widjet->Event.JoyButton3 = 0;
	widjet->Event.JoyHatR = 0;
	widjet->Event.JoyHatL = 0;
	widjet->Event.JoyHatU = 0;
	widjet->Event.JoyHatD = 0;
}
void GreenScreen(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	SDL_Rect rec;
	int x, y;
	int tab[6][5];
	tab[0][0] = 336; tab[1][0] = 332; tab[2][0] = 332; tab[3][0] = 332; tab[4][0] = 332; tab[5][0] = 337;
	tab[0][1] = 335; tab[1][1] = 339; tab[2][1] = 339; tab[3][1] = 339; tab[4][1] = 339; tab[5][1] = 333;
	tab[0][2] = 335; tab[1][2] = 339; tab[2][2] = 339; tab[3][2] = 339; tab[4][2] = 339; tab[5][2] = 333;
	tab[0][3] = 335; tab[1][3] = 339; tab[2][3] = 339; tab[3][3] = 339; tab[4][3] = 339; tab[5][3] = 333;
	tab[0][4] = 331; tab[1][4] = 334; tab[2][4] = 334; tab[3][4] = 334; tab[4][4] = 334; tab[5][4] = 338;
	SDL_Surface *MemRender = SDL_DisplayFormat(widjet->render);
	SDL_Surface *temp = SDL_DisplayFormat(widjet->render);
	if (widjet->render->format->BitsPerPixel == 8) {
		WidjetColorSurface(0, &widjet->render);
	}
	for (y = 0; y < 15; y++) {
		for (x = 0; x < 20; x++) {
			rec.x = x * 32;
			rec.y = y * 32;
			if ((x == 0) || (y == 0) || (x == 19) || (y == 14)) {
				SDL_BlitSurface(PKG->spriteBMP[96], 0, widjet->render, &rec);

			}
		}
	}
#define center3x 230
#define center3y 140
	for (y = 0; y <= 4; y++) {
		for (x = 0; x <= 5; x++) {
			rec.x = center3x + (x * 32);
			rec.y = center3y + (y * 32);
			SDL_BlitSurface(PKG->spriteBMP[tab[x][y]], 0, widjet->render, &rec);
		}
	}
#define centerx 10
#define centery -100
	rec.x = 264 + centerx;
	rec.y = 256 + centery;
	SDL_BlitSurface(PKG->spriteBMP[22], 0, widjet->render, &rec);
	WidjetText(300 + centerx, 260 + centery, "X", widjet);
	WidjetNumericText(330 + centerx, 260 + centery, levelgrid->NombreDeVie, widjet);

#define center2x 10
#define center2y -60
	rec.x = 264 + center2x;
	rec.y = 256 + center2y;
	SDL_BlitSurface(PKG->spriteBMP[80], 0, widjet->render, &rec);
	WidjetText(300 + center2x, 260 + center2y, "X", widjet);
	WidjetNumericText(330 + center2x, 260 + center2y, levelgrid->CleTotalInLevel, widjet);

	WidjetText(260, 250, "Level", widjet);
	WidjetNumericText(360, 250, PKG->leveluse + 1, widjet);

	SDL_BlitSurface(widjet->render, 0, temp, 0);
	FadeInFadeOut(NULL, 16, 33, temp, widjet);
	SDL_Delay(2000);
	FadeInFadeOut(temp, 16, 33, NULL, widjet);
	SDL_FreeSurface(temp);
	SDL_FreeSurface(MemRender);
}

int StandardSpriteEnemiConvertion(TypeReturn DIFERENCE_TYPE_ADD, int SpriteCheck, LevelGrid *levelgrid, ImagePKG *PKG) {
	if (DIFERENCE_TYPE_ADD == DIFERENCE) {
		if COUCHESPRITEACTIF(SpriteCheck) { return 0; }
		if COUCHESPRITEACTIFSCORPION(SpriteCheck) { return 64; }
		return 0;
	}
	else if (DIFERENCE_TYPE_ADD == TYPE) {
		if COUCHESPRITEACTIF(SpriteCheck) { return 2; }
		if COUCHESPRITEACTIFSCORPION(SpriteCheck) { return 3; }
		return 0;
	}
	else if (DIFERENCE_TYPE_ADD == ADD) {
		if (SpriteCheck == 2) { return 0; }
		if (SpriteCheck == 3) { return 64; }
		return 0;
	}
	return 0;
}

char TheEnd(SDL_Joystick** joyst, LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	ResetAllControl(levelgrid, widjet);
	//sin
#define degre ((3.1416*2)/360)
	int deg = 0;
	//var frame rate
	int spin2 = 8;
	uint32_t delay1 = 0, delay2 = 0, delay3 = 0;
	WidjetGetFont(50, WidjetRGB(0, 255, 255), WidjetRGB(0, 0, 0), widjet);
	FadeInFadeOutNoSoundModif(NULL, 16, 33, PKG->spriteBMP[20], widjet);
#define AL -20
#define xa 80
#define xb -80
	float down = 0;
	char flip = 0;
	//boucle the end
	while ((!widjet->Event.key[SDLK_ESCAPE]) && (!widjet->Event.quit)) {
		UpdateEvents(&widjet->Event);
		JoystickUpdateEvents(joyst, &widjet->Event, levelgrid);
		if (((widjet->Event.JoyButton1 == 1)) || ((widjet->Event.key[SDLK_RETURN] == 1))) { widjet->Event.quit = 1; }
		if (flip == 0) { down = down + .3; }
		else { down = down - .3; }
		if (down >= 32) { flip = 1; }
		if (down <= 0) { flip = 0; }
		if (deg < 360) { deg = deg + 2; }
		else { deg = 0; }
		SDL_BlitSurface(PKG->spriteBMP[20], 0, widjet->render, 0);
		WidjetText(50 + AL + xa, 180 + 32 * sin(deg*degre), "T", widjet);
		WidjetText(114 + AL + xa, 180 + 32 * sin((deg + 45)*degre), "H", widjet);
		WidjetText(178 + AL + xa, 180 + 32 * sin((deg + 90)*degre), "E", widjet);
		WidjetText(434 + AL + xb, 180 + 32 * sin((deg + 135)*degre), "E", widjet);
		WidjetText(498 + AL + xb, 180 + 32 * sin((deg + 180)*degre), "N", widjet);
		WidjetText(562 + AL + xb, 180 + 32 * sin((deg + 225)*degre), "D", widjet);
		delay2 = SDL_GetTicks();
		delay3 = delay2 - delay1;
		if ((delay3 < spin2)) { SDL_Delay(spin2 - delay3); }
		delay1 = SDL_GetTicks();
		SDL_Flip(widjet->render);
	}
	FadeInFadeOut(widjet->render, 16, 33, NULL, widjet);
	widjet->Event.quit = 0;
	widjet->Event.key[SDLK_ESCAPE] = 0;
	WidjetGetFont(12, WidjetRGB(255, 255, 0), WidjetRGB(0, 0, 0), widjet);
	ResetAllControl(levelgrid, widjet);
	return 0;
}

void ParticularityOfLevel(LevelGrid *levelgrid, ImagePKG *PKG) {
	switch (PKG->leveluse) {
	case (0):levelgrid->FrontPic = 11; levelgrid->cloud = 17; levelgrid->background = 31; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-1.wav"); break;
	case (1):levelgrid->FrontPic = 11; levelgrid->cloud = 17; levelgrid->background = 5; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-3.wav"); break;
	case (2):levelgrid->FrontPic = 11; levelgrid->cloud = 17; levelgrid->background = 3; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-1.wav"); break;
	case (3):levelgrid->FrontPic = 11; levelgrid->cloud = 17; levelgrid->background = 53; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-3.wav"); break;
	case (4):levelgrid->FrontPic = 19; levelgrid->cloud = -1; levelgrid->background = 21; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-2.wav"); break;
	case (5):levelgrid->FrontPic = 11; levelgrid->cloud = 17; levelgrid->background = 5; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-1.wav"); break;
	case (6):levelgrid->FrontPic = 11; levelgrid->cloud = 17; levelgrid->background = 5; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-3.wav"); break;
	case (7):levelgrid->FrontPic = 19; levelgrid->cloud = -1; levelgrid->background = 21; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-2.wav"); break;
	case (8):levelgrid->FrontPic = 11; levelgrid->cloud = 17; levelgrid->background = 3; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-1.wav"); break;
	case (9):levelgrid->FrontPic = 11; levelgrid->cloud = 17; levelgrid->background = 5; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-2.wav"); break;
	case (10):levelgrid->FrontPic = 19; levelgrid->cloud = -1; levelgrid->background = 21; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-3.wav"); break;
	case (11):levelgrid->FrontPic = 11; levelgrid->cloud = 17; levelgrid->background = 3; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-1.wav"); break;
	case (12):levelgrid->FrontPic = 11; levelgrid->cloud = 17; levelgrid->background = 31; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-2.wav"); break;
	case (13):levelgrid->FrontPic = 19; levelgrid->cloud = -1; levelgrid->background = 21; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-3.wav"); break;
	case (14):levelgrid->FrontPic = 11; levelgrid->cloud = 17; levelgrid->background = 3; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-2.wav"); break;
	case (15):levelgrid->FrontPic = 19; levelgrid->cloud = -1; levelgrid->background = 62; levelgrid->backgroudScoll = 0; strcpy(levelgrid->music, "MUSIC/music-3.wav"); break;
	case (16):levelgrid->FrontPic = 11; levelgrid->cloud = 17; levelgrid->background = 53; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-1.wav"); break;
	case (17):levelgrid->FrontPic = 11; levelgrid->cloud = 17; levelgrid->background = 5; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music-3.wav"); break;
	case (18):levelgrid->FrontPic = 19; levelgrid->cloud = -1; levelgrid->background = 62; levelgrid->backgroudScoll = 0; strcpy(levelgrid->music, "MUSIC/music-1.wav"); break;
	case (19):levelgrid->FrontPic = 19; levelgrid->cloud = -1; levelgrid->background = 62; levelgrid->backgroudScoll = 0; strcpy(levelgrid->music, "MUSIC/music-4.wav"); break;
	case (20):levelgrid->FrontPic = 19; levelgrid->cloud = -1; levelgrid->background = 62; levelgrid->backgroudScoll = 0; strcpy(levelgrid->music, "MUSIC/music-2.wav"); break;
	case (21):levelgrid->FrontPic = 19; levelgrid->cloud = -1; levelgrid->background = 62; levelgrid->backgroudScoll = 0; strcpy(levelgrid->music, "MUSIC/music-3.wav"); break;

	default:levelgrid->FrontPic = 11; levelgrid->cloud = 17; levelgrid->background = 31; levelgrid->backgroudScoll = 1; strcpy(levelgrid->music, "MUSIC/music.wav"); break;
		break;
	}
}

int GameOver(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	FadeInFadeOut(widjet->render, 16, 33, NULL, widjet);
	ResetAllControl(levelgrid, widjet);
	return 1;
}

void SetupConfig(config *Config, LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	WidjetGetFont(DEFAULT, WidjetRGB(255, 255, 0), WidjetRGB(0, 0, 0), widjet);
	SDL_Rect rectq;
	int cont = 1;
	int Reboot = 0;
	char joys = 0;
	int bit = Config->BIT;
	int SOFT = Config->SOFTWARERENDER;
	int DoubleBuff = Config->DOUBLEBUF;
	int FULLSCREEN = Config->FULLSCREEN;
	int check1 = Config->BACKGROUND;
	int check2 = Config->BACK;
	int check3 = Config->FRONT;
	int check4 = Config->MusicConfig;
	int check5 = Config->FXConfig;
	FILE * fichier = NULL;
	fichier = fopen("Config.ini", "w");
	if (DoubleBuff == 1) { widjet->radiobutton[0] = 144; widjet->radiobutton[1] = 306; }
	else { widjet->radiobutton[0] = 174; widjet->radiobutton[1] = 306; }
	while (cont == 1) {
		UpdateEvents(&widjet->Event);
		if ((widjet->Event.key[SDLK_ESCAPE] == 1) || (widjet->Event.quit == 1)) {
			cont = 0;
			widjet->Event.key[SDLK_ESCAPE] = 0;
			widjet->Event.quit = 0;
		}
		rectq.x = 0;
		rectq.y = 0;
		SDL_BlitSurface(PKG->spriteBMP[44], 0, widjet->render, &rectq);
		WidjetFlatPanel(10, 222, 270, 250, widjet);
		if (WidjetButtonBMP(14, 228, 60, 32, "8 Bit", &PKG->spriteBMP[12], widjet)) { bit = 8; }
		if (WidjetButtonBMP(75, 228, 60, 32, "16Bit", &PKG->spriteBMP[12], widjet)) { bit = 16; }
		if (WidjetButtonBMP(136, 228, 60, 32, "32Bit", &PKG->spriteBMP[12], widjet)) { bit = 32; }
		WidjetNumericText(220, 240, bit, widjet);
		if (WidjetButtonBMP(14, 260, 91, 32, "Soft Render", &PKG->spriteBMP[12], widjet)) { SOFT = 1; }
		if (WidjetButtonBMP(105, 260, 91, 32, "Hard Render", &PKG->spriteBMP[12], widjet)) { SOFT = 0; }
		if (SOFT) { WidjetText(220, 272, "SOFT", widjet); }
		else { WidjetText(220, 272, "HARD", widjet); }
		WidjetText(14, 306, "Double Buff Render", widjet);
		if (WidjetRadioButton(144, 306, "ON", widjet)) { DoubleBuff = 1; }
		if (WidjetRadioButton(174, 306, "OFF", widjet)) { DoubleBuff = 0; }
		if (WidjetButtonBMP(14, 332, 90, 32, "Full Screen", &PKG->spriteBMP[12], widjet)) { FULLSCREEN = 1; }
		if (WidjetButtonBMP(106, 332, 90, 32, "Windows Mode", &PKG->spriteBMP[12], widjet)) { FULLSCREEN = 0; }
		if (WidjetButtonBMP(180, 368, 64, 64, " ", &PKG->spriteBMP[251], widjet)) { joys = 1; }
		if (FULLSCREEN) { WidjetText(200, 344, "FullScreen", widjet); }
		else { WidjetText(200, 344, "Windows", widjet); }
		WidjetCheckBox(12, 382, "BackGround Scroll", &check1, widjet);
		WidjetCheckBox(12, 402, "Cloud Scroll", &check2, widjet);
		WidjetCheckBox(12, 422, "Front Scroll", &check3, widjet);
		WidjetFlatPanel(12, 434, 100, 32, widjet);
		WidjetCheckBox(20, 438, "Music", &check4, widjet);
		WidjetCheckBox(20, 452, "Sound FX", &check5, widjet);
		if (WidjetButtonBMP(146, 434, 128, 32, "Back To Menu", &PKG->spriteBMP[12], widjet)) { cont = 0; }
		SDL_Flip(widjet->render);
		if (joys == 1) {
			joys = 0;
			FadeInFadeOutNoSoundModif(widjet->render, 16, 15, PKG->spriteBMP[252], widjet);
			while (cont != 2) {
				UpdateEvents(&widjet->Event);
				SDL_BlitSurface(PKG->spriteBMP[252], 0, widjet->render, &rectq);
				if (WidjetButton(8, 10, 33, 16, "BACK", widjet)) { widjet->Event.quit = 1; }
				SDL_Flip(widjet->render);
				if ((widjet->Event.key[SDLK_ESCAPE] == 1) || (widjet->Event.quit == 1)) {
					cont = 2;
					widjet->Event.key[SDLK_ESCAPE] = 0;
					widjet->Event.quit = 0;
				}
			}
			FadeInFadeOutNoSoundModif(widjet->render, 16, 15, PKG->spriteBMP[44], widjet);
		}
		if (cont == 2) { cont = 1; }
	}
	if (fichier != NULL) {
		//écrire dans le fichier
		fprintf(fichier, "bit=%d\n", bit);
		fprintf(fichier, "SOFTWARERENDER=%d\n", SOFT);
		fprintf(fichier, "DOUBLEBUFFER=%d\n", DoubleBuff);
		fprintf(fichier, "FULLSCREEN=%d\n", FULLSCREEN);
		fprintf(fichier, "BACKGROUND=%d\n", check1);
		fprintf(fichier, "BACK=%d\n", check2);
		fprintf(fichier, "FRONT=%d\n", check3);
		fprintf(fichier, "MUSIC=%d\n", check4);
		fprintf(fichier, "SOUNDFX=%d\n", check5);

		if (Config->FULLSCREEN != FULLSCREEN) { Reboot = 1; }
		if (Config->BIT != bit) { Reboot = 1; }
		if (Config->SOFTWARERENDER != SOFT) { Reboot = 1; }
		if (Config->DOUBLEBUF != DoubleBuff) { Reboot = 1; }
		Config->FXConfig = check5;
		Config->MusicConfig = check4;
		Config->BIT = bit;
		Config->SOFTWARERENDER = SOFT;

		Config->DOUBLEBUF = DoubleBuff;
		Config->FULLSCREEN = FULLSCREEN;
		Config->BACKGROUND = check1;
		Config->BACK = check2;
		Config->FRONT = check3;

		if (Reboot == 1) {
			cont = 1;
			while (cont == 1) {
				SDL_BlitSurface(PKG->spriteBMP[44], 0, widjet->render, &rectq);
				UpdateEvents(&widjet->Event);
				WidjetFlatPanel(220, 195, 200, 90, widjet);
				if (WidjetButtonBMP(256, 224, 128, 32, "Save and Quit", &PKG->spriteBMP[12], widjet)) { cont = 0; widjet->Event.quit = 1; }
				SDL_Flip(widjet->render);
			}
		}
	}
	else {
		printf("Impossible d'ouvrir le fichier config.ini");
	}
	WidjetGetFont(12, WidjetRGB(255, 255, 0), WidjetRGB(0, 0, 0), widjet);
	fclose(fichier);
}

int JoystickUpdateEvents(SDL_Joystick** joyst, Input* in, LevelGrid *levelgrid) {
	if (NumberofJoystick != 0) {
		SDL_JoystickUpdate();
		if ((in->JoyButton0 == 2) && (!SDL_JoystickGetButton(*joyst, 2))) { in->JoyButton0 = 0; }
	}
	else {
		if ((in->JoyButton0 == 2)) { in->JoyButton0 = 0; }
	}
	in->JoyButton1 = 0;
	in->JoyButton2 = 0;
	in->JoyButton3 = 0;
	in->JoyHatR = 0;
	in->JoyHatL = 0;
	in->JoyHatU = 0;
	in->JoyHatD = 0;
	if (NumberofJoystick != 0) {
		//joystick
		if ((SDL_JoystickGetButton(*joyst, 0)) && (in->JoyButton3 != 2)) { in->JoyButton3 = 1; }
		if ((SDL_JoystickGetButton(*joyst, 2)) && levelgrid->DoubleJumpCancel == 0) { in->JoyButton1 = 1; }
		if (SDL_JoystickGetButton(*joyst, 1)) { in->JoyButton0 = 1; }
		if (SDL_JoystickGetButton(*joyst, 3)) { in->JoyButton2 = 1; }

		if ((SDL_JoystickGetHat(*joyst, 0)) == SDL_HAT_RIGHT) { in->JoyHatR = 1; }
		if ((SDL_JoystickGetHat(*joyst, 0)) == SDL_HAT_LEFT) { in->JoyHatL = 1; }
		if ((SDL_JoystickGetHat(*joyst, 0)) == SDL_HAT_UP) { in->JoyHatU = 1; }
		if ((SDL_JoystickGetHat(*joyst, 0)) == SDL_HAT_DOWN) { in->JoyHatD = 1; }

		if ((SDL_JoystickGetHat(*joyst, 0)) == SDL_HAT_RIGHTUP) { in->JoyHatR = 1; in->JoyHatU = 1; }
		if ((SDL_JoystickGetHat(*joyst, 0)) == SDL_HAT_LEFTUP) { in->JoyHatL = 1; in->JoyHatU = 1; }
		if ((SDL_JoystickGetHat(*joyst, 0)) == SDL_HAT_LEFTDOWN) { in->JoyHatD = 1; in->JoyHatL = 1; }
		if ((SDL_JoystickGetHat(*joyst, 0)) == SDL_HAT_RIGHTDOWN) { in->JoyHatD = 1; in->JoyHatR = 1; }

		if (SDL_JoystickGetAxis(*joyst, 0) > 10000) { in->JoyHatR = 1; }
		if (SDL_JoystickGetAxis(*joyst, 0) < -10000) { in->JoyHatL = 1; }
		if (SDL_JoystickGetAxis(*joyst, 1) > 10000) { in->JoyHatD = 1; }
		if (SDL_JoystickGetAxis(*joyst, 1) < -10000) { in->JoyHatU = 1; }
		//-32768 à 32768
	}

	//Clavier
	if ((in->key[SDLK_LSHIFT]) && (in->JoyButton0 != 2)) { in->JoyButton0 = 1; }
	if ((in->key[SDLK_LCTRL]) && levelgrid->DoubleJumpCancel == 0) { in->JoyButton1 = 1; }
	if (in->key[SDLK_RETURN]) { in->JoyButton2 = 1; }

	if (in->key[SDLK_LEFT]) { in->JoyHatL = 1; }
	if (in->key[SDLK_RIGHT]) { in->JoyHatR = 1; }

	if (NumberofJoystick != 0)
	{

		if (levelgrid->VitalInjuryTimer >= 0) {
			if (((SDL_JoystickGetButton(*joyst, 2)) == 0) && (in->key[SDLK_LCTRL] == 0) && ((levelgrid->speedaxey > -1) && (levelgrid->speedaxey < 1))) { levelgrid->DoubleJumpCancel = 0; }
		}
		else {
			if (((SDL_JoystickGetButton(*joyst, 2)) == 0) && (in->key[SDLK_LCTRL] == 0)) { levelgrid->DoubleJumpCancel = 0; }
		}
	}
	else
	{
		if (levelgrid->VitalInjuryTimer >= 0) {
			if ((in->key[SDLK_LCTRL] == 0) && ((levelgrid->speedaxey > -1) && (levelgrid->speedaxey < 1))) 
			{ 
				levelgrid->DoubleJumpCancel = 0; 
			}
		}
		else {
			if (in->key[SDLK_LCTRL] == 0) 
			{ 
				levelgrid->DoubleJumpCancel = 0; 
			}
		}
	}

	if (levelgrid->speedaxey > 3) { levelgrid->DoubleJumpCancel = 1; in->JoyButton1 = 0; }

	if (in->JoyButton1 != 0) { return 1; }
	if (in->JoyButton2 != 0) { return 1; }
	if (in->JoyButton3 != 0) { return 1; }
	if (in->JoyHatR != 0) { return 1; }
	if (in->JoyHatL != 0) { return 1; }
	if (in->JoyHatU != 0) { return 1; }
	if (in->JoyHatD != 0) { return 1; }
	return 0;
}





void FullFrameRoutine(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	SDL_Rect RECT;
	int x = 0, y = 0, a = 0, b = 0;

	// animation water 
	if (levelgrid->ActualFrame % 14 == 0) {
		if ((levelgrid->WaterUpDown == 1)) {
			if (levelgrid->WaterAnim < 4) { levelgrid->WaterAnim = levelgrid->WaterAnim + 1; }
			else { levelgrid->WaterUpDown = 0; levelgrid->WaterAnim = levelgrid->WaterAnim - 1; }
		}
		else {
			if (levelgrid->WaterAnim > 0) { levelgrid->WaterAnim = levelgrid->WaterAnim - 1; }
			else { levelgrid->WaterUpDown = 1; levelgrid->WaterAnim = levelgrid->WaterAnim + 1; }
		}
	}
	if (levelgrid->WaterSlowMove == 1) {
		speedmax = 1; //normale 1
		speedmaxy = 2; //normale 2
	}
	else {
		speedmax = 2; //normale 2
		speedmaxy = 4; //normale 4
	}
	//fin water

	//anim lamp
	AnimLamp(levelgrid, PKG, widjet);

	//animation switch
	if (levelgrid->ActualFrame % 8 == 0) {
		if (levelgrid->Switch == 1) { if (levelgrid->AnimSwitch <= 32) { levelgrid->AnimSwitch = levelgrid->AnimSwitch + 1; } }
		else {
			if (levelgrid->AnimSwitch >= 1) { levelgrid->AnimSwitch = levelgrid->AnimSwitch - 1; }
		}
	}
	// anim switch fin

#define mspo32x (trunc(levelgrid->MainSpriteoffset32x))
#define mspo32y (trunc(levelgrid->MainSpriteoffset32y))
#define mx ((levelgrid->mainspritegridx*32)+mspo32x)-((levelgrid->Scrollxlevel*32)-(trunc(levelgrid->Scrollx32)))
#define my ((levelgrid->mainspritegridy*32)+mspo32y)-((levelgrid->Scrollylevel*32)-(trunc(levelgrid->Scrolly32)))
#define mxo (0)
#define myo (0)


	if ((levelgrid->Explose <= 0)) {
		if (levelgrid->MainSpriteoffset32x >= 16) { levelgrid->bombeXgrid = levelgrid->mainspritegridx + 1; }
		else { levelgrid->bombeXgrid = levelgrid->mainspritegridx; } //ok
		if (levelgrid->MainSpriteoffset32y >= 16) { levelgrid->bombeYgrid = levelgrid->mainspritegridy + 1; }
		else { levelgrid->bombeYgrid = levelgrid->mainspritegridy; } //ok
	}
	x = ((round((levelgrid->bombeXgrid - levelgrid->Scrollxlevel) * 32)) + levelgrid->Scrollx32) - 32; //ok position sur la grille a exploser
	y = ((round((levelgrid->bombeYgrid - levelgrid->Scrollylevel) * 32)) + levelgrid->Scrolly32) - 32; //ok position sur la grille a exploser
	RECT.x = x;
	RECT.y = y;
	if (levelgrid->Explose > 0) { // 15 frame valeur 15 déclenche l'Animation a l'endrois choisi
		if (levelgrid->Explose == 7) {
			for (a = -1; a < 2; a++) {
				for (b = -1; b < 2; b++) {
					if (levelgrid->sprite[levelgrid->bombeXgrid + a][levelgrid->bombeYgrid + b] == 0) {
						ReafectSprite(levelgrid->bombeXgrid + a, levelgrid->bombeYgrid + b, 16777215, 0, 0, levelgrid);
					}
				}
			}
		}
		if (levelgrid->Explose < 15) {
			if (levelgrid->Explose == 12) {
				a = levelgrid->bombeXgrid;
				b = levelgrid->bombeYgrid;
				ReafectSprite(a, b, 16777215, 0, 0, levelgrid);
			}
#define SOUFLE 4
			RECT.x = (x)-((15 - (levelgrid->Explose))*SOUFLE);
			RECT.y = (y)-((15 - (levelgrid->Explose))*SOUFLE);
			SDL_BlitSurface(PKG->spriteBMP[184 + (14 - levelgrid->Explose)], 0, widjet->render, &RECT);
			RECT.x = (x)+((15 - (levelgrid->Explose))*SOUFLE);
			RECT.y = (y)-((15 - (levelgrid->Explose))*SOUFLE);
			SDL_BlitSurface(PKG->spriteBMP[184 + (14 - levelgrid->Explose)], 0, widjet->render, &RECT);
			RECT.x = (x)-((15 - (levelgrid->Explose))*SOUFLE);
			RECT.y = (y)+((15 - (levelgrid->Explose))*SOUFLE);
			SDL_BlitSurface(PKG->spriteBMP[184 + (14 - levelgrid->Explose)], 0, widjet->render, &RECT);
			RECT.x = (x)+((15 - (levelgrid->Explose))*SOUFLE);
			RECT.y = (y)+((15 - (levelgrid->Explose))*SOUFLE);
			SDL_BlitSurface(PKG->spriteBMP[184 + (14 - levelgrid->Explose)], 0, widjet->render, &RECT);
			RECT.x = x;
			RECT.y = y;	SDL_BlitSurface(PKG->spriteBMP[184 + (14 - levelgrid->Explose)], 0, widjet->render, &RECT);
		}
		else {
			a = levelgrid->bombeXgrid;
			b = levelgrid->bombeYgrid;
			if (levelgrid->sprite[a][b] == 16777215) { ReafectSprite(a, b, 276, 0, 0, levelgrid); }
		}
		if ((levelgrid->ActualFrame % 5) == 0) {
			if (levelgrid->Explose == 14) { PlaySND(PLAY, EXPLOSE); }
			levelgrid->Explose = levelgrid->Explose - 1;
		}
	}

	//endurence et vie
	if (levelgrid->VitalInjuryTimer >= 0) {
		if (levelgrid->ActualFrame % 8 == 0) { levelgrid->VitalInjuryTimer = levelgrid->VitalInjuryTimer - 1; }
	}
	else { levelgrid->VisibleMainSprite = 1; }

	//EndurenceTrackbar
	WidjetProgressBar(388, 4, 2, 28, 0, 4, EndurenceTrackbar, widjet);
	if (EndurenceTrackbar == 0) { PlaySND(PLAY, VITAL); }
}

void VitalInjury(LevelGrid *levelgrid, Widjet *widjet) {
	levelgrid->DoubleJumpCancel = 1;
	if (levelgrid->VitalInjuryTimer <= 0) {
		PlaySND(PLAY, OUTCH);
		EndurenceTrackbar = EndurenceTrackbar - 1;
		levelgrid->speedaxey = -levelgrid->speedaxey + (-4);
		if (levelgrid->speedaxex >= 0) { levelgrid->speedaxex = levelgrid->speedaxex - 4; }
		else {
			levelgrid->speedaxex = levelgrid->speedaxex + 4;
		}
		levelgrid->VitalInjuryTimer = 20;
	}
}

void PlaySND(PLAYSTOP playstop, SND snd) {
	int x = 0;

	switch (snd) {
	case JUMP:  sndmatrice[0] = playstop; break;
	case MUSIC1: sndmatrice[1] = playstop;  break;
	case THEEND:  sndmatrice[2] = playstop; break;
	case KEY: sndmatrice[5] = playstop;  break;
	case BUMP: sndmatrice[6] = playstop;  break;
	case DOOR1:  sndmatrice[7] = playstop; break;
	case DOOR2:  sndmatrice[8] = playstop; break;
	case OUTCH:  sndmatrice[9] = playstop; break;
	case COIN:  sndmatrice[10] = playstop; break;
	case EXPLOSE:  sndmatrice[11] = playstop; break;
	case GAMEOVER:  sndmatrice[12] = playstop; break;
	case SWITCHON:  sndmatrice[13] = playstop; break;
	case SWITCHOFF: sndmatrice[14] = playstop; break;
	case OPENDOOR: sndmatrice[15] = playstop; break;
	case BADABOUM: sndmatrice[16] = playstop; break;
	case VITAL: sndmatrice[17] = playstop; break;
	case ALLSND:
		for (x = 0; x < 19; x++) {
			sndmatrice[x] = playstop;
		}Mix_HaltChannel(-1); Mix_HaltMusic(); Mix_PauseMusic(); Mix_RewindMusic(); break; //Arrête tout son et musique
	}
}
int RestartGame(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	PKG->leveluse = 0;
	levelgrid->CleTotaleAmasser = 0;
	levelgrid->NombreDeVie = NBVIE;
	EndurenceTrackbar = 4;
	levelgrid->Switch = 0;
	GreenScreen(levelgrid, PKG, widjet);
	LevelGridInit(levelgrid, PKG);
	PlaySND(STOP, ALLSND);
	MainSpriteEnable = ON;
	return 0;
}

int NextLevel(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	FadeInFadeOut(widjet->render, 16, 33, NULL, widjet);
	PKG->leveluse = PKG->leveluse + 1;
	levelgrid->Switch = 0;
	levelgrid->CleTotaleAmasser = 0;
	MainSpriteEnable = ON;
	if (PKG->leveluse > LevelTotal - 1) { return 1; }
	else {
		LevelGridInit(levelgrid, PKG);
		GreenScreen(levelgrid, PKG, widjet);
		PlaySND(PLAY, MUSIC1);
	}
	return 0;
}

int OpenDoorProcedure(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	if (levelgrid->ActualFrame % 7 == 0) {
		if ((levelgrid->DoorOpen == 1)) {
			if (levelgrid->DoorOpenCurentFrameAnim < 23) {
				levelgrid->ScrollStep = levelgrid->ScrollStep / 1.2;
				levelgrid->ScrollStepy = levelgrid->ScrollStepy / 1.2;
				if (levelgrid->DoorOpenCurentFrameAnim == 2) { PlaySND(PLAY, OPENDOOR); }levelgrid->DoorOpenCurentFrameAnim = levelgrid->DoorOpenCurentFrameAnim + 1;
			}
			else { levelgrid->DoorOpen = 2; }
			if (levelgrid->DoorOpenCurentFrameAnim == 22) { MainSpriteEnable = OFF; }
		}
		else {
			if (levelgrid->DoorOpenCurentFrameAnim > 0) {
				levelgrid->DoorOpenCurentFrameAnim = levelgrid->DoorOpenCurentFrameAnim - 1;
			}
		}
	}
	if ((MainSpriteEnable == OFF) && (levelgrid->DoorOpenCurentFrameAnim == 0)) {
		if (NextLevel(levelgrid, PKG, widjet)) {
			levelgrid->DoorOpen = 0; return 2;
		}
		return 1;
	}
	return 0;
}


int MoveEnemi(int ScanXX, int ScanYY, LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	int retour = 0;//pour eviter double blit

	//var temporaire de balayage
	char bypass = 0;
	int gauche = 0;
	int droite = 0;
	char typea = 0;
	char typeb = 0;
	char backup = 0;
	int rest = 0;
	int x = 0;
	int y = 0;
	uint32_t surface1 = 0, surface2 = 0;
	int LARGEURdeSCANx = COLLISIONBALAYAGE;
	int LARGEURdeSCANy = COLLISIONBALAYAGE;
	typea = 0;
	if ((levelgrid->MapSpriteEnemiSimplifier[ScanXX][ScanYY] > 0)) {
		gauche = 356;
		droite = 385;
		typea = 1;
	}
	if (COUCHESPRITEACTIFBLOB(levelgrid->MapSpriteEnemi[ScanXX][ScanYY])) {
		typea = 3;
	}

	//anim blob
	int ttt = (rand() % 200);
	if (ttt == 1) { if (levelgrid->MapSpriteEnemi[ScanXX][ScanYY] == 255) { levelgrid->MapSpriteEnemi[ScanXX][ScanYY] = 256; } }
	if ((levelgrid->MapSpriteEnemi[ScanXX][ScanYY] >= 256) && (levelgrid->ActualFrame % 2 == 0) && (COUCHESPRITEACTIFBLOB(levelgrid->MapSpriteEnemi[ScanXX][ScanYY]))) {
		if (levelgrid->MapSpriteEnemi[ScanXX][ScanYY] < 274) { levelgrid->MapSpriteEnemi[ScanXX][ScanYY] = levelgrid->MapSpriteEnemi[ScanXX][ScanYY] + 1; }
		else { levelgrid->MapSpriteEnemi[ScanXX][ScanYY] = 255; }
	}

	//anim sprite enemi
	if ((levelgrid->ActualFrame % 6 == 0) && ((levelgrid->MapSpriteEnemiSimplifier[ScanXX][ScanYY] > 0))) {
		if ((levelgrid->MapSpriteEnemi[ScanXX][ScanYY] >= 360) && (levelgrid->MapSpriteEnemi[ScanXX][ScanYY] <= 384)) {
			levelgrid->MapSpriteEnemi[ScanXX][ScanYY] = levelgrid->MapSpriteEnemi[ScanXX][ScanYY] + 1;
			if (levelgrid->MapSpriteEnemi[ScanXX][ScanYY] >= 384) { levelgrid->MapSpriteEnemi[ScanXX][ScanYY] = 363; }
		}
		else {
			levelgrid->MapSpriteEnemi[ScanXX][ScanYY] = levelgrid->MapSpriteEnemi[ScanXX][ScanYY] + 1;
			if ((levelgrid->MapSpriteEnemi[ScanXX][ScanYY]) >= 413) { levelgrid->MapSpriteEnemi[ScanXX][ScanYY] = 392; }
		}
	}
	for (y = (ScanYY + LARGEURdeSCANy); y < ScanYY + 3; y++) {
		for (x = (ScanXX + LARGEURdeSCANx); x < ScanXX + 3; x++) {
			bypass = 0;

			//test
			if (x > levelgrid->width - 1) { bypass = 1; }
			if (x < 0) { bypass = 1; }
			if (y > levelgrid->height - 1) { bypass = 1; }
			if (y < 0) { bypass = 1; }
			//fin test


			if (bypass == 0) {
				//simplifier la direction des enemi
				typeb = 0;

				if ((levelgrid->MapSpriteEnemiSimplifier[x][y] > 0)) {

					typeb = 1;
				}

				if (COUCHESPRITEACTIFBLOB(levelgrid->MapSpriteEnemi[x][y])) {
					typeb = 3;
				}
				if (((ScanXX != x) || (ScanYY != y)) && (typeb != 0) && (typea != 3)) {
					//test collision sur grille enemi

					surface1 = 140;
					surface2 = 140;

					if (levelgrid->MapEnemiDirection[ScanXX][ScanYY] == 1) {
						rest = colisiononeVSone(surface1, (ScanXX * 32) + (levelgrid->MapEnemiOffSetX[ScanXX][ScanYY]) - (ENEMISTEPMOVE + 1), (ScanYY * 32) + levelgrid->MapEnemiOffSetY[ScanXX][ScanYY], PKG->spriteBMP[surface1]->w, PKG->spriteBMP[surface1]->h,
							surface2, (x * 32) + (levelgrid->MapEnemiOffSetX[x][y]), (y * 32) + levelgrid->MapEnemiOffSetY[x][y], PKG->spriteBMP[surface2]->w, PKG->spriteBMP[surface2]->h, levelgrid, PKG, widjet);
					}
					else {
						rest = colisiononeVSone(surface1, (ScanXX * 32) + (levelgrid->MapEnemiOffSetX[ScanXX][ScanYY]) + (ENEMISTEPMOVE + 1), (ScanYY * 32) + levelgrid->MapEnemiOffSetY[ScanXX][ScanYY], PKG->spriteBMP[surface1]->w, PKG->spriteBMP[surface1]->h,
							surface2, (x * 32) + (levelgrid->MapEnemiOffSetX[x][y]), (y * 32) + levelgrid->MapEnemiOffSetY[x][y], PKG->spriteBMP[surface2]->w, PKG->spriteBMP[surface2]->h, levelgrid, PKG, widjet);
					}
					if (rest == 1) {
						backup = 1;
						if (levelgrid->MapEnemiDirection[ScanXX][ScanYY] != 1) { levelgrid->MapEnemiDirection[ScanXX][ScanYY] = 1; levelgrid->MapSpriteEnemi[ScanXX][ScanYY] = gauche; }
						else { levelgrid->MapEnemiDirection[ScanXX][ScanYY] = 0; levelgrid->MapSpriteEnemi[ScanXX][ScanYY] = droite; }
						return 0;
					}
				}

				//fin test2

				if (((!COUCHESPRITEACTIFBLOB(levelgrid->MapSpriteEnemi[ScanXX][ScanYY]))) && (levelgrid->sprite[x][y] != 16777215) && (levelgrid->SpriteSimplifer[x][y] == 1)) {
					if (levelgrid->MapEnemiDirection[ScanXX][ScanYY] == 1) {
						rest = colisiononeVSone(140, (ScanXX * 32) + (levelgrid->MapEnemiOffSetX[ScanXX][ScanYY]) - ENEMISTEPMOVE, (ScanYY * 32) + levelgrid->MapEnemiOffSetY[ScanXX][ScanYY], PKG->spriteBMP[levelgrid->MapSpriteEnemi[ScanXX][ScanYY]]->w, PKG->spriteBMP[levelgrid->MapSpriteEnemi[ScanXX][ScanYY]]->h,
							levelgrid->sprite[x][y], (x * 32), (y * 32), PKG->spriteBMP[levelgrid->sprite[x][y]]->w, PKG->spriteBMP[levelgrid->sprite[x][y]]->h, levelgrid, PKG, widjet);
					}
					else {
						rest = colisiononeVSone(140, (ScanXX * 32) + (levelgrid->MapEnemiOffSetX[ScanXX][ScanYY]) + ENEMISTEPMOVE, (ScanYY * 32) + levelgrid->MapEnemiOffSetY[ScanXX][ScanYY], PKG->spriteBMP[levelgrid->MapSpriteEnemi[ScanXX][ScanYY]]->w, PKG->spriteBMP[levelgrid->MapSpriteEnemi[ScanXX][ScanYY]]->h,
							levelgrid->sprite[x][y], (x * 32), (y * 32), PKG->spriteBMP[levelgrid->sprite[x][y]]->w, PKG->spriteBMP[levelgrid->sprite[x][y]]->h, levelgrid, PKG, widjet);
					}
				}

				if ((rest == 1) && (typeb != 3)) {
					backup = 1;
					if (levelgrid->MapEnemiDirection[ScanXX][ScanYY] != 1) {
						levelgrid->MapEnemiDirection[ScanXX][ScanYY] = 1; levelgrid->MapSpriteEnemi[ScanXX][ScanYY] = gauche; return 0;
					}
					else {
						levelgrid->MapEnemiDirection[ScanXX][ScanYY] = 0; levelgrid->MapSpriteEnemi[ScanXX][ScanYY] = droite; return 0;
					}
				}
			}
			//fin bypass
		}
	}// boucle for
   //////fin check collision enemi
	if (COUCHESPRITEACTIFBLOB(levelgrid->MapSpriteEnemi[ScanXX][ScanYY])) {}
	else {
		if (backup == 1) {}
		else {
			if (levelgrid->MapEnemiDirection[ScanXX][ScanYY] != 1) {
				if (levelgrid->MapEnemiOffSetX[ScanXX][ScanYY] < 32) {
					levelgrid->MapEnemiOffSetX[ScanXX][ScanYY] = levelgrid->MapEnemiOffSetX[ScanXX][ScanYY] + ENEMISTEPMOVE;
				}
				else {
					//transfere de la case a la suivante
					if ((levelgrid->MapSpriteEnemi[ScanXX + 1][ScanYY] == 16777215)) {
						levelgrid->MapEnemiDirection[ScanXX + 1][ScanYY] = levelgrid->MapEnemiDirection[ScanXX][ScanYY];
						levelgrid->MapEnemiOffSetX[ScanXX + 1][ScanYY] = 32 - levelgrid->MapEnemiOffSetX[ScanXX][ScanYY];
						levelgrid->MapSpriteEnemi[ScanXX + 1][ScanYY] = levelgrid->MapSpriteEnemi[ScanXX][ScanYY]; //transfere de case
						levelgrid->MapSpriteEnemiSimplifier[ScanXX + 1][ScanYY] = levelgrid->MapSpriteEnemiSimplifier[ScanXX][ScanYY]; //transfere de case
						//fin du transfere de case

						//remise a zero de la case plus utilisé
						levelgrid->MapEnemiDirection[ScanXX][ScanYY] = 1;
						levelgrid->MapEnemiOffSetX[ScanXX][ScanYY] = 0;//remise a zero du offset
						levelgrid->MapSpriteEnemi[ScanXX][ScanYY] = 16777215;//remise a zero de la case plus utilisé
						levelgrid->MapSpriteEnemiSimplifier[ScanXX][ScanYY] = 0; //transfere de case
						//fin de la remise a zero
						retour = 1;
					}
				}
			}
			else {
				if (levelgrid->MapEnemiOffSetX[ScanXX][ScanYY] > 0) {
					levelgrid->MapEnemiOffSetX[ScanXX][ScanYY] = levelgrid->MapEnemiOffSetX[ScanXX][ScanYY] - ENEMISTEPMOVE;
				}
				else {

					if ((levelgrid->MapSpriteEnemi[ScanXX - 1][ScanYY] == 16777215)) {
						//transfere de la case a la suivante
						levelgrid->MapEnemiDirection[ScanXX - 1][ScanYY] = levelgrid->MapEnemiDirection[ScanXX][ScanYY];
						levelgrid->MapEnemiOffSetX[ScanXX - 1][ScanYY] = 31;
						levelgrid->MapSpriteEnemi[ScanXX - 1][ScanYY] = levelgrid->MapSpriteEnemi[ScanXX][ScanYY]; //transfere de case
						levelgrid->MapSpriteEnemiSimplifier[ScanXX - 1][ScanYY] = levelgrid->MapSpriteEnemiSimplifier[ScanXX][ScanYY]; //transfere de case
						//fin du transfere de case

						//remise a zero de la case plus utilisé
						levelgrid->MapEnemiDirection[ScanXX][ScanYY] = 1;
						levelgrid->MapEnemiOffSetX[ScanXX][ScanYY] = 0;//remise a zero du offset
						levelgrid->MapSpriteEnemi[ScanXX][ScanYY] = 16777215;//remise a zero de la case plus utilisé
						levelgrid->MapSpriteEnemiSimplifier[ScanXX][ScanYY] = 0;
						//fin de la remise a zero
					}
				}
			}
		}//fin backup
	}//type3
	return retour;
}

void ResetLevel(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	PlaySND(PLAY, MUSIC1);
	ResetGridInit(levelgrid, PKG);
	EndurenceTrackbar = 4;
	levelgrid->NombreDeVie = levelgrid->NombreDeVie - 1;//live
	GreenScreen(levelgrid, PKG, widjet);
}
void MainBoot() {
	speedmax = 2; //normale 2
	speedmaxy = 4; //normale 4
	NumberofJoystick = 0;
	int x = 0;
	MainSpriteEnable = ON;
	for (x = 0; x < 19; x++) {
		sndmatrice[x] = STOP;
	}
	EndurenceTrackbar = 4;
}

int LoadConfig(config *cfg) {
	int t = 0;
	char suite[255] = "";
	char c[2] = "";
	if (FichierPresent("Config.ini")) {
		FILE *fichier;
		fichier = fopen("Config.ini", "r");
		//nombre d'élément de configuration
		for (t = 0; t < 11; t++) {
			while (c[0] != EOF) {
				c[0] = fgetc(fichier);
				if (c[0] == '\n') { break; }
				if (c[0] == '0' || c[0] == '1' || c[0] == '2' || c[0] == '3' || c[0] == '4' || c[0] == '5' || c[0] == '6' || c[0] == '7' || c[0] == '8' || c[0] == '9') {
					strcat(suite, &c[0]);
				}
			}
			if (t == 0) { cfg->BIT = atoi(suite); }
			if (t == 1) { cfg->SOFTWARERENDER = atoi(suite); }
			if (t == 2) { cfg->DOUBLEBUF = atoi(suite); }
			if (t == 3) { cfg->FULLSCREEN = atoi(suite); }
			if (t == 4) { cfg->BACKGROUND = atoi(suite); }
			if (t == 5) { cfg->BACK = atoi(suite); }
			if (t == 6) { cfg->FRONT = atoi(suite); }
			if (t == 7) { cfg->MusicConfig = atoi(suite); }
			if (t == 8) { cfg->FXConfig = atoi(suite); }
			strcpy(suite, "");
		}
		fclose(fichier);
		cfg->MODESANSECHEC = 0;
	}
	else {
		cfg->BIT = 8;
		cfg->SOFTWARERENDER = 1;
		cfg->DOUBLEBUF = 0;
		cfg->FULLSCREEN = 0;
		cfg->BACKGROUND = 0;
		cfg->BACK = 0;
		cfg->FRONT = 0;
		cfg->MODESANSECHEC = 1;
		cfg->FXConfig = 1;
		cfg->MusicConfig = 1;
	}
	return 0;
}

int RenduDeCouche(int Couche, LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
#define expl levelgrid->Explose
#define explose ((round((rand()%expl))))
	int retour = 0;
	float cameragrid = 0;
	float cameragridy = 0;
	float scrollx32 = 0;
	float scrolly32 = 0;
	if ((Couche == DEFAULT)) {
		cameragrid = levelgrid->Scrollxlevel;
		cameragridy = levelgrid->Scrollylevel;
		scrollx32 = levelgrid->Scrollx32;
		scrolly32 = levelgrid->Scrolly32;
	}
	SDL_Rect frect;
	int xx, yy;
	int MEMSPRITEENEMIGRILSIMPLIFIER = 0;
	FloatRect rect = { 0, };
	float x, y, xoffset = 0, yoffset = 0;
	uint32_t color = 0;
	uint32_t color2 = 0;
	int scrX32 = 0;
	int scrY32 = 0;
	float gridX32 = 0;
	float gridY32 = 0;
	//x
	if (cameragrid < 11) {
		xoffset = -cameragrid;
	}
	else { xoffset = -10; }
	//y
	if (cameragridy < 11) {
		yoffset = -cameragridy;
	}
	else { yoffset = -10; }
	scrX32 = (scrollx32); //memoire avant balayage
	scrY32 = (scrolly32); //memoire avant balayage
	gridX32 = cameragrid; //memoire avant balayage
	gridY32 = cameragridy; //memoire avant balayage
	for (y = yoffset; y < 16; y++) {
		for (x = xoffset; x < 21; x++) {
			xx = (x + gridX32);
			yy = (y + gridY32);
			rect.x = ((x * 32) + scrX32);
			rect.y = ((y * 32) + scrY32);
			if (xx >= levelgrid->width) { xx = levelgrid->width - 1; }
			if (yy >= levelgrid->height) { yy = levelgrid->height - 1; }
			color = levelgrid->sprite[xx][yy];
			color2 = levelgrid->MapSpriteEnemi[xx][yy];
			MEMSPRITEENEMIGRILSIMPLIFIER = levelgrid->MapSpriteEnemiSimplifier[xx][yy];
			if ((color != 16777215 && color < SpriteTotal)) {
				if ((color == 203) || (color == 204)) {
					if ((levelgrid->Switch == 0)) { color = 203; }
					if ((levelgrid->Switch == 1)) { color = 204; }
				}
				if ((levelgrid->sprite[xx][yy] > 79) && (levelgrid->sprite[xx][yy] < 96) && (levelgrid->ActualFrame % 11 == 0)) { if (levelgrid->sprite[xx][yy] + 1 < 94) { levelgrid->sprite[xx][yy] = levelgrid->sprite[xx][yy] + 1; } else { levelgrid->sprite[xx][yy] = 82; } }
				if ((levelgrid->sprite[xx][yy] > 165) && (levelgrid->sprite[xx][yy] < 172) && (levelgrid->ActualFrame % 8 == 0)) { if (levelgrid->sprite[xx][yy] + 1 < 172) { levelgrid->sprite[xx][yy] = levelgrid->sprite[xx][yy] + 1; } else { levelgrid->sprite[xx][yy] = 166; } }
				if ((levelgrid->sprite[xx][yy] > 171) && (levelgrid->sprite[xx][yy] < 178) && (levelgrid->ActualFrame % 8 == 0)) { if (levelgrid->sprite[xx][yy] + 1 < 178) { levelgrid->sprite[xx][yy] = levelgrid->sprite[xx][yy] + 1; } else { levelgrid->sprite[xx][yy] = 172; } }
				if ((levelgrid->sprite[xx][yy] >= 276) && (levelgrid->sprite[xx][yy] <= 279) && (levelgrid->ActualFrame % 12 == 0)) { if (levelgrid->sprite[xx][yy] + 1 < 279) { levelgrid->sprite[xx][yy] = levelgrid->sprite[xx][yy] + 1; } else { levelgrid->sprite[xx][yy] = 276; } }

				// fait vibré lécran pour l'explosion de la bombe
				if ((levelgrid->Explose > 0) && (levelgrid->Explose < 15)) {
					frect.x = rect.x + explose;
					if ((!TEMPORARYDESABLESPRITE)) { frect.y = rect.y + explose; }
					else { frect.y = rect.y + levelgrid->AnimSwitch + explose; }
					frect.w = rect.w;
					frect.h = rect.h;
				}
				else {
					frect.x = rect.x;
					if ((!TEMPORARYDESABLESPRITE)) { frect.y = rect.y; }
					else { frect.y = rect.y + levelgrid->AnimSwitch; }
					frect.w = rect.w;
					frect.h = rect.h;
				}
				if (color == 141) {
					if (levelgrid->CleTotaleAmasser == levelgrid->CleTotalInLevel) { color = 141 + 1 + levelgrid->DoorOpenCurentFrameAnim; }
				}
				//water anim
				if ((color > 204) && (color < 210)) {
					color = 205 + levelgrid->WaterAnim;
				}
				//fin water anim
				//anim lamp
				if (color == 309) { color = levelgrid->Lamp; }
				//fin anim lamp

				if ((NOPRINT) || (((levelgrid->AnimSwitch >= 30)) && (TEMPORARYDESABLESPRITE))) {}
				else {
					SDL_BlitSurface(PKG->spriteBMP[color], 0, widjet->render, &frect);
				}
			}
			if ((color2 != 16777215 && color2 < SpriteTotal)) {
				//controle de déplacement des sprite enemi
				if (levelgrid->ActualFrame % 3 == 0) {
					retour = MoveEnemi(xx, yy, levelgrid, PKG, widjet);
				}
				rect.x = ((x * 32) + (scrollx32)+(levelgrid->MapEnemiOffSetX[xx][yy]));
				frect.x = rect.x;
				frect.y = rect.y;
				frect.w = rect.w;
				frect.h = rect.h;
				//retour=1 ne pas inscrire le perssonage enemie (transfere de case) évite les doublon
				if ((retour != 1)) {
					SDL_BlitSurface(PKG->spriteBMP[color2 + StandardSpriteEnemiConvertion(ADD, MEMSPRITEENEMIGRILSIMPLIFIER, levelgrid, PKG)], 0, widjet->render, &frect);
				}
			}
		}
	}
	//inscription du personnage principal
	if (Couche == DEFAULT) {
		frect.x = ((levelgrid->mainspritegridx * 32) + (levelgrid->MainSpriteoffset32x)) - ((levelgrid->Scrollxlevel * 32) - (levelgrid->Scrollx32));
		frect.y = ((levelgrid->mainspritegridy * 32) + (levelgrid->MainSpriteoffset32y)) - ((levelgrid->Scrollylevel * 32) - (levelgrid->Scrolly32));
		levelgrid->MainSprite_render_pos_x = frect.x;
		levelgrid->MainSprite_render_pos_y = frect.y;
		frect.x = trunc((levelgrid->mainspritegridx * 32) + trunc(levelgrid->MainSpriteoffset32x)) - ((levelgrid->Scrollxlevel * 32) - trunc(levelgrid->Scrollx32));
		frect.y = trunc((levelgrid->mainspritegridy * 32) + trunc(levelgrid->MainSpriteoffset32y)) - ((levelgrid->Scrollylevel * 32) - trunc(levelgrid->Scrolly32));
		if ((MainSpriteEnable == ON) && (levelgrid->VisibleMainSprite == 1)) {
			if ((levelgrid->VitalInjuryTimer >= 0)) {

				if (WidjetTimer(100, &timer1)) {
					SDL_BlitSurface(PKG->spriteBMP[levelgrid->AnimSprite], 0, widjet->render, &frect);
				}
			}
			else {
				SDL_BlitSurface(PKG->spriteBMP[levelgrid->AnimSprite], 0, widjet->render, &frect);
			}
		}
	}
	return 0;
}

void AnimLamp(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	if (levelgrid->ActualFrame % 8 == 0) {
		levelgrid->Lamp = levelgrid->Lamp + 1;
		if (levelgrid->Lamp > 313) { levelgrid->Lamp = 309; }
	}
}

void ImagePKGInit(config *cfg, ImagePKG *PKG)
{	
	SDL_Surface** tspriteBMP = malloc(SpriteTotal * sizeof(SDL_Surface*));
	PKG->spriteBMP = (SDL_Surface**)malloc(SpriteTotal * sizeof(SDL_Surface*));//tableau SDL_Surface*
	PKG->level = (SDL_Surface**)malloc(LevelTotal * sizeof(SDL_Surface*)); //tableau SDL_Surface*
	//sprite BMP loading
	int t;
	int x = 0;
	int y = 0;
	char RED = 0;
	char file[255] = "";
	char inttochar[255] = "";
	for (t = 0; t < SpriteTotal; t++) {
		strcpy(file, "sprite/sprite");
		sprintf(inttochar, "%d", t);
		strcat(file, inttochar);
		strcat(file, ".bmp");
		tspriteBMP[t] = SDL_LoadBMP(file);
		for (y = 0; y < tspriteBMP[t]->h; y++) {
			for (x = 0; x < tspriteBMP[t]->w; x++) {
				if (getpixel(tspriteBMP[t], x, y) == WidjetRGB(255, 0, 0)) {
					RED = 1;
				}
			}
		}
		if (RED) {
			SDL_SetColorKey(tspriteBMP[t], SDL_SRCCOLORKEY, SDL_MapRGB(tspriteBMP[t]->format, 255, 0, 0));
			RED = 0;
		}
	}

	for (t = 0; t < LevelTotal; t++) {
		strcpy(file, "levels/level");
		sprintf(inttochar, "%d", t);	strcat(file, inttochar);
		strcat(file, ".bmp");
		PKG->level[t] = SDL_LoadBMP(file);
	}
	//fin sprite BMP loading

	//init level
	PKG->leveluse = 0;

	for (t = 0; t < SpriteTotal; t++) {
		PKG->spriteBMP[t] = SDL_DisplayFormat(tspriteBMP[t]);
		SDL_FreeSurface(tspriteBMP[t]);
	}
	if (cfg->BIT == 8) {}
	else {
		for (t = 205; t <= 212; t++) {
			SDL_SetAlpha(PKG->spriteBMP[t], SDL_SRCALPHA, 128);
		}
		int shade = 255;
		for (t = 195; t <= 198; t++) {
			SDL_SetAlpha(PKG->spriteBMP[t], SDL_SRCALPHA, shade);
			shade = shade - 65;
		}
		//appliquer la transparence
		SDL_SetAlpha(PKG->spriteBMP[281], SDL_SRCALPHA, 180);
		SDL_SetAlpha(PKG->spriteBMP[284], SDL_SRCALPHA, 220);
		SDL_SetAlpha(PKG->spriteBMP[99], SDL_SRCALPHA, 180);
		SDL_SetAlpha(PKG->spriteBMP[100], SDL_SRCALPHA, 180);
		SDL_SetAlpha(PKG->spriteBMP[138], SDL_SRCALPHA, 180);
	}

	free(tspriteBMP);
}

void ImagePKGFree(ImagePKG *PKG) {
	//sprite BMP free
	int t;
	for (t = 0; t < SpriteTotal; t++) {
		SDL_FreeSurface(PKG->spriteBMP[t]);
	}
	free(PKG->spriteBMP);
	for (t = 0; t < LevelTotal; t++) {
		SDL_FreeSurface(PKG->level[t]);
	}
	free(PKG->level);
}

void UpdateScroll(LevelGrid *levelgrid, Widjet *widjet) {
	float fx = (levelgrid->ScrollStep / 80);
	float fy = (levelgrid->ScrollStepy / 60);
	if ((fx > -.1) && (fx < .1)) { fx = 0; }
	if ((fy > -.1) && (fy < .1)) { fy = 0; }
	// axe x
	//game scroll
	if (levelgrid->ScrollStep > 0) {
		if (levelgrid->Scrollxlevel != levelgrid->width - 20) {
			//back scroll
			if ((levelgrid->Scrollx256 - (fx / 2)) < -204) { levelgrid->Scrollx256 = 0; }
			else
			{
				levelgrid->Scrollx256 = levelgrid->Scrollx256 - (fx / 2);
			}
			//backscroll end

			//front scroll
			if ((levelgrid->Scrollxfront256 - (fx * 2)) < -320) { levelgrid->Scrollxfront256 = 0; }
			else
			{
				levelgrid->Scrollxfront256 = levelgrid->Scrollxfront256 - (fx * 2);
			}
			//front scroll end

			//Background scroll
			if (levelgrid->backgroudScoll == 1) {
				if ((levelgrid->ScrollxBackGround - (fx / 4)) < -640) { levelgrid->ScrollxBackGround = 0; }
				else
				{
					levelgrid->ScrollxBackGround = levelgrid->ScrollxBackGround - (fx / 4);
				}
			}
			//Background scroll end

			levelgrid->Scrollx32 = levelgrid->Scrollx32 - fx;
		}
		if ((levelgrid->Scrollx32) < -31) { levelgrid->Scrollx32 = 0; levelgrid->Scrollxlevel = levelgrid->Scrollxlevel + 1; }
	}
	if (levelgrid->ScrollStep < 0) {
		if ((levelgrid->Scrollxlevel == 0) && (levelgrid->Scrollx32 >= 0)) {}
		else {

			//back scroll
			if ((levelgrid->Scrollx256 - (fx / 2)) >= 0) { levelgrid->Scrollx256 = -204; }
			else
			{
				levelgrid->Scrollx256 = levelgrid->Scrollx256 - (fx / 2);
			}
			//back scroll end

			//front scroll
			if ((levelgrid->Scrollxfront256 - (fx * 2)) >= 0) { levelgrid->Scrollxfront256 = -320; }
			else
			{
				levelgrid->Scrollxfront256 = levelgrid->Scrollxfront256 - (fx * 2);
			}
			//front scroll end

			//Background scroll
			if (levelgrid->backgroudScoll == 1) {
				if ((levelgrid->ScrollxBackGround - (fx / 4)) >= 0) { levelgrid->ScrollxBackGround = -640; }
				else
				{
					levelgrid->ScrollxBackGround = levelgrid->ScrollxBackGround - (fx / 4);
				}
			}

			//Background scroll end
			levelgrid->Scrollx32 = levelgrid->Scrollx32 - fx;
		}
		if ((levelgrid->Scrollx32) > 0) { if (levelgrid->Scrollxlevel > 0) { levelgrid->Scrollx32 = -31; levelgrid->Scrollxlevel = levelgrid->Scrollxlevel - 1; } }
	}

	//axe y
	//game scroll
	if (levelgrid->ScrollStepy > 0) {
		if (levelgrid->Scrollylevel != levelgrid->height - 15) {

			//y backgroud scroll
			if (levelgrid->backgroudScoll == 0) {
				if ((levelgrid->ScrollyBackGround - (fy / 4)) < -480) { levelgrid->ScrollyBackGround = 0; }
				else
				{
					levelgrid->ScrollyBackGround = levelgrid->ScrollyBackGround - (fy / 4);
				}
			}

			levelgrid->Scrolly32 = levelgrid->Scrolly32 - fy;
		}
		if ((levelgrid->Scrolly32) < -31) { levelgrid->Scrolly32 = 0; levelgrid->Scrollylevel = levelgrid->Scrollylevel + 1; }
	}
	if (levelgrid->ScrollStepy < 0) {
		if ((levelgrid->Scrollylevel == 0) && (levelgrid->Scrolly32 >= 0)) {}
		else {

			//y background scrool
			if (levelgrid->backgroudScoll == 0) {
				if ((levelgrid->ScrollyBackGround - (fy / 4)) >= 0) { levelgrid->ScrollyBackGround = -480; }
				else
				{
					levelgrid->ScrollyBackGround = levelgrid->ScrollyBackGround - (fy / 4);
				}
			}
			levelgrid->Scrolly32 = levelgrid->Scrolly32 - fy;
		}
		if ((levelgrid->Scrolly32) > 0) { if (levelgrid->Scrollylevel > 0) { levelgrid->Scrolly32 = -31; levelgrid->Scrollylevel = levelgrid->Scrollylevel - 1; } }
	}
}

void ResetGridInit(LevelGrid *levelgrid, ImagePKG *PKG) {
	int a = 0, b = 0;
	uint32_t getpix = 0;
	int x = 0;
	int y = 0;
	x = levelgrid->MainSpriteOrigineX;
	y = levelgrid->MainSpriteOriginey;
	levelgrid->mainspritegridx = x;
	levelgrid->mainspritegridy = y;
	levelgrid->ScrollStep = 0;
	levelgrid->ScrollStepy = 0;
	levelgrid->Scrollx32 = 0;
	levelgrid->Scrolly32 = 0;

	//centrer sur le perssonnage principal
	if (levelgrid->MainSpriteOrigineX >= levelgrid->width / 2) {
		levelgrid->Scrollxlevel = 0;
	}
	else {
		if (levelgrid->width - 20 >= 0) { levelgrid->Scrollxlevel = levelgrid->width - 20; }
		else { levelgrid->Scrollxlevel = 0; }
	}if (levelgrid->MainSpriteOriginey >= levelgrid->height / 2) {
		levelgrid->Scrollylevel = 0;
	}
	else { if (levelgrid->height - 15 >= 0) { levelgrid->Scrollylevel = levelgrid->height - 15; } else { levelgrid->Scrollylevel = 0; } }

	//réinitialisation des variables du level
	levelgrid->DoubleJumpCancel = 0;
	levelgrid->renderx = 0;
	levelgrid->rendery = 0;
	levelgrid->MainSpriteoffset32x = 0;
	levelgrid->MainSpriteoffset32y = 0;
	levelgrid->MainSprite_render_pos_x = 0;
	levelgrid->MainSprite_render_pos_y = 0;
	levelgrid->mainSpritejump = 0;
	levelgrid->mainSpritejumpcancel = 0;
	levelgrid->framecount = 0;
	levelgrid->AnimSprite = 22;
	levelgrid->AnimFrameRate = 0;
	levelgrid->animjumpcount = 0;
	levelgrid->speedaxex = 0;
	levelgrid->speedaxey = 0;
	levelgrid->ScrollxBackGround = 0;
	levelgrid->ScrollyBackGround = 0;
	levelgrid->VisibleMainSprite = 1;
	levelgrid->Explose = 0; // 15 frame valeur 15 déclenche l'Animation a l'endrois choisi
	levelgrid->bombeXgrid = 0;
	levelgrid->bombeYgrid = 0;
	levelgrid->ActualFrame = 0; // de 0 a 120 frame
	levelgrid->DoorOpenCurentFrameAnim = 0; //animation open door frame locked 0 a 25
	levelgrid->DoorOpen = 0; // open door

	//fait réaparaitre les bomb apres la perte d'une vie
	bomberCount = 0;
	SDL_LockSurface(PKG->level[PKG->leveluse]);
	for (b = 0; b < levelgrid->height; b++) {
		for (a = 0; a < levelgrid->width; a++) {
			getpix = getpixel(PKG->level[PKG->leveluse], a, b);
			if (getpix == 110) { levelgrid->sprite[a][b] = 110; }
			if (getpix == 290) { levelgrid->sprite[a][b] = 290; }
			if (getpix == 0) { levelgrid->SpriteSimplifer[a][b] = 1; levelgrid->sprite[a][b] = 0; }
			if ((levelgrid->sprite[a][b] >= 276) && (levelgrid->sprite[a][b] <= 279)) { levelgrid->sprite[a][b] = 16777215; }
		}
	}
}

void LevelGridInit(LevelGrid *levelgrid, ImagePKG *PKG) {
	int x = 0, y = 0;
	levelgrid->CleTotalInLevel = 0;
	PartielLevelGridFree(levelgrid);
	levelgrid->width = PKG->level[PKG->leveluse]->w;
	levelgrid->height = PKG->level[PKG->leveluse]->h;
	ParticularityOfLevel(levelgrid, PKG);
	//allocation dinamique du level
	levelgrid->sprite = malloc((levelgrid->width) * sizeof(uint32_t*));
	levelgrid->SpriteSimplifer = malloc((levelgrid->width) * sizeof(uint32_t*));
	//grille de sprite enemi couche2
	levelgrid->MapEnemiOffSetX = malloc((levelgrid->width) * sizeof(uint32_t*));
	levelgrid->MapEnemiDirection = malloc((levelgrid->width) * sizeof(uint32_t*));
	levelgrid->MapSpriteEnemi = malloc((levelgrid->width) * sizeof(uint32_t*));
	levelgrid->MapSpriteEnemiSimplifier = malloc((levelgrid->width) * sizeof(uint32_t*));
	levelgrid->MapEnemiOffSetY = malloc((levelgrid->width) * sizeof(uint32_t*));
	levelgrid->Decalage32Y = malloc((levelgrid->width) * sizeof(uint32_t*));
	levelgrid->Decalage32x = malloc((levelgrid->width) * sizeof(uint32_t*));
	//allocation dinamique 2eme dimention
	for (x = 0; x < levelgrid->width; x++) {
		levelgrid->sprite[x] = calloc((levelgrid->height), sizeof(uint32_t));
		levelgrid->SpriteSimplifer[x] = calloc((levelgrid->height), sizeof(uint32_t));
		//grille de sprite active
		levelgrid->MapEnemiOffSetX[x] = calloc((levelgrid->height), sizeof(uint32_t));
		levelgrid->MapEnemiDirection[x] = calloc((levelgrid->height), sizeof(uint32_t));
		levelgrid->MapSpriteEnemi[x] = calloc((levelgrid->height), sizeof(uint32_t));
		levelgrid->MapSpriteEnemiSimplifier[x] = calloc((levelgrid->height), sizeof(uint32_t));
		levelgrid->MapEnemiOffSetY[x] = calloc((levelgrid->height), sizeof(uint32_t));
		levelgrid->Decalage32Y[x] = calloc((levelgrid->height), sizeof(uint32_t));
		levelgrid->Decalage32x[x] = calloc((levelgrid->height), sizeof(uint32_t));
	}
	//init levelgrid
	int gridx = 0;
	int gridy = 0;
	for (y = 0; y < levelgrid->height; y++) {
		for (x = 0; x < levelgrid->width; x++) {
			levelgrid->sprite[x][y] = 16777215;
			levelgrid->SpriteSimplifer[x][y] = 0;
			//grille de sprite active
			levelgrid->MapEnemiOffSetX[x][y] = 0;
			levelgrid->MapEnemiOffSetY[x][y] = 0;
			levelgrid->MapEnemiDirection[x][y] = 0;
			levelgrid->MapSpriteEnemi[x][y] = 16777215;
			levelgrid->MapSpriteEnemiSimplifier[x][y] = 0;
			levelgrid->Decalage32x[x][y] = 0;
			levelgrid->Decalage32Y[x][y] = 0;
		}
	}
	for (y = 0; y < levelgrid->height; y++) {
		for (x = 0; x < levelgrid->width; x++) {
			//check
			if (getpixel(PKG->level[PKG->leveluse], x, y) < SpriteTotal) {
				levelgrid->sprite[x][y] = getpixel(PKG->level[PKG->leveluse], x, y);
			}
			//key count
			if ((levelgrid->sprite[x][y] > 79) && (levelgrid->sprite[x][y] < 96)) { levelgrid->CleTotalInLevel = levelgrid->CleTotalInLevel + 1; }//cle total dans le tableau
			//key count end
			if (MAINSPRITE) {
				levelgrid->mainspritegridx = x;
				levelgrid->mainspritegridy = y;
				levelgrid->MainSpriteOrigineX = x;
				levelgrid->MainSpriteOriginey = y;
				//centrer sur le perssonnage principal
				if (levelgrid->MainSpriteOrigineX >= levelgrid->width / 2) {
					levelgrid->Scrollxlevel = 0;
				}
				else {
					if (levelgrid->width - 20 >= 0) { levelgrid->Scrollxlevel = levelgrid->width - 20; }
					else { levelgrid->Scrollxlevel = 0; }
				}if (levelgrid->MainSpriteOriginey >= levelgrid->height / 2) {
					levelgrid->Scrollylevel = 0;
				}
				else { if (levelgrid->height - 15 >= 0) { levelgrid->Scrollylevel = levelgrid->height - 15; } else { levelgrid->Scrollylevel = 0; } }
				levelgrid->sprite[x][y] = 16777215;
				levelgrid->MapSpriteEnemi[x][y] = 16777215;
			}
			else if (COUCHESPRITEACTIF(levelgrid->sprite[x][y] - StandardSpriteEnemiConvertion(DIFERENCE, levelgrid->sprite[x][y], levelgrid, PKG))) {
				levelgrid->MapSpriteEnemi[x][y] = levelgrid->sprite[x][y] - StandardSpriteEnemiConvertion(DIFERENCE, levelgrid->sprite[x][y], levelgrid, PKG);
				levelgrid->MapSpriteEnemiSimplifier[x][y] = StandardSpriteEnemiConvertion(TYPE, levelgrid->sprite[x][y], levelgrid, PKG);
				levelgrid->sprite[x][y] = 16777215;
				if ((levelgrid->MapSpriteEnemi[x][y] >= 356) && (levelgrid->MapSpriteEnemi[x][y] <= 384)) {
					levelgrid->MapEnemiDirection[x][y] = 1;
				}
				if ((levelgrid->MapSpriteEnemi[x][y] >= 385) && (levelgrid->MapSpriteEnemi[x][y] <= 413)) {
					levelgrid->MapEnemiDirection[x][y] = 0;
				}
			}
			else if (COUCHESPRITEACTIFBLOB(levelgrid->sprite[x][y])) {
				levelgrid->MapSpriteEnemi[x][y] = 255;
				levelgrid->sprite[x][y] = 16777215;
				levelgrid->MapEnemiDirection[x][y] = 1;
			}
			else if ((levelgrid->sprite[x][y] >= 309) && (levelgrid->sprite[x][y] <= 313)) {
				levelgrid->sprite[x][y] = 309;//lamp
			}
			else if (SPRITEEXCLUS) {
				levelgrid->sprite[x][y] = 16777215;
			}
			else if ((SOLIDSPRITE) || (HOLETOUTCH)) {
				levelgrid->SpriteSimplifer[x][y] = 1;
			}
		}
	}
	levelgrid->DoubleJumpCancel = 0;
	levelgrid->ScrollxBackGround = 0;
	levelgrid->ScrollyBackGround = 0;
	levelgrid->VisibleMainSprite = 1;
	levelgrid->Lamp = 309;
	bomberCount = 0; //nombre de bombe par tableau
	levelgrid->Explose = 0; // 15 frame valeur 15 déclenche l'Animation a l'endrois choisi
	levelgrid->bombeXgrid = 0;
	levelgrid->bombeYgrid = 0;
	levelgrid->CleTotaleAmasser = 0; //cle ramaser
	levelgrid->ActualFrame = 0; // de 0 a 120 frame
	levelgrid->DoorOpenCurentFrameAnim = 0; //animation open door frame locked 0 a 25
	levelgrid->DoorOpen = 0; // open door
	levelgrid->ScrollStep = 0;
	levelgrid->ScrollStepy = 0;
	levelgrid->Scrollx32 = 0;
	levelgrid->Scrolly32 = 0;
	levelgrid->renderx = 0;
	levelgrid->rendery = 0;
	levelgrid->MainSpriteoffset32x = 0;
	levelgrid->MainSpriteoffset32y = 0;
	levelgrid->MainSprite_render_pos_x = 0;
	levelgrid->MainSprite_render_pos_y = 0;
	levelgrid->mainSpritejump = 0;
	levelgrid->mainSpritejumpcancel = 0;
	levelgrid->framecount = 0;
	levelgrid->AnimSprite = 22;
	levelgrid->AnimFrameRate = 0;
	levelgrid->animjumpcount = 0;
	levelgrid->speedaxex = 0;
	levelgrid->speedaxey = 0;
}

void PartielLevelGridFree(LevelGrid *levelgrid) {
	int y;
	for (y = 0; y < levelgrid->width; y++) {
		free(levelgrid->sprite[y]);
		free(levelgrid->SpriteSimplifer[y]);
		free(levelgrid->MapEnemiOffSetX[y]);//use
		free(levelgrid->MapEnemiDirection[y]);
		free(levelgrid->MapSpriteEnemi[y]);
		free(levelgrid->MapSpriteEnemiSimplifier[y]);
		free(levelgrid->MapEnemiOffSetY[y]);
		free(levelgrid->Decalage32x[y]);
		free(levelgrid->Decalage32Y[y]);
	}

	free(levelgrid->sprite);
	free(levelgrid->SpriteSimplifer);
	free(levelgrid->MapEnemiOffSetX);//use
	free(levelgrid->MapEnemiDirection);
	free(levelgrid->MapSpriteEnemi);
	free(levelgrid->MapSpriteEnemiSimplifier);
	free(levelgrid->MapEnemiOffSetY);
	free(levelgrid->Decalage32x);
	free(levelgrid->Decalage32Y);

}

void LevelGridFree(LevelGrid *levelgrid) {
	int y;

	//tout détruire les variable alouer dinamiquement
	for (y = 0; y < levelgrid->width; y++) {
		free(levelgrid->sprite[y]);
		free(levelgrid->SpriteSimplifer[y]);
		free(levelgrid->MapEnemiOffSetX[y]);
		free(levelgrid->MapEnemiDirection[y]);
		free(levelgrid->MapSpriteEnemi[y]);
		free(levelgrid->MapSpriteEnemiSimplifier[y]);
		free(levelgrid->MapEnemiOffSetY[y]);
		free(levelgrid->Decalage32x[y]);
		free(levelgrid->Decalage32Y[y]);
	}

	free(levelgrid->sprite);
	free(levelgrid->SpriteSimplifer);
	free(levelgrid->MapEnemiOffSetX);
	free(levelgrid->MapEnemiDirection);
	free(levelgrid->MapSpriteEnemi);
	free(levelgrid->MapSpriteEnemiSimplifier);
	free(levelgrid->MapEnemiOffSetY);
	free(levelgrid->Decalage32x);
	free(levelgrid->Decalage32Y);


}

void RenduDeFond(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	if (levelgrid->cloud != -1) {
		int x = 0;
		int tempx = 0;
		tempx = (levelgrid->Scrollx256);
		SDL_Rect rect;
		rect.y = 0;
		for (x = 0; x < 5; x++) {
			rect.x = x * 204 + tempx;
			SDL_BlitSurface(PKG->spriteBMP[levelgrid->cloud], 0, widjet->render, &rect);
		}
	}
}

void RenduDeFront(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	int x = 0;
	int tempx = 0;
	SDL_Rect rect;
	tempx = levelgrid->Scrollxfront256;
	for (x = 0; x < 3; x++) {
		rect.x = x * 320 + tempx;
#define ymainpos ((levelgrid->Scrollylevel*32)-levelgrid->Scrolly32)
#define grilletotaley (levelgrid->height*32)

		if (levelgrid->Scrollylevel > levelgrid->height - 18) {
			rect.y = (480 - (2 * (ymainpos - (grilletotaley - (19 * 32))))) + 160;
			SDL_BlitSurface(PKG->spriteBMP[levelgrid->FrontPic], 0, widjet->render, &rect);
		}
	}
}

void RenduDeBackGround(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	int x = 0;
	int y = 0;
	int tempx = 0;
	int tempy = 0;
	SDL_Rect rect;
	tempx = levelgrid->ScrollxBackGround;
	tempy = levelgrid->ScrollyBackGround;
	if (levelgrid->backgroudScoll == 1) {
		for (x = 0; x < 2; x++) {
			rect.x = x * 640 + tempx;
			rect.y = 0;
			SDL_BlitSurface(PKG->spriteBMP[levelgrid->background], 0, widjet->render, &rect);
		}
	}
	else {
		for (y = 0; y < 2; y++) {
			rect.x = 0;
			rect.y = y * 480 + tempy;
			SDL_BlitSurface(PKG->spriteBMP[levelgrid->background], 0, widjet->render, &rect);
		}
	}
}

void ControlUpdate(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
#define retromove 2
#define testmovex(movex) levelgrid->MainSpriteoffset32y=	levelgrid->MainSpriteoffset32y-movex;\
												if (	levelgrid->MainSpriteoffset32y<0){\
												levelgrid->MainSpriteoffset32y=levelgrid->MainSpriteoffset32y+32;\
												levelgrid->mainspritegridy=levelgrid->mainspritegridy-1;}

#define cancelmovex(cancelx) levelgrid->MainSpriteoffset32y=	levelgrid->MainSpriteoffset32y+cancelx;\
												if (	levelgrid->MainSpriteoffset32y>31){\
												levelgrid->MainSpriteoffset32y=levelgrid->MainSpriteoffset32y-32;\
												levelgrid->mainspritegridy=levelgrid->mainspritegridy+1;}

#define speedstep .08// .08
#define speedoffset .1
#define speedstepy .09//.09
#define speedoffsety .1
#define resetspeedx levelgrid->speedaxex=0
#define resetspeedy if  (levelgrid->speedaxey<0){levelgrid->mainSpritejumpcancel=1;}else{levelgrid->mainSpritejumpcancel=0;}levelgrid->speedaxey=0;
	int mem1, mem2;

	if (widjet->Event.JoyButton0 == 1)
	{
		if ((bomberCount > 0) && (levelgrid->Explose <= 0) && (levelgrid->sprite[levelgrid->bombeXgrid][levelgrid->bombeYgrid] == 16777215)) { levelgrid->Explose = 56; if ((rand() % 2) == 1) { PlaySND(PLAY, BADABOUM); }bomberCount = bomberCount - 1; }
		widjet->Event.JoyButton0 = 2;
	}

	//horizontal controle left right
	if ((widjet->Event.JoyHatL == 1)) { levelgrid->framecount2 = 0; levelgrid->DirectionMainSprite = gauche; if (levelgrid->speedaxex != -speedmax) { levelgrid->speedaxex = levelgrid->speedaxex - speedstep; } }
	else if ((widjet->Event.JoyHatR == 1)) { levelgrid->framecount2 = 0; levelgrid->DirectionMainSprite = droite; if (levelgrid->speedaxex != speedmax) { levelgrid->speedaxex = levelgrid->speedaxex + speedstep; } }
	else
	{
		if (levelgrid->speedaxex > speedoffset) { levelgrid->DirectionMainSprite = stopdroite; levelgrid->speedaxex = levelgrid->speedaxex - speedstep / 2; }
		else if (levelgrid->speedaxex < -speedoffset) { levelgrid->DirectionMainSprite = stopgauche; levelgrid->speedaxex = levelgrid->speedaxex + speedstep / 2; }
	}
	//horizontal controle left right

	if (widjet->Event.JoyButton1 == 0) { levelgrid->animjumpcount = 0; if ((levelgrid->speedaxey > -.01) && (levelgrid->speedaxey < .01)) { levelgrid->mainSpritejump = 0; }if (levelgrid->speedaxey < 0) { levelgrid->speedaxey = levelgrid->speedaxey + speedstepy * 2; }sndtrig = 0; }
	if (((widjet->Event.JoyButton1 == 1) && (levelgrid->mainSpritejumpcancel == 0)) && ((levelgrid->speedaxey > -speedoffsety) && (levelgrid->speedaxey < speedoffsety))) { levelgrid->framecount2 = 0; if (levelgrid->mainSpritejump == 0) { levelgrid->speedaxey = -speedmaxy; levelgrid->mainSpritejump = 1; if (sndtrig != 2) { sndtrig = 1; } } }
	if ((levelgrid->mainSpritejump == 0) || (levelgrid->mainSpritejumpcancel == 1)) {
		if (levelgrid->speedaxey < speedmaxy) { levelgrid->speedaxey = levelgrid->speedaxey + speedstepy; }
	}
	else {
		if (levelgrid->speedaxey < 0) {
			levelgrid->speedaxey = levelgrid->speedaxey + speedstepy / 1.5;
		}
		else {
			levelgrid->mainSpritejumpcancel = 1;
		}
	}

	//speed limiter 
	if (levelgrid->speedaxex < -speedmax) { levelgrid->speedaxex = -speedmax; }
	if (levelgrid->speedaxex > speedmax) { levelgrid->speedaxex = speedmax; }
	if (levelgrid->speedaxey < -speedmaxy) { levelgrid->speedaxey = -speedmaxy; }
	if (levelgrid->speedaxey > speedmaxy) { levelgrid->speedaxey = speedmaxy; }
	//speed limiter

	//reactive move

	//axe x right
	if (levelgrid->speedaxex > speedoffset) {
		mem1 = (levelgrid->MainSpriteoffset32x);
		mem2 = levelgrid->mainspritegridx;
		levelgrid->MainSpriteoffset32x = levelgrid->MainSpriteoffset32x + levelgrid->speedaxex;

		if (levelgrid->MainSpriteoffset32x > 31) {
			levelgrid->MainSpriteoffset32x = levelgrid->MainSpriteoffset32x - 32;
			levelgrid->mainspritegridx = levelgrid->mainspritegridx + 1;
		}

		if (colisiontestmaster(levelgrid, PKG, widjet)) {
			//test bute
			testmovex(retromove)

				if (colisiontestmaster(levelgrid, PKG, widjet)) {
					cancelmovex(retromove)
						levelgrid->MainSpriteoffset32x = (mem1);
					levelgrid->mainspritegridx = (mem2);
					resetspeedx;
					if (levelgrid->DirectionMainSprite == gauche) { levelgrid->DirectionMainSprite = stopgauche; }
					if (levelgrid->DirectionMainSprite == droite) { levelgrid->DirectionMainSprite = stopdroite; }
				}
		}
	}
	//axe x left
	if (levelgrid->speedaxex < -speedoffset) {
		mem1 = (levelgrid->MainSpriteoffset32x);
		mem2 = levelgrid->mainspritegridx;
		levelgrid->MainSpriteoffset32x = levelgrid->MainSpriteoffset32x + levelgrid->speedaxex;
		if (trunc(levelgrid->MainSpriteoffset32x) < 0) {
			levelgrid->MainSpriteoffset32x = levelgrid->MainSpriteoffset32x + 32;
			levelgrid->mainspritegridx = levelgrid->mainspritegridx - 1;
		}

		if (colisiontestmaster(levelgrid, PKG, widjet)) {
			//test bute
			testmovex(retromove)
				if (colisiontestmaster(levelgrid, PKG, widjet)) {
					cancelmovex(retromove)
						levelgrid->MainSpriteoffset32x = (mem1);
					levelgrid->mainspritegridx = (mem2);
					resetspeedx;
					if (levelgrid->DirectionMainSprite == gauche) { levelgrid->DirectionMainSprite = stopgauche; }
					if (levelgrid->DirectionMainSprite == droite) { levelgrid->DirectionMainSprite = stopdroite; }
				}
			//fin test
		}
	}
	//axe y UP
	if (levelgrid->speedaxey < -speedoffsety) {
		mem1 = levelgrid->MainSpriteoffset32y;
		mem2 = levelgrid->mainspritegridy;
		levelgrid->MainSpriteoffset32y = levelgrid->MainSpriteoffset32y + levelgrid->speedaxey;
		if (trunc(levelgrid->MainSpriteoffset32y) < 0) {
			levelgrid->MainSpriteoffset32y = levelgrid->MainSpriteoffset32y + 32;
			levelgrid->mainspritegridy = levelgrid->mainspritegridy - 1;
		}
		if (colisiontestmaster(levelgrid, PKG, widjet)) {
			PlaySND(PLAY, BUMP);
			levelgrid->MainSpriteoffset32y = (mem1);
			levelgrid->mainspritegridy = (mem2);
			resetspeedy;
		}
	}
	//axe y DOWN
	if (levelgrid->speedaxey > speedoffsety) {
		mem1 = levelgrid->MainSpriteoffset32y;
		mem2 = levelgrid->mainspritegridy;
		levelgrid->MainSpriteoffset32y = levelgrid->MainSpriteoffset32y + levelgrid->speedaxey;
		if (levelgrid->MainSpriteoffset32y > 31) {
			levelgrid->MainSpriteoffset32y = levelgrid->MainSpriteoffset32y - 32;
			levelgrid->mainspritegridy = levelgrid->mainspritegridy + 1;
		}
		if (colisiontestmaster(levelgrid, PKG, widjet)) {
			levelgrid->MainSpriteoffset32y = (mem1);
			levelgrid->mainspritegridy = (mem2);
			resetspeedy;
		}
	}

	//controle de l animation du mainsprite
	MainSpriteAnim(levelgrid, widjet);
	//controle de l animation du mainsprite

	levelgrid->ScrollStep = (levelgrid->MainSprite_render_pos_x - 304)*RETROACTIVITYSCROLLING;
	levelgrid->ScrollStepy = (levelgrid->MainSprite_render_pos_y - 240)*(RETROACTIVITYSCROLLING / 2);
}

void ReafectSprite(int gridpositionx, int gridpositiony, uint32_t sprite, int Decalage32x, int Decalage32Y, LevelGrid *levelgrid) {
	levelgrid->sprite[gridpositionx][gridpositiony] = sprite;
	levelgrid->SpriteSimplifer[gridpositionx][gridpositiony] = sprite;
	levelgrid->Decalage32x[gridpositionx][gridpositiony] = Decalage32x;
	levelgrid->Decalage32Y[gridpositionx][gridpositiony] = Decalage32Y;
}

unsigned char colisiontestmaster(LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	unsigned char result = 0;
	int cx = 0, cy = 0;
	int x = 0, y = 0, ax = 0, bx = 0, ay = 0, by = 0;
#define mspo32x (trunc(levelgrid->MainSpriteoffset32x))
#define mspo32y (trunc(levelgrid->MainSpriteoffset32y))
	ax = COLLISIONBALAYAGE; bx = 1; ay = COLLISIONBALAYAGE; by = 1;

	//main sprite
#define mx ((levelgrid->mainspritegridx*32)+mspo32x)-((levelgrid->Scrollxlevel*32)-(trunc(levelgrid->Scrollx32)))
#define my ((levelgrid->mainspritegridy*32)+mspo32y)-((levelgrid->Scrollylevel*32)-(trunc(levelgrid->Scrolly32)))
#define mxo (0)
#define myo (0)
#define mw (PKG->spriteBMP[18]->w)
#define mh (PKG->spriteBMP[18]->h)

//position main sprite
	int gridx = (levelgrid->mainspritegridx);
	int gridy = (levelgrid->mainspritegridy);

#define sx(xx,yy) (xx*32)+(((((gridx+xx))*32)+(levelgrid->Decalage32x[gridx+xx][gridy+yy]))-(((levelgrid->Scrollxlevel+xx)*32)-(trunc(levelgrid->Scrollx32))))
#define sy(xx,yy) (yy*32)+(((((gridy+yy))*32)+(levelgrid->Decalage32Y[gridx+xx][gridy+yy]))-(((levelgrid->Scrollylevel+yy)*32)-(trunc(levelgrid->Scrolly32))))

#define sw(xx,yy)  PKG->spriteBMP[levelgrid->sprite[gridx+x][gridy+y]]->w
#define sh(xx,yy) PKG->spriteBMP[levelgrid->sprite[gridx+x][gridy+y]]->h

	//couche2 enemi
#define sw2(xx,yy)  PKG->spriteBMP[levelgrid->MapSpriteEnemi[gridx+x][gridy+y]]->w
#define sh2(xx,yy) PKG->spriteBMP[levelgrid->MapSpriteEnemi[gridx+x][gridy+y]]->h

//water detect
	levelgrid->WaterSlowMove = 0;

	for (cy = ay; cy < by + 1; cy++) {
		for (cx = ax; cx < bx + 1; cx++) {
			x = cx;
			y = cy;
			if (gridx + x < 0) { while (gridx + x < 0) { x++; } }
			if (gridy + y < 0) { while (gridy + y < 0) { y++; } }
			if (gridx + x > levelgrid->width - 1) { while (gridx + x > levelgrid->width - 1) { x--; } }
			if (gridy + y > levelgrid->height - 1) { while (gridy + y > levelgrid->height - 1) { y--; } }

			if ((TOUTCHWATERNOANIM) ||
				(TOUTCHWATER) ||
				(SOLIDSPRITEINJURY) ||
				(SOLIDSPRITEINJURYFIX) ||
				(SOLIDSPRITEPICKUPBOMBER) ||
				(SOLIDSPRITEPICKUPALLKEY) ||
				(SOLIDSPRITEPICKUP100BOMB) ||
				(levelgrid->SpriteSimplifer[gridx + x][gridy + y] == 1) ||
				SOLIDSPRITEPICKUP ||
				SOLIDSPRITEPICKUPENDURENCE ||
				SOLIDSPRITEPICKUPLIVE ||
				TOUTCHSPRITE ||
				TOUTCHSWITCH ||
				TOUTCHSWITCHRESET ||
				(HOLETOUTCH)) {

				if (colisiondetect(levelgrid->sprite[gridx + x][gridy + y], mx + mxo, my + myo, mw, mh, sx(x, y), sy(x, y), sw(x, y), sh(x, y), levelgrid, PKG, widjet)) {

					//rafinement final de detection
					if ((SOLIDSPRITEPICKUPBOMBER) ||
						(SOLIDSPRITEPICKUPALLKEY) ||
						(SOLIDSPRITEPICKUP100BOMB) ||
						(SOLIDSPRITEPICKUP) ||
						(SOLIDSPRITEPICKUPENDURENCE) ||
						(SOLIDSPRITEPICKUPLIVE) ||
						(SOLIDSPRITEINJURY) ||
						(SOLIDSPRITEINJURYFIX)) {
						if ((SOLIDSPRITEINJURY) && (levelgrid->AnimSwitch <= 29)) { if (levelgrid->VitalInjuryTimer <= 0) { VitalInjury(levelgrid, widjet); } }
						if ((SOLIDSPRITEINJURYFIX)) { if (levelgrid->VitalInjuryTimer <= 0) { VitalInjury(levelgrid, widjet); } }
						if (SOLIDSPRITEPICKUP) { if (levelgrid->CleTotaleAmasser < levelgrid->CleTotalInLevel) { levelgrid->CleTotaleAmasser = levelgrid->CleTotaleAmasser + 1; }PlaySND(PLAY, KEY); }
						if (SOLIDSPRITEPICKUPBOMBER) { PlaySND(PLAY, COIN); bomberCount = bomberCount + 1; }
						if (SOLIDSPRITEPICKUPALLKEY) { PlaySND(PLAY, COIN); levelgrid->CleTotaleAmasser = levelgrid->CleTotalInLevel; }
						if (SOLIDSPRITEPICKUP100BOMB) { PlaySND(PLAY, COIN); bomberCount = bomberCount + 100; }
						if (SOLIDSPRITEPICKUPENDURENCE) { EndurenceTrackbar = 4; PlaySND(PLAY, COIN); }
						if (SOLIDSPRITEPICKUPLIVE) { levelgrid->NombreDeVie = levelgrid->NombreDeVie + 1; PlaySND(PLAY, COIN); }
						if (!(SOLIDSPRITEINJURY) && !(SOLIDSPRITEINJURYFIX)) { ReafectSprite(gridx + x, gridy + y, 16777215, 0, 0, levelgrid); }
					}
					else {
						if ((levelgrid->SpriteSimplifer[gridx + x][gridy + y] == 1)) {
							if (HOLETOUTCH) { EndurenceTrackbar = 0; }
							result = 1;
						}
					}
					if ((TOUTCHWATER) || (TOUTCHWATERNOANIM)) {
						levelgrid->WaterSlowMove = 1;
					}

					if ((TOUTCHSPRITE)) {
						if (levelgrid->CleTotaleAmasser == levelgrid->CleTotalInLevel) {
							if ((widjet->Event.JoyButton2) && (levelgrid->DoorOpen != 2)) {
								levelgrid->DoorOpen = 1;
							}
						}
					}

					if (TOUTCHSWITCH) {
						if (widjet->Event.JoyButton2 == 1) {
							if (levelgrid->SwitchTrigger == 0) { levelgrid->SwitchTrigger = 1; }
							if (levelgrid->SwitchTrigger == 1) {

								if (levelgrid->Switch == 1) { levelgrid->Switch = 0; PlaySND(PLAY, SWITCHOFF); }
								else { PlaySND(PLAY, SWITCHON); levelgrid->Switch = 1; }
								levelgrid->SwitchTrigger = 2;
							}
						}
						else { levelgrid->SwitchTrigger = 0; }
					}

					if (TOUTCHSWITCHRESET) {
						if (levelgrid->Switch == 1) {
							levelgrid->SwitchTrigger = 0;
							levelgrid->Switch = 0;
							PlaySND(PLAY, SWITCHOFF);
						}
					}
				}
			}

			//Colision test enemi
			if ((COUCHESPRITEACTIFBLOB(levelgrid->MapSpriteEnemi[gridx + x][gridy + y])) || (levelgrid->MapSpriteEnemiSimplifier[gridx + x][gridy + y] > 0) || (COUCHESPRITEACTIFSCORPION(levelgrid->MapSpriteEnemi[gridx + x][gridy + y]))) {
				if (levelgrid->MapEnemiDirection[gridx + x][gridy + y] == 1) {
					if (colisiondetect(levelgrid->MapSpriteEnemi[gridx + x][gridy + y], mx + mxo, my + myo, mw, mh, sx(x, y) + levelgrid->MapEnemiOffSetX[gridx + x][gridy + y], sy(x, y), sw2(x, y), sh2(x, y), levelgrid, PKG, widjet)) {
						if (levelgrid->VitalInjuryTimer <= 0) { VitalInjury(levelgrid, widjet); }
					}
				}
				else {
					if (colisiondetect(levelgrid->MapSpriteEnemi[gridx + x][gridy + y], mx + mxo, my + myo, mw, mh, sx(x, y) + levelgrid->MapEnemiOffSetX[gridx + x][gridy + y], sy(x, y), sw2(x, y), sh2(x, y), levelgrid, PKG, widjet)) {
						if (levelgrid->VitalInjuryTimer <= 0) { VitalInjury(levelgrid, widjet); }
					}
				}
			}
		}
	}
	return result;
}

void MainSpriteAnim(LevelGrid *lg, Widjet *widjet) {
	char StopAnim = 0;
	//anim timer
	if (speedmax == 2) {
		if (lg->ActualFrame % 8 == 0) { lg->framecount = lg->framecount + 1; }
	}
	else {
		if (lg->ActualFrame % 12 == 0) { lg->framecount = lg->framecount + 1; }
	}
	if (lg->framecount > 7) { lg->framecount = 0; }
#define XTIME 100
	//anim timer2
	if (WidjetTimerTrigger(100, &lg->AnimFrameRate2)) {
		if (lg->framecount2 < XTIME + 7) { lg->framecount2 = lg->framecount2 + 1; }
		if (lg->framecount2 > XTIME + 6) {
			if (lg->framecount2 == XTIME + 7) { lg->framecount2 = XTIME + 8; }
			else { lg->framecount2 = XTIME + 7; }
		}
	}
	//fin anim timer

	//stop motion anim
	if ((lg->speedaxex > -1) && (lg->speedaxex < 1)) {
		if (lg->speedaxex > 0) {
			if ((lg->DirectionMainSprite != gauche) || (lg->DirectionMainSprite == stopdroite)) {
				if (lg->framecount2 < XTIME + 0) { lg->AnimSprite = 63; StopAnim = 1; }
				else {
					if (lg->framecount2 == XTIME + 1) { lg->AnimSprite = 63; StopAnim = 1; }
					else
						if (lg->framecount2 == XTIME + 2) { lg->AnimSprite = 64; StopAnim = 1; }
						else
							if (lg->framecount2 == XTIME + 3) { lg->AnimSprite = 65; StopAnim = 1; }
							else
								if (lg->framecount2 == XTIME + 4) { lg->AnimSprite = 66; StopAnim = 1; }
								else
									if (lg->framecount2 == XTIME + 5) { lg->AnimSprite = 67; StopAnim = 1; }
									else
										if (lg->framecount2 == XTIME + 6) { lg->AnimSprite = 68; StopAnim = 1; }
										else
											if (lg->framecount2 == XTIME + 7) { lg->AnimSprite = 69; StopAnim = 1; }
											else
												if (lg->framecount2 == XTIME + 8) { lg->AnimSprite = 70; StopAnim = 1; }
				}
			}
		}
		if (lg->speedaxex < 0) {
			if ((lg->DirectionMainSprite != droite) || (lg->DirectionMainSprite == stopgauche)) {
				if (lg->framecount2 < XTIME + 0) { lg->AnimSprite = 71; StopAnim = 1; }
				else {
					if (lg->framecount2 == XTIME + 1) { lg->AnimSprite = 71; StopAnim = 1; }
					else
						if (lg->framecount2 == XTIME + 2) { lg->AnimSprite = 72; StopAnim = 1; }
						else
							if (lg->framecount2 == XTIME + 3) { lg->AnimSprite = 73; StopAnim = 1; }
							else
								if (lg->framecount2 == XTIME + 4) { lg->AnimSprite = 74; StopAnim = 1; }
								else
									if (lg->framecount2 == XTIME + 5) { lg->AnimSprite = 75; StopAnim = 1; }
									else
										if (lg->framecount2 == XTIME + 6) { lg->AnimSprite = 76; StopAnim = 1; }
										else
											if (lg->framecount2 == XTIME + 7) { lg->AnimSprite = 77; StopAnim = 1; }
											else
												if (lg->framecount2 == XTIME + 8) { lg->AnimSprite = 78; StopAnim = 1; }
				}
			}
		}
	}
	else {
		//solution pour le moment
		if (lg->DirectionMainSprite == droite) {
			if (lg->framecount == 0) { lg->AnimSprite = 22; StopAnim = 1; }
			else
				if (lg->framecount == 1) { lg->AnimSprite = 23; StopAnim = 1; }
				else
					if (lg->framecount == 2) { lg->AnimSprite = 24; StopAnim = 1; }
					else
						if (lg->framecount == 3) { lg->AnimSprite = 25; StopAnim = 1; }
						else
							if (lg->framecount == 4) { lg->AnimSprite = 26; StopAnim = 1; }
							else
								if (lg->framecount == 5) { lg->AnimSprite = 27; StopAnim = 1; }
								else
									if (lg->framecount == 6) { lg->AnimSprite = 28; StopAnim = 1; }
									else
										if (lg->framecount == 7) { lg->AnimSprite = 29; StopAnim = 1; }
		}

		if (lg->DirectionMainSprite == gauche) {
			if (lg->framecount == 0) { lg->AnimSprite = 36; StopAnim = 1; }
			else
				if (lg->framecount == 1) { lg->AnimSprite = 37; StopAnim = 1; }
				else
					if (lg->framecount == 2) { lg->AnimSprite = 38; StopAnim = 1; }
					else
						if (lg->framecount == 3) { lg->AnimSprite = 39; StopAnim = 1; }
						else
							if (lg->framecount == 4) { lg->AnimSprite = 40; StopAnim = 1; }
							else
								if (lg->framecount == 5) { lg->AnimSprite = 41; StopAnim = 1; }
								else
									if (lg->framecount == 6) { lg->AnimSprite = 42; StopAnim = 1; }
									else
										if (lg->framecount == 7) { lg->AnimSprite = 43; StopAnim = 1; }
		}
	}

	//jump sprite
	if ((lg->speedaxey < -1)) {
		if ((lg->mainSpritejump == 1) && ((lg->DirectionMainSprite == droite) || (lg->DirectionMainSprite == stopdroite))) {
			if (lg->animjumpcount == 0) { lg->AnimSprite = 45; StopAnim = 1; }
			else
				if (lg->animjumpcount == 1) { lg->AnimSprite = 46; StopAnim = 1; }
				else
					if (lg->animjumpcount == 2) { lg->AnimSprite = 47; StopAnim = 1; }
					else
						if (lg->animjumpcount == 3) { lg->AnimSprite = 48; StopAnim = 1; }
						else
							if (lg->animjumpcount == 4) { lg->AnimSprite = 49; StopAnim = 1; }
							else
								if (lg->animjumpcount == 5) { lg->AnimSprite = 50; StopAnim = 1; }
								else
									if (lg->animjumpcount == 6) { lg->AnimSprite = 51; StopAnim = 1; }
									else
										if (lg->animjumpcount == 7) { lg->AnimSprite = 52; StopAnim = 1; }
			if (WidjetTimerTrigger(40, &lg->AnimFrameRatejump)) { lg->animjumpcount = lg->animjumpcount + 1; }
			if (lg->animjumpcount > 7) { lg->animjumpcount = 7; }
		}
		//jump sprite
		if ((lg->mainSpritejump == 1) && ((lg->DirectionMainSprite == gauche) || (lg->DirectionMainSprite == stopgauche))) {
			if (lg->animjumpcount == 0) { lg->AnimSprite = 54; StopAnim = 1; }
			else
				if (lg->animjumpcount == 1) { lg->AnimSprite = 55; StopAnim = 1; }
				else
					if (lg->animjumpcount == 2) { lg->AnimSprite = 56; StopAnim = 1; }
					else
						if (lg->animjumpcount == 3) { lg->AnimSprite = 57; StopAnim = 1; }
						else
							if (lg->animjumpcount == 4) { lg->AnimSprite = 58; StopAnim = 1; }
							else
								if (lg->animjumpcount == 5) { lg->AnimSprite = 59; StopAnim = 1; }
								else
									if (lg->animjumpcount == 6) { lg->AnimSprite = 60; StopAnim = 1; }
									else
										if (lg->animjumpcount == 7) { lg->AnimSprite = 61; StopAnim = 1; }
			if (WidjetTimerTrigger(40, &lg->AnimFrameRatejump)) { lg->animjumpcount = lg->animjumpcount + 1; }
			if (lg->animjumpcount > 7) { lg->animjumpcount = 7; }
		}
	}
	if (StopAnim == 0) {
		if (((lg->AnimSprite >= 36) && (lg->AnimSprite <= 43)) || ((lg->AnimSprite >= 54) && (lg->AnimSprite <= 61)) || ((lg->AnimSprite >= 71) && (lg->AnimSprite <= 78))) {
			lg->AnimSprite = 36;
		}
		else {
			lg->AnimSprite = 45;
		}
	}
}

int colisiondetect(uint32_t spritetester, int x1, int  y1, int w1, int h1, int x2, int  y2, int w2, int h2, LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	int xmain = 0;
	int ymain = 0;
	Uint32 pixelm = 0, pixels = 0;
	int newx = 0;
	int newy = 0;
	int neww = 0;
	int newh = 0;
	int result = 0;
	if (x1 > x2) { newx = x1; }
	else { newx = x2; }
	if (y1 > y2) { newy = y1; }
	else { newy = y2; }
	if (x1 + w1 < x2 + w2) { neww = x1 + w1; }
	else { neww = x2 + w2; }
	if (y1 + h1 < y2 + h2) { newh = y1 + h1; }
	else { newh = y2 + h2; }
	if (spritetester != 16777215) {
		if (x1 > x2 + w2 || y1 > y2 + h2 || x1 + w1 < x2 || y1 + h1 < y2) { return 0; }
		else {
			SDL_LockSurface(PKG->spriteBMP[spritetester]);
			SDL_LockSurface(PKG->spriteBMP[18]);
			for (ymain = newy; ymain < newh; ymain++) {
				for (xmain = newx; xmain < neww; xmain++) {
					pixelm = getpixel(PKG->spriteBMP[18], xmain - x1, ymain - y1);
					pixels = getpixel(PKG->spriteBMP[spritetester], xmain - x2, ymain - y2);
					if (pixels != getpixel(PKG->spriteBMP[18], 0, 0) && pixelm != getpixel(PKG->spriteBMP[18], 0, 0)) { result = 1; }
				}
			}
			SDL_UnlockSurface(PKG->spriteBMP[18]);
			SDL_UnlockSurface(PKG->spriteBMP[spritetester]);
		}
	}
	return result;
}

int colisiononeVSone(uint32_t sprite1, float x1, float  y1, float w1, float h1, uint32_t sprite2, float x2, float  y2, float w2, float h2, LevelGrid *levelgrid, ImagePKG *PKG, Widjet *widjet) {
	int xmain = 0;
	int ymain = 0;
	Uint32 pixelm = 0, pixels = 0;
	int newx = 0;
	int newy = 0;
	int neww = 0;
	int newh = 0;
	int result = 0;
	if (x1 > x2) { newx = x1; }
	else { newx = x2; }
	if (y1 > y2) { newy = y1; }
	else { newy = y2; }
	if (x1 + w1 < x2 + w2) { neww = x1 + w1; }
	else { neww = x2 + w2; }
	if (y1 + h1 < y2 + h2) { newh = y1 + h1; }
	else { newh = y2 + h2; }
	if (sprite2 != 16777215) {
		if (x1 > x2 + w2 || y1 > y2 + h2 || x1 + w1 < x2 || y1 + h1 < y2) { return 0; }
		else {
			SDL_LockSurface(PKG->spriteBMP[sprite2]);
			SDL_LockSurface(PKG->spriteBMP[sprite1]);
			for (ymain = newy; ymain < newh; ymain++) {
				for (xmain = newx; xmain < neww; xmain++) {
					pixelm = getpixel(PKG->spriteBMP[sprite1], xmain - x1, ymain - y1);
					pixels = getpixel(PKG->spriteBMP[sprite2], xmain - x2, ymain - y2);
					if (pixels != getpixel(PKG->spriteBMP[sprite1], 0, 0) && pixelm != getpixel(PKG->spriteBMP[sprite1], 0, 0)) { result = 1; }
				}
			}
			SDL_UnlockSurface(PKG->spriteBMP[sprite1]);
			SDL_UnlockSurface(PKG->spriteBMP[sprite2]);
		}
	}
	return result;
}

void FadeInFadeOutNoSoundModif(SDL_Surface *INSurface, int FadeStep, int FrameRate, SDL_Surface *OUTSurface, Widjet *widjet) {
	//var frame rate
	int bite = widjet->render->format->BitsPerPixel;
	int FADESTEP = FadeStep;
#define FADESTEPCOMP (256/FADESTEP)
	uint32_t delay1 = 0, delay2 = 0, delay3 = 0;
	int spin2 = 1000 / FrameRate; int t = 0; //variable pour les boucle
	if (bite == 8) {
		t = 0;
		while (t < FADESTEP) {
			SDL_Flip(widjet->render);
			delay2 = SDL_GetTicks();
			delay3 = delay2 - delay1;
			if ((delay3 < spin2)) { SDL_Delay(spin2 - delay3); }
			delay1 = SDL_GetTicks();
			t = t + 20;
		}
	}
	else {
		SDL_Rect rec;
		rec.x = 0;
		rec.y = 0;
		rec.w = 640;
		rec.h = 480;
		SDL_Surface *in;
		SDL_Surface *out;		
		SDL_Surface** output = malloc(FADESTEP * sizeof(SDL_Surface*));


		in = SDL_CreateRGBSurface(SDL_HWSURFACE, 640, 480, bite, 0, 0, 0, 0);
		if (INSurface == NULL) {
			SDL_FillRect(in, &rec, 0);
		}
		out = SDL_CreateRGBSurface(SDL_HWSURFACE, 640, 480, bite, 0, 0, 0, 0);
		if (OUTSurface == NULL) {
			SDL_FillRect(out, &rec, 0);
		}
		while (t < FADESTEP) {
			output[t] = SDL_CreateRGBSurface(SDL_HWSURFACE, 640, 480, bite, 0, 0, 0, 0);
			t++;
		}
		if (INSurface != NULL) {
			SDL_BlitSurface(INSurface, 0, in, 0);
		}
		if (OUTSurface != NULL) {
			SDL_BlitSurface(OUTSurface, 0, out, 0);
		}

		t = 0;
		while (t < FADESTEP) {
			SDL_SetAlpha(in, SDL_SRCALPHA, 256 - (t*FADESTEPCOMP));
			SDL_SetAlpha(out, SDL_SRCALPHA, (t*FADESTEPCOMP));
			SDL_FillRect(widjet->render, &rec, 0);
			SDL_BlitSurface(out, 0, output[t], 0);
			SDL_BlitSurface(in, 0, output[t], 0);
			t++;
		}
		t = 1;
		while (t < FADESTEP) {
			SDL_BlitSurface(output[t], 0, widjet->render, 0);
			SDL_Flip(widjet->render);
			delay2 = SDL_GetTicks();
			delay3 = delay2 - delay1;
			if ((delay3 < spin2)) { SDL_Delay(spin2 - delay3); }
			delay1 = SDL_GetTicks();
			t++;
		}
		SDL_FreeSurface(in);
		SDL_FreeSurface(out);
		t = 0;
		while (t < FADESTEP) {
			SDL_FreeSurface(output[t]);
			t++;
		}

		free(output);
	}
}

void FadeInFadeOut(SDL_Surface *INSurface, int FadeStep, int FrameRate, SDL_Surface *OUTSurface, Widjet *widjet) {
	//var frame rate
	int bite = widjet->render->format->BitsPerPixel;
	int FADESTEP = FadeStep;
#define FADESTEPCOMP (256/FADESTEP)
#define SOUNDFADEUNITY (round((MUSICVOLUME)/FadeStep))
#define SOUNDFADETOTAL (MUSICVOLUME)
	short int SOUNDMIX = SOUNDFADETOTAL;
	uint32_t delay1 = 0, delay2 = 0, delay3 = 0;
	int spin2 = 1000 / FrameRate; int t = 0; //variable pour les boucle
	if (bite == 8) {
		t = 0;
		while (t < FADESTEP) {
			SDL_Flip(widjet->render);
			delay2 = SDL_GetTicks();
			delay3 = delay2 - delay1;
			if ((delay3 < spin2)) { SDL_Delay(spin2 - delay3); }
			delay1 = SDL_GetTicks();
			t++;
		}
	}
	else {
		SDL_Rect rec;
		rec.x = 0;
		rec.y = 0;
		rec.w = 640;
		rec.h = 480;
		SDL_Surface *in;
		SDL_Surface *out;		
		SDL_Surface** output = malloc(FADESTEP * sizeof(SDL_Surface*));
		in = SDL_CreateRGBSurface(SDL_HWSURFACE, 640, 480, bite, 0, 0, 0, 0);
		if (INSurface == NULL) {
			SDL_FillRect(in, &rec, 0);
		}
		out = SDL_CreateRGBSurface(SDL_HWSURFACE, 640, 480, bite, 0, 0, 0, 0);
		if (OUTSurface == NULL) {
			SDL_FillRect(out, &rec, 0);
		}
		while (t < FADESTEP) {
			output[t] = SDL_CreateRGBSurface(SDL_HWSURFACE, 640, 480, bite, 0, 0, 0, 0);
			t++;
		}
		if (INSurface != NULL) {
			SDL_BlitSurface(INSurface, 0, in, 0);
		}
		if (OUTSurface != NULL) {
			SDL_BlitSurface(OUTSurface, 0, out, 0);
		}
		t = 0;
		while (t < FADESTEP) {
			SDL_SetAlpha(in, SDL_SRCALPHA, 256 - (t*FADESTEPCOMP));
			SDL_SetAlpha(out, SDL_SRCALPHA, (t*FADESTEPCOMP));
			SDL_FillRect(widjet->render, &rec, 0);
			SDL_BlitSurface(out, 0, output[t], 0);
			SDL_BlitSurface(in, 0, output[t], 0);
			t++;
		}
		t = 1;
		while (t < FADESTEP) {
			SOUNDMIX = SOUNDMIX - SOUNDFADEUNITY;
			Mix_VolumeMusic(SOUNDMIX);
			SDL_BlitSurface(output[t], 0, widjet->render, 0);
			SDL_Flip(widjet->render);
			delay2 = SDL_GetTicks();
			delay3 = delay2 - delay1;
			if ((delay3 < spin2)) { SDL_Delay(spin2 - delay3); }
			delay1 = SDL_GetTicks();
			t++;
		}
		SDL_FreeSurface(in);
		SDL_FreeSurface(out);
		t = 0;
		while (t < FADESTEP) {
			SDL_FreeSurface(output[t]);
			t++;
		}

		free(output);
	}
	Mix_HaltMusic(); Mix_RewindMusic(); Mix_PauseMusic();
	Mix_VolumeMusic(MUSICVOLUME);
}

void SplashScreen(ImagePKG *PKG, Widjet *widjet) {
	InfoText(PKG, widjet);
	FadeInFadeOut(NULL, 32, 15, widjet->render, widjet);
	SDL_Delay(2000);
	FadeInFadeOut(widjet->render, 32, 15, NULL, widjet);
	SplashScreenGPL(PKG, widjet);
}

void SplashScreenGPL(ImagePKG *PKG, Widjet *widjet) {
	InfoTextGPL(PKG, widjet);
	FadeInFadeOut(NULL, 32, 15, widjet->render, widjet);
	SDL_Delay(4000);
	FadeInFadeOut(widjet->render, 32, 15, NULL, widjet);
}

void InfoTextGPL(ImagePKG *PKG, Widjet *widjet) {
	//						GILBERT AND THE DOORS V1.0
	//			COPYRIGHT (c) all project Daniel Champagne 2016-2017
	//				Contact IMAIL: phoenixbozo@gmail.com
	SDL_Rect rec;
	rec.x = 0;
	rec.y = 0;
	rec.w = 640;
	rec.h = 480;
	SDL_FillRect(widjet->render, &rec, WidjetRGB(0, 0, 0));
#define cox 60
#define coy 0
#define ALx 0
#define ALy 20
	WidjetGetFont(2, WidjetRGB(255, 1, 255), WidjetRGB(255, 255, 255), widjet);
	WidjetText(204 + 30, 20, "COPYRIGHT 2016-2017", widjet);
	WidjetText(220 + 30, 40, "Daniel Champagne", widjet);
	WidjetText(204 + 30, 60, "All Rights Reserved.", widjet);
	WidjetGetFont(0, WidjetRGB(0, 255, 255), WidjetRGB(255, 255, 0), widjet);
	WidjetBlitAndStretch(&PKG->spriteBMP[253], 0, 276, 100 + ALy, 88, 31, &widjet->render);
	WidjetText(cox, 160 + concep + ALy, "Gilbert and the doors is free software: you can redistribute it and/or modify", widjet);
	WidjetText(cox, 180 + concep + ALy, "it under the terms of the GNU General Public License as published by", widjet);
	WidjetText(cox, 200 + concep + ALy, "the Free Software Foundation, either version 3 of the License, or", widjet);
	WidjetText(cox, 220 + concep + ALy, "(at your option) any later version.", widjet);
	WidjetText(cox, 260 + concep + ALy, "This program is distributed in the hope that it will be useful,", widjet);
	WidjetText(cox, 280 + ALy, "but WITHOUT ANY WARRANTY; without even the implied warranty of", widjet);
	WidjetText(cox, 300 + ALy, "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the", widjet);
	WidjetText(cox, 320 + ALy, "GNU General Public License for more details.", widjet);
	WidjetText(cox, 360 + ALy, "You should have received a copy of the GNU General Public License", widjet);
	WidjetText(cox, 380 + ALy, "along with this program.  If not, see <http://www.gnu.org/licenses/>.", widjet);
}

int NumberOfLevel() {
	int t = 0;
	char count[10] = { 0 };
	char file[255] = { 0 };
	while (1) {
		sprintf(count, "%d", t);
		strcpy(file, "levels/level");
		strcat(file, count);
		strcat(file, ".bmp");
		if (FichierPresent(file)) {
			t = t + 1;
		}
		else {
			return t;
		}
	}
}
int NumberOfSprite() {
	int t = 0;
	char count[10] = { 0 };
	char file[255] = { 0 };
	while (1) {
		sprintf(count, "%d", t);
		strcpy(file, "sprite/sprite");
		strcat(file, count);
		strcat(file, ".bmp");
		if (FichierPresent(file)) {
			t = t + 1;
		}
		else {
			return t;
		}
	}
}

int FichierPresent(const char* NomDuFichier) {
	FILE* FileTemp = fopen(NomDuFichier, "r");
	if (FileTemp != NULL) { fclose(FileTemp); return 1; }
	return 0;
}


