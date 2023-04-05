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
#ifndef _MAIN_OP_H
#define _MAIN_OP_H

struct opciones
{
	int modo_video;
	int sonido;
	int musica;
	int pantalla_completa;
	int opcion_seleccionada;
	int pos_y;
	int pos_x;
	int delay;
};

int iniciar(SDL_Surface **screen, SDL_Surface **fondo, SDL_Surface **items);
int actualizar(struct opciones *opciones, int *salir);
void imprimir(SDL_Surface *screen, SDL_Surface *fondo, SDL_Surface *items, struct opciones *opciones);
int cargar_opciones(const char *ruta, struct opciones *opciones);
void pintar_imagen(SDL_Surface *screen, SDL_Surface *items, int indice, int x, int y);
int procesar_eventos(void);
void cambiar_opcion(int *pnumero, int maximo_valor);
void salir_guardar_cambios(struct opciones *opciones);

#endif
