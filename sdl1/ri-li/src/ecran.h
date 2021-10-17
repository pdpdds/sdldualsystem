#ifndef _ECRAN_DOM_
#define _ECRAN_DOM_

#include <SDL.h>
#include "sprite.h"
#include "preference.h"

/*****************************/
struct s_Ecran {
	e_Sprite NumSpr;
	int Num;
	int x, y;
	int fx, fy;
};

/*************************************/
class Ecran
{
public:
	Ecran(void);
	~Ecran(void);

	/*** Fonctions ***/
	void Affiche(e_Sprite NumSpr, int Num, int x, int y);
	void AfficheCable(int dx, int dy, int fx, int fy);
	void Affiche_Text(e_Sprite Text, int x, int y);
	void AfficheOptions(int NVies, int NouveauScore);
	void Efface(e_Sprite NumSpriteFondEcran);
	void Cls(e_Sprite NumSpriteFondEcran);

	/*** Variables ***/
	int N;
	int NVie;
	int Score;
	s_Ecran B[LT * HT * 2];
};

#endif
