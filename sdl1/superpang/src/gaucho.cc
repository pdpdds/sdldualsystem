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


#include "gaucho.h"
#include "procesos.h"
#include "juego.h"
#include "utils.h"

///estados del gaucho don ceferino
enum {PARADO, CAMINA, DISPARA, MUERE, GANA, SUBIENDO_ESCALERA, AGACHADO, CAE, BARRIDA, GIRANDO, CAE_GIRANDO, BOMBA};

///tipo de arma utilizada
enum {NORMAL, TRIDENTE};

/*!
 * \brief vincula al gaucho ceferino al juego
 */
int gaucho :: iniciar(class juego *_juego, grafico *_graficos, int _x, int _y)
{
	w=h=10;
	graficos = _graficos;
	x=_x;
	y=_y;
	flip=1;
	pjuego = _juego;

	tecla=NULL;
	cont_delay=0;
	paso=0;
	max_tiros=1;
	tipo_tiro=NORMAL;
	disparo_pulsado=0;
	vel=1;
	pnivel=pjuego->nivel;
	
	cargar_animacion(animaciones[PARADO], "0");
	cargar_animacion(animaciones[CAMINA], "1,2,1,0,3,4,3,0");
	cargar_animacion(animaciones[DISPARA], "5,6,7");
	cargar_animacion(animaciones[MUERE], "13,13,13,13,13,13,13");
	cargar_animacion(animaciones[SUBIENDO_ESCALERA], "8,9,8,10");
	cargar_animacion(animaciones[AGACHADO], "14");
	cargar_animacion(animaciones[CAE], "15");
	cargar_animacion(animaciones[BARRIDA], "11,11");
	cargar_animacion(animaciones[GIRANDO], "19,20,21,22,23");
	cargar_animacion(animaciones[CAE_GIRANDO], "19,20,21,22,23");
	cargar_animacion(animaciones[BOMBA], "16,17,17,18");

	if (pnivel->get_dist_suelo(x,y,1) > 0)
	{
		reiniciar_animacion();
		estado=CAE;
	}
	else
		estado=PARADO;

	return 0;
}

bool gaucho :: slash(void)
{
    return tecla[SDLK_1] or tecla[SDLK_3] or tecla[SDLK_z] or tecla[SDLK_c];
}

bool gaucho :: left(void)
{
    return tecla[SDLK_LEFT] or tecla[SDLK_h] or tecla[SDLK_a];
}

bool gaucho :: right(void)
{
    return tecla[SDLK_RIGHT] or tecla[SDLK_l] or tecla[SDLK_d];
}

bool gaucho :: shot(void)
{
    return tecla[SDLK_x] or tecla[SDLK_2];
}

bool gaucho :: up(void)
{
    return tecla[SDLK_UP] or tecla[SDLK_k] or tecla[SDLK_w];
}

bool gaucho :: down(void)
{
    return tecla[SDLK_DOWN] or tecla[SDLK_j] or tecla[SDLK_s];
}

/*!
 * \brief actualizacion logica
 */
void gaucho :: actualizar(void)
{
	tecla = SDL_GetKeyState(NULL);
	
	switch (estado)
	{
		case PARADO:
			parado();

			/* Cheat */
			if (tecla[SDLK_b] && tecla[SDLK_o]) // (bo)om
			{
				reiniciar_animacion();
				estado=BOMBA;
			}
			break;
			
		case CAMINA:
			caminar();
			break;

		case DISPARA:
			disparar();
			break;

		case MUERE:
			morir();
			break;

		case SUBIENDO_ESCALERA:
			usar_escalera();
			break;
			
		case AGACHADO:
			agachado();
			break;
			
		case CAE:
			cae();
			break;
			
		case BARRIDA:
			barrida();
			break;

		case GIRANDO:
			barrida();
			break;
			
		case CAE_GIRANDO:
			cae_girando();
			break;
			
		case BOMBA:
			tirando_bomba();
			break;
	}
}

/*!
 * \brief estado detenido
 */
