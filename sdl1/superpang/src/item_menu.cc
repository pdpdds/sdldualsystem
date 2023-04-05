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


#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#include "item_menu.h"

///estados de la cadena de texto
enum {ENTRANDO, NORMAL};

/*!
 * \brief genera el item
 */
void item_menu :: iniciar (class fuente2 * fuente, int n, const char *_cadena)
{
	this->fuente = fuente;
	
	x_destino = 320;
	y_destino = 170 + (n * 50);

	/* es par ? */
	if (n % 2 == 0)
	{
		x = 320 + 400;
		fuerza = -200;
	}
	else
	{
		x = 320 - 400;
		fuerza = 200;
	}
	
	y = y_destino;

	seleccionado = 0;
	strcpy (cadena, _cadena);

	estado = ENTRANDO;
}

/*!
 * \brief actualizacion logica de la cadena
 */
void item_menu :: actualizar (void)
{
	static double t = 0;
	t += 0.05;
	
	switch (estado)
	{
		case ENTRANDO:

			if (abs(x_destino - x) < 10 && x_destino - x != 0)
				x+= (x_destino - x) / abs(x_destino - x);
			else
				x+= (x_destino - x) / 10;

			
			if (abs(y_destino - y) < 10 && y_destino - y != 0)
				y+= (y_destino - y) / abs(y_destino -y);
			else
				y+= (y_destino - y) / 10;

			
			if (x_destino == x)
				estado = NORMAL;

			if (seleccionado)
				y = y_destino + int ((sin (t)) * 5);
			
			break;

		case NORMAL:

			if (seleccionado)
				y = y_destino + int ((sin (t)) * 5);

			break;
	}
}



/*!
 * \brief muestra en pantalla la cadena de textos
 */
void item_menu :: imprimir (SDL_Surface *screen, SDL_Rect *rect, int * nrect)
{
	fuente->myprintf (screen, x, y, rect, nrect, !seleccionado, CENTRADO, \
			cadena);
}

/*!
 * \brief el objeto es seleccionado por el usuario
 */
void item_menu :: seleccionar(void)
{
	seleccionado = 1;
}

/*!
 * \brief el objeto deja de estar seleccionado
 */
void item_menu :: no_seleccionar(void)
{
	seleccionado = 0;
}
