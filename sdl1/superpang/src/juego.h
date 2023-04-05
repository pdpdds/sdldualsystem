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
#ifndef _JUEGO_H
#define _JUEGO_H

#include <SDL.h>
#include "procesos.h"
#include "escena.h"
#include "grafico.h"
#include "nivel.h"
#include "barra.h"
#include "mundo.h"


/// diferentes estados de la escena juego
enum {INICIANDO, JUGANDO, PIERDE_VIDA, PIERDE_TODO, PIERDE_POR_TIEMPO, PASA_NIVEL, DETENIDO_POR_BONUS, DETENIDO_POR_BOMBA, GUARDA_MARCA, PAUSA};

/*!
 * \brief Un estado del juego: juego
 */
class juego : public escena
{
	public:
		juego();
		~juego();
		int iniciar(class mundo *_pmundo, int conf_video, SDL_Surface *screen);
		void actualizar(void);
		void imprimir(void);
		
		void crear_gaucho(int x, int y);
		void crear_tiro(int x, int y, int tipo_tiro);
		void crear_pelota(int x, int y, int tam, int flip);
		void crear_item_por_azar(int x, int y);
		void crear_bloque_romper(int x, int y);
		void crear_bomba(int x, int y, int flip);
		
		int get_cant_tiros(void);
		void reiniciar_nivel(void);
		void sumar_puntos(int inc);
		void sumar_vidas();
		void restar_vidas();
		void pagar_item(int tipo_item);
		void efecto_bomba(void);

		void cambiar_estado(int _estado);

		void pausar(void);
		void restaurar(void);

		class procesos *procesos;
		class nivel *nivel;
		class barra *barra;
		
	private:
		SDL_Surface *screen;
		SDL_Surface *fondo;
		void reiniciar_reloj_local(void);
		int avanzar_nivel(void);
		void procesar_tiempo();
		void crear_mensaje_bonus_tiempo(int segundos);
		int es_marca(int puntos);

		// funciones de cada estado
		void iniciando(void);
		void jugando(void);
		void pierde_vida(void);
		void pierde_todo(void);
		void pierde_por_tiempo(void);
		void pasa_nivel(void);
		void detenido_por_bonus(void);
		void detenido_por_bomba(void);
		void guarda_marca(void);
		void imprimir_marca(SDL_Surface *screen, SDL_Rect *rect_actual, int *lim_actual);
		void imprimir_nombre(SDL_Surface *screen, SDL_Rect *rect_actual, int *lim_actual);
		void salvar_marca_en_archivo(void);
		void pausa(void);
		void imprime_pausa(SDL_Surface *screen, \
				SDL_Rect *rect_actual, int *lim_actual);
		void imprime_pierde_todo (SDL_Surface *screen, \
				SDL_Rect *rect_actual, int *lim_actual);
		
		SDL_Rect rect_actual[1024];
		SDL_Rect todos[1024];
		int lim_actual;
		int lim_todos;
		
		int tiempo;
		int tick_ant;
		int vidas;
		int puntos;
		int estado;
		int tiempo_bonus;

		int proximo_pago_vida;
		Uint8 *tecla;

		// estado guarda_marca
		char nombre[19];
		int delay_tecla;
};
		

#endif
