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
#include "comojugar.h"
#include "mundo.h"
#include "grafico.h"
#include "utils.h"
#include "int.h"

comojugar :: comojugar(void)
{
	strcpy(mensaje,_("how to play"));
	
	paso = 0;
	pos = 0;
	posy_letra = 0;
	posx_letra = 0;
}

comojugar :: ~comojugar(void)
{
	SDL_FreeSurface(fondo);
}


/*!
 * \brief genera la escena como jugar
 *
 * return 1 en caso de error
 */
int comojugar :: iniciar(class mundo *_pmundo, int _modo_video, SDL_Surface *_screen)
{
	SDL_Rect rect;
	pmundo = _pmundo;
	screen = _screen;
	fondo = SDL_DisplayFormat(screen);

	pmundo->libgrafico.ima_menu->imprimir(0, fondo, &rect, 0, 0, 1);
	pmundo->libgrafico.ima_how_to_play->imprimir(0, fondo, &rect, 0, 0, 1);
	
	SDL_BlitSurface(fondo, NULL, screen, NULL);
	pmundo->fuente->myprintf (screen, 0, 0, NULL, NULL, false, CONSALTO, \
			mensaje);
	pmundo->fuente->myprintf (screen, 320, 420, NULL, NULL, false, \
			CENTRADO, _("(escape to continue)"));
	SDL_Flip(screen);

	return 0;
}


/*!
 * \brief actualiza los textos que se muestran en pantalla
 */
void comojugar :: actualizar(void)
{
	Uint8 *tecla;

	tecla = SDL_GetKeyState(NULL);

	if (tecla[SDLK_ESCAPE])
		pmundo->cambiar_escena(MENU);
}


/*!
 * \brief impresion programada desde mundo
 */
void comojugar:: imprimir(void)
{
}


void comojugar :: pausar(void)
{
}
