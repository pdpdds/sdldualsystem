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


#include <string.h>
#include "procesos.h"
#include "math.h"
#include "utils.h"
#include "int.h"

#define colisiona(posy) sqrt( pow(x-(xg+10), 2) + pow((y+posy)-yg,2) ) <= d + 20

procesos :: procesos()
{
	gaucho = NULL;
	lista_fuente = NULL;
	lista_pelota = NULL;
	lista_tiro = NULL;
	lista_bloque = NULL;
	lista_item = NULL;
	lista_bomba = NULL;

	hab_gaucho=1;
	hab_enemigos=1;
	hab_tiro=1;
	pjuego = NULL;
}


/*!
 * \brief asigna los valores iniciales para los procesos
 */
void procesos :: iniciar(class juego *_pjuego)
{
	pjuego = _pjuego;
}

/*!
 * \brief genera el protagonista del juego
 *
 * \param grafico plancha de sprites
 * \param x coordenada inicial
 * \param y coordenada inicial
 */
void procesos :: crear_gaucho(grafico *grafico, int x, int y)
{	
	if (gaucho != NULL)
	{
		printf(_("error, already exist a cowboy in the scene, "\
					"verify the level created\n"));
		exit(-1);
	}
	
	gaucho = new class gaucho;
	gaucho->iniciar(pjuego, grafico, x, y);
}


/*!
 * \brief crea una pelota
 *
 * \param grafico plancha de sprites
 * \param x coordenada inicial
 * \param y coordenada inicial
 * \param tam tama? inicial
 * \param flip direcci? inicial (1 derecha, -1 izquierda)
 */
void procesos :: crear_pelota(grafico *grafico, int x, int y, int tam, int flip)
{
	struct nodo <class pelota> *pnuevo;

	pnuevo = new struct nodo <class pelota>;
	pnuevo->obj = new class pelota;
	pnuevo->obj->iniciar(pjuego, grafico, x, y, tam, flip);

	agregar(pnuevo, &lista_pelota);
}


/*!
 * \brief enviar un tiro
 *
 * \param grafico plancha de sprites
 * \param x coordenada inicial
 * \param y coordenada inicial
 * \param tipo_tiro 0 es normal y 1 es tridente
 */
void procesos :: crear_tiro(grafico *grafico, int x, int y, int tipo_tiro)
{
	struct nodo <class tiro> *pnuevo;

	pnuevo = new struct nodo <class tiro>;
	pnuevo->obj = new class tiro;
	pnuevo->obj->iniciar(pjuego, grafico, x, y, tipo_tiro);

	agregar(pnuevo, &lista_tiro);
}


/*!
 * \brief genera un item
 *
 * \param grafico plancha de sprites
 * \param x coordenada inicial
 * \param y coordenada inicial
 */
void procesos :: crear_item(grafico *ima_item, int x, int y, int tipo)
{
	struct nodo <class item> *pnuevo;

	pnuevo = new struct nodo <class item>;
	pnuevo->obj = new class item;
	pnuevo->obj->iniciar(pjuego, ima_item, x, y, tipo);
	
	agregar(pnuevo, &lista_item);
}


/*!
 * \brief genera un bloque que se puede romper
 *
 * \param grafico plancha de sprites
 * \param x coordenada inicial
 * \param y coordenada inicial
 */
void procesos :: crear_bloque_romper(grafico *ima_item, int x, int y)
{
	struct nodo <class bloque> *pnuevo;

	pnuevo = new struct nodo <class bloque>;
	pnuevo->obj = new class bloque;
	pnuevo->obj->iniciar(pjuego, ima_item, x, y);

	agregar(pnuevo, &lista_bloque);
}


/*!
 * \brief genera un mensaje en la pantalla
 *
 * \param letras imagenes de las fuentes
 * \param cadena mensaje a imprimir
 */
