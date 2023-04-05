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


#include "bloque.h"

///estados del bloque de madera
enum {NORMAL, ROMPER};


/*!
 * \brief vincula el bloque de madera al nivel
 */
int bloque :: iniciar(class juego *_juego, grafico *_graficos, int _x, int _y)
{
	graficos=_graficos;
	estado=NORMAL;
	x=_x;
	y=_y;
	w=h=32;
	flip=1;
	pjuego=_juego;
	
	paso=0;
	cont_delay=0;

	cargar_animacion(animaciones[NORMAL], "4");
	cargar_animacion(animaciones[ROMPER], "5,6");

	return 0;
}


/*!
 * \brief actualizaci? l?ica
 */
void bloque :: actualizar(void)
{
	if (estado == ROMPER)
	{
		if (avanzar_animacion())
			estado=-1;
	}
}


/*!
 * \brief imprime la plataforma
 */
void bloque :: imprimir(SDL_Surface *screen, SDL_Rect *rect)
{
	graficos->imprimir(animaciones[estado][paso], screen, rect, x, y, 1);
}


/*!
 * \brief es golpeado por un tiro
 */
void bloque :: colisiona_con_tiro(void)
{
	if (estado == NORMAL)
	{
		paso=0;
		cont_delay=0;
		estado = ROMPER;
		pjuego->nivel->limpiar_bloque(y/32, x/32);
		pjuego->pmundo->audio.play(8);
	}

	pjuego->crear_item_por_azar(x,y);
}

/*!
 * \brief gestina la animaci? cuando este se rompe
 * 
 * \return 1 si la animacion termina, 0 en otro caso
 */
int bloque :: avanzar_animacion(void)
{
	if (cont_delay>10)
	{
		if (animaciones[estado][paso+1] == -1)
		{
			cont_delay=0;
			paso=0;
			return 1;
		}
		else
			paso++;
		
		cont_delay=0;
	}
	else
		cont_delay++;

	return 0;
}
