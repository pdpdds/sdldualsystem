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


#include <string.h>
#include <ctype.h>

#include "fuente.h"
#include "procesos.h"
#include "juego.h"


/*!
 * \brief vincula la barra al juego
 */
void barra :: iniciar(class juego *_juego, grafico *_graficos, SDL_Surface *_screen)
{
	graficos=_graficos;
	pjuego=_juego;
	screen=_screen;
	
	rect.x=0;
	rect.y=screen->h - graficos->get_h();
	rect.w=screen->w;
	rect.h=graficos->get_h();

	ant_tiempo=-1;
	ant_vidas=-1;
	ant_puntos=-1;
}


/*!
 * \brief imprime todo el fondo de la barra
 */
void barra :: imprimir_fondo(void)
{
	SDL_Rect tmp;
	int i;

	graficos->imprimir(0, screen, &tmp, 0, 448, 1);

	for (i=0; i<6; i++)
		graficos->imprimir(1, screen, &tmp, 32+i*32, 448, 1);

	// etiqueta tiempo
	graficos->imprimir(3, screen, &tmp, 224, 448, 1);
	graficos->imprimir(4, screen, &tmp, 256, 448, 1);

	for (i=0; i<5; i++)
		graficos->imprimir(1, screen, &tmp, 288+i*32, 448, 1);

	// etiqueta puntos
	graficos->imprimir(5, screen, &tmp, 448, 448, 1);
	graficos->imprimir(6, screen, &tmp, 480, 448, 1);

	for (i=0; i<2; i++)
		graficos->imprimir(1, screen, &tmp, 512+i*32, 448, 1);
	
	graficos->imprimir(2, screen, &tmp, 608, 448, 1);
}


/*!
 * \brief imprime el tiempo restante
 */
void barra :: imprimir_tiempo(void)
{
	SDL_Rect tmp;

	if (tiempo<0)
		tiempo=0;
	
	graficos->imprimir(1, screen, &tmp, 224+64, 448,1);
	graficos->imprimir(1, screen, &tmp, 224+64+32, 448,1);
	graficos->imprimir(10+ tiempo/10, screen, &tmp, 224+32+32, 448, 1);
	graficos->imprimir(10+ tiempo%10, screen, &tmp, 224+32+32+32, 448, 1);
}

/*!
 * \brief informa la cantidad de vidas
 */
void barra :: imprimir_vidas(void)
{
	SDL_Rect tmp;
	int i;
	
	// imprime las vidas
	for (i=0; i < vidas && i <6; i++)
		graficos->imprimir(8, screen, &tmp, 32*i, 448, 1);

	graficos->imprimir(1, screen, &tmp, 32*i, 448, 1);
}

/*!
 * \brief informa los puntos acumulados
 */
void barra :: imprimir_puntos(void)
{
	SDL_Rect tmp;
	
	graficos->imprimir(1, screen, &tmp, 512, 448, 1);
	graficos->imprimir(1, screen, &tmp, 544, 448, 1);
	graficos->imprimir(1, screen, &tmp, 576, 448, 1);
	graficos->imprimir(1, screen, &tmp, 608, 448, 1);

	graficos->imprimir(10+(puntos/1000), screen, &tmp, 512, 448, 1);
	graficos->imprimir(10+(puntos/100)%10, screen, &tmp, 544, 448, 1);
	graficos->imprimir(10+(puntos/10)%10, screen, &tmp, 576, 448, 1);
	graficos->imprimir(10+(puntos%10), screen, &tmp, 608, 448, 1);
}

/*!
 * \brief imprime la barra nuevamente
 */
void barra :: imprimir_todo(void)
{
	int h;

	if (screen->h == 480)
		h=32;
	else
		h=16;
	
	imprimir_fondo(); 
	imprimir_tiempo();
	imprimir_vidas();
	imprimir_puntos();
	
	SDL_UpdateRect(screen, 0, screen->h-h, screen->w, h);
}

/*!
 * \brief imprime y actualiza los nuevos valores
 *
 * \param _tiempo segundos restantes para pasar de nivel
 * \param _vidas vidas del protagonista
 * \param _puntos puntaje del protagonista
 */
void barra :: actualizar(int _tiempo, int _vidas, int _puntos)
{
	tiempo = _tiempo;
	vidas = _vidas;
	puntos = _puntos;
	
	if (ant_tiempo != tiempo)
		imprimir_tiempo();

	if (ant_vidas != vidas)
		imprimir_vidas();

	if (ant_puntos != puntos)
		imprimir_puntos();

	ant_tiempo = tiempo;
	ant_vidas = vidas;
	ant_puntos = puntos;
	
	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
}
