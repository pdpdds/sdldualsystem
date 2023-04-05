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
#include <errno.h>
#include <stdlib.h>
#include "grafico.h"
#include "main_ed.h"
#include "utils.h"


/*!
 * \brief Programa principal del editor de niveles
 *
 * \return 1 en caso de error
 */
int main (int argc, char * argv [])
{
	SDL_Surface *screen;
	grafico bloques;
	grafico panel;
	struct mouse mouse;
	struct barra barra;
	FILE *arch;
	int salir=0;
	SDL_Event evento;
	char mapa[14][20];

	mouse.item=0;
	barra.offset=0;
	barra.nivel=1;
	barra.estado=1;

	bloques.iniciar("niveles.png", 3, 7, 0, 0, 0);
	panel.iniciar("editor.png", 3, 10, 0, 0, 0);
	
	if (iniciar(&screen, &arch))
		return 1;

	cargar_nivel(mapa, arch, barra.nivel);
	imprimir_todo(screen, mapa, &bloques, &panel, &barra);
	
	while (!salir)
	{
		while (SDL_PollEvent(&evento))
		{
			if (evento.type == SDL_QUIT)
				salir=1;
			
			if (evento.type == SDL_KEYDOWN)
			{
				if (evento.key.keysym.sym == SDLK_q || evento.key.keysym.sym == SDLK_ESCAPE)
					salir=1;

				if (evento.key.keysym.sym == SDLK_f)
					SDL_WM_ToggleFullScreen(screen);
			}
		}

		actualizar_mouse(&mouse, screen, mapa, &bloques, &panel, &barra, arch, &salir);
	}

	guardar_nivel(mapa, arch, barra.nivel);
	fclose(arch);
	SDL_FreeSurface(screen);
	SDL_Quit();
	return 0;
}


/*!
 * \brief crea la ventana e inicializa la biblioteca
 *
 * \param screen pantalla principal
 * \param arch archivo binario a modificar
 *
 * \return 1 en caso de error
 */
int iniciar(SDL_Surface **screen, FILE **arch)
{
	char tmp[1024];
	char tmp_original[1024] =  "./data/levels/base.map";
	
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		printf("error: %s\n", SDL_GetError());
		return 1;
	}

	*screen = SDL_SetVideoMode(640, 480, 16,  SDL_HWSURFACE);

	if (*screen == NULL)
	{
		printf("error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_WM_SetCaption("Editor de niveles para el juego Don Ceferino Haza? - Losers", NULL);

	
#ifdef WIN32
	strcpy(tmp, "levels/base.map");
#else
	strcpy(tmp, getenv("HOME"));
	strcat(tmp, "/");
	strcat(tmp, ".ceferinoniveles.map");
#endif
	*arch = fopen(tmp, "rb+");

	if (!*arch)
	{
		printf("No se puede abrir el archivo '%s'.\n", tmp);
		printf("Creando una copia en %s a partir de %s\n", tmp, \
				tmp_original);

		copiar(tmp_original, tmp);
		
		*arch = fopen(tmp, "rb+");

		if (!*arch)
		{
			printf("No se puede abrir el archivo '%s'.\n");
			return 1;
		}
	}

	return 0;
}

/*!
 * \brief imprime la barra inferior
 *
 * \param screen pantalla principal
 * \param bloques imagenes de tiles (pisos, escaleras)
 * \param panel imagenes del panel (botones, items)
 * \param barra estados y numero de nivel
 */
void imprimir_barra(SDL_Surface *screen, grafico *bloques, grafico *panel, struct barra *barra)
{
	SDL_Rect rect = {0, 480-32, 640, 32};
	SDL_Rect tmp;
	int i;
	int x;

	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 120, 120, 120));

	panel->imprimir(19, screen, &tmp, 0, 480-32, 1);
	panel->imprimir((barra->nivel)/10+20, screen, &tmp, 32, 480-32, 1);
	panel->imprimir((barra->nivel)%10+20, screen, &tmp, 64, 480-32, 1);

	panel->imprimir(16, screen, &tmp, 224-96, 480-32, 1);
	panel->imprimir(17, screen, &tmp, 224-64, 480-32, 1);
	panel->imprimir(18, screen, &tmp, 224-32, 480-32, 1);
	
	if (barra->estado==1)
	{
		for (i= barra->offset, x=224; x<640; x+=32, i++)
			bloques->imprimir(i, screen, &tmp, x, 480-32,1);
	}
	else
	{
		for (i= barra->offset, x=224; x<640; x+=32, i++)
			panel->imprimir(i, screen, &tmp, x, 480-32,1);
	}

	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
}

