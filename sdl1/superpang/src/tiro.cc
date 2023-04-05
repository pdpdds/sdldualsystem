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


#include "tiro.h"
#include "mundo.h"
#include "int.h"

/// estados del tiro
enum {NORMAL, DETENIDO, TERMINA, TERMINA_DETENIDO};

/// tipos de tiro
enum {SIMPLE, TRIDENTE};

/*!
 * \brief genera el tiro que dispara el protagonista
 *
 * \param _juego objeto juego
 * \param _graficos imagenes del tiro
 * \param _x coordenada de origen
 * \param _y coordenada de origen
 */
int tiro :: iniciar(class juego *_juego, grafico *_graficos, int _x, int _y, int _tipo_tiro)
{
	graficos = _graficos;
	estado = NORMAL;
	x = _x;
	y = _y-34;
	h = w = 34;
	flip = 1;
	pjuego = _juego;
	
	pjuego->pmundo->audio.play(0);

	tipo = _tipo_tiro;
	h_grafico=34;
	y_inicial=_y;
	cont_vida = 0;
	return 0;
}


/*!
 * \brief actualizacion logica del tiro
 */
void tiro :: actualizar(void)
{
	switch (estado)
	{
		case TERMINA:
		case TERMINA_DETENIDO:
			if (cont_vida > 7)
				estado=-1;
			else
				cont_vida++;
			break;

		case NORMAL:
			if (pjuego->nivel->get_dist_techo(x,y,-1) == 0)
			{
				if (tipo == SIMPLE)
				{
					estado=TERMINA;
					pjuego->pmundo->audio.play(7);
				}
				else
				{
					pjuego->pmundo->audio.play(6);
					estado=DETENIDO;
				}
			}

			y+=pjuego->nivel->get_dist_techo(x,y,-4);
			
			break;

		case DETENIDO:
			cont_vida++;

			if (cont_vida > 150)
			{
				cont_vida=0;
				estado=TERMINA_DETENIDO;
			}
			break;
	}
	
	h = y_inicial - y + h_grafico;
}

/*!
 * \brief imprime el tiro en pantalla
 *
 * \param screen pantalla destino de impresion
 * \param rect rectangulo modificado
 */
void tiro :: imprimir(SDL_Surface *screen, SDL_Rect *rect)
{
	int i;
	int nro_grafico;

	switch (estado)
	{
		case NORMAL:
			nro_grafico = 0+tipo*4;
			break;

		case DETENIDO:
			nro_grafico = 8;
			break;

		case TERMINA:
			nro_grafico = 2+tipo*4;
			break;
			
		case TERMINA_DETENIDO:
			nro_grafico = 10;
			break;
		
		default:
			printf(_("tiro: error, trying to print "\
						"with state %d\n"), estado);
			nro_grafico = 2+tipo*4;
			break;
	}

	// punta del arma
	graficos->imprimir(nro_grafico, screen, rect, x, y, 1);

	// soga
	for (i=y+h_grafico; i<y_inicial-h_grafico; i+=h_grafico)
		graficos->imprimir(nro_grafico + 1, screen, rect, x, i, 1);

	// completa el fin de soga recortado
	graficos->imprimir_parte(nro_grafico +1 , screen, rect, x, i, y_inicial - i);

	
	// modo de video = 2 (320x240)
	if (screen->w == 320)
	{
		rect->h = h/2;
		rect->y = (y/2);
	}
	else
	{
		rect->h = h;
		rect->y = y;
	}
	
}

/*!
 * \brief accion a realizar cuando colisiona con la pelota
 */
void tiro :: colisiona_con_pelota(void)
{
	cont_vida=0;
	
	if (estado == DETENIDO)
		estado = TERMINA_DETENIDO;
	else
		estado=TERMINA;
}

/*!
 * \brief accion a realizar cuando se rompe un bloque
 */
void tiro :: colisiona_con_bloque(void)
{
	estado=TERMINA;
}
