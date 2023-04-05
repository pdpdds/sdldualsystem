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
#ifndef _ACTOR_H
#define _ACTOR_H

#include "grafico.h"

/*!
 * \brief Es la base para todos los personajes y objetos
 */
class actor
{
	public:
		virtual ~actor(void);
		virtual void imprimir(SDL_Surface *screen, SDL_Rect *rect) = 0;
		
		int get_x(void) {return x;}
		int get_y(void) {return y;}
		int get_w(void) {return w;}
		int get_h(void) {return h;}
		int get_estado(void) {return estado;}

			
	protected:
		grafico *graficos;
		int estado;
		int x, y, w, h;
		int flip;
		SDL_Rect rect;
		
		class juego *pjuego;
		void cargar_animacion(int *vec, const char *cuadros);
};

#endif
