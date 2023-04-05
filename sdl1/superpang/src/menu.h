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
#ifndef _MENU_H
#define _MENU_H

#include <SDL.h>
#include <stdlib.h>
#include "escena.h"
#include "grafico.h"
#include "item_menu.h"
#include "simple_sprite.h"

/*!
 * \brief Un estado del juego: menu principal
 */
class menu : public escena
{
	public:
		menu();
		~menu();
		int iniciar(class mundo *_mundo, int conf, SDL_Surface *screen);
		void actualizar(void);
		void imprimir(void);

		void pausar(void);

	
	private:
                Uint8 * tecla;
		SDL_Surface *screen;
		SDL_Surface *fondo;
		SDL_Rect rect_actual[200];
		SDL_Rect rect_todos[200];
		int lim_actual;
		int lim_todos;

		int titulo_activo;
		int tiempo;
		int opcion;
		int tecla_pulsada;
		int modo_video;
		class item_menu *item_menu [6];
		class simple_sprite *simple_sprite[3];

		int termino_titulo(void);
                bool shot(void);
                bool up(void);
                bool down(void);
};	
#endif
