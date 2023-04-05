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
#ifndef _FINAL_H
#define _FINAL_H

#include <SDL.h>
#include "escena.h"
#include "grafico.h"

/// etapas de la presentaci?
enum {F1, F2, F3, F4, F5, F6};

/*!
 * \brief Un estado del juego: final
 */
class final : public escena
{
	public:
		final();
		~final();
		int iniciar(class mundo *_pmundo, int conf_video, SDL_Surface *screen);
		void actualizar(void);
		void imprimir(void);

		void pausar(void);
	
	private:
		SDL_Surface *screen;
		SDL_Surface *fondo;
		
		SDL_Rect rect_actual[100];
		SDL_Rect rect_todos[100];

		SDL_Surface * imas[6];
		
		int lim_actual;
		int lim_todos;

		int pos;
		int posx_letra;
		int posy_letra;

		int paso;
		int etapa;
		char mensaje[1024];

		void imprimir_etapa(int etapa);
		void imprimir_caracter(char letra, int x, int y, int resaltar);
		void cambiar_etapa(int etapa);
};	
#endif