/*!
 * \brief define un bloque del nivel
 * 
 * \param mouse estado del mouse
 * \param screen pantalla principal
 * \param mapa bloques del nivel
 * \param graficos imagenes de los bloques del nivel
 */
void set_bloque(struct mouse *mouse, SDL_Surface *screen, char mapa[][20], grafico *graficos)
{
	mapa[mouse->f][mouse->c] = mouse->item;
	imprimir_bloque(screen, graficos, mouse->f, mouse->c, mouse->item);
}

/*!
 * \brief define un objeto para el nivel
 * 
 * \param mouse estado del mouse
 * \param screen pantalla principal
 * \param mapa bloques del nivel
 * \param objetos imagenes de los objetos del nivel
 */
void set_objeto(struct mouse *mouse, SDL_Surface *screen, char mapa[][20], grafico *objetos)
{
	// si imprime el protagonista elimina cualquier copia
	if (mouse->item == 0)
		eliminar_protagonista(screen, mapa);
				
	mapa[mouse->f][mouse->c] = mouse->item + 'a';
	imprimir_bloque(screen, objetos, mouse->f, mouse->c, mouse->item);
}

/*!
 * \brief limpia un bloque
 * 
 * \param mouse estado del mouse
 * \param screen pantalla principal
 * \param mapa bloques del nivel
 */
void set_bloque_limpio(struct mouse *mouse, SDL_Surface *screen, char mapa[][20])
{
	mapa[mouse->f][mouse->c] = '-';
	limpiar_bloque(screen, mouse->f, mouse->c);
}

/*!
 * \brief imprime sobre la pantalla
 *
 * \param screen pantalla principal
 * \param graficos imagenes de bloques
 * \param f fila
 * \param c columna
 * \param indice numero de grafico
 */
void imprimir_bloque(SDL_Surface *screen, grafico *graficos, int f, int c, int indice)
{
	SDL_Rect tmp;
	
	tmp.x = c*32;
	tmp.y = f*32;
	tmp.w = tmp.h = 32;
	SDL_FillRect(screen, &tmp, SDL_MapRGB(screen->format, 80,80,80));

	graficos->imprimir(indice, screen, &tmp, c*32, f*32, 1);
	SDL_UpdateRect(screen, tmp.x, tmp.y, tmp.w, tmp.h);
}

/*!
 * \brief imprime un rectangulo de color solido
 *
 * \param screen pantalla principal
 * \param f fila
 * \param c columna
 */
void limpiar_bloque(SDL_Surface *screen, int f, int c)
{
	SDL_Rect tmp;
	
	tmp.x = c*32;
	tmp.y = f*32;
	tmp.w = tmp.h = 32;
	SDL_FillRect(screen, &tmp, SDL_MapRGB(screen->format, 80,80,80));
	SDL_UpdateRect(screen, tmp.x, tmp.y, tmp.w, tmp.h);
}

/*!
 * \brief imprime el nivel completo y la barra inferior
 *
 * \param screen pantalla principal
 * \param mapa bloques del nivel
 * \param graficos imagenes de los bloques del nivel
 * \param panel imagenes del panel
 * \param barra estado y numero de nivel
 */
void imprimir_todo(SDL_Surface *screen, char mapa[][20], grafico *graficos, grafico *panel, struct barra *barra)
{
	int f,c;
	SDL_Rect rect;
	
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 80, 80, 80));
	
	for (f=0; f<14; f++)
	{
		for (c=0; c<20; c++)
		{
			if (mapa[f][c] != '-')
			{
				if (mapa[f][c] < 'a')
					graficos->imprimir(mapa[f][c], screen, &rect, c*32, f*32, 1);
				else
					panel->imprimir(mapa[f][c]-'a', screen, &rect, c*32, f*32, 1);

			}
		}
	}

	imprimir_barra(screen, graficos, panel, barra);
	SDL_Flip(screen);
}


