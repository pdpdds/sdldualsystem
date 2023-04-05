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
#include "menu.h"
#include "mundo.h"
#include "grafico.h"
#include "utils.h"
#include "int.h"

menu :: menu(void)
{
	lim_todos = 0;
	opcion = 0;
	tecla_pulsada = 0;
	titulo_activo = 1;
}

menu :: ~menu(void)
{
	int i;

	for (i = 0; i < 6; i ++)
		delete item_menu [i];
	
	for (i = 0; i < 3; i ++)
		delete simple_sprite [i];
	
	SDL_FreeSurface (fondo);
}


/*!
 * \brief genera los componentes del menu
 *
 * \return 1 si falla, 0 en caso contrario 
 */
int menu :: iniciar(class mundo *_pmundo, int _modo_video, SDL_Surface *_screen)
{
	SDL_Rect rect;
	class fuente2 * fuente = _pmundo->fuente;
	
	pmundo = _pmundo;
	modo_video = _modo_video;
	screen = _screen;

	fondo = SDL_DisplayFormat (_screen);

	if (fondo == NULL)
	{
		printf(_("error: can't create a copy of the screen : '%s'\n"),\
					SDL_GetError());
		return 1;
	}
	
	for (int i = 0; i < 6; i ++)
		item_menu [i] = new class item_menu;

	for (int i = 0; i < 3; i ++)
		simple_sprite [i] = new class simple_sprite;


	item_menu [0]->iniciar (fuente, 0, _("new game"));
	item_menu [1]->iniciar (fuente, 1, _("how to play"));
	item_menu [2]->iniciar (fuente, 2, _("options"));
	item_menu [3]->iniciar (fuente, 3, _("credits"));
	item_menu [4]->iniciar (fuente, 4, _("high scores"));
	item_menu [5]->iniciar (fuente, 5, _("exit"));

	simple_sprite [0]->iniciar(pmundo->libgrafico.ima_tit_1, -594, 616, \
			90, 8);
	
	simple_sprite [1]->iniciar(pmundo->libgrafico.ima_tit_2, 830, 616, \
			230, 0);
	
	simple_sprite [2]->iniciar(pmundo->libgrafico.ima_tit_3, -500, 750, \
			204, 87);
	
	item_menu [0]->seleccionar();
	
	pmundo->libgrafico.ima_menu->imprimir(0, screen, &rect, 0, 0, 1);
	pmundo->libgrafico.ima_menu->imprimir(0, fondo, &rect, 0, 0, 1);
	
	SDL_Flip(screen);

	pmundo->audio.play_musica(1);
	pmundo->reiniciar_reloj();
	return 0;
}


/*!
 * \brief actualizacion logica del menu
 */
void menu :: actualizar(void)
{

	if (termino_titulo ()) // termin?la animaci? del titulo
	{
		for (int i = 0; i < 6; i ++)
			item_menu [i]->actualizar ();

		tecla = SDL_GetKeyState(NULL);

		// mover el cursor
		if (down() && tecla_pulsada == 0)
		{
			item_menu [opcion]->no_seleccionar ();
			
			if (opcion == 5)
				opcion = 0;
			else
				opcion ++;
			
			item_menu [opcion]->seleccionar ();
			
			tecla_pulsada = 1;
			pmundo->audio.play (5);
			
		}

		// mover el cursor
		if (up() && !tecla_pulsada)
		{
			item_menu[opcion]->no_seleccionar();

			if (opcion == 0)
				opcion = 5;
			else
				opcion --;
			
			item_menu[opcion]->seleccionar();
			tecla_pulsada=1;
			pmundo->audio.play(5);
		}


		// selecciona una opcion	
		if (shot())
		{
			switch (opcion)
			{
				case 0:
					pmundo->cambiar_escena(JUEGO);
					break;

				case 1:
					pmundo->cambiar_escena(COMOJUGAR);
					break;

				case 2:
					pmundo->cambiar_escena (OPCIONES);
					break;
					
				case 3:
					pmundo->cambiar_escena(CREDITOS);
					break;

				case 4:
					pmundo->cambiar_escena(MARCAS);
					break;

				case 5:
					pmundo->terminar();
					break;
			}

			pmundo->audio.play(4);
		}

		if (!down() && ! up())
			tecla_pulsada=0;
	}
	else
	{
		simple_sprite[0]->actualizar();
		simple_sprite[1]->actualizar();
		simple_sprite[2]->actualizar();
	}
}


/*!
 * \brief imprime el titulo del juego y las opciones del menu
 */
void menu :: imprimir(void)
{
	int i;

	lim_actual = 0;

	if (termino_titulo())
	{
		if (titulo_activo) // fija el titulo al fondo de pantalla
		{
			for (i = 0; i < 3; i++)
			{
				simple_sprite[i]->imprimir(fondo, rect_actual,\
						&lim_actual);
				simple_sprite[i]->imprimir(screen, rect_actual,\
						&lim_actual);
			}
			
			titulo_activo = 0;
			pmundo->reiniciar_reloj();
		}
	
		for (i = 0; i < 6; i++)
		{
			item_menu [i]->imprimir (screen, rect_actual, \
					& lim_actual);
		}
		
	}
	else
	{
		for (i=0; i<3; i++)
		{
			simple_sprite[i]->imprimir (screen, rect_actual, \
					&lim_actual);
		}
	}

	copiar_rectangulos(rect_todos, &lim_todos, rect_actual, &lim_actual, \
			screen->w, screen->h);
	
	SDL_UpdateRects(screen, lim_todos, rect_todos);

	lim_todos = 0;
	
	copiar_rectangulos (rect_todos, &lim_todos, rect_actual, &lim_actual, \
			screen->w, screen->h);

	// pinta el fondo modificado
	for (i=0; i<lim_actual; i++)
	{
		SDL_BlitSurface(fondo, &(rect_actual[i]), screen, \
				&(rect_actual[i]));
	}


}


/*!
 * \brief informa si la animacion del titulo 'Don Ceferino...' termin? * 
 * \return 1 si termino, 0 en caso contrario
 */
int menu :: termino_titulo(void)
{
	if (simple_sprite[0]->termino_anim() && \
			simple_sprite[1]->termino_anim() && \
			simple_sprite[2]->termino_anim())
	{
		return 1;
	}
	else
		return 0;
}



void menu :: pausar(void)
{
}

bool menu :: shot(void)
{
    return tecla[SDLK_RETURN] or tecla[SDLK_SPACE] or tecla[SDLK_x] or tecla[SDLK_2];
}

bool menu :: up(void)
{
    return tecla[SDLK_UP] or tecla[SDLK_k] or tecla[SDLK_w];
}

bool menu :: down(void)
{
    return tecla[SDLK_DOWN] or tecla[SDLK_j] or tecla[SDLK_s];
}
