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
#ifndef _LIBGRAFICO_H
#define _LIBGRAFICO_H

#include <SDL.h>
#include "grafico.h"

/*
 * \brief Gestiona un conjunto de graficos
 */
class libgrafico
{
	public:
		libgrafico();
		~libgrafico();
		int iniciar(int modo_video);
		int terminar(void);
		
		grafico *ima_gaucho;
		grafico *ima_pelota_4;
		grafico *ima_pelota_3;
		grafico *ima_pelota_2;
		grafico *ima_pelota_1;
		grafico *ima_nivel;
		grafico *ima_fondos;
		grafico *ima_tiros;
		grafico *ima_fuente_1;
		grafico *ima_fuente_2;
		grafico *ima_barra;
		grafico *ima_item;
		grafico *ima_mate;
		grafico *ima_menu;
		grafico *ima_tit_1;
		grafico *ima_tit_2;
		grafico *ima_tit_3;
		grafico *ima_how_to_play;

	private:
		int modo_video;

		int crear_graficos(void);
		int cargar_imagenes(void);
		void eliminar_graficos(void);
};
		
#endif
