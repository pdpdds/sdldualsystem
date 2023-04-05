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
#ifndef _ITEM_MENU_H
#define _ITEM_MENU_H

#include "grafico.h"
#include "fuente2.h"

/*!
 * \brief Opcion para seleccionar desde el men? */
class item_menu
{
	public:
		void iniciar (class fuente2 * fuente, int n, const char *_cadena);
		void actualizar(void);
		void imprimir(SDL_Surface *screen, SDL_Rect *rect, \
				int *cant_modif);
		void seleccionar(void);
		void no_seleccionar(void);
		
	private:
		class fuente2 * fuente;
		char cadena[100];
		int x_destino;
		int y_destino;
		int x;
		int y;
		int seleccionado;
		int estado;
		int fuerza;
};


#endif
