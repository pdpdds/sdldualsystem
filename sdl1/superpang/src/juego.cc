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
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "juego.h"
#include "procesos.h"
#include "mundo.h"
#include "utils.h"
#include "int.h"


juego :: juego()
{
	nivel = new class nivel;
	procesos = new class procesos;
	barra = new class barra;
	lim_todos=0;
	lim_actual=0;

	srand(time(NULL));
	
	puntos = 0;
	vidas = 3;
	tiempo_bonus = 0;
	strcpy (nombre, (""));
	proximo_pago_vida = 300;
}

juego :: ~juego(void)
{
	delete nivel;
	delete barra;
	procesos->eliminar_todos();
	delete procesos;
	SDL_FreeSurface(fondo);
}

/*!
 * \brief inicializa la escena juego
 *
 * \brief _pmundo controlador principal de todo el videojuego
 * \brief _modo_video resolucion en pantalla
 * \brief _screen superficie principal
 *
 * \return 1 si no puede iniciar, 0 en caso contrario
 */
int juego :: iniciar(mundo *_pmundo, int _modo_video, SDL_Surface *_screen)
{
	pmundo = _pmundo;
	modo_video = _modo_video;
	screen = _screen;
	
	fondo = SDL_DisplayFormat(_screen);

	if (fondo == NULL)
	{
		printf(_("error, can't create a copy of the\
					screen (juego::inciar)\n"));
		return 1;
	}

	
	cambiar_estado(INICIANDO);

	procesos->iniciar(this);
	nivel->iniciar(this, pmundo->libgrafico.ima_nivel, pmundo->libgrafico.ima_fondos, fondo);
	barra->iniciar(this, pmundo->libgrafico.ima_barra, screen);

	avanzar_nivel();
	
	pmundo->audio.play_musica(1);
	reiniciar_reloj_local();

	return 0;
}


/*!
 * \brief altera el estado actual
 *
 * \param _estado nuevo estado
 */
void juego :: cambiar_estado(int _estado)
{
	estado = _estado;
}


/*!
 * \brief carga y muestra el siguiente nivel
 */
int juego :: avanzar_nivel(void)
{
	SDL_Rect rect = {0,0, screen->w, screen->h - screen->h/15};
	SDL_Rect tmp[400];
	int tmp_contador = 0;
	char mensaje[50];
	int nivel_actual;

	procesos->eliminar_todos();

	nivel_actual = nivel->avanzar_nivel();

	// se terminaron todos los niveles
	if (nivel_actual == -1)	
	{
		procesos->eliminar_todos();
	
		SDL_FillRect(screen, NULL, 0);
		SDL_FillRect(fondo, NULL, 0);
		SDL_Flip(screen);

		if (es_marca(puntos))
		{
			cambiar_estado(GUARDA_MARCA);
			pmundo->deshabilitar_letras();
		}
		else
		{
			pmundo->cambiar_escena(FINAL);
		}
		
		return 1;
	}

	procesos->pausar_gaucho();
	procesos->pausar_tiros();
	procesos->pausar_enemigos();

	sprintf (mensaje, _("level %d"), nivel_actual);
	
	procesos->crear_mensaje (pmundo->fuente, mensaje);
	
	tiempo=30;
	
	barra->actualizar(tiempo, vidas, puntos);
	barra->imprimir_todo();

	SDL_BlitSurface(fondo, &rect, screen, &rect);
	procesos->imprimir(screen, tmp, &tmp_contador);
	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
	pmundo->reiniciar_reloj();
	
	return 0;
}



/*!
 * \brief actualizacion logica de todo el juego (incluidos sus procesos)
 */