void procesos :: crear_mensaje(class fuente2 * fuente, const char *cadena)
{
	struct nodo <class fuente> * pnuevo;

	pnuevo = new struct nodo <class fuente>;
	pnuevo->obj = new class fuente;
	pnuevo->obj->iniciar (fuente, 320, 200, cadena, 1);
	agregar (pnuevo, &lista_fuente);
}

/*!
 * \brief genera un mensaje en la pantalla
 *
 * \param letras imagenes de las fuentes
 * \param cadena mensaje a imprimir
 */
void procesos :: crear_mensaje_rapido (class fuente2 * fuente, char * cadena)
{
	struct nodo <class fuente> * pnuevo;

	pnuevo = new struct nodo <class fuente>;
	pnuevo->obj = new class fuente;
	pnuevo->obj->iniciar (fuente, 320, 200, cadena, 0);
	agregar (pnuevo, &lista_fuente);
}


/*!
 * \brief genera una bomba que reduce el tama? de los enemigos
 */
void procesos :: crear_bomba(class grafico *ima, int x, int y, int flip)
{
	struct nodo <class bomba> *pnuevo;

	pnuevo = new struct nodo <class bomba>;
	pnuevo->obj = new class bomba;
	pnuevo->obj->iniciar(pjuego, ima, x, y, flip);
	
	agregar(pnuevo, &lista_bomba);
}


/*!
 * \brief realiza la actualizaci? l?ica de todos los procesos almacenados
 */
void procesos :: actualizar(void)
{
	struct nodo <class pelota> *pleer;
		
	eliminar_antiguos(&lista_fuente);
	eliminar_antiguos(&lista_pelota);
	eliminar_antiguos(&lista_tiro);
	eliminar_antiguos(&lista_bloque);
	eliminar_antiguos(&lista_item);
	eliminar_antiguos(&lista_bomba);	
	
	actualizar_lista(lista_fuente);
	actualizar_lista(lista_bloque);
	actualizar_lista(lista_item);
	actualizar_lista(lista_bomba);
	
	if (hab_enemigos)
	{
		for (pleer = lista_pelota; pleer; pleer = pleer->psgte)
			pleer->obj->actualizar(1);
	}
	else
	{
		for (pleer = lista_pelota; pleer; pleer = pleer->psgte)
			pleer->obj->actualizar(0);
	}

	
	if (hab_tiro)
		actualizar_lista(lista_tiro);

	if (hab_gaucho)
		gaucho->actualizar();
}




/*!
 * \brief imprime todos los procesos en la pantalla
 * 
 * \param screen superficie destino
 * \param modif almacena todos los rectangulos modificados
 * \param lim limite de rectangulos modificados
 */
void procesos :: imprimir(SDL_Surface *destino, SDL_Rect *modif, int *lim)
{
	imprimir_lista(destino, modif, lim, lista_bloque);
	imprimir_lista(destino, modif, lim, lista_item);
	imprimir_lista(destino, modif, lim, lista_pelota);
	imprimir_lista(destino, modif, lim, lista_tiro);
	imprimir_lista(destino, modif, lim, lista_bomba);

	if (gaucho && gaucho->get_estado() != -1)
	{
		gaucho->imprimir(destino, modif+*lim);
		(*lim)++;
	}

	imprimir_lista(destino, modif, lim, lista_fuente);
}



/*!
 * \brief informa a cada proceso con quien estan colisionando
 *
 * \param screen superficie destino
 */
