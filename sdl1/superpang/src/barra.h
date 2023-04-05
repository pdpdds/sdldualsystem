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

#ifndef _BARRA_H
#define _BARRA_H

#include "grafico.h"
#include "juego.h"

/*!
 * \brief Barra de estado del juego
 */
class barra
{
	public:
		void iniciar(class juego *_juego, grafico *_grafico, SDL_Surface *screen);
		void actualizar(int tiempo, int vidas, int puntos);
		void imprimir_todo(void);

	private:
		void imprimir_fondo(void);
		void imprimir_tiempo(void);
		void imprimir_vidas(void);
		void imprimir_puntos(void);

		SDL_Rect rect;
		int tiempo;
		int vidas;
		int puntos;
		int ant_tiempo;
		int ant_vidas;
		int ant_puntos;
		
		class juego *pjuego;
		class grafico *graficos;
		SDL_Surface *screen;
};

#endif
