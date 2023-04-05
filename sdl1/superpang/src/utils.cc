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


#include <stdio.h>
#include <SDL.h>
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include "int.h"

/*!
 * \brief limpia la pantalla
 */
void limpiar_pantalla(void)
{
	//printf("\e[2J \e[0;0H");
}

/*!
 * \brief imprime [OK] con color verde (al menos en GNU/linux)
 */
void imprimir_ok(void)
{
#ifdef WIN32
	printf(" [OK]\n");
#else
	printf(" \e[32m[OK]\e[0m\n");
#endif
}

/*!
 * \brief informa un error
 */
void imprimir_error(void)
{
#ifdef WIN32
	printf(" [ERROR]\n");
#else
	printf(" \e[31m[ERROR]\e[0m\n");
#endif
}

/*!
 * \brief copia varios rectangulos de un vector a otro y corrige los desbordes
 *
 * \param destino vector de rectangulos
 * \param dest_lim cantidad de elementos iniciales en el primer vector
 * \param fuente vector fuente
 * \param fuente_lim cantidad de elementos a copiar
 * \param w ancho de la pantalla
 * \param h alto de la pantalla
 */
void copiar_rectangulos(SDL_Rect *destino, int *dest_lim, SDL_Rect *fuente, int *fuente_lim, int w, int h)
{
	int i;
	int lim=*dest_lim;
	int unir=0;
	
	for (i= 0; i< (*fuente_lim); i++)
	{
		// aplica limites
		if (fuente[i].y < 0)
			fuente[i].y = 0;

		if (fuente[i].y > h || fuente[i].x < 0) 
		{
			fuente[i].y=0;
			fuente[i].x=0;
			fuente[i].w=0;
			fuente[i].h=0;
		}
	
		if (fuente[i].y + fuente[i].h > h)
			fuente[i].h = h - fuente[i].y;

		if (fuente[i].x + fuente[i].w > w)
			fuente[i].w = w - fuente[i].x;
			
		unir=0;
		
		for (int j=0; j<lim; j++)
		{
			
			// union perfecta
			if (destino[j].x == fuente[i].x && destino[j].y == fuente[i].y && destino[j].w == fuente[i].w && destino[j].h == fuente[i].h)
			{
				unir=1;
				break;
			}

		}
	
		// lo agrega como nuevo si no encuentra otro rectangulo para unir
		if (!unir)		
		{
			destino[lim] = fuente[i];
			lim++;
		}
	}

	*dest_lim = lim;
}



/*!
 * \brief obtiene un pixel (tomado de las referencias de SDL)
 */
Uint32 get_pixel(SDL_Surface *_ima, int x, int y)
{
	int bpp = _ima->format->BytesPerPixel;
	Uint8 *p= (Uint8 *) _ima->pixels + y*_ima->pitch + x*bpp;

	switch (bpp)
	{
		case 1:
			return *p;
			break;
			
		case 2:
			return *(Uint16 *)p;
			break;
			
		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;
			
			break;
			
		case 4:
			return *(Uint32 *)p;
			break;
	}

	return 0;
}
  

/*!
 * \brief define un pixel (tomado de las referencias de SDL)
 */
void put_pixel(SDL_Surface *_ima, int x, int y, Uint32 pixel)
{
	int bpp = _ima->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)_ima->pixels + y * _ima->pitch + x*bpp;

	switch (bpp)
	{
		case 1:
			*p = pixel;
			break;
			
		case 2:
			*(Uint16 *)p = pixel;
			break;
			
		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
				p[0]=(pixel >> 16) & 0xff;
				p[1]=(pixel >> 8) & 0xff;
				p[2]=pixel & 0xff;
			}
			else
			{
				p[0]=pixel & 0xff;
				p[1]=(pixel >> 8) & 0xff;
				p[2]=(pixel >> 16) & 0xff;
			}
			break;
			
		case 4:
			*(Uint32 *) p = pixel;
			break;
	}
}


/*!
 * \brief obtiene los componentes de color (rojo, verde, azul y alpha) de un pixel de la imagen
 */
