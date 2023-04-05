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
#include "intro.h"
#include "mundo.h"
#include "grafico.h"
#include "utils.h"
#include "int.h"
#include "fuente2.h"

intro :: intro(void)
{
	paso=0;
	cambiar_etapa(PRESENTA);
}

intro :: ~intro(void)
{
	printf(_("- Loading presentation: "));

	for (int i=0; i<6; i++)
		SDL_FreeSurface(imas[i]);

	SDL_FreeSurface(fondo);
	imprimir_ok();
}

/*!
 * \brief carga las pantalla de la animaci? e inicia el objeto
 */
int intro :: iniciar(class mundo *_pmundo, int _modo_video, SDL_Surface *_screen)
{
	char base[200];
	char tmp[200];
	char nombres[8][200] = {\
		{"pres_losers.jpg"},\
		{"pres_sentado.jpg"},\
		{"pres_lee.jpg"},\
		{"pres_casa.jpg"},\
		{"pres_rapto.jpg"},\
		{"pres_vs.jpg"}};

	
	strcpy(base, "./data/ima/");
	
	modo_video = _modo_video;
	
	printf(_("+ Loading intro: "));
	
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
			printf(_("\t Can't load '%s' : '%s'\n"), tmp,\
					SDL_GetError());
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

	cambiar_etapa(PRESENTA);
	imprimir_etapa(etapa);

	fondo = SDL_DisplayFormat(screen);
	return 0;
}

/*!
 * \brief actualizaci? l?ica
 */
void intro :: actualizar(void)
{
	Uint8 *tecla;

	tecla = SDL_GetKeyState(NULL);


	/* avance autom?ico */
	if (paso > 1000)
	{
		avanzar_etapa ();
		paso = 0;
	}

	/* avance con Space o Esc */
	if (paso > 100 && (tecla[SDLK_SPACE] || tecla[SDLK_RETURN]))
	{
		avanzar_etapa ();
		paso = 0;
	}
	
	paso ++;
	
	
	if (tecla[SDLK_ESCAPE])
	{
		pmundo->audio.play(5);
		pmundo->cambiar_escena(MENU);
	}
}


/*!
 * \brief impresion programada desde mundo
 */
void intro :: imprimir(void)
{
	static int etapa_anterior = -1;

	if (etapa != etapa_anterior)
	{
		imprimir_etapa (etapa);
		etapa_anterior = etapa;
	}
}

/*!
 * \brief imprime un fondo de pantalla nuevo
 */
void intro :: imprimir_etapa(int etapa)
{
	SDL_BlitSurface(imas[etapa], NULL, screen, NULL);
	imprimir_texto ();
	SDL_Flip(screen);
}


void intro :: avanzar_etapa (void)
{
	if (etapa == VS)
	{
		pmundo->audio.play(5);
		pmundo->cambiar_escena(MENU);
	}
	else
	{
		etapa ++;
		cambiar_etapa(etapa);
	}
}

/*!
 * \brief altera la etapa actual cambiando textos y la imagen de fondo
 */
void intro :: cambiar_etapa(int etapa)
{
	pos=0;
	posx_letra=0;
	posy_letra=0;
	
	switch (etapa)
	{
		case PRESENTA:
		strcpy(mensaje,_("Presents ..."));
		break;

		case SENTADO:
		strcpy(mensaje,_("the cowboy Don Ceferino lived "\
					"very happy in his quite and modest "\
					"house in the countryside"));
		break;

		case LEE:
		strcpy(mensaje,_("but one day he reades in the "\
					"newspaper that aliens were "\
					"kidnaping all the cows around"));
		break;

		case ASOMBRO:
		strcpy(mensaje,_("whith not exception. The aliens "\
					"arrive at the farm of "\
					"don ceferino..."));
		break;

		case RAPTO:
		strcpy(mensaje,_("and kidnapped his only cow."));
		break;

		case VS:
		strcpy(mensaje,_("but this doesn't end here."\
					"Don Ceferino is "\
					"determinat to recover "\
					"his precious cow."));
		break;
	}

	this->etapa = etapa;
}


/*!
 * \brief imprime una letra
 */
void intro :: imprimir_caracter(char letra, int x, int y, int resaltar)
{
	/*
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
					36+resaltar*40, screen, &rect, x, y, 1);
			break;

		case ':':
			pmundo->libgrafico.ima_fuente_2->imprimir(\
					37+resaltar*40, screen, &rect, x, y, 1);
			break;
			
		case '(':
			pmundo->libgrafico.ima_fuente_2->imprimir(\
					38+resaltar*40, screen, &rect, x, y, 1);
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
	*/
}


void intro :: pausar(void)
{
}



void intro :: imprimir_texto (void)
{
	pmundo->fuente->myprintf_inferior (screen, NULL, NULL, false, mensaje);
}
