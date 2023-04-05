/*
 * Don Ceferino Hazaña - video game similary to Super Pang!
 * Copyright (c) 2004, 2005 Hugo Ruscitti
 * web site: http://www.loosersjuegos.com.ar
 * 
 * This file is part of Don Ceferino Hazaña (ceferino).
 * Written by Hugo Ruscitti <hugoruscitti@yahoo.com.ar>
 *
 * Don Ceferino Hazaña is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Don Ceferino Hazaña is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */


///etapas de la sección creditos
enum {INICIAL, WEB, GPL};


#include <SDL.h>
#include <string.h>

#include "menu.h"
#include "mundo.h"
#include "grafico.h"
#include "utils.h"
#include "int.h"

/*!
 * \brief asigna los valores iniciales al menu creditos
 */
creditos :: creditos(void)
{
	f=0;
	c=0;
	delay=100;
	etapa=INICIAL;
	limite=86;	

	fila_letra=0;
	columna_letra=0;
	color=0;
}

creditos :: ~creditos(void)
{
	SDL_FreeSurface(fondo);
}

/*!
 * \brief genera la escena
 *
 * \return 1 en caso de error
 */
int creditos :: iniciar(class mundo *_pmundo, int _modo_video, SDL_Surface *_screen)
{
	SDL_Rect rect;	
	
	pmundo = _pmundo;
	modo_video = _modo_video;
	screen = _screen;

	fondo = SDL_DisplayFormat(screen);

	if (fondo == NULL)
	{
		printf(_("error: Can't copy screen: '%s'\n"), SDL_GetError());
		return 1;
	}

	
	pmundo->libgrafico.ima_menu->imprimir(0, fondo, &rect, 0, 0, 1);
	SDL_BlitSurface(fondo, NULL, screen, NULL);
	cambiar_etapa(INICIAL);
	imprimir_etapa ();
	
	SDL_Flip(screen);
	return 0;
}


/*!
 * \brief actualizacón lógica
 */
void creditos :: actualizar(void)
{
	Uint8 * tecla;
	
	tecla = SDL_GetKeyState(NULL);

	if (tecla [SDLK_ESCAPE])
	{
		pmundo->audio.play (4);
		pmundo->cambiar_escena (MENU);
	}

	if (tecla [SDLK_SPACE] && delay == 0)
	{
		delay = 100;
			
		if (etapa == GPL)
		{
			pmundo->audio.play (4);
			pmundo->cambiar_escena (MENU);
		}
		else
		{
			etapa ++;
			pausa = 0;
			fila_letra = 0;
			columna_letra = 0;
			c = 0;
			f = 0;
			cambiar_etapa (etapa);
			pmundo->audio.play (5);
			SDL_BlitSurface (fondo, NULL, screen, NULL);
			
			imprimir_etapa ();
			SDL_Flip (screen);
		}
	}
	
	if (delay > 0)
		delay --;
}

/*!
 * \brief imprime un texto de creditos
 */
void creditos :: imprimir(void)
{	
	static int etapa_anterior = -1;
	
	if (etapa != etapa_anterior)
	{
		imprimir_etapa ();
		etapa_anterior = etapa;
	}
}


void creditos :: imprimir_etapa (void)
{
	pmundo->fuente->myprintf (screen, 0, 0, NULL, NULL, false, CONSALTO, \
			mensaje);
	
	imprimir_pie ();
	SDL_Flip (screen);	
}


/*!
 * \brief mensaje (space para continuar)
 */
void creditos :: imprimir_pie(void)
{
	pmundo->fuente->myprintf (screen, 320, 420, NULL, NULL, false, \
			CENTRADO, _("(space to continue)"));
}


/*!
 * \brief asigna una nueva etapa (ver enum)
 */
void creditos :: cambiar_etapa(int nueva)
{
	etapa = nueva;
	
	switch (etapa)
	{
		case INICIAL:
			strcpy(mensaje, _(\
"Developed by:\n    Hugo Ruscitti\n\nGraphics by:\n    Walter Velazquez\n\n"\
"Music by:\n    Javier Da Silva"));
		break;

		case WEB:
	strcpy(mensaje,\
_(\
"You can know more about what we are doing visitting the web site of the "\
"Losers Proyect:\n\nwww.losersjuegos.com.ar"));

			break;

		case GPL:
			strcpy(mensaje,\
_(\
"License:\n\nThis program is free software and is distributed under the"\
"terms of the GPL license. please read the copying file "\
"to obtain more information."));
			break;

			
	}
}


void creditos :: pausar(void)
{
}