void gaucho :: parado()
{
	if (left())
	{
		reiniciar_animacion();
		estado=CAMINA;
		return;
	}

	if (right())
	{
		reiniciar_animacion();
		estado=CAMINA;
		return;
	}

	if (shot() && puede_disparar())
	{
		reiniciar_animacion();
		crear_tiro();
		estado=DISPARA;
		return;
	}

	if (! shot())
	{
		disparo_pulsado=0;
	}

	if (up() && hay_escalera(-4-3))
	{
		reiniciar_animacion();
		estado=SUBIENDO_ESCALERA;
		return;
	}

	if (down())
	{
		if (hay_escalera(+4-3))
		{
			reiniciar_animacion();
			estado=SUBIENDO_ESCALERA;
		}
		else
		{
			reiniciar_animacion();
			estado=AGACHADO;
		}
		
	return;
	}

	if (slash())
	{
		reiniciar_animacion();
		estado=BARRIDA;
	}
	
	avanzar_animacion();
}

/*!
 * \brief estado de caminar
 */
void gaucho :: caminar(void)
{
	if (left())
	{
	        flip=-1;
		x+=pnivel->get_dist_pared(x,y-5-3, -2);
	}
	else
		if (right())
		{
			flip=1;
			x+=pnivel->get_dist_pared(x+20,y-5-3, +2);
		}
		else
		{
			reiniciar_animacion();
			estado=PARADO;
		}

	if (shot() && puede_disparar())
	{
		reiniciar_animacion();
		crear_tiro();
		estado=DISPARA;
		return;
	}

	if (! shot())
		disparo_pulsado=0;

	avanzar_animacion();

	if (pnivel->get_dist_suelo(x,y,1) > 0)
	{
		reiniciar_animacion();
		estado=CAE;
	}

	if (slash())
	{
		reiniciar_animacion();
		estado=BARRIDA;
	}


	if (up() && hay_escalera(-4-3))
	{
		reiniciar_animacion();
		estado=SUBIENDO_ESCALERA;
		return;
	}

	if (down())
	{
		if (hay_escalera(+4-3+2))
		{
			reiniciar_animacion();
			estado=SUBIENDO_ESCALERA;
		}
		else
		{
			reiniciar_animacion();
			estado=AGACHADO;
		}
		
	return;
	}

	
}

/*!
 * \brief esta muriendo
 */
void gaucho :: morir(void)
{
	if (avanzar_animacion())
		estado = PARADO;
}

/*!
 * \brief esta disparando
 */
void gaucho :: disparar(void)
{
	if (avanzar_animacion())
		estado = PARADO;
}

/*!
 * \brief accion a realizar cuando es golpeado con una pelota
 */
void gaucho :: colisiona_con_pelota(void)
{
	if (estado != MUERE)
	{
		pjuego->restar_vidas();
		reiniciar_animacion();
		estado = MUERE;
		pjuego->pmundo->audio.play(2);
	}
}


/*!
 * \brief captura un item
 */
void gaucho :: colisiona_con_item(int tipo)
{
	switch (tipo)
	{
		case 0:
			reiniciar_animacion();
			estado=BOMBA;
			break;
			
		case 1:
			max_tiros++;
			break;
			
		case 2:
			tipo_tiro=TRIDENTE;
			break;
	}
}

/*!
 * \brief imprime al gaucho en la pantalla
 */
void gaucho :: imprimir(SDL_Surface *screen, SDL_Rect *rect)
{
	graficos->imprimir(animaciones[estado][paso], screen, rect, x, y, flip);
}


/*!
 * \brief avanza una animacion e informa cuando esta termina
 *
 * \return 1 cuando termina la animacion
 */
int gaucho :: avanzar_animacion(void)
{
	if (cont_delay>5)
	{
		if (animaciones[estado][paso+1] == -1)
		{
			paso=0;
			cont_delay=0;
			return 1;
		}
		else
			paso++;
		
		cont_delay=0;
	}
	else
		cont_delay++;

	return 0;
}

/*!
 * \brief reinicia los contadores de la animacion
 */
