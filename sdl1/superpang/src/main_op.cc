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
#include <stdio.h>
#include "main_op.h"


/*!
 * \brief Programa principal del menu de opciones
 *
 * \return 1 en caso de error
 */
int main (int argc, char * argv [])
{
	struct opciones opciones;
	int salir=0;
	SDL_Surface *screen;
	SDL_Surface *fondo;
	SDL_Surface *items;
	int t, tl, tg, repeticiones, i;
	
	if (iniciar(&screen, &fondo, &items))
		return 1;

	cargar_opciones(".ceferino", &opciones);
	imprimir(screen, fondo, items, &opciones);

	SDL_Delay(100);
	t = tl = tg = SDL_GetTicks();
	
	while (!salir)
	{
		t = SDL_GetTicks();
		salir = procesar_eventos();

		if (t - tl > 10) // 1000/fps -> 100 fps aprox.
		{
			repeticiones = (t - tl) / 10;

			for (i=0; i < repeticiones; i++)
				actualizar(&opciones, &salir);
			
			tl=t;
		}
		
		if (t - tg > 10)
		{
			imprimir(screen, fondo, items, &opciones);
			tg=t;
		}
	}

	SDL_FreeSurface(screen);
	SDL_FreeSurface(fondo);
	SDL_FreeSurface(items);
	SDL_Quit();
	return 0;
}

/*!
 * \brief carga todas las imagenes e inicializa la biblioteca
 * 
 * \param screen superficie destino
 * \param fondo imagen de fondo
 * \param items sprites con las opciones
 */
int iniciar(SDL_Surface **screen, SDL_Surface **fondo, SDL_Surface **items)
{
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		printf("error: %s\n", SDL_GetError());
		return 1;
	}

	*screen = SDL_SetVideoMode(320, 240, 16,  SDL_HWSURFACE);

	if (*screen == NULL)
	{
		printf("error: %s\n", SDL_GetError());
		return 1;
	}

	*fondo = IMG_Load ( "./data/ima/fondo.jpg");

	if (*fondo == NULL)
	{
		printf("error: no se encuentra la imagen '" \
				"./data/ima/fondo.jpg'\n");
		return 1;
	}
	
	*items = IMG_Load ("./data/ima/op_items.png");

	if (*items == NULL)
	{
		printf("error: no se encuentra la imagen '" \
				"./data/ima/op_items.png'\n");
		return 1;
	}

	SDL_WM_SetCaption("Menu de Opciones - Losers", NULL);
	SDL_ShowCursor(SDL_DISABLE);
	return 0;
}


/*!
 * \brief lee los controles y actualiza las opciones en memoria
 *
 * \param opciones estructura a modificar
 * \return 1 si las opciones cambiaron
 */
int actualizar(struct opciones *opciones, int *salir)
{
	Uint8 *tecla;
	int opcion_anterior = opciones->opcion_seleccionada;
	int opcion = opciones->opcion_seleccionada;
	int destino = opciones->opcion_seleccionada*30 +40;

	// acerca el cursos a su posicion destino
	if (abs(destino - opciones->pos_y) < 5 && destino != opciones->pos_y)
		opciones->pos_y += (destino -opciones->pos_y) / abs(destino-opciones->pos_y);
	else
		opciones->pos_y += (destino - opciones->pos_y) / 5;

	if (opciones->pos_x < 5 && opciones->pos_x != 0)
		opciones->pos_x --;
	else
		opciones->pos_x -= (opciones->pos_x) / 5;
	
	// espera para recibir ordenes desde el teclado	
	if (opciones->delay > 0)
	{
		opciones->delay--;
		return 0;
	}
	
	tecla = SDL_GetKeyState(NULL);

	if (tecla[SDLK_DOWN])
		opcion++;

	if (tecla[SDLK_UP])
		opcion--;

	if (tecla[SDLK_SPACE] || tecla[SDLK_RETURN])
	{
		switch (opcion)
		{
			case 0:
				cambiar_opcion(&opciones->modo_video, 1);
				break;
			case 1:
				cambiar_opcion(&opciones->pantalla_completa, 1);
				break;
			case 2:
				cambiar_opcion(&opciones->musica, 1);
				break;
			case 3:
				cambiar_opcion(&opciones->sonido, 1);
				break;
			case 4:
				*salir = 1;
				printf("No se guardaron los cambios en el archivo.\n");
				break;
			case 5:
				*salir = 1;
				salir_guardar_cambios(opciones);
				break;
		}

		opciones->pos_x=10;
		opciones->delay=10;
		return 1;
	}

	if (opcion < 0)
		opcion=0;

	if (opcion > 5)
		opcion=5;

	if (opcion_anterior != opcion)
	{
		opciones->delay=10;
		opciones->opcion_seleccionada = opcion;
		return 1;
	}
	else
		return 0;
}


