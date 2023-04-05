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

#include "fuente2.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "int.h"
#include "utils.h"



fuente2 :: fuente2 (int modo)
{
	char ruta [1024] = "./data/ima/fuente_2.png";

	ima = IMG_Load (ruta);

	if (ima == NULL)
		printf ("error: %s\n", SDL_GetError ());

	if (modo)
	{
		ima = reescalar_mitad(ima);

		if(ima==NULL)
		{
			printf(_("error : error reshaping the image: '%s' "),\
					ruta);
			imprimir_error();
			exit(1);
		}
	}

	modo_reducido = modo;
	letras = 0;

	cargar_identificar_recs ();
	
	printf (_("+ loading: %s\n"), ruta);
}



void fuente2 :: guardar_rect (int i, int x, int y, int w, int h)
{
	rects [i].x = x;
	rects [i].y = y;
	rects [i].w = w;
	rects [i].h = h;
}


bool fuente2 :: es_pixel_opaco (SDL_Surface * ima, int fila, int col)
{
	int bpp = ima->format->BytesPerPixel;
	Uint8 r, g, b, a;
	Uint8 * p; 
	Uint32 color;

	p = (Uint8 *) ima->pixels;
	p += col * bpp + fila * ima->pitch;
	
	switch (bpp)
	{
		case 1:
			color = * p;
			break;

		case 2:
			color = * (Uint16 *) p;
			break;

		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				color = p[0] << 16 | p[1] << 8 | p[2];
			else
				color = p[0] | p[1] << 8 | p[2] << 16;
			break;

		case 4:
			color = * (Uint32 *) p;
			break;

		default:
			printf (_("invalid bpp: %d\n"), bpp);
			break;
	}

	SDL_GetRGBA (color, ima->format, &r, &g, &b, &a);

	if (r == KEY_R && g == KEY_G && b == KEY_B && a == KEY_A)
		return false;
	
	return true;
}


bool fuente2 :: es_columna_vacia (SDL_Surface * ima, int col)
{
	int fila;

	for (fila = 0; fila < (ima->h / 2); fila ++)
	{
		if (es_pixel_opaco (ima, fila, col))
			return false;
	}

	return true;
}


void fuente2 :: cargar_identificar_recs (void)
{
	int i;
	int hay_letra = 0;
	int indice = 0;
	int i_ant;

	for (i = 0; i < ima->w; i++)
	{
		if (es_columna_vacia (ima, i))
		{
			if (hay_letra)
			{
				hay_letra = 0;
				guardar_rect (indice, i_ant, 0, \
						i - i_ant, ima->h / 2);
				indice ++;
			}
		}
		else
		{
			if (!hay_letra)
			{
				hay_letra = 1;
				i_ant = i;
			}
		}
	}

	letras = indice;
	printf (_("Found %d fonts in bitmap\n"), indice);
}

fuente2 :: ~fuente2 ()
{
	SDL_FreeSurface (ima);
}


SDL_Rect fuente2 :: obtener_referencia (char letra, bool transparente)
{
	static SDL_Rect src;
	static char letras [] = \
				"!\"#$%&'()*+,-./0123456789:;<=>?" \
				"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`" \
				"abcdefghijklmnopqrstuvwxyz" \
			//	"{|}~¡¢£€¥Š§š©ª«¬­®¯°±²³Žµ¶·ž¹º»ŒœŸ¿" \
				//"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäå" \
			//	"æçèéêëìíîïðñòóôõö÷øùúûüýþÿ";
				"àèìòùâêîôû";

	if (letra == ' ' || letra == '\0' || letra == '\n')
	{
		src = rects [0];
		return src;
	}

	for (int i = 0; letras [i] != '\0'; i ++)
	{
		if (letras [i] == letra)
		{
			if (transparente)
			{
				src = rects [i];
				src.y += src.h;
				return src;
			}
			else
			{
				src = rects [i];
				return src;
			}
		}
	}

	printf (_("'%c' not found in font tiles\n"), letra);
	src = rects [0];
	return src;
}


int fuente2 :: imprimir_letra (SDL_Surface * destino, char letra, int x, int y, SDL_Rect * rect, bool transparente)
{
	SDL_Rect dst = {x, y, 0, 0};
	SDL_Rect src;

	if (modo_reducido)
	{
		dst.x /= 2;
		dst.y /= 2;
	}
	
	src = obtener_referencia (letra, transparente);
	
	if (letra == ' ')
	{
		if (rect)
			(*rect) = dst;
		
		return src.w;
	}

	src = obtener_referencia (letra, transparente);
	SDL_BlitSurface (ima, & src, destino, & dst);

	if (rect)
		(*rect) = dst;
		
	return get_w (letra);
}


void fuente2 :: myprintf (SDL_Surface * dst, int x, int y, SDL_Rect * rect, \
		int * nrect, bool transparente, enum posiciones posicion, \
		const char * format, ...)
{
	char buffer [1024];
	va_list va;	

	if (! format)
		return;
	
	va_start (va, format);
	vsprintf (buffer, format, va);


	switch (posicion)
	{
		case CONSALTO:
			imprimir_con_salto (dst, buffer, x, y, rect, nrect, \
					transparente);
			break;
			
		case NINGUNO:
			imprimir (dst, buffer, x, y, rect, nrect, transparente);
			break;

		case CENTRADO:
			x -= (longitud (buffer) / 2);
			imprimir (dst, buffer, x, y, rect, nrect, transparente);
			break;

	}

	va_end (va);
}


