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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mundo.h"
#include "utils.h"
#include "opciones.h"
#include "int.h"

#ifdef HAVE_CONFIG_H
	#include "../config.h"
#endif

mundo :: mundo()
{
	actual=NULL;
	salir=0;
	leer_teclas=1;
}


/*!
 * \brief inicializa la biblioteca y los recursos b?icos
 *
 * \return 1 en caso de error
 */
int mundo :: iniciar (void)
{
	int flags=0;
	SDL_Surface *ico;

	printf(_("+ Starting the SDL library:"));

	if (SDL_Init(SDL_INIT_VIDEO))
	{
		imprimir_error();
		printf("error: %s\n", SDL_GetError());
		return 1;
	}
	
	imprimir_ok();

#ifndef WIN32
	flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
#else
	// fullscreen bugfix
	flags = SDL_SWSURFACE;
#endif
	
	if (opciones.pantalla_completa)
		flags |= SDL_FULLSCREEN;

	if (opciones.modo_video == 0)
		screen = SDL_SetVideoMode(640, 480, 16,  flags);
	else
		screen = SDL_SetVideoMode(320, 240, 16,  flags);

	fuente = new fuente2 (opciones.modo_video);

	
	if (screen == NULL)
	{
		printf("error: %s\n", SDL_GetError());
		return 1;
	}

	ico = IMG_Load ("./data/ima/icono.png");
		
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("Don Ceferino Haza? - ver : 1.0", NULL);
	
	if (ico)
	{
		SDL_WM_SetIcon(ico, NULL);
		SDL_FreeSurface(ico);
	}

	imprimir_cargando();
	
	libgrafico.iniciar(opciones.modo_video);
	
	audio.iniciar(opciones.sonido, opciones.musica);

	escena_anterior=-1;
	escena=0;

	cambiar_escena(INTRO);
	return 0;
}


/*!
 * \brief altera la escena actual
 */
void mundo :: cambiar_escena(int nueva_escena)
{
	escena = nueva_escena;
}

/*!
 * \brief mantiene la velocidad constante del juego
 */
void mundo :: correr (void)
{
	SDL_Event evento;
	int fps=0;
	int log=0;
	int tframe = -100; // tick del ?timo fps actualizado
	int rep, i;
	
	#define TICK_POR_LOGICO (1000/100)
	#define TICK_POR_FRAME (1000/100)

	reiniciar_reloj();

	while (!salir)
	{
		if (escena != escena_anterior)
			intercambiar_escenas();

		
		salir = procesar_eventos(&evento);
		t = SDL_GetTicks();

		/*
		if (t - tframe > 1000)	// cuadros por segundo
		{
//			printf("----------\nfps %d\nlogicos %d\n", fps, log);
			tframe = t;
			fps=0;
			log=0;
		}
		*/
		
		if (t - tl > TICK_POR_LOGICO)	// l?ica
		{
			rep = (t - tl) / TICK_POR_LOGICO;

			for (i=0; i <rep; i++)
				actual->actualizar();

			tl+=rep*TICK_POR_LOGICO;
			log+=rep;

			if (t - tg > TICK_POR_FRAME)	// grafica
			{
				actual->imprimir();
				fps++;
				tg += TICK_POR_FRAME;
			}

		}
		else
			SDL_Delay(TICK_POR_LOGICO - (t-tl));
		

	}
}


/*!
 * \brief atiende los eventos de la ventana
 * 
 * \return 1 si la aplicacion debe terminar
 */
int mundo :: procesar_eventos(SDL_Event *evento)
{
	while (SDL_PollEvent(evento))
	{
		switch (evento->type)
		{
			case SDL_QUIT:
				return 1;
				break;
				
			case SDL_KEYDOWN:
				
				if (leer_teclas)
				{
					if (evento->key.keysym.sym == SDLK_f)
					{
						SDL_WM_ToggleFullScreen(screen);
						reiniciar_reloj();
					}
				}
				break;

			case SDL_ACTIVEEVENT:
				
				if (evento->active.state == SDL_APPINPUTFOCUS)
				{
					if (! evento->active.gain)
						actual->pausar();
				}
					
						
				break;
		}
	}
	
	return 0;
}