/*!
 * \brief mueve la barra con items o bloques
 *
 * \param screen pantalla principal
 * \param graficos imagenes de bloques
 * \param barra estado y numero de nivel
 * \param inc desplazamiento y direccion (+|-)
 */
void mover_barra(SDL_Surface *screen, grafico *graficos, grafico *panel, struct barra *barra, int inc)
{
	// aplica limites a la barra
	if (inc<0)
	{
		if (barra->offset <= 0)
			return;
	}
	else
	{
		if (barra->estado == 0)
		{
			if (barra->offset >= 16 -13)
				return;
		}
		else
			if ( barra->offset >= graficos->get_lim()-13)
				return;
	}

	barra->offset+=inc;
	imprimir_barra(screen, graficos, panel, barra);
}


/*!
 * \brief realiza una accion en base al boton pulsado
 *
 * \param screen pantalla principal
 * \param mouse posicion y estado del mouse
 * \param barra estado y numero de nivel
 * \param bloques imagenes de los bloques del nivel
 * \param panel imagenes de botones e items
 * \param mapa bloques del nivel
 * \param arch archivo binario con los bloques del nivel
 * \param salir control de ejecucion del programa
 */
void seleccionar_barra(SDL_Surface *screen, struct mouse *mouse, struct barra *barra, grafico *bloques, grafico *panel, char mapa[][20], FILE *arch, int *salir)
{
	
	// selecciona un bloque u objeto 
	if (mouse->x > 224)
		mouse->item = barra->offset + ((mouse->x -224)/32);


	//cambio de nivel
	if (mouse->x < 32 && mouse->y > 480-16)
	{
		if (mouse->x < 16)
		{
			ir_nivel_anterior(mapa, arch, &(barra->nivel));
			imprimir_todo(screen, mapa, bloques, panel, barra);
		}
		else
		{
			ir_siguiente_nivel(mapa, arch, &(barra->nivel));
			imprimir_todo(screen, mapa, bloques, panel, barra);
		}
	}


	// mover panel de obj-bloq, y seleccionar obj-bloq
	if (mouse->x > 160 && mouse->x < 224)
	{
		if (mouse->y < 480 -16)	// mover panel
		{
			if (mouse->x < 192)
				mover_barra(screen, bloques, panel, barra, -1);
			else
				mover_barra(screen, bloques, panel, barra, 1);
		}
		else // selecciona obj-tiles
		{
			if (mouse->x < 192)
				barra->estado = 0;
			else
				barra->estado = 1;

			barra->offset = 0;
			imprimir_barra(screen, bloques, panel, barra);
		}
	}

	
	// undo y salir
	if (mouse->x > 128 && mouse->x < 160)
	{
		if (mouse->y < 480-16) // undo = deshacer cambios
		{
			cargar_nivel(mapa, arch, barra->nivel);
			imprimir_todo(screen, mapa, bloques, panel, barra);
		}
		else // salir
		{
			*salir=1;
		}
	}
}


/*!
 * \brief carga la matriz con el contenido del archivo
 *
 * \param mapa matriz con bloques del nivel
 * \param arch archivo binario con los bloques del nivel
 * \param indice numero de nivel a cargar
 */
int cargar_nivel(char mapa[][20], FILE *arch, int indice)
{
	printf("+ Cargando nivel %d: ", indice);
	
	if (fseek(arch, (indice-1)*sizeof(char)*14*20l, SEEK_SET) == -1)
	{
		imprimir_error();
		printf("Error: %s\n", strerror(errno));
		return -1;
	}
	
	if (fread(mapa, sizeof(char), 14*20, arch) > 0)
	{
		imprimir_ok();
		return 1;
	}
	else
	{
		imprimir_error();
		printf("\t error : Fall?la lectura del archivo\n");
		return -1;
	}
}

