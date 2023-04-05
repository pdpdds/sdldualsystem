/*
 * Don Ceferino Haza? - video game similary to Super Pang!
 * Copyright (c) 2004, 2005 Hugo Ruscitti
 * web site: http://www.loosersjuegos.com.ar
 * 
 * This file is part of Don Ceferino Haza? (ceferino).
 * Written by Hugo Ruscitti <hugoruscitti@yahoo.com.ar>
 *
 * Don Ceferino Haza? is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Don Ceferino Haza? is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#include <SDL.h>
#include <SDL_image.h>
#include <string.h>
#include "final.h"
#include "mundo.h"
#include "grafico.h"
#include "utils.h"
#include "int.h"

final :: final(void)
{
	paso=0;
	cambiar_etapa(F1);
}

final :: ~final(void)
{
	for (int i=0; i<6; i++)
		SDL_FreeSurface(imas[i]);

	SDL_FreeSurface(fondo);
	imprimir_ok();
}


/*!
 * \brief carga todas las imagenes de la final
 *
 * return 1 en caso de error
 */
int final :: iniciar(class mundo *_pmundo, int _modo_video, SDL_Surface *_screen)
{
	char base[512];
	char tmp[512];
	char nombres[6][200] = {\
		{"final1.jpg"},\
		{"final2.jpg"},\
		{"final3.jpg"},\
		{"final4.jpg"},\
		{"final5.jpg"},\
		{"final6.jpg"}};

	
	strcpy(base, "./data/ima/");	
	modo_video = _modo_video;
	
	printf(_("+ Loading final: "));

	
	// carga todas las imagenes
	for (int i=0; i<6; i++)
	{
		printf(".");
		fflush(stdout);

		strcpy(tmp, base);
		strcat(tmp, nombres[i]);
		imas[i] = IMG_Load(tmp);
		
		if (imas[i] == NULL)
		{
			imprimir_error();
			printf(_("\t Can't load '%s' : '%s'\n"), tmp, SDL_GetError());
			return 1;
		}

		if (modo_video)
		{
			imas[i] = reescalar_mitad(imas[i]);

			if (imas[i] == NULL)
			{
				exit(1);
			}
		}

	}
	imprimir_ok();
	
	pmundo = _pmundo;
	screen = _screen;

	imprimir_etapa(etapa);

	fondo = SDL_DisplayFormat(screen);
	return 0;
}


/*!
 * \brief actualiza los textos que se muestran en pantalla
 */
void final :: actualizar(void)
{
	Uint8 *tecla;

	tecla = SDL_GetKeyState(NULL);

	paso ++;
	
	/* avance autom?ico */
	if (paso > 1000 || (paso > 100 && \
				(tecla [SDLK_SPACE] || tecla [SDLK_RETURN])))
	{
		etapa ++;
		paso = 0;

		if (etapa > F6)
		{
			pmundo->cambiar_escena(MENU);
			return;
		}
		
		cambiar_etapa(etapa);
	}

	if (tecla[SDLK_ESCAPE])
		pmundo->cambiar_escena(MENU);
}


/*!
 * \brief impresion programada desde mundo
 */
void final :: imprimir(void)
{
	static int etapa_anterior = -1;

	if (etapa != etapa_anterior)
	{
		imprimir_etapa (etapa);
		etapa_anterior = etapa;
	}
}

/*!
 * \brief imprime una imagen de fondo
 */
void final :: imprimir_etapa(int etapa)
{
	if (etapa <= F6)
	{
		SDL_BlitSurface(imas[etapa], NULL, screen, NULL);
		pmundo->fuente->myprintf_inferior (screen, NULL, NULL, \
				false, mensaje);
		SDL_Flip(screen);
	}
}

/*!
 * \brief altera las etapas de la final asignando nuevo fondo y textos
 */
void final :: cambiar_etapa(int etapa)
{
	pos=0;
	posx_letra=0;
	posy_letra=0;
	
	switch (etapa)
	{
		case F1:
		strcpy(mensaje,_("Good job !, Don Ceferino arribe to spaceship ..."));
		break;

		case F2:
		strcpy(mensaje,_("uses a Mate (a drink) loaded with powder."));
		break;

		case F3:
		strcpy(mensaje,_("he escape with the spaceship rescuing the cow."));
		break;

		case F4:
		strcpy(mensaje,_("It seems like all is getting back to normal."));
		break;

		case F5:
		strcpy(mensaje,_("Though it seems like don ceferino is very hungry after all this action..."));
		break;

		case F6:
		strcpy(mensaje,_("At the end. Thanks for playing this game."));
		break;

			
	}

	this->etapa = etapa;
}

/*!
 * \brief imprime un caracter en pantalla
 */
void final :: imprimir_caracter(char letra, int x, int y, int resaltar)
{
	SDL_Rect rect;
	
	switch (letra)
	{
		case ' ':
			rect.x=0;
			rect.y=0;
			rect.w=1;
			rect.h=1;
			break;
			
		case '.':
			pmundo->libgrafico.ima_fuente_2->imprimir(\
					36+resaltar*40,screen, &rect, x, y, 1);
			break;

		case ':':
			pmundo->libgrafico.ima_fuente_2->imprimir(\
					37+resaltar*40,screen, &rect, x, y, 1);
			break;
			
		case '(':
			pmundo->libgrafico.ima_fuente_2->imprimir(\
					38+resaltar*40,screen, &rect, x, y, 1);
			break;
				
		case ')':
			pmundo->libgrafico.ima_fuente_2->imprimir(\
					39+resaltar*40, screen, &rect, x, y, 1);
			break;

		default:
			pmundo->libgrafico.ima_fuente_2->imprimir(\
					letra - 'a'+resaltar*40, screen,\
					&rect, x, y, 1);
			break;
	}

	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
}

void final :: pausar(void)
{
}