void fuente2 :: myprintf_inferior (SDL_Surface * dst, SDL_Rect * rect, \
		int * nrect, bool transparente, const char * format, ...)
{
	char buffer [1024];
	va_list va;
	int x1, y1;

	if (! format)
		return;
	
	va_start (va, format);
	vsprintf (buffer, format, va);

	alinear_inferior (& x1, & y1, buffer);
	myprintf (dst, x1, y1, rect, nrect, transparente, CONSALTO, buffer);

	va_end (va);
}

int fuente2 :: get_h (void)
{
	int altura = (int) (rects [0].h / 1.5);
	
	if (modo_reducido)
		return altura * 2;
	else
		return altura;
	/*
		(modo_reducido)? (rects [0].h * 2) / 1.5: 
		return altura;
	*/
}


/*!
 * \brief busca los valor de x e y para que la cadena se pueda imprimr al pie
 * de la página
 */
void fuente2 :: alinear_inferior (int * x1, int * y1, char * cadena)
{
	int f = 0;
	int c = 20;
	char * palabra = cadena;	
	int limite_w = 640;/*(modo_reducido)? 320: 640;*/
	int limite_h = 480;/*(modo_reducido)? 240: 480;*/

	while (palabra)
	{
		if (c + ancho_palabra (palabra) > limite_w)
		{
			/* salta una linea */
			c = ancho_palabra (palabra) + get_w (' ');
			f += get_h ();
		}
		else
		{
			c += ancho_palabra (palabra) + get_w (' ');
		}
		
		palabra = strstr (palabra, " ");

		if (palabra)
		{
			palabra += 1;
		}
	}

	
	/* se aplica la alineación inferior */
	(* x1) = 20;
	(* y1) = limite_h - (f + get_h () * 2);
}



int fuente2 :: ancho_palabra (char * palabra)
{
	int i;
	int ancho = 0;	

	for (i = 0; palabra [i] != ' ' && palabra [i] != '\0' \
			&& palabra [i] != '\n'; i ++)
	{
		ancho += get_w (palabra [i]);
	}

	return ancho;
}


int fuente2 :: get_w (char letra)
{
	SDL_Rect rect;

	rect = obtener_referencia (letra, false);

	if (modo_reducido)
		return rect.w * 2;
	else
		return rect.w;
}


void fuente2 :: imprimir_con_salto (SDL_Surface * dst, char * cadena, int x, \
		int y, SDL_Rect *rect, int * nrect, bool transparente)
{
	int limite_w = 640;/*(modo_reducido)? 320: 640;*/
	char * palabra = cadena;
	x = 20;

	while (palabra)
	{
		if (x + ancho_palabra (palabra) > limite_w)
		{
			x = 20;
			y += get_h ();
		}

		imprimir_palabra (dst, palabra, x, y, rect, nrect,transparente);
		
		x += ancho_palabra (palabra) + get_w (' ');

		palabra = saltar_palabra (palabra);

		if (palabra)
		{
			if (palabra [0] == '\n')
			{
				x = 20;
				y += get_h ();
			}

			palabra += 1;
		}
	}
}


char * fuente2 :: saltar_palabra (char * cadena)
{
	int i;

	for (i = 0; cadena [i]; i ++)
	{
		if (cadena [i] == ' ' || cadena [i] == '\n')
			return cadena + i;
	}

	if (cadena [i] == '\0')
		return NULL;

	return NULL;
}

void fuente2 :: imprimir (SDL_Surface * dst, char * cadena, int x, \
		int y, SDL_Rect *rect, int * nrect, bool transparente)
{
	int dx = 0;

	for (int i = 0; cadena [i]; i++)
	{
		int w;
		
		if (rect)
		{
			w = imprimir_letra (dst, cadena [i], x + dx, y, \
					rect + * nrect, transparente);
		}
		else
		{
			w = imprimir_letra (dst, cadena [i], x + dx, y, NULL, \
					transparente);
		}

		dx += w;

		if (nrect)
		{
			(* nrect) ++;
		}
	}
}



void fuente2 :: imprimir_palabra (SDL_Surface * dst, char * cadena, int x, \
		int y, SDL_Rect *rect, int * nrect, bool transparente)
{
	int i = 0;
	int dx = 0;

	while (cadena [i] && cadena [i] != ' ' && cadena [i] != '\n')
	{
		int w;
		
		if (rect)
		{
			w = imprimir_letra (dst, cadena [i], x + dx, y, \
					rect + * nrect, transparente);
		}
		else
		{
			w = imprimir_letra (dst, cadena [i], x + dx, y, NULL, \
					transparente);
		}

		dx += w;

		if (nrect)
		{
			(* nrect) ++;
		}

		i ++;
	}
}

int fuente2 :: longitud (char * cadena)
{
	int w = 0;

	for (int i = 0; cadena [i]; i ++)
	{
		w += get_w (cadena [i]) + 2;
	}

	return w;
}