/*!
 * \brief imprime toda la pantalla
 * 
 * \param screen superficie destino
 * \param fondo imagen de fondo
 * \param items sprites con las opciones
 * \param opciones datos a imprimir
 */
void imprimir(SDL_Surface *screen, SDL_Surface *fondo, SDL_Surface *items, struct opciones *opciones)
{
	
	SDL_BlitSurface(fondo, NULL, screen, NULL);

	
	// imprime las opciones
	pintar_imagen(screen, items, 5, 20, 40);
	pintar_imagen(screen, items, 0, 20, 40+30);
	pintar_imagen(screen, items, 3, 20, 40+30*2);
	pintar_imagen(screen, items, 4, 20, 40+30*3);
	pintar_imagen(screen, items, 6, 20, 40+30*4);
	pintar_imagen(screen, items, 7, 20, 40+30*5);

	// imprime los valores
	if (opciones->modo_video == 0)
		pintar_imagen(screen, items, 9, 150+20, 40);
	else
		pintar_imagen(screen, items, 8, 150+20, 40);

	if (opciones->pantalla_completa)
		pintar_imagen(screen, items, 1, 150+20, 40+30);
	else
		pintar_imagen(screen, items, 2, 150+20, 40+30);

	if (opciones->musica)
		pintar_imagen(screen, items, 1, 150+20,40+30*2);
	else
		pintar_imagen(screen, items, 2, 150+20, 40+30*2);

	if (opciones->sonido)
		pintar_imagen(screen, items, 1, 150+20, 40+30*3);
	else
		pintar_imagen(screen, items, 2, 150+20, 40+30*3);

	// imprime el cursor
	pintar_imagen(screen, items, 10, opciones->pos_x, opciones->pos_y);
	SDL_Flip(screen);
}

/*!
 * \brief carga todos los datos iniciales para opciones, intenta cargarlos desde un archivo
 *
 * \param ruta archivo que contiene los datos iniciales
 * \param opciones estructura destino
 */
int cargar_opciones(const char *ruta, struct opciones *opciones)
{
	FILE *arch;
	char buffer[200];
	char archivo[100];
	
	char *nombre;
	char *valor;
	
	char etiquetas[4][20] = {"musica", "sonido", "pantalla_completa", "modo_video"};
	int *registros[4];
	int num_opciones=4;
	
	int i;
	int tmp_encuentra;

	registros[0] = &(opciones->musica);
	registros[1] = &(opciones->sonido);
	registros[2] = &(opciones->pantalla_completa);
	registros[3] = &(opciones->modo_video);
	
	// carga los valores estandar
	*(registros[0]) = 1;
	*(registros[1]) = 1;
	*(registros[2]) = 0;
	*(registros[3]) = 0;
	opciones->opcion_seleccionada=0;
	opciones->pos_x=0;
	opciones->pos_y=40;
	opciones->delay=0;

#ifdef WIN32
	strcpy(archivo, "opciones.txt");
#else
	strcpy(archivo, getenv("HOME"));
	strcat(archivo, "/");
	strcat(archivo, ruta);
#endif
	
	if ((arch = fopen(archivo, "rt")) == NULL)
	{
		fprintf(stderr, "No se puede abrir el archivo de configuracion '%s'\n", archivo);
		return 1;
	}
	
	while (fgets(buffer, 200, arch))
	{
		if (buffer[0] != '#' && buffer[0] != '\n')
		{
			nombre = strtok(buffer, "=");
			valor = strtok(NULL, " =;\n#");
			
			tmp_encuentra = 0;

			for (i=0; i< num_opciones; i++)
			{
				if (strcmp(nombre, etiquetas[i]) == 0)
				{
					tmp_encuentra = 1;
					*(registros[i]) = atoi(valor);
				}
			}
			
			if (tmp_encuentra == 0)
			{
				printf("La opci? '%s' no es valida\n", nombre);
				fclose(arch);
				return 1;
			}

			
		}
	}
	
	fclose(arch);
	return 0;
}


