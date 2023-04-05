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


#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "nivel.h"
#include "juego.h"
#include "int.h"

/*!
 * \brief genera el objeto nivel y carga el primer nivel
 */
void nivel::iniciar(juego *_pjuego, grafico *_grafico, grafico *_graf_fondos, SDL_Surface *_fondo)
{
	fondo = _fondo;
	pgrafico = _grafico;
	pfondos = _graf_fondos;
	nivel_actual = 0;
	pjuego = _pjuego;
}

/*!
 * \brief carga un nivel
 */
void nivel :: ir_nivel(int nro_nivel)
{
	nivel_actual = nro_nivel;
	cargar_nivel();
	imprimir_nivel();
}

/*!
 * \brief adelanta un nivel
 *
 * \return el numero del nuevo nivel, o -1 si se terminaron todos los niveles
 */
int nivel :: avanzar_nivel(int salto)
{
	nivel_actual++;
	int ret;
	
	ret = cargar_nivel();
	
	switch (ret)
	{
		case -1:
			printf(_("Can't load the level %d\n"), nivel_actual);
			return -1;
			break;
			
		case -2:
			return -2;
			break;
	}

	imprimir_nivel();

	return nivel_actual;
}


/*!
 * \brief imprime sobre la superficie fondo el nivel actual, crea todos los objetos
 */
void nivel :: imprimir_nivel(void)
{
	int i,j;
	SDL_Rect rect;

	pfondos->imprimir(0, fondo, &rect, 0, 0, 1);
	
	for (i=0; i<14; i++)
	{
		for (j=0; j<20; j++)
		{

			if (mapa[i][j] < 'a' && mapa[i][j] != '-')
				pgrafico->imprimir(mapa[i][j], fondo, &rect, j*32,i*32, 1) ;
			else
			{
				switch (mapa[i][j])
				{
					case 'a':
					pjuego->crear_gaucho(j*32, i*32 +38);
					break;

					case 'b':
					pjuego->crear_pelota(j*32, i*32, 1, 1);
					break;

					case 'c':
					pjuego->crear_pelota(j*32, i*32, 2, 1);
					break;

					case 'd':
					pjuego->crear_pelota(j*32, i*32, 3, 1);
					break;

					case 'e':
					pjuego->crear_pelota(j*32, i*32, 4, 1);
					break;

					case 'f':
					pjuego->crear_pelota(j*32, i*32, 1, -1);
					break;

					case 'g':
					pjuego->crear_pelota(j*32, i*32, 2, -1);
					break;

					case 'h':
					pjuego->crear_pelota(j*32, i*32, 3, -1);
					break;

					case 'i':
					pjuego->crear_pelota(j*32, i*32, 4, -1);
					break;

					case 'j':
					pjuego->crear_bloque_romper(j*32+16, i*32+16+12);
					break;
				}

			if (mapa[i][j] != 'j') // bloque que se debe romper
				mapa[i][j]='-';
			else
				mapa[i][j]=0;
			}
		}
	}

}


/*!
 * \brief carga el nivel indicado en 'nivel_actual'
 *
 * \return -1 si terminaron todos los niveles, -2 si exite un error serio
 */
int nivel :: cargar_nivel()
{
	FILE *arch;
	char tmp[1024];

#ifdef WIN32
	strcpy(tmp, "./data/levels/base.map");
#else
	strcpy(tmp, getenv("HOME"));
	strcat(tmp, "/");
	strcat(tmp, ".ceferinoniveles.map");
#endif
	arch = fopen(tmp, "rb");

	if (!arch)
		arch = fopen ("./data/levels/base.map", "rb");

	if (!arch)
	{
		printf(_("Can't open the file '%s'"), \
				"./data/levels/base.map'\n");
		exit(-1);
	}

	if (fseek(arch, 20*14*(nivel_actual-1)*sizeof(char) ,SEEK_SET) == -1)
	{
		fclose(arch);
		return -1;
	}
	else
	{
		if (fread(mapa, sizeof(char), 20*14, arch) <= 0)
		{
			fclose(arch);
			return -1;
		}
	}

	if (verificar_nivel())
	{
		fclose(arch);
		return 0;
	}
	else
	{
		fclose(arch);
		return -2;
	}
}


/*!
 * \brief obtiene la distancia al suelo o techo para objetos como la pelota o el gaucho
 *
 * \param x coordenada del punto origen
 * \param y coordenada del punto origen
 * \param max distancia que se quiere avanzar
 * \return != max si encuentra un suelo o techo, = max si no hay suelo o techo
 */
