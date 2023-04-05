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
#ifndef _GRAFICO_H
#define _GRAFICO_H

#include <SDL.h>

/*!
 * \brief Maneja un conjunto de imagenes para imprimir
 */
class grafico
{
	public:
		~grafico();
		int iniciar(const char *ruta, int f, int c, int punto_x, int punto_y, int conf);
		void imprimir(int i, SDL_Surface *destino, SDL_Rect *rect, int x, int y, int flip);
		void imprimir_parte(int i, SDL_Surface *dest, SDL_Rect *rect, int x, int y, int h);
		int get_h(void) {return h;};
		int get_w(void) {return w;};
		int get_lim(void) {return fil*col;};
	
	private:
		SDL_Surface *ima;
		int fil;
		int col;
		int w;
		int h;
		int px;
		int py;
		int modo_video;
		int i;
		int w_temp;
		SDL_Rect src_rect; // lee barras vericales para invertir un grafico
		SDL_Rect dest_rect; // escribe barras verticales para invertir un grafico
};

#endif