void juego :: actualizar(void)
{
	tecla = SDL_GetKeyState(NULL);

	if (tecla[SDLK_ESCAPE] && estado != GUARDA_MARCA)
	{
		pmundo->cambiar_escena(MENU);
		return;
	}
	

	switch (estado)
	{
		case INICIANDO:
			iniciando();
			break;
			
		case JUGANDO:
			jugando();

			/* cheats */
			
			if (tecla[SDLK_j] && tecla[SDLK_u]) // (ju)mp
				pasa_nivel();

			if (tecla[SDLK_s] && tecla[SDLK_j]) // (s)uper(j)mp
			{
				procesos->eliminar_todos();
				nivel->ir_nivel(25);
				pasa_nivel();
			}
			break;
			
		case PIERDE_VIDA:
			pierde_vida();
			break;
			
		case PIERDE_TODO:
			pierde_todo();
			break;
			
		case PIERDE_POR_TIEMPO:
			pierde_por_tiempo();
			break;

		case PASA_NIVEL:
			pasa_nivel();
			break;

		case DETENIDO_POR_BONUS:
			detenido_por_bonus();
			break;

		case DETENIDO_POR_BOMBA:
			detenido_por_bomba();
			break;
			
		case GUARDA_MARCA:
			guarda_marca();
			break;
			
		case PAUSA:
			pausa();
			break;
	}


}

/*!
 * \brief descuenta tiempo, y hace perder al jugador si el tiempo es insuficiente
 */
void juego :: procesar_tiempo(void)
{
	if (SDL_GetTicks() - tick_ant > 1500) //paso un segundo
	{
		tiempo--;

		if (tiempo<0)
		{
			procesos->crear_mensaje (pmundo->fuente, _("time out"));
			cambiar_estado(PIERDE_POR_TIEMPO);
			procesos->pausar_gaucho();
			procesos->pausar_tiros();
			procesos->pausar_enemigos();
		}
		else
		{
			barra->actualizar(tiempo, vidas, puntos);
			tick_ant+=1000;
		}

	}
}


/*!
 * \brief Realiza la impresion en pantalla utilizando la t?nica Dirty Rectangles
 */
void juego :: imprimir(void)
{
	int i;

	lim_actual=0;
	
	procesos->imprimir(screen, rect_actual, &lim_actual);
	
	if (estado == GUARDA_MARCA)
	{
		imprimir_marca(screen, rect_actual, &lim_actual);
		imprimir_nombre(screen, rect_actual, &lim_actual);
	}

	if (estado == PAUSA)
		imprime_pausa(screen, rect_actual, &lim_actual);

	if (estado == PIERDE_TODO)
		imprime_pierde_todo (screen, rect_actual, & lim_actual);
	
	copiar_rectangulos(todos, &lim_todos, rect_actual, &lim_actual, screen->w, screen->h);

	SDL_UpdateRects(screen, lim_todos, todos);
	
	lim_todos=0;
	copiar_rectangulos(todos, &lim_todos, rect_actual, &lim_actual, screen->w, screen->h);

	
	// restaura el fondo
	for (i=0; i<lim_actual; i++)
		SDL_BlitSurface(fondo, &(rect_actual[i]), screen, &(rect_actual[i]));
}


/*!
 * \brief genera un tiro
 */
void juego :: crear_tiro(int x, int y, int tipo_tiro)
{
	procesos->crear_tiro(pmundo->libgrafico.ima_tiros, x, y, tipo_tiro);
}

/*!
 * \brief crea al protagonista del juego
 *
 * \brief x coordenada x
 * \brief y coordenada y
 */
void juego :: crear_gaucho(int x, int y)
{
	procesos->crear_gaucho(pmundo->libgrafico.ima_gaucho, x, y);
}

/*!
 * \brief genera un enemigo del juego
 * 
 * \param x coordenada x
 * \param y coordenada y
 * \param tam tama? de la pelota (4 muy grande, 3 grande, 2 mediana, 1 chica
 * \param flip direccion de la pelota (1 derecha, -1 izquierda)
 */
void juego :: crear_pelota(int x, int y, int tam, int flip)
{
	switch (tam)
	{
		case 4:
		procesos->crear_pelota(pmundo->libgrafico.ima_pelota_4, x, y, tam, flip);
		break;
		
		case 3:
		procesos->crear_pelota(pmundo->libgrafico.ima_pelota_3, x, y, tam, flip);
		break;
		
		case 2:
		procesos->crear_pelota(pmundo->libgrafico.ima_pelota_2, x, y, tam, flip);
		break;

		case 1:
		procesos->crear_pelota(pmundo->libgrafico.ima_pelota_1, x, y, tam, flip);
		break;
	}
}