int nivel :: get_dist_suelo(int x, int y, int max)
{
	int i;
	int c=x/32;

	if (max>0)
	{
		for (i=0; i<max; i++)
		{
			if ((y+i)/32 >= 13)
				return i;

			if ((y+i)%32 < 10)
				if (es_suelo(mapa[(y+i)/32][c]))
					return i;
		}
	}
	else
	{
		for (i=0; i>max; i--)
		{
			if (y+i < 1)
				return i;

			if (es_suelo(mapa[(y+i)/32][c]))
				return i;
		}
	}
			
	
	return max;
}


/*!
 * \brief informa si un bloque es suelo
 *
 * \param l bloque a verificar
 * \return 1 en caso de ser suelo
 */
int nivel :: es_suelo(char l)
{
	if (l < 15)
		return 1;
	else
		return 0;
}


/*!
 * \brief obtiene la distancia a una pared
 *
 * \param x coordenada del punto origen
 * \param y coordenada del punto origen
 * \param max distancia que se quiere avanzar en direccion horizontal, <0 = hacia la izquierda.
 * \return != max distancia hacia la pared, = max no hay una pared
 */
int nivel :: get_dist_pared(int x, int y, int max)
{
	int i;
	int f = y/32;
	
	if (max>0)
	{
		for (i=0; i<max; i++)
		{
			if (x+i > 640-10)
				return i;

			if (es_suelo(mapa[f][(x+i)/32]))
				return i;
		}
	}
	else
	{
		for (i=0; i>max; i--)
		{
			if (x+i < 0+5)
				return i;
			
			if (es_suelo(mapa[f][(x+i)/32]))
				return i;
		}

	}

	return max;
}


/*!
 * \brief carga nuevamente el nivel actual
 *
 * \return numero de nivel actual
 */
int nivel :: reiniciar_nivel(void)
{
	cargar_nivel();
	imprimir_nivel();

	return nivel_actual;
}


/*!
 * \brief verifica si un bloque es escalera
 *
 * \return 1 si es escalera, 0 en caso contrario
 */
int nivel :: get_escalera(int x, int y)
{
	if (mapa[y/32][x/32] == 13 || mapa[y/32][x/32] == 15)
		return 1;
	else
		return 0;
}


/*!
 * \brief verifica que el nivel no contenga errores
 *
 * \return 1 si el nivel es correcto, 0 si tiene errores
 */
int nivel :: verificar_nivel(void)
{
	int i, j;
	int hay_gaucho=0;
	int hay_pelota=0;

	for (i=0; i<14; i++)
	{
		for (j=0; j<20; j++)
		{
			switch (mapa[i][j])
			{
				case 'a':
					if (hay_gaucho == 1)
					{
						printf(_("error: there is 2 cowboys in the same level\n"));
						return 0;
					}
					else
						hay_gaucho=1;
					break;
					
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
				case 'g':
				case 'h':
				case 'i':
					hay_pelota=1;
					break;
					
			}
		}
	}

	// si el nivel es correcto
	if (hay_gaucho && hay_pelota)
		return 1;

	if (!hay_gaucho)
		printf(_("error, the actual level does't define the posicion"\
					"of the object 'gaucho'\n")); 

	if (!hay_pelota)
		printf(_("error, no enemy was created in this level, you must"\
					"have at least one\n")); 
		
	return 0;
}





/*!
 * \brief obtiene la distancia al un techo solido y no un bloque que se pueda romper (solo se utiliza para el tiro)
 *
 * \param x coordenada del punto origen
 * \param y coordenada del punto origen
 * \param max distancia que se quiere avanzar
 * \return != max si encuentra un suelo o techo, = max si no hay suelo o techo
 */
int nivel :: get_dist_techo(int x, int y, int max)
{
	int i;
	int c=x/32;

	for (i=0; i>max; i--)
	{
		if (y+i < 1)
			return i;
		
		if (mapa[(y+20+i)/32][c] <  15 && mapa[(y+20+i)/32][c] > 0)
			return i;
	}

	return max;
}



/*!
 * \brief define como 'no colisionable' a un bloque
 *
 * \param f fila del bloque
 * \param c columna del bloque
 */
void nivel :: limpiar_bloque(int f, int c)
{
	mapa[f][c] = '-';
}
