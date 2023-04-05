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

#ifndef _FUENTE2_H
#define _FUENTE2_H

#include <SDL.h>
#include <SDL_image.h>

#define KEY_R 0
#define KEY_G 0
#define KEY_B 0
#define KEY_A 0

enum posiciones {CENTRADO, NINGUNO, CONSALTO};

class fuente2
{
	public:
		fuente2 (int modo);
		~fuente2 ();

		void myprintf (SDL_Surface * dst, int x, int y, \
				SDL_Rect * rect, int * nrect, \
				bool transparente, enum posiciones posicion, \
				const char * format, ...);

		void myprintf_inferior (SDL_Surface * dst, \
				SDL_Rect * rect, int * nrect, \
				bool transparente, const char * format, ...);

		int imprimir_letra (SDL_Surface * destino, char letra, int x, \
				int y, SDL_Rect * rect, \
				bool transparente = false);
		
		void imprimir (SDL_Surface * dst, char * cadena, int x, int y, \
				SDL_Rect *rect, int * nrect, bool transparente);

		void imprimir_con_salto (SDL_Surface * dst, char * cadena, \
				int x, int y, SDL_Rect *rect, int * nrect, \
				bool transparente);

		int get_w (char letra);
		int get_h (void);

	private:
		SDL_Rect rects [1024];
		int letras;
		SDL_Surface * ima;
		int modo_reducido;

		
		char * saltar_palabra (char * cadena);
		SDL_Rect obtener_referencia (char letra, bool transparente);
		void guardar_rect (int i, int x, int y, int w, int h);
		void cargar_identificar_recs (void);
		bool es_columna_vacia (SDL_Surface * ima, int col);
		bool es_pixel_opaco (SDL_Surface * ima, int fila, int col);

		int longitud (char * cadena);
		int ancho_palabra (char * palabra);
		void alinear_inferior (int * x1, int * y1, char * cadena);
		void imprimir_palabra (SDL_Surface * dst, \
				char * cadena, int x, int y, SDL_Rect *rect, \
				int * nrect, bool transparente);

};

#endif
