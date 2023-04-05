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
#ifndef _CREDITOS_H
#define _CREDITOS_H

#include <SDL.h>
#include "escena.h"
#include "grafico.h"

/*!
 * \brief Un estado del juego: menu creditos
 */
class creditos : public escena
{
	public:
		creditos();
		~creditos();
		int iniciar(class mundo *_pmundo, int conf_video, SDL_Surface *screen);
		void actualizar(void);
		void imprimir(void);

		void pausar(void);
	
	private:
		SDL_Surface *fondo;
		SDL_Surface *screen;
		int etapa;
		char mensaje[1024];
		
		int pausa;
		int delay;
		int f;
		int c;
		int fila_letra;
		int columna_letra;
		int limite;
		int color;

		void cambiar_etapa(int nueva_etapa);
		void crear_letra(char letra, int f, int c);
		void imprimir_pie(void);
		void imprimir_etapa (void);
};	
#endif