/*!
 * \brief imprime una imagen de la plancha de items
 *
 * \param screen superficie destino
 * \param items plancha de graficos
 * \param indice numero de imagen a imprimir
 * \param x coordenada x
 * \param y coordenada y
 */
void pintar_imagen(SDL_Surface *screen, SDL_Surface *items, int indice, int x, int y)
{
	SDL_Rect src;
	SDL_Rect dst;
	
	src.x=0;
	src.y=indice*30;
	src.w=140;
	src.h=30;
	
	dst.x=x;
	dst.y=y;
	dst.w=140;
	dst.h=30;
	
	SDL_BlitSurface(items, &src, screen, &dst);
}


/*!
 * \brief Gestina los eventos de la aplicacion
 *
 * \return 1 si el usuario termina el programa
 */
int procesar_eventos(void)
{
	SDL_Event evento;

	while (SDL_PollEvent(&evento))
	{
		switch (evento.type)
		{
			case SDL_QUIT:
				return 1;
				break;
					
			case SDL_KEYDOWN:
				if (evento.key.keysym.sym == SDLK_q)
					return 1;
				break;
		}
	}

	return 0;
}


/*!
 * \brief altera el valor de un numero entero (acepta un maximo)
 *
 * \param pnumero numero a modificar
 * \param maximo_valor valor limite, superado este valor asigna 0
 */
void cambiar_opcion(int *pnumero, int maximo_valor)
{
	(*pnumero)++;

	if ((*pnumero) > maximo_valor)
		(*pnumero) = 0;
}



/*!
 * \brief guarda los datos de memoria en el archivo de configuracion
 *
 * \param opciones datos a guardar
 */
void salir_guardar_cambios(struct opciones *opciones)
{
	FILE *arch;
	char cadena[100];

#ifdef WIN32
	strcpy(cadena, "opciones.txt");
#else
	strcpy(cadena, getenv("HOME"));
	strcat(cadena, "/.ceferino");
#endif
	
	arch = fopen(cadena, "wt");

	if (arch == NULL)
	{
		printf("No se puede abrir '%s' para escritura\n", cadena);
		printf("No se guardaron los cambios en el archivo\n");
		return;
	}
	
	fprintf(arch, "# Archivo de configuracion\n");
	fprintf(arch, "#\n");
	fprintf(arch, "# 1 = habilitado\n");
	fprintf(arch, "# 0 = deshabilitado\n");
	fprintf(arch, "# en 'modo_video' 0 alpha+640x480, 1 alpha+320x240\n");
	fprintf(arch, "\n");
	fprintf(arch, "musica=%d\n", opciones->musica);
	fprintf(arch, "sonido=%d\n", opciones->sonido);
	fprintf(arch, "pantalla_completa=%d\n", opciones->pantalla_completa);
	fprintf(arch, "modo_video=%d\n", opciones->modo_video);

	printf("Se guadaron todos los cambios en '%s'\n", cadena);
	
	fclose(arch);
}
