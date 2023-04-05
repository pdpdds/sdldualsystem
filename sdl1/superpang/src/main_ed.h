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
#ifndef _MAIN_ED_H
#define _MAIN_ED_H

#include <SDL.h>
#include <SDL_image.h>

struct mouse
{
	int x;
	int y;
	int botones;
	int item;
	int f;
	int c;
};

struct barra
{
	int offset;
	int estado; // 0 bloques, 1 objetos
	int nivel;
};



int iniciar(SDL_Surface **screen, FILE **arch);
void imprimir_barra(SDL_Surface *screen, grafico *bloques, grafico *panel, struct barra *barra);
void set_bloque(struct mouse *mouse, SDL_Surface *screen, char mapa[][20], grafico *graficos);
void set_objeto(struct mouse *mouse, SDL_Surface *screen, char mapa[][20], grafico *objetos);
void set_bloque_limpio(struct mouse *mouse, SDL_Surface *screen, char mapa[][20]);
void imprimir_bloque(SDL_Surface *screen, grafico *graficos, int f, int c, int indice);
void limpiar_bloque(SDL_Surface *screen, int f, int c);
void imprimir_todo(SDL_Surface *screen, char mapa[][20], grafico *graficos, grafico *panel, struct barra *barra);
void mover_barra(SDL_Surface *screen, grafico *graficos, grafico *panel, struct barra *barra, int inc);
void seleccionar_barra(SDL_Surface *screen, struct mouse *mouse, struct barra *barra, grafico *graficos, grafico *panel, char mapa[][20], grafico *bloques, int *salir);
int cargar_nivel(char mapa[][20], FILE *arch, int indice);
void guardar_nivel(char mapa[][20], FILE *arch, int indice);
void ir_nivel_anterior(char mapa[][20], FILE *arch, int *nivel);
void ir_siguiente_nivel(char mapa[][20], FILE *arch, int *nivel);
void actualizar_mouse(struct mouse *mouse, SDL_Surface *screen, char mapa[][20], grafico *bloques, grafico *panel, struct barra *barra, FILE *arch, int *salir);
void eliminar_protagonista(SDL_Surface *screen, char mapa[][20]);
void copiar (char *fuente, char *destino);
#endif
