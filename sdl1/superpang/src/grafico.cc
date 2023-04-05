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


#include <SDL.h>
#include <SDL_image.h>
#include <string.h>
#include <stdlib.h>
#include "grafico.h"
#include "utils.h"
#include "int.h"


grafico :: ~grafico()
{
	SDL_FreeSurface(ima);	
}

/*!
 * \brief carga una biblioteca de graficos
 *
 * \param ruta ubicacion del archivo de imagen
 * \param f numero de filas
 * \param c numero de columnas
 * \param punto_x punto de control x
 * \param punto_y punto de control y
 *
 * \return 1 en caso de error
 */
int grafico ::  iniciar (const char * ruta, int f, int c, int punto_x, int punto_y, int conf)
{
	char ruta_completa[150] =  "./data/ima/";

	ima=NULL;
	
	strcat(ruta_completa, ruta);
	
	ima=IMG_Load(ruta_completa);
	
	if(ima==NULL)
	{
		printf(_("error : can't open: '%s' "), ruta_completa);
		imprimir_error();
		exit(1);
	}
	
	if (conf)
	{
		ima = reescalar_mitad(ima);

		if(ima==NULL)
		{
			printf(_("error : error reshaping the image: '%s' "),\
					ruta_completa);
			imprimir_error();
			exit(1);
		}
	}
	
	w=ima->w/c;
	h=ima->h/f;
	px=punto_x;
	py=punto_y;
	fil=f;
	col=c;
	modo_video = conf;

	return 0;
}


/*!
 * \brief imprime un grafico de la biblioteca sobre 'destino'
 *
 * \param indice numero de imagen a imprimir
 * \param destino superficie sobre la que se debe imprimir
 * \param rect rectangulo que informa la seccion modificada
 * \param x coordenada x
 * \param y coordenada y
 * \param flip espejado de la imagen 1=normal, -1=invertido
 */
void grafico :: imprimir(int indice, SDL_Surface *destino, SDL_Rect *rect, int x, int y, int flip)
{
	SDL_Rect captura;

	if (indice <0 || indice > (col*fil) )
	{
		rect->x=0;
		rect->y=0;
		rect->w=1;
		rect->h=1;
		printf(_("Can't show the graphic %d\n"), indice);
		return;
	}

	// recorte del grafico a imprimir
	captura.w=w;
	captura.h=h;
	captura.y=(indice/col)*h;
	captura.x=(indice%col)*w;

	// rectangulo destino
	rect->y=y-py;
	rect->x=x-px;

	// compatibilidad con modos 320x240
	if (modo_video != 0)
	{
		rect->x/=2;
		rect->y/=2;
	}

	// imprime el grafico
	if (flip==1)
	{
		rect->w=w;
		rect->h=h;
		SDL_BlitSurface(ima,&captura,destino,rect);
	}
	else
	{
		w_temp=0;
		src_rect.x = captura.x;
		src_rect.y = captura.y;
		src_rect.w = 1;
		src_rect.h = h;
		
		dest_rect.x = rect->x +w;
		dest_rect.w = 1;
		dest_rect.h = h;
		dest_rect.y = rect->y;
		
		for (i=0; i< w; i++)
		{
			if (dest_rect.x>=0)
			{
				SDL_BlitSurface(ima, &src_rect, destino, &dest_rect);
				w_temp++;
			}
			dest_rect.x --;
			src_rect.x ++;

			dest_rect.y = rect->y;
		}
		
		if (modo_video != 0)
			rect->x= (x-px)/2;
		else
			rect->x= (x-px);
			
		rect->w = w_temp;
		rect->h = dest_rect.h;

		if (rect->y < 0)
			rect->y = 0;

		if (rect->x < 0)
			rect->x = 0;
	}

	/* --  Imprime el rectangulo de la imagen -- */
	
	/*	
	SDL_Rect r;

	r.x=rect->x;
	r.y=rect->y;
	r.h=rect->h;
	r.w=1;
	SDL_FillRect(destino , &r, 255);

	r.x=rect->x+rect->w-1;
	SDL_FillRect(destino, &r, 255);

	r.x=rect->x;
	r.y=rect->y;
	r.h=1;
	r.w=rect->w;
	SDL_FillRect(destino, &r, 255);

	r.y=rect->y+rect->h-1;
	SDL_FillRect(destino, &r, 255);
	*/

}


/*!
 * \brief imprime un grafico de la biblioteca sobre 'destino' recortando su altura
 *
 * \param indice numero de imagen a imprimir
 * \param destino superficie sobre la que se debe imprimir
 * \param rect rectangulo que informa la seccion modificada
 * \param x coordenada x
 * \param y coordenada y
 * \param h altura final
 */
void grafico :: imprimir_parte(int indice,SDL_Surface *dest, SDL_Rect *rect, int _x, int _y, int _h)
{
	SDL_Rect captura;
	SDL_Rect dst;

	if (indice <0 || indice > (col*fil) )
	{
		rect->x=0;
		rect->y=0;
		rect->w=1;
		rect->h=1;
		printf(_("Can't show the graphic %d\n"), indice);
		return;
	}

	// recorte del grafico a imprimir
	captura.w=w;
	captura.h=1;
	captura.y=(indice/col)*h;
	captura.x=(indice%col)*w;

	// rectangulo destino
	rect->y=_y-py;
	rect->x=_x-px;

	// compatibilidad con modos 320x240
	if (modo_video != 0)
	{
		rect->x/=2;
		rect->y/=2;
		_h/=2;
	}
	
	dst.x=rect->x;
	dst.y=rect->y;
	dst.w=w;
	dst.h=1;
	
	for (i=0; i<_h; i++)
	{
		SDL_BlitSurface(ima, &captura, dest, &dst);
		dst.y++;
		captura.y++;
	}

	rect->w=w;
	rect->h=_h;
	
	if (modo_video != 0)
		rect->x= (_x-px)/2;
	else
		rect->x= (_x-px);
		
	if (rect->y < 0)
		rect->y = 0;

	if (rect->x < 0)
		rect->x = 0;

}
