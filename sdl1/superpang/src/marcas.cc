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
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "menu.h"
#include "mundo.h"
#include "grafico.h"
#include "utils.h"
#include "int.h"

marcas :: marcas(void)
{
	delay = 0;
	pausa = 0;
}


marcas :: ~marcas(void)
{
	SDL_FreeSurface(fondo);
}

/*!
 * \brief vincula el objeto creado
 */
int marcas :: iniciar(class mundo *_pmundo, int _mvideo, SDL_Surface *_screen)
{
	SDL_Rect rect;	
	
	pmundo = _pmundo;
	modo_video = _mvideo;
	screen = _screen;

	fondo = SDL_DisplayFormat(screen);

	if (fondo == NULL)
	{
		printf(_("error: Can't copy screen: '%s'\n"), SDL_GetError());
		exit(1);
	}

	cargar_marcas();

	pmundo->libgrafico.ima_menu->imprimir(0, fondo, &rect, 0, 0, 1);
	SDL_BlitSurface(fondo, NULL, screen, NULL);
	imprimir_puntajes ();
	SDL_Flip(screen);
	return 0;
}

/*!
 * \brief actualizaci? l?ica
 */
void marcas :: actualizar(void)
{
	Uint8 *tecla;
	
	tecla = SDL_GetKeyState(NULL);

	if (tecla[SDLK_ESCAPE])
	{
		pmundo->cambiar_escena(MENU);
		pmundo->audio.play(4);
	}
}

/*!
 * \brief imprime los records en pantalla
 */
void marcas :: imprimir(void)
{
}

void marcas :: imprimir_puntajes (void)
{
	pmundo->fuente->myprintf (screen, 320, 0, NULL, NULL, false, \
			CENTRADO, _("best point"));
	
	for (int i = 0; i < 7; i ++)
	{
		char puntos [5];

		pmundo->fuente->myprintf (screen, 30, 60 + i * 50, NULL, \
				NULL, false, NINGUNO, vec_marcas [i].nombre);

		puntos[0] = (vec_marcas [i].puntos / 1000) % 10 +'0';
		puntos[1] = (vec_marcas [i].puntos / 100) % 10+'0';
		puntos[2] = (vec_marcas [i].puntos / 10) % 10 +'0';
		puntos[3] = (vec_marcas [i].puntos) % 10 + '0';
		puntos[4] = '\0';

		pmundo->fuente->myprintf (screen, 520, 60 + i * 50, NULL, \
				NULL, false, NINGUNO, puntos);
	}
	
	pmundo->fuente->myprintf (screen, 320, 420, NULL, NULL, false, \
			CENTRADO, _("(escape to continue)"));
}



/*!
 * \brief carga las marcas desde el archivo .ceferinomarcas
 */
void marcas :: cargar_marcas(void)
{
	FILE *arch;
	char ruta_completa[100];

#ifdef WIN32
	strcpy(ruta_completa, "marcas.dat");
#else
	strcpy(ruta_completa, getenv("HOME"));
	strcat(ruta_completa, "/");
	strcat(ruta_completa, ".ceferinomarcas");
#endif

	arch = fopen(ruta_completa, "rb");

	if (!arch)
	{
		printf(_("Can't open the marcs file\n"));
		cargar_marcas_estandar();
		arch = fopen(ruta_completa, "rb");
	}
	
	fread(vec_marcas, sizeof(struct entrada), 7, arch);
	fclose(arch);
}


void marcas :: pausar(void)
{
}
