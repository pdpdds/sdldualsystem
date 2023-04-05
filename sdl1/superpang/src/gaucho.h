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
#ifndef _GAUCHO_H
#define _GAUCHO_H

#include "grafico.h"
#include "actor.h"

/*!
 * \brief Protagonista del juego
 */
class gaucho : public actor
{
	public:
		int iniciar(class juego *_juego, grafico *_grafico, int x, int y);
		void actualizar(void);
		void imprimir(SDL_Surface *screen, SDL_Rect *rect);
		void colisiona_con_pelota(void);
		void colisiona_con_item(int tipo);
		int animaciones[12][10];
		
	private:
		Uint8 *tecla;
		int paso;
		int cont_delay;
		int max_tiros;
		int tipo_tiro;
		float vel;
		int disparo_pulsado;
		class juego *pjuego;
		class nivel *pnivel;

		int hay_escalera(int inc_y); 
		int avanzar_animacion(void);
		void reiniciar_animacion(void);
		int puede_disparar(void);
		
		// estados
		void crear_tiro(void);
		void caminar();
		void parado();
		void morir();
		void disparar();
		void usar_escalera();
		void agachado();
		void cae();
		void barrida();
		void cae_girando(void);
		void tirando_bomba(void);

                // Controles
                bool left(void);
                bool right(void);
                bool up(void);
                bool down(void);
                bool shot(void);
                bool slash(void);
};

#endif