void obtener_componentes(SDL_Surface *ima, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a, int x, int y)
{
	Uint8 *buffer = (Uint8*) ima->pixels + ima->pitch*y + x*ima->format->BytesPerPixel;
	Uint32 *p;

	p = (Uint32*) buffer;
	
	SDL_GetRGBA(*p, ima->format, r, g, b, a);
}



/*!
 * \brief reduce una imagen a la mitad
 *
 * \return imagen reducida
 */
SDL_Surface *reescalar_mitad(SDL_Surface *ima)
{
	SDL_Surface *ima2;

	ima2 = SDL_CreateRGBSurface(SDL_SWSURFACE, ima->w/2, ima->h/2, ima->format->BitsPerPixel, ima->format->Rmask, ima->format->Gmask, ima->format->Bmask, ima->format->Amask);

	if (ima2==NULL)
	{
		printf(_("Can't copy the image\n"));
		return NULL;
	}

	SDL_LockSurface(ima);
	SDL_LockSurface(ima2);

	
	for (int i=0; i<ima2->w; i++)
	{
		for (int j=0; j<ima2->h; j++)
		{
			Uint8 r, g, b, a;
			Uint32 r_aux=0, g_aux=0, b_aux=0, a_aux=0;

			obtener_componentes(ima, &r, &g, &b, &a, i*2, j*2);
	
			r_aux += r;
			g_aux += g;
			b_aux += b;
			a_aux += a;
			
			obtener_componentes(ima, &r, &g, &b, &a, i*2+1, j*2);

			r_aux += r;
			g_aux += g;
			b_aux += b;
			a_aux += a;
		
			obtener_componentes(ima, &r, &g, &b, &a, i*2, j*2+1);

			r_aux += r;
			g_aux += g;
			b_aux += b;
			a_aux += a;
			
			obtener_componentes(ima, &r, &g, &b, &a, i*2+1, j*2+1);
			
			r_aux += r;
			g_aux += g;
			b_aux += b;
			a_aux += a;

			r = (Uint8) (r_aux/4);
			g = (Uint8) (g_aux/4);
			b = (Uint8) (b_aux/4);
			a = (Uint8) (a_aux/4);

			put_pixel(ima2, i, j, SDL_MapRGBA(ima->format, r, g, b, a));
		}
	}
	
	SDL_UnlockSurface(ima);
	SDL_UnlockSurface(ima2);

	SDL_FreeSurface(ima);	

	return ima2;
}

/*!
 * \brief define records por defecto (si no se pueden cargar desde el archivo)
 */
void cargar_marcas_estandar(void)
{
	struct entrada vec_marcas[7];
	char ruta_completa[100];
	FILE *arch;

#ifdef WIN32
	strcpy(ruta_completa, "marcas.dat");
#else
	strcpy(ruta_completa, getenv("HOME"));
	strcat(ruta_completa, "/");
	strcat(ruta_completa, ".ceferinomarcas");
#endif
	strcpy(vec_marcas[0].nombre, "matar bros");
	strcpy(vec_marcas[1].nombre, "pepe");
	strcpy(vec_marcas[2].nombre, "kenny");
	strcpy(vec_marcas[3].nombre, "vaca");
	strcpy(vec_marcas[4].nombre, "martian");
	strcpy(vec_marcas[5].nombre, "raton");
	strcpy(vec_marcas[6].nombre, "toto");

	vec_marcas[0].puntos = 500;
	vec_marcas[1].puntos = 450;
	vec_marcas[2].puntos = 425;
	vec_marcas[3].puntos = 413;
	vec_marcas[4].puntos = 411;
	vec_marcas[5].puntos = 300;
	vec_marcas[6].puntos = 299;

	arch = fopen(ruta_completa, "wb");

	if (arch)
	{
		printf(_("Creating the marcs file '%s'\n"), ruta_completa);
		fwrite(vec_marcas, sizeof(struct entrada), 7, arch);
		fclose(arch);
	}
	else
		printf(_("Can't create the marcs file '%s'\n"), ruta_completa);

}
