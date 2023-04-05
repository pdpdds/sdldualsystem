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
#include <stdlib.h>
#include "actor.h"

actor :: ~actor(void)
{
}

/*!
 * \brief almacena en un vector los cuadros de animaci? de un estado
 *
 * \param vec vector del estado
 * \param cuadros cadena con el formato "1,2,3" (cuadros 1 -> 2 -> 3)
 */
void actor :: cargar_animacion(int *vec, const char *cuadros)
{
	char *p=NULL;
	char buffer[100];
	int i=0;
	strcpy(buffer, cuadros);
	
	p=strtok(buffer, ",");

	while (p)
	{
		vec[i] = atoi(p);
		p=strtok(NULL, ",");
		i++;
	}

	vec[i] = -1;
}
