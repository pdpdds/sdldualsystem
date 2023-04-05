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
#ifndef _UTILS_H
#define _UTILS_H

/// una entrada en el archivo de marcas (records)
struct entrada
{
	char nombre[17];
	int puntos;
};

void limpiar_pantalla(void);
void imprimir_ok(void);
void imprimir_error(void);
void copiar_rectangulos(SDL_Rect *destino, int *dest_lim, SDL_Rect *fuente, int *fuente_lim, int w, int h);
SDL_Surface *reescalar_mitad(SDL_Surface *ima);
void put_pixel(SDL_Surface *ima, int x, int y, Uint32 pixel);
Uint32 get_pixel(SDL_Surface *ima, int x, int y);
void cargar_marcas_estandar(void);


#endif