/*!
 * \brief crea un bloque de suelo que se pueda romper
 *
 * \brief x coordenada x
 * \brief y coordenada y
 */
void juego :: crear_bloque_romper(int x, int y)
{
	procesos->crear_bloque_romper(pmundo->libgrafico.ima_item, x, y);
}


/*!
 * \brief determina si crea un item y su tipo
 *
 * \brief x coordenada x
 * \brief y coordenada y
 */
void juego :: crear_item_por_azar(int x, int y)
{
	// obtiene un numero aleatorio entre 0 y 10 utilizando
	// los bits mas significativos (vea 'man rand')
	int azar = (int) (10.0*rand()/(RAND_MAX+1.0));
	int item;

	switch (azar)
	{
		case 0:
			item=0;
			break;
			
		case 1:
		case 2:
		case 3:
			item=1;
			break;
			
		case 4:
		case 6:
			item=2;
			break;

		case 7:
		case 8:
		case 9:
			item=3;
			break;

		default:
			item=-1;
			break;
	}

	if (item != -1)
		procesos->crear_item(pmundo->libgrafico.ima_item, x, y, item);
}

/*!
 * \brief genera una bomba que reduce a los enemigos a la mitad
 *
 * \param x coordenada x
 * \param y coordenada y
 * \param flip 1 = hacia la derecha, -1 hacia la izquierda
 */
void juego :: crear_bomba(int x, int y, int flip)
{
	estado = DETENIDO_POR_BOMBA;
	procesos->crear_bomba(pmundo->libgrafico.ima_mate, x, y, flip);
}

/*!
 * \brief informa la cantidad de nodos de la lista tiros
 * 
 * \return cantidad de tiros en la pantalla
 */
int juego :: get_cant_tiros(void)
{
	return procesos->get_cant_tiros();
}

/*!
 * \brief incrementa los puntos del protagonista
 *
 * \param inc puntos a incrementar
 */
void juego :: sumar_puntos(int inc)
{
	puntos+=inc;
	
	// aumenta una vida
	if (puntos > proximo_pago_vida)
	{
		proximo_pago_vida+=300;
		sumar_vidas();
	}
	
	barra->actualizar(tiempo, vidas, puntos);
}

/*!
 * \brief suma una vida al protagonista
 */
void juego :: sumar_vidas(void)
{
	vidas++;
	barra->actualizar(tiempo, vidas, puntos);
}

/*!
 * \brief hace perder una vida al protagonista (altera el estado del la escena 'juego')
 */
void juego :: restar_vidas(void)
{
	char mensajes[3][10] = { {"ouch"}, {"uh"}, {"aaah"}};
	vidas--;
	barra->actualizar(tiempo, vidas, puntos);

	if (vidas < 0)
	{
		procesos->pausar_gaucho();
		procesos->pausar_tiros();
		procesos->pausar_enemigos();
		if (es_marca(puntos))
		{
		    pmundo->deshabilitar_letras();
		    cambiar_estado(GUARDA_MARCA);
                    return;
		}
                else
                {
		    cambiar_estado(PIERDE_TODO);
                }
	}
	else
	{
		procesos->crear_mensaje (pmundo->fuente, mensajes[rand()%3]);
		procesos->pausar_gaucho();
		procesos->pausar_tiros();
		procesos->pausar_enemigos();
		cambiar_estado(PIERDE_VIDA);
	}

}

/*!
 * \brief carga nuevamente el nivel actual
 */
void juego :: reiniciar_nivel(void)
{
	SDL_Rect rect = {0,0, screen->w, screen->h - screen->h/15};
	SDL_Rect tmp[400];
	int tmp_contador = 0;
	char mensaje[9];
	int nivel_actual;
	
	procesos->eliminar_todos();
	nivel_actual = nivel->reiniciar_nivel();

	sprintf (mensaje, _("level %d"), nivel_actual);
	procesos->crear_mensaje (pmundo->fuente, mensaje);

	tiempo=30;
	
	barra->actualizar(tiempo, vidas, puntos);
	barra->imprimir_todo();

	SDL_BlitSurface(fondo, &rect, screen, &rect);
	procesos->imprimir(screen, tmp, &tmp_contador);
	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
	pmundo->reiniciar_reloj();
}


