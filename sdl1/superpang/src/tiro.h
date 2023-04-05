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
#ifndef _TIRO_H
#define _TIRO_H

#include "grafico.h"
#include "actor.h"
#include "juego.h"

/*!
 * \brief Disparo realizado por el protagonista
 */
class tiro : public actor
{
	public:
		int iniciar(class juego *procesos, grafico *_grafico, int x, int y, int tipo_tiro);
		void actualizar(void);
		void imprimir(SDL_Surface *screen, SDL_Rect *rect);
		void colisiona_con_pelota(void);
		void colisiona_con_bloque(void);
		
	private:
		int y_inicial;
		int cont_vida;
		int h_grafico;
		int tipo;
};

#endif