/*!
 * \brief libera recursos y termina la ejecucion del programa
 */
void mundo :: terminar(void)
{
	salir=1;
}

/*!
 * \brief termina el programa
 */
void mundo :: eliminar(void)
{
	if (actual)
		delete actual;
	
	SDL_FreeSurface(screen);
	SDL_Quit();
}

/*!
 * \brief Lee las opciones del archivo 'ruta'
 *
 * \return 1 en caso de error
 */
int mundo :: cargar_opciones(const char *ruta)
{
	FILE *arch;
	char buffer[200];
	char ruta_completa[100];
	
	char *nombre;
	char *valor;
	
	char etiquetas[4][20] = {"musica", "sonido", "pantalla_completa", "modo_video"};
	int *registros[4];
	int num_opciones=4;
	
	int i;
	int tmp_encuentra;

	registros[0] = &(opciones.musica);
	registros[1] = &(opciones.sonido);
	registros[2] = &(opciones.pantalla_completa);
	registros[3] = &(opciones.modo_video);

#ifdef WIN32
	strcpy(ruta_completa, "opciones.txt");
#else
	strcpy(ruta_completa, getenv("HOME"));
	strcat(ruta_completa, "/");
	strcat(ruta_completa, ruta);
#endif

	if ((arch = fopen(ruta_completa, "rt")) == NULL)
	{
		printf(_("It doesn't exist the configuration file '%s',"\
					"execute 'ceferinosetup':\n"),\
					ruta_completa);
		opciones.musica=1;
		opciones.sonido=1;
		opciones.pantalla_completa=0;
		opciones.modo_video=0;
		
		return 0;
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
				printf(_("Incorrect '%s' parameter\n"),\
						nombre);
				fclose(arch);
				return 1;
			}

			
		}
	}

	fclose(arch);
	return 0;
}


/*!
 * \brief reinicia los contadores de tiempo, por ejemplo para evitar saltos luego de cargar imagenes
 */
void mundo :: reiniciar_reloj(void)
{
	t = tl = tg = SDL_GetTicks();
}


/*!
 * \brief elimina una escena y carga otra
 */
void mundo :: intercambiar_escenas(void)
{
	if (actual)
		delete actual;
	
	switch (escena)
	{
		case INTRO:
			actual = new class intro;
			break;

		case MENU:
			actual = new class menu;
			break;

		case JUEGO:
			actual = new class juego;
			break;
			
		case CREDITOS:
			actual = new class creditos;
			break;
			
		case MARCAS:
			actual = new class marcas;
			break;

		case FINAL:
			actual = new class final;
			break;

		case COMOJUGAR:
			actual = new class comojugar;
			break;

		case OPCIONES:
			actual = new class opciones;
			break;
	}

	if (actual->iniciar(this, opciones.modo_video, screen))
	{
		printf(_("Error at changing scene\n"));
		terminar();
	}
	
	escena_anterior = escena;
	reiniciar_reloj();
	
}

/*!
 * \brief deja de procesar las teclas q y f
 */
void mundo :: deshabilitar_letras (void)
{
	leer_teclas = 0;
}

/*!
 * \brief comienza a procesar las teclas q y f como eventos
 */
void mundo :: habilitar_letras (void)
{
	leer_teclas = 1;
}


/*!
 * \brief imprime el primer mensaje 'cargando...'
 */
void mundo :: imprimir_cargando (void)
{
	SDL_Surface *ima;

	ima = IMG_Load ("./data/ima/cargando.png");

	if (ima)
	{
		SDL_BlitSurface(ima, NULL, screen, NULL);
		SDL_Flip(screen);
	}
	else
	{
		printf(_("Can't find the file '%s'\n"), \
				"./data/ima/cargando.png");
	}
}


void mundo :: reiniciar (void)
{
	printf ("Reiniciando\n");
}

void mundo :: pantalla_completa (void)
{
}