/*!
 * \brief inicializa el conteo del reloj
 */
void juego :: reiniciar_reloj_local(void)
{
	tick_ant = SDL_GetTicks();
}


/*!
 * \brief asigna el premio del item al juego o protagonista
 *
 * \param tipo_item 0 bomba, 3 deterner el reloj
 */
void juego :: pagar_item(int tipo_item)
{
	pmundo->audio.play(10);

	switch (tipo_item)
	{
		case 0: // bomba
			procesos->pausar_enemigos();
			break;
			
		case 3: // detiene el reloj
			procesos->pausar_enemigos();

			tiempo_bonus += 2;
			
			if (estado != DETENIDO_POR_BONUS)
				crear_mensaje_bonus_tiempo (tiempo_bonus);

			estado=DETENIDO_POR_BONUS;
			break;
			
	}
}


/*!
 * \brief efecto visual que produce el personaje cuando se dispara la bomba
 */
void juego :: efecto_bomba(void)
{
	SDL_Rect tmp[400];
	int tmp_contador = 0;
	SDL_Rect rect = {0, 0, screen->w, screen->h - screen->h/15};

	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format,255,255,255));
	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);

	
	// restaura la pantalla
	SDL_BlitSurface(fondo, &rect, screen, &rect);
	procesos->imprimir(screen, tmp, &tmp_contador);
	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
	SDL_BlitSurface(fondo, &rect, screen, &rect);
	barra->imprimir_todo();
	pmundo->reiniciar_reloj();


	
	pmundo->reiniciar_reloj();
}



/*!
 * \brief genera un mensaje r?ido con los segundos disponibles
 *
 * \param segundos tiempo restante
 */
void juego :: crear_mensaje_bonus_tiempo (int segundos)
{
	char mensaje [50];
	char tiempo [10];

	strcpy (mensaje, _("bonus"));
	sprintf (tiempo, " %d", segundos);
	strcat (mensaje, tiempo);
	
	procesos->crear_mensaje_rapido (pmundo->fuente, mensaje);
}


/*!
 * \brief controlador de un estado del juego
 */
void juego :: iniciando(void)
{
	if (procesos->hay_mensaje_activo())
		procesos->actualizar();
	else
	{
		procesos->descongelar_todos();
		cambiar_estado(JUGANDO);
		reiniciar_reloj_local();
	}
}


/*!
 * \brief controlador de un estado del juego
 */
void juego :: jugando(void)
{
	procesar_tiempo();
	
	procesos->actualizar();
	procesos->avisar_colisiones(screen);

	if (procesos->hay_enemigos() == 0)
	{
		procesos->crear_mensaje (pmundo->fuente, _("level complete"));
		procesos->pausar_gaucho();
		procesos->pausar_tiros();
		procesos->pausar_enemigos();
		cambiar_estado(PASA_NIVEL);
	}

	
	if (tecla[SDLK_p])
	{
		cambiar_estado(PAUSA);
		delay_tecla=20;
	}
}


/*!
 * \brief controlador de un estado del juego
 */
void juego :: pierde_vida(void)
{
	if (procesos->hay_mensaje_activo())
		procesos->actualizar();
	else
	{
		reiniciar_nivel();
		cambiar_estado(INICIANDO);
	}
}


/*!
 * \brief controlador de un estado del juego
 */
void juego :: pierde_todo(void)
{

	tecla = SDL_GetKeyState(NULL);

	if (procesos->hay_mensaje_activo ())
	{
		procesos->actualizar();
	}
	else
	{
		if (tecla[SDLK_ESCAPE])
		{
		    	pmundo->cambiar_escena(MENU);
			return;
		}

		if (tecla [SDLK_SPACE])
		{
			puntos = 0;
			vidas = 3;
			barra->actualizar(tiempo, vidas, puntos);
			reiniciar_nivel ();
			cambiar_estado(INICIANDO);
		}
	}
}


/*!
 * \brief controlador de un estado del juego
 */
