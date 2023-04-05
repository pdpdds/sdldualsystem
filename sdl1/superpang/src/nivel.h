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
#ifndef _NIVEL_H
#define _NIVEL_H

#include <SDL.h>
#include "grafico.h"
#include "juego.h"

/*!
 * \brief imprime y verifica colisiones en los niveles
 */
class nivel
{
	public:
		void iniciar(juego* _pjuego, grafico* _grafico, grafico* _graf_fondos, SDL_Surface* _fondo);
		int avanzar_nivel(int salto = 1);
		int reiniciar_nivel(void);
		int get_dist_suelo(int x, int y, int max);
		int get_dist_techo(int x, int y, int max);
		int get_dist_pared(int x, int y, int max);
		int get_escalera(int x, int y);
		void limpiar_bloque(int f, int c);
		void ir_nivel(int numero_nivel);
		
	private:
		char mapa[14][20];
		SDL_Surface *fondo;
		grafico *pgrafico;
		grafico *pfondos;
		int nivel_actual;
		juego *pjuego;
		
		void imprimir_nivel(void);
		int cargar_nivel(void);
		int es_suelo(char l);
		int verificar_nivel(void);
};

#endif
