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
#include "libgrafico.h"
#include "utils.h"
#include "int.h"

libgrafico :: libgrafico()
{
	modo_video=0;

	ima_gaucho=NULL;
	ima_pelota_1=NULL;
	ima_pelota_2=NULL;
	ima_pelota_3=NULL;
	ima_pelota_4=NULL;
	ima_nivel=NULL;
	ima_fondos=NULL;
	ima_tiros=NULL;
	ima_fuente_1=NULL;
	ima_fuente_2=NULL;
	ima_barra=NULL;
	ima_item=NULL;
	ima_mate=NULL;
	ima_menu=NULL;
	ima_tit_1=NULL;
	ima_tit_2=NULL;
	ima_tit_3=NULL;
	ima_how_to_play=NULL;
}

libgrafico :: ~libgrafico(void)
{
	eliminar_graficos();
}

/*!
 * \brief genera la biblioteca de graficos
 */
int libgrafico :: iniciar(int _modo_video)
{
	modo_video = _modo_video;
	
	if (crear_graficos())
		printf(_("error at creating the object 'grafico'\n"));
	else
		if (cargar_imagenes())
			printf(_("can't continue\n"));
		else
			return 0;

	return 1;
}

/*!
 * \brief genera todos los graficos del juego
 *
 * \return 1 en caso de error, 0 si todo es correcto
 */
int libgrafico :: crear_graficos(void)
{
	ima_gaucho=new grafico;
	ima_pelota_1=new grafico;
	ima_pelota_2=new grafico;
	ima_pelota_3=new grafico;
	ima_pelota_4=new grafico;
	ima_nivel=new grafico;
	ima_fondos=new grafico;
	ima_tiros=new grafico;
	ima_fuente_1=new grafico;
	ima_fuente_2=new grafico;
	ima_barra=new grafico;
	ima_item=new grafico;
	ima_mate=new grafico;
	ima_menu=new grafico;
	ima_tit_1=new grafico;
	ima_tit_2=new grafico;
	ima_tit_3=new grafico;
	ima_how_to_play=new grafico;
	
	return 0;
}


/*!
 * \brief carga todas la imagenes de la biblioteca
 *
 * \return 1 en caso de errores
 */
int libgrafico :: cargar_imagenes(void)
{
	if (modo_video)
		printf(_("+ Loading and reshapping images: "));
	else
		printf(_("+ Loading images: "));

	printf(".");
	fflush(stdout);
	ima_gaucho->iniciar("gaucho.png", 3, 8, 43, 105, modo_video);

	printf(".");
	fflush(stdout);
	ima_pelota_1->iniciar("pelota_1.png", 1, 3, 9, 9, modo_video);
	
	printf(".");
	fflush(stdout);
	ima_pelota_2->iniciar("pelota_2.png", 1, 3, 18, 18, modo_video);
	
	printf(".");
	fflush(stdout);
	ima_pelota_3->iniciar("pelota_3.png", 1, 3, 37, 37, modo_video);
	
	printf(".");
	fflush(stdout);
	ima_pelota_4->iniciar("pelota_4.png", 1, 3, 75, 75, modo_video);
	
	printf(".");
	fflush(stdout);
	ima_nivel->iniciar("niveles.png", 3, 7, 0, 0, modo_video);
	
	printf(".");
	fflush(stdout);
	ima_fondos->iniciar("fondos.jpg", 1, 1, 0, 0, modo_video);
	
	printf(".");
	fflush(stdout);
	ima_tiros->iniciar("tiros.png", 12, 1, 7, 0, modo_video);

	printf(".");
	fflush(stdout);
	ima_fuente_1->iniciar("fuente1.png", 2, 40, 0, 0, modo_video);

	printf(".");
	fflush(stdout);
	ima_fuente_2->iniciar("fuente2.png", 2, 40, 0, 0, modo_video);

	printf(".");
	fflush(stdout);
	ima_barra->iniciar("barra.png", 2, 10, 0, 0, modo_video);
	
	printf(".");
	fflush(stdout);
	ima_item->iniciar("items.png", 3, 7, 16, 32, modo_video);
	
	printf(".");
	fflush(stdout);
	ima_mate->iniciar("mate.png", 1, 4, 30, 27, modo_video);

	printf(".");
	fflush(stdout);
	ima_menu->iniciar("menu.jpg", 1, 1, 0, 0, modo_video);

	printf(".");
	fflush(stdout);
	ima_tit_1->iniciar("tit_1.png", 1, 1, 0, 0, modo_video);

	printf(".");
	fflush(stdout);
	ima_tit_2->iniciar("tit_2.png", 1, 1, 0, 0, modo_video);

	printf(".");
	fflush(stdout);
	ima_tit_3->iniciar("tit_3.png", 1, 1, 0, 0, modo_video);

	printf(".");
	fflush(stdout);
	ima_how_to_play->iniciar("how_to_play.png", 1, 1, 0, 0, modo_video);
	
	imprimir_ok();
	return 0;
}


/*!
 * \brief elimina los objetos grafico
 */
void libgrafico :: eliminar_graficos(void)
{
	delete ima_gaucho;
	delete ima_pelota_4;
	delete ima_pelota_3;
	delete ima_pelota_2;
	delete ima_pelota_1;
	delete ima_nivel;
	delete ima_fondos;
	delete ima_tiros;
	delete ima_fuente_1;
	delete ima_fuente_2;
	delete ima_barra;
	delete ima_item;
	delete ima_mate;
	delete ima_menu;
	delete ima_tit_1;
	delete ima_tit_2;
	delete ima_tit_3;
	delete ima_how_to_play;
}
