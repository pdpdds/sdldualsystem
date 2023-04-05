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
#ifndef _PROCESOS_H
#define _PROCESOS_H

#include <stdlib.h>
#include "actor.h"
#include "gaucho.h"
#include "pelota.h"
#include "tiro.h"
#include "fuente.h"
#include "item.h"
#include "bloque.h"
#include "bomba.h"

///nodo utilizado en las listas
template <class T>
struct nodo
{
	T *obj;
	struct nodo *psgte;
};



/*!
 * \brief mantiene una lista de actores
 */
class procesos
{
	public:
		procesos ();
		
		void iniciar (class juego *_pjuego);
		
		void crear_gaucho (grafico *grafico, int x, int y);
		void crear_pelota (grafico *grafico, int x, int y, int tam, int flip);
		void crear_tiro (grafico *grafico, int x, int y, int tipo_tiro);
		void crear_item (grafico *ima_item, int x, int y, int tipo);
		
		void reducir_todos_los_enemigos (void);
		void crear_bloque_romper (grafico *ima_bloque, int x, int y);
		void actualizar ();
		void imprimir (SDL_Surface *screen, SDL_Rect *modificados, int *lim_modificados);
		void avisar_colisiones (SDL_Surface *screen);
	
		void crear_bomba (class grafico *ima, int x, int y, int flip);
		
		void crear_mensaje (class fuente2 * fuente, const char *cadena);
		void crear_mensaje_rapido (class fuente2 * fuente, char *cadena);
		int hay_mensaje_activo (void);
		int hay_enemigos (void);
		int hay_bomba (void);
		
		void eliminar_todos (void);
		void descongelar_todos (void);
		void pausar_enemigos (void);
		void pausar_gaucho (void);
		void pausar_tiros (void);
		int get_cant_tiros (void);

		class juego * pjuego;
		
	private:
		class gaucho *gaucho;
		struct nodo <class tiro> *lista_tiro;
		struct nodo <class fuente> *lista_fuente;
		struct nodo <class pelota> *lista_pelota;
		struct nodo <class bloque> *lista_bloque;
		struct nodo <class item> *lista_item;
		struct nodo <class bomba> *lista_bomba;
		
		template <class S> void agregar (S *pnuevo, S **pplista);
		template <class S> void limpiar_todos (S **pplista);
		template <class S> void eliminar_antiguos (S **pplista);
		template <class S> void imprimir_lista (SDL_Surface * screen, \
				SDL_Rect *modif, int *lim, S *lista);
		template <class S> void actualizar_lista (S *lista);

		int hab_tiro;
		int hab_gaucho;
		int hab_enemigos;
};

#endif
