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


#include "bomba.h"

/*!
 * \brief asigna los valores iniciales del la bomba
 *
 * \param _graficos imagenes de los items
 * \param _x coordenada horizontal
 * \param _y coordenada vertical
 * \param _tipo tipo de item (bomba, doble tiro, etc)
 */
void bomba :: iniciar(class juego *_juego, grafico *_graficos, int _x, int _y, int _flip)
{
	x = _x;
	y = _y-40;
	estado=0;
	flip = _flip;
	velocidad=-4;
	velocidad_inicial=-4;
	ima = _graficos;
	cuadro=0;
	juego = _juego;
}

/*!
 * \brief actualizacion logica
 */
void bomba :: actualizar(void)
{
	x+=flip*4;
	y+= juego->nivel->get_dist_suelo(x, y, (int) velocidad);
	velocidad += 0.1;


	if (juego->nivel->get_dist_suelo(x,y, 1) == 0)
	{
		velocidad = ++velocidad_inicial;
		y-=1;

		if (velocidad_inicial < 0)
			velocidad= velocidad_inicial;
	}

	if (x>620 || x < 20)
	{
		juego->efecto_bomba();
		juego->procesos->reducir_todos_los_enemigos();
//		juego->pmundo->audio.play(11);
		estado = -1;
	}
}

/*!
 * \brief muestra el item por pantalla
 *
 * \param screen pantalla
 * \param rect rectangulo que modifica
 */
void bomba :: imprimir(SDL_Surface *screen, SDL_Rect *rect)
{
	cuadro++;

	if (cuadro>3)
		cuadro=0;
	
	ima->imprimir(cuadro, screen, rect, x, y, flip);
}
