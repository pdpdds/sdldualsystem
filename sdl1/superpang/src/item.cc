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


#include "item.h"


enum item_estado_t {QUITAR = -1, NORMAL, SELECCIONADO, TRANSPARENTE};

/*!
 * \brief asigna los valores iniciales del item
 *
 * \param _graficos imagenes de los items
 * \param _x coordenada horizontal
 * \param _y coordenada vertical
 * \param _tipo tipo de item (bomba, doble tiro, etc)
 */
void item :: iniciar(class juego *_juego, grafico *_graficos, int _x, int _y, int _tipo)
{
	tipo = _tipo;
	x = _x;
	y = _y;
	estado = NORMAL;
	vida = 400;
	velocidad = -3;
	ima_items = _graficos;
	juego = _juego;
}

/*!
 * \brief actualización lógica
 */
void item :: actualizar (void)
{
	if (juego->nivel->get_dist_suelo (x, y-10, 1) != 0)
	{
		y+= juego->nivel->get_dist_suelo (x, y-10, (int) velocidad);
		velocidad += 0.1;
	}
	else
		velocidad = 0;

	switch (estado)
	{
		case NORMAL:
			if (vida < 100)
				estado = TRANSPARENTE;
			break;

		case TRANSPARENTE:
		case SELECCIONADO:

			if (vida < 0)
				estado = QUITAR;
			break;
			
		default:
			break;
	}
	
	vida--;
}

/*!
 * \brief muestra el item por pantalla
 *
 * \param screen pantalla
 * \param rect rectangulo que modifica
 */
void item :: imprimir (SDL_Surface *screen, SDL_Rect *rect)
{
	int n = tipo;

	switch (estado)
	{
		case SELECCIONADO:
			n += 7;
			break;

		case TRANSPARENTE:
			n += 14;
			break;

		default:
			break;
	}
	
	ima_items->imprimir (n, screen, rect, x, y, 1);
}

/*!
 * \brief accion a realizar si colisiona con el protagonista
 */
void item :: colisiona_con_protagonista(void)
{	
	if (estado == NORMAL || estado == TRANSPARENTE)
	{
		juego->pagar_item(tipo);
		estado = SELECCIONADO;
		vida = 10;
	}
}
