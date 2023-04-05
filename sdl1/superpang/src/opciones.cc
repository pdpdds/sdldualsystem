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
#include <string.h>

#include "menu.h"
#include "mundo.h"
#include "grafico.h"
#include "utils.h"
#include "int.h"
#include "opciones.h"

/*!
 * \brief asigna los valores iniciales al menu opciones
 */
opciones :: opciones (void)
{
}

opciones :: ~opciones (void)
{
	SDL_FreeSurface(fondo);
}

/*!
 * \brief genera la escena
 *
 * \return 1 en caso de error
 */
int opciones :: iniciar(class mundo *_pmundo, int _modo_video, SDL_Surface *_screen)
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


	pmundo->fuente->myprintf (screen, 0, 50, NULL, NULL, false, CONSALTO, \
			_("This feature has been temporaly disabled, please run 'ceferinosetup' instead."));
	
	imprimir_pie ();

	SDL_Flip(screen);
	return 0;
}


/*!
 * \brief actualizac? l?ica
 */
void opciones :: actualizar(void)
{
	Uint8 * tecla;
	
	tecla = SDL_GetKeyState(NULL);

	if (tecla [SDLK_ESCAPE] || tecla [SDLK_SPACE])
	{
		pmundo->audio.play (4);
		pmundo->cambiar_escena (MENU);
	}
}

/*!
 * \brief imprime un texto de opciones
 */
void opciones :: imprimir(void)
{	
}

/*!
 * \brief mensaje (space para continuar)
 */
void opciones :: imprimir_pie(void)
{
	pmundo->fuente->myprintf (screen, 320, 420, NULL, NULL, false, \
			CENTRADO, _("(space to continue)"));
}

void opciones :: pausar(void)
{
}