void procesos :: avisar_colisiones(SDL_Surface *screen)
{
	struct nodo <class pelota> *pelota=NULL;
	struct nodo <class tiro> *tiro=NULL;
	struct nodo <class bloque> *bloque=NULL;
	struct nodo <class item> *item=NULL;
	int x, y, d; // datos de la pelota
	int xt, yt, ht; // datos del tiro
	int xg, yg, hg, wg; // datos del gaucho

	// obtiene los datos del graucho
	xg = gaucho->get_x();
	yg = gaucho->get_y();
	wg = 27;

	// se reduce la altura del gaucho se se esta barriendo o esta agachado.
	if (gaucho->get_estado() == 6 || gaucho->get_estado() == 8 || gaucho->get_estado() == 9) 
		hg=60/2;
	else
		hg=90/2;

	
	// compara los objetos pelota con el gaucho y los tiros
	for (pelota=lista_pelota; pelota; pelota = pelota->psgte)
	{
		x = pelota->obj->get_x();
		y = pelota->obj->get_y();
		d = pelota->obj->get_w();

		if (pelota->obj->get_estado() == 0)
		{
			// compara con los tiros
			for (tiro=lista_tiro; tiro; tiro = tiro->psgte)
			{
				xt = tiro->obj->get_x();
				yt = tiro->obj->get_y();
				ht = tiro->obj->get_h();
				
				if (tiro->obj->get_estado() < 2 && tiro->obj->get_estado() != -1 && abs(xt-x) < d && yt<y && yt+ht>y)
				{
					pelota->obj->colisiona_con_tiro();
					tiro->obj->colisiona_con_pelota();
					break;
				}

			}

			// se compara con el protagonista
			// primera vista, determina si se debe evaluar la colision
			if (hab_enemigos && (abs((xg+10)-x) < (d+wg)) && (abs((yg-hg)-y) < (d+hg)))
			{
				// afina la busqueda (compara la pelota con tres circulos)
				if (colisiona(15) || colisiona(25)|| colisiona(55) || colisiona(75))
					gaucho->colisiona_con_pelota();
						
			}
		}
		
	}


	// compara los bloques que se pueden romper con los tiros
	for (bloque = lista_bloque; bloque; bloque = bloque->psgte)
	{
		x = bloque->obj->get_x();
		y = bloque->obj->get_y();
		d = 16;
			
		for (tiro=lista_tiro; tiro; tiro = tiro->psgte)
		{
			xt = tiro->obj->get_x();
			yt = tiro->obj->get_y();
			ht = tiro->obj->get_h();

			if (tiro->obj->get_estado()==0 && abs(xt-x) <= d && yt<=y && yt+ht>=y)
			{
				bloque->obj->colisiona_con_tiro();
				tiro->obj->colisiona_con_bloque();
				break;
			}
		}
	}

	// verifica si el gaucho colisiona con un item
	for (item = lista_item; item; item = item->psgte)
	{
		x = item->obj->get_x();
		y = item->obj->get_y();
		d = 16;

		if ((abs(x-xg) < d+wg) && (abs(y-yg) < d+hg))
		{
			gaucho->colisiona_con_item(item->obj->get_tipo());
			item->obj->colisiona_con_protagonista();
		}
	}
}


/*!
 * \brief elimina todos los actores que tiene almacenados
 */
void procesos :: eliminar_todos(void)
{
	delete gaucho;
	gaucho=NULL;

	limpiar_todos(&lista_pelota);
	limpiar_todos(&lista_fuente);
	limpiar_todos(&lista_tiro);
	limpiar_todos(&lista_bloque);
	limpiar_todos(&lista_item);
}


/*!
 * \brief cuenta la cantidad de tiros en la pantalla
 * 
 * \return numero de tiros en la pantalla
 */
int procesos :: get_cant_tiros(void)
{
	struct nodo <class tiro> *pleer;
	int i;
	

	for (i=0, pleer=lista_tiro ; pleer; pleer=pleer->psgte, i++)
		;

	return i;
}

/*!
 * \brief informa si existe un mensaje en pantalla
 *
 * \return 1 si existe, 0 en caso contrario
 */
int procesos :: hay_mensaje_activo(void)
{
	if (lista_fuente == NULL)
		return 0;
	else
		return 1;
}


/*!
 * \brief informa si existen enemigos en ejecuci?
 *
 * \return 1 si hay enemigos, 0 en caso contrario
 */
