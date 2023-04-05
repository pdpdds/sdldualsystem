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


#include "simple_sprite.h"
#include <stdlib.h>

/*!
 * \brief genera el grafico y se posiciona en la posicion inicial
 */
void simple_sprite :: iniciar(grafico *_ima, int _x_inicial, int _y_inicial, int _x_destino, int _y_destino)
{
	ima = _ima;
	x_destino = _x_destino;
	y_destino = _y_destino;
	x = _x_inicial;
	y = _y_inicial;
}

/*!
 * \brief actualizacion logica de la cadena
 */
void simple_sprite :: actualizar(void)
{

	if (abs(x_destino - x) < 10 && x_destino - x != 0)
		x+= (x_destino - x) / abs(x_destino - x);
	else
		x+= (x_destino - x) / 10;
	

	if (abs(y_destino - y) < 10 && y_destino - y != 0)
		y+= (y_destino - y) / abs(y_destino -y);
	else
		y+= (y_destino - y) / 10;
}


/*!
 * \brief muestra el objeto por pantalla
 */
void simple_sprite :: imprimir(SDL_Surface *screen, SDL_Rect *rect, int *cant_modif)
{
	ima->imprimir(0, screen, rect+*cant_modif, x, y, 1);
	(*cant_modif)++;
}

/*!
 * \brief informa si terminó su animación
 */
int simple_sprite :: termino_anim(void)
{
	if ( (y_destino - y) == 0 && (x_destino - x) == 0)
		return 1;
	else
		return 0;
}
