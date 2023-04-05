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
#ifndef _MUNDO_H
#define _MUNDO_H

#include <SDL.h>
#include "escena.h"
#include "menu.h"
#include "creditos.h"
#include "juego.h"
#include "intro.h"
#include "marcas.h"
#include "final.h"
#include "comojugar.h"
#include "audio.h"
#include "libgrafico.h"
#include "fuente2.h"

///todas las escenas gestinadas por mundo
enum {INTRO, MENU, JUEGO, CREDITOS, MARCAS, FINAL, COMOJUGAR, OPCIONES};

/// parametros definidos por el archivo de configuracion
struct opciones_t
{
	int pantalla_completa;
	int sonido;
	int musica;
	int modo_video;
};


/*!
 * \brief Mantiene todos los objetos del juego e inicia el juego
 */
class mundo
{
	public:
		mundo();
		int iniciar(void);
		void correr(void);
		void terminar(void);
		int cargar_opciones(const char *ruta);
		void cambiar_escena(int nueva_escena);
		void reiniciar_reloj(void);
		void eliminar(void);
		void deshabilitar_letras(void);
		void habilitar_letras(void);

		class audio audio;
		class libgrafico libgrafico;
		class fuente2 * fuente;

		void pantalla_completa (void);
		void reiniciar (void);
		
	private:
		int procesar_eventos(SDL_Event *evento);
		void intercambiar_escenas(void);
		void imprimir_cargando(void);

		int salir;
		int escena;
		int escena_anterior;
		SDL_Surface *screen;
		class escena *actual;
		struct opciones_t opciones;
		int leer_teclas;
	
		int t, tl, tg; // tick actual, l?ico y grafico
};

#endif