void gaucho :: reiniciar_animacion(void)
{
	cont_delay=0;
	paso=0;
}

/*!
 * \brief dispara un tiro
 */
void gaucho :: crear_tiro(void)
{
	disparo_pulsado=1;
	pjuego->crear_tiro(x+7, y-10, tipo_tiro);
}

/*!
 * \brief determina si se puede disparar
 * \return 1 si puede disparar
 */
int gaucho :: puede_disparar(void)
{
	if (pjuego->get_cant_tiros() < max_tiros && !disparo_pulsado)
		return 1;
	else
		return 0;
}

/*!
 * \brief informa si esta junto a una escalera (puede utilizarla)
 *
 * \return 1 hay una escalea
 */
int gaucho :: hay_escalera(int inc_y)
{
	return (pnivel->get_escalera(x, y+inc_y));
}


/*!
 * \brief estado, utilizando la escalera
 */
void gaucho :: usar_escalera(void)
{
	flip=1;
	x=(x/32)*32 + 9;

	if (up())
	{
		avanzar_animacion();
		y-=1;

		if (! hay_escalera(-4-3))
		{
			reiniciar_animacion();
			estado=PARADO;
		}

	}

	if (down())
	{
		avanzar_animacion();
		y+=1;

		if (! hay_escalera(-3-3))
		{
			reiniciar_animacion();
			estado=PARADO;
		}
	
	}
}


/*!
 * \brief estado agachado
 */
void gaucho :: agachado(void)
{
	if (!down())
	{
		reiniciar_animacion();
		estado=PARADO;
	}

	if (shot() && puede_disparar())
	{
		reiniciar_animacion();
		crear_tiro();
		estado=DISPARA;
		return;
	}

	if (slash())
	{
		reiniciar_animacion();
		estado=BARRIDA;
	}

}

/*!
 * \brief estado de caida
 */
void gaucho :: cae(void)
{
	int dy;

	vel+=0.1;
	dy = pnivel->get_dist_suelo(x,y, (int) vel);
	y+= dy;
	
	if (dy < (int) vel)
	{
		vel=0;
		reiniciar_animacion();
		estado=PARADO;
		y+=3+3;
	}

	if (left())
		x+=pnivel->get_dist_pared(x-15,y-5-3, -1);

	if (right())
		x+=pnivel->get_dist_pared(x-15,y-5-3, +1);

}

/*!
 * \brief estado de barrida y giro
 */
void gaucho :: barrida(void)
{

	if (flip==1)
		x+=pnivel->get_dist_pared(x+20, y-5-3, +7);
	else
		x+=pnivel->get_dist_pared(x, y-5-3, -7);

	
	if (pnivel->get_dist_suelo(x,y,1) > 0)
	{
		reiniciar_animacion();
		estado=CAE_GIRANDO;
	}

	if ( !slash())
	{
		if (pnivel->get_dist_suelo(x,y,1) > 0)
		{
			reiniciar_animacion();
			estado=CAE;
		}
		else
		{
			reiniciar_animacion();
			estado=PARADO;
		}
	}

	if (avanzar_animacion() && estado==BARRIDA) 
		estado=GIRANDO;
}


/*!
 * \brief llamado cuando el personaje se barre y no tiene suelo
 */
void gaucho :: cae_girando(void)
{
	int dy;

	vel+=0.1;
	dy = pnivel->get_dist_suelo(x,y, (int) vel);
	y+= dy;
	
	if (dy < (int) vel)
	{
		vel=0;
		reiniciar_animacion();
		estado=PARADO;
		y+=3+3;
	}

	if (left())
		x+=pnivel->get_dist_pared(x-15,y-5-3, -1);

	if (right())
		x+=pnivel->get_dist_pared(x-15,y-5-3, +1);


	avanzar_animacion(); 
}


/*!
 * \brief animación del disparo de la bomba
 */
void gaucho :: tirando_bomba(void)
{
	if (avanzar_animacion())
	{
		pjuego->crear_bomba(x, y, flip);
		reiniciar_animacion();
		estado=NORMAL;
	}
}
