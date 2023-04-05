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
#ifndef _SIMPLE_SPRITE_H
#define _SIMPLE_SPRITE_H

#include "grafico.h"

/*!
 * \brief un grafico simple con movimiento
 */
class simple_sprite
{
	public:
		void iniciar(grafico *_ima, int _x_inicial, int _y_inicial, int _x_destino, int _y_destino);
		void actualizar(void);
		void imprimir(SDL_Surface *screen, SDL_Rect *rect, int *cant_modif);
		int termino_anim(void);
		
	private:
		int x_destino;
		int y_destino;
		int x;
		int y;
		class grafico *ima;
};

#endif
