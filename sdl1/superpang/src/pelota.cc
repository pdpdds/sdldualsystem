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


#include "pelota.h"

/*!
 * \brief vincula el enemigo al juego
 */
int pelota :: iniciar(class juego *_juego, grafico *_graf, int _x, int _y, int _tam, int _flip)
{
	graficos = _graf;
	estado=0;
	x=_x;
	y=_y;
	flip=_flip;
	pjuego = _juego;

	dir_y=1;
	velocidad=-2;
	tam = _tam;
	nro_grafico=0;
	paso=0;

	switch (tam)
	{
		case 1:
			w=9;
			vel_salto=-4;
			break;
		case 2:
			w=18;
			vel_salto=-4,3;
			break;
		case 3:
			w=37;
			vel_salto=-4,8;
			break;
		case 4:
			w=75;
			vel_salto=-5;
			break;
	}

	h=w;
	return 0;
}


/*!
 * \brief actualizaci? l?ica
 */
void pelota :: actualizar(int mover=1)
{

	
	switch (estado)
	{
		case 2:
			nro_grafico=1;
			paso++;

			if (paso>10)
				estado=-1;
			break;

		default:
			if (mover)
				animar_pelota();
			break;
	}
	

}


/*!
 * \brief imprime el enemigo
 */
void pelota :: imprimir(SDL_Surface *screen, SDL_Rect *rect)
{
	graficos->imprimir(nro_grafico, screen, rect, x, y, 1);
}

/*!
 * \brief accion a realizar cuando es golpeada con el arma de ceferino
 */
void pelota :: colisiona_con_tiro(void)
{	
	if (estado == 0)
	{
		estado = 2;
		
		if (tam != 1)
		{
			pjuego->crear_pelota(x+10, y, tam-1, 1);
			pjuego->crear_pelota(x-10, y, tam-1, -1);
			pjuego->crear_item_por_azar(x,y);
		}

		pjuego->sumar_puntos(tam);
		pjuego->pmundo->audio.play(1);
	}
}

/*!
 * \brief animacion de la esfera 'saltando'
 */
void pelota :: animar_pelota(void)
{
	int dy;
	
	velocidad+=0.04;
	dy = (int) velocidad;
	
	if (dy>0)
		dir_y = 1;
	else
		dir_y = -1;
	
	
	if (pjuego->nivel->get_dist_suelo(x, y+dy+(dir_y*(w-8)), 16) < 15)
		nro_grafico=2;
	else
		nro_grafico=0;
				
			
	if (pjuego->nivel->get_dist_suelo(x, y+dy+(dir_y*(w-8)), dy) != dy)
	{
		// estaba bajando
		if (velocidad>0)
			velocidad= vel_salto;
		else
			velocidad*=-1;
	}
	else
	{
		y+=dy;
		
		if (pjuego->nivel->get_dist_pared(x+(flip*(w-8)),y,flip) != flip)
			flip*=-1;
	}

	x+=flip;

}