void juego :: pierde_por_tiempo(void)
{
	if (procesos->hay_mensaje_activo())
		procesos->actualizar();
	else
		restar_vidas();
}


/*!
 * \brief controlador de un estado del juego
 */
void juego :: pasa_nivel(void)
{
	if (procesos->hay_mensaje_activo())
		procesos->actualizar();
	else
	{
		puntos+=tiempo;
		tiempo=0;

		if (avanzar_nivel() != 1)
			cambiar_estado(INICIANDO);
	}
}


/*!
 * \brief controlador de un estado del juego
 */
void juego :: detenido_por_bonus(void)
{
	if (procesos->hay_mensaje_activo())
	{
		procesos->actualizar();
		procesos->avisar_colisiones(screen);
	}
	else
	{
		if (tiempo_bonus > 1 && procesos->hay_enemigos())
		{
			tiempo_bonus--;
			crear_mensaje_bonus_tiempo(tiempo_bonus);
		}
		else
		{
			procesos->descongelar_todos();
			reiniciar_reloj_local();
			estado=JUGANDO;
		}
	}
}


/*!
 * \brief controlador de un estado del juego
 */
void juego :: detenido_por_bomba(void)
{
	if (procesos->hay_bomba())
	{
		procesos->actualizar();
		procesos->avisar_colisiones(screen);
	}
	else
	{
		if (estado != DETENIDO_POR_BONUS)
		{
			procesos->descongelar_todos();
			reiniciar_reloj_local();
			estado=JUGANDO;
		}
	}
}


/*!
 * \brief controlador de un estado del juego
 */
void juego :: guarda_marca(void)
{
	Uint8 *key;
	int i;
	int lim = strlen(nombre);

	if (delay_tecla==0)
	{
		key = SDL_GetKeyState(NULL);

		// agrega letras
		if (lim<17)
		{
			// letras
			for (i=SDLK_a; i <= SDLK_z; i++)
			{
				if (key[i])
				{
					nombre[lim]=(char)i;
					nombre[lim+1]='\0';
					delay_tecla=15;
					return;
				}
			}

			if (key[SDLK_SPACE])
			{
				nombre[lim]=' ';
				nombre[lim+1]='\0';
				delay_tecla=15;
			}
			
			if (key[SDLK_ESCAPE])
			{
				pmundo->habilitar_letras();
				
				if (vidas<0)
		                        cambiar_estado(PIERDE_TODO);
				else
					pmundo->cambiar_escena(FINAL);
					
				delay_tecla=15;
			}

			
		}


		if (key[SDLK_RETURN])
		{
			pmundo->habilitar_letras();

			if (vidas<0)
		                cambiar_estado(PIERDE_TODO);
			else
				pmundo->cambiar_escena(FINAL);

			salvar_marca_en_archivo();
			delay_tecla=15;
		}

		
		if (lim>0 && key[SDLK_BACKSPACE])
		{
			nombre[lim-1]='\0';
			delay_tecla=20;
		}
	}
	else
	{
		delay_tecla--;
	}
}

/*!
 * \brief verifica si el puntaje del jugador es un record
 *
 * \param puntaje cantidad de puntos del jugador
 * 
 * \return 1 si, 0 no
 */
int juego :: es_marca(int puntaje)
{
	FILE *arch;
	char ruta_completa[100];
	struct entrada vec_marcas[7];

#ifdef WIN32
	strcpy(ruta_completa, "marcas.dat");
#else
	strcpy(ruta_completa, getenv("HOME"));
	strcat(ruta_completa, "/");
	strcat(ruta_completa, ".ceferinomarcas");
#endif

	arch = fopen(ruta_completa, "rb");

	if (arch == NULL)
	{
		printf(_("Can't open the marcs file\n"));
		cargar_marcas_estandar();
		arch = fopen(ruta_completa, "rb");
	}
	
	fread(vec_marcas, sizeof(struct entrada), 7, arch);

	for (int i=0; i<7; i++)
	{
		if (vec_marcas[i].puntos < puntaje)
		{
			fclose(arch);
			return 1;
		}
	}
	
	fclose(arch);
	return 0;	
}