/*!
 * \brief almacena los bloques de nivel en el archivo
 *
 * \param mapa matriz con bloques del nivel
 * \param arch archivo binario con los bloques del nivel
 * \param indice numero de nivel a cargar
 */
void guardar_nivel(char mapa[][20], FILE *arch, int indice)
{
	printf("+ Guardando el nivel %d ", indice);
	
	fseek(arch, (indice-1)*sizeof(char)*14*20l, SEEK_SET);

	if (fwrite(mapa, sizeof(char), 14*20, arch) > 0)
		imprimir_ok();
	else
		imprimir_error();
}

/*!
 * \brief avanza un nivel
 * \param mapa matriz con bloques del nivel
 * \param arch archivo binario con los bloques del nivel
 * \param indice numero de nivel a cargar
 */
void ir_nivel_anterior(char mapa[][20], FILE *arch, int *nivel)
{
	guardar_nivel(mapa, arch, *nivel);

	if (*nivel > 1 && cargar_nivel(mapa, arch, (*nivel)-1) != -1)
		(*nivel)--;
}

/*!
 * \brief retrocede un nivel
 * \param mapa matriz con bloques del nivel
 * \param arch archivo binario con los bloques del nivel
 * \param indice numero de nivel a cargar
 */
void ir_siguiente_nivel(char mapa[][20], FILE *arch, int *nivel)
{
	guardar_nivel(mapa, arch, *nivel);

	if (cargar_nivel(mapa, arch, (*nivel)+1) != -1)
		(*nivel)++;
}

/*!
 * \brief captura los eventos del mouse
 *
 * \param mouse posicion y estado del mouse
 * \param screen pantalla principal
 * \param mapa bloques del nivel
 * \param bloques imagenes de los bloques del nivel
 * \param panel imagenes de botones e items
 * \param barra estado y numero de nivel
 * \param arch archivo binario con los bloques del nivel
 * \param salir control de ejecucion del programa
 */
void actualizar_mouse(struct mouse *mouse, SDL_Surface *screen, char mapa[][20], grafico *bloques, grafico *panel, struct barra *barra, FILE *arch, int *salir)
{
	// actualiza el mouse
	mouse->botones = SDL_GetMouseState(&mouse->x, &mouse->y);

	mouse->f = mouse->y/32;
	mouse->c = mouse->x/32;

	if (mouse->botones == SDL_BUTTON(1))
	{
		if (mouse->y < 480-32)
		{
			if (barra->estado==1)
				set_bloque(mouse, screen, mapa, bloques);
			else
				set_objeto(mouse, screen, mapa, panel);
		}
		else
			seleccionar_barra(screen, mouse, barra, bloques, panel, mapa, arch, salir);
	}
		
	if (mouse->botones == SDL_BUTTON(3))
	{
		if (mouse->y < 480-32)
			set_bloque_limpio(mouse, screen, mapa);
	}
}

/*!
 * \brief busca y elimina una copia del protagonista
 *
 * \param screen pantalla principal
 * \param mapa bloques del nivel
 */
void eliminar_protagonista(SDL_Surface *screen, char mapa[][20])
{
	int i, j;

	for (i=0; i<14; i++)
		for (j=0; j<20; j++)
		{
			if (mapa[i][j] == 'a')
			{
				mapa[i][j] = '-';
				limpiar_bloque(screen, i, j);
			}
		}
}



void copiar (char *fuente, char *destino)
{
	FILE *in;
	FILE *out;
	unsigned char buffer[1];


	in = fopen(fuente, "rb");

	if (in == NULL)
	{
		printf("No se pueden abrir los niveles originales de '%s'\n", \
				fuente);
		return;
	}
	
	out = fopen(destino, "wb");

	if (out == NULL)
	{
		printf("No se puede crear una copia del nivel en '%s'\n", \
				destino);
	}
	
	while (fread(buffer, sizeof(unsigned char), 1, in))
	{
		fwrite (buffer, sizeof(unsigned char), 1, out);
	}

	printf("Copiando '%s' en '%s', para editar\n", fuente, destino);
}