int procesos :: hay_enemigos(void)
{
	struct nodo <class pelota> *pleer;
	int i;

	for (pleer=lista_pelota, i=0; pleer; pleer=pleer->psgte, i++)
		;
	
	return i;
}


/*!
 * \brief quita la pausa de todos los objetos
 */
void procesos :: descongelar_todos(void)
{
	hab_gaucho=1;
	hab_enemigos=1;
	hab_tiro=1;
}

/*!
 * \brief detiene a todos los enemigos
 */
void procesos :: pausar_enemigos(void)
{
	hab_enemigos = 0;
}

/*!
 * \brief detiene al protagonista
 */
void procesos :: pausar_gaucho(void)
{
	hab_gaucho=0;
}

/*!
 * \brief detiene a los procesos tiro
 */
void procesos :: pausar_tiros(void)
{
	hab_tiro=0;
}


/*!
 * \brief reduce todas las pelotas del nivel al tama? 1
 */
void procesos :: reducir_todos_los_enemigos(void)
{
	struct nodo <class pelota> *pleer;

	for (pleer = lista_pelota; pleer; pleer = pleer->psgte)
	{
		if (pleer->obj->get_tam() > 1)
			pleer->obj->colisiona_con_tiro();
	}
}

/*!
 * \brief informa si existe alguna bomba en el juego
 *
 * \return 1 exite, 0 no hay ninguna
 */
int procesos :: hay_bomba(void)
{
	if (lista_bomba)
		return 1;
	else
		return 0;
}




/*!
 * \brief agrega un nodo en una lista
 *
 * \param pnuevo nodo a agregar
 * \param pplista lista destino
 */
template <class S> void procesos :: agregar(S *pnuevo, S **pplista)
{
	pnuevo->psgte = *pplista;
	*pplista = pnuevo;
}


/*!
 * \brief elimina a los objetos con estado = -1
 * 
 * \param pplista lista a evaluar
 */
template <class S> void procesos ::  eliminar_antiguos(S **pplista)
{
	S *pleer;
	S *panterior=NULL;
	S *quitar;

	
	pleer = *pplista;

	while (pleer)
	{
		if (pleer->obj->get_estado() == -1)
		{

			quitar=pleer;

			if (panterior)
			{
				panterior->psgte=pleer->psgte;
				pleer = pleer->psgte;
				delete quitar->obj;
				delete quitar;

			}
			else
			{
			
				pleer=pleer->psgte;
				(*pplista)=pleer;
				delete quitar->obj;
				delete quitar;
			}
			
		}	
		else
		{
			panterior = pleer;
			pleer = pleer->psgte;
		}
	}
}



/*!
 * \brief elimina todos los nodos de la lista
 *
 * \brief pplista lista a limpiar
 */
template <class S> void procesos ::  limpiar_todos(S **pplista)
{
	S *pleer;
	S *panterior=NULL;

	for (pleer = *pplista; pleer;)
	{
		panterior=pleer; 
		pleer=pleer->psgte;

		delete panterior->obj;
		delete panterior;
	}

	*pplista = NULL;
	
}





/*!
 * \brief imprime todos los procesos de la lista
 *
 * \param screen superficie destino
 * \param modif rectangulos que se modifican
 * \param lim se le suma la cantidad de rectangulos modificados
 * \param pleer lista a imprimir
 */
template <class S> void procesos ::  imprimir_lista(SDL_Surface *screen, SDL_Rect *modif, int *lim, S *pleer)
{
	for (; pleer; pleer=pleer->psgte)
	{
		if (pleer->obj->get_estado() != -1)
		{
			pleer->obj->imprimir(screen, modif+*lim);
			(*lim)++;
		}
	}
	
}




/*!
 * \brief realiza la actualizacion logica de todos los objetos
 *
 * \param pleer lista de objetos a actualizar
 */
template <class S> void procesos ::  actualizar_lista(S *pleer)
{
	while (pleer)
	{
		pleer->obj->actualizar();
		pleer=pleer->psgte;
	}
}