/*!
 * \brief imprime el mensaje de 'ha marcado un record...'
 *
 * \param screen pantalla principal
 * \param rect_actual vector de rectangulos modificados
 * \param lim_actual cantididad de rectangulos modificados
 */
void juego :: imprimir_marca(SDL_Surface *screen, SDL_Rect *rect_actual, int *lim_actual)
{
	char mensaje1[200];
	char mensaje2[200];

	strcpy(mensaje1, _("you made a new record"));
	strcpy(mensaje2, _("enter your name"));

	pmundo->fuente->myprintf (screen, 320, 100, rect_actual, lim_actual, \
			false, CENTRADO, mensaje1);
	
	pmundo->fuente->myprintf (screen, 320, 150, rect_actual, lim_actual, \
			false, CENTRADO, mensaje2);
}


/*!
 * \brief imprime el nombre ingresado por el jugador
 *
 * \param screen pantalla pricipal
 * \param rect_actual rentangulos modificados
 * \param lim_actual cantidad de rectangulos modificados
 */
void juego :: imprimir_nombre(SDL_Surface *screen, SDL_Rect *rect_actual, int *lim_actual)
{
	pmundo->fuente->myprintf (screen, 320, 200, rect_actual, lim_actual, \
			false, CENTRADO, nombre);
}


/*!
 * \brief guarda la marca record en el archivo
 */
void juego :: salvar_marca_en_archivo(void)
{
	FILE *arch;
	char ruta_completa[100];
	struct entrada vec_marcas[7];
	int i;
	int pos=6;

#ifdef WIN32
	strcpy(ruta_completa, "marcas.dat");
#else
	strcpy(ruta_completa, getenv("HOME"));
	strcat(ruta_completa, "/");
	strcat(ruta_completa, ".ceferinomarcas");
#endif
	
	arch = fopen(ruta_completa, "rb+");

	if (arch == NULL)
	{
		printf(_("Can't open the marcs file\n"));
		return;
	}
	else
	{
		fread(vec_marcas, sizeof(struct entrada), 7, arch);

		// selecciona posicion
		for (i=6; i>=0; i--)
		{
			if (puntos > vec_marcas[i].puntos)
				pos = i;
		}

		// mueve otras marcas
		for (i=6; i>pos; i--)
		{
			strcpy(vec_marcas[i].nombre, vec_marcas[i-1].nombre);
			vec_marcas[i].puntos = vec_marcas[i-1].puntos;
		}

		// guarda la marca actual
		strcpy(vec_marcas[pos].nombre, nombre);
		vec_marcas[pos].puntos = puntos;

		rewind(arch);
		fwrite(vec_marcas, sizeof(struct entrada), 7, arch);
		fclose(arch);
	}
}


/*!
 * \brief detiene el juego
 */
void juego :: pausa(void)
{
	if (tecla[SDLK_SPACE] || tecla[SDLK_z] || tecla[SDLK_x] || tecla[SDLK_c] || tecla[SDLK_RETURN])
	{
		reiniciar_reloj_local();
		cambiar_estado(JUGANDO);
	}
}

/*!
 * \brief imprime el mensaje de pausa
 */
void juego :: imprime_pausa(SDL_Surface *screen, SDL_Rect *rect_actual, int *lim_actual)
{
	char mensaje[200];
	strcpy(mensaje, _("pause"));

	pmundo->fuente->myprintf (screen, 320, 100, rect_actual, lim_actual, \
			false, CENTRADO, mensaje);
}



/*!
 * \brief realiza una pausa en el juego
 */
void juego :: pausar(void)
{
	if (estado == JUGANDO)
		cambiar_estado(PAUSA);
}


/*!
 * \brief imprime el mensaje de GameOver
 */
void juego :: imprime_pierde_todo (SDL_Surface *screen, SDL_Rect *rect_actual, int *lim_actual)
{
	pmundo->fuente->myprintf (screen, 320, 100, rect_actual, lim_actual, \
			false, CENTRADO, _("Game Over"));
	
	pmundo->fuente->myprintf (screen, 320, 200, rect_actual, lim_actual, \
			false, CONSALTO, _("If you want to continue push 'space' key"));
}
