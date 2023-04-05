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
#include "audio.h"
#include "utils.h"
#include "int.h"

audio :: audio()
{
	hab_sonido = hab_musica = 0;
}

audio :: ~audio()
{
	if (hab_sonido)
		descargar_sonidos();
	
	if (hab_musica)
		descargar_musicas();

	Mix_CloseAudio();
}


/*!
 * \brief inicia el subsistema de audio de la biblioteca
 *
 * \param _hab_sonido habilita/deshabilita los sonidos
 * \param _hab_sonido habilita/deshabilita las musicas
 */
void audio :: iniciar(int _hab_sonido, int _hab_musica)
{
	hab_sonido = _hab_sonido;
	hab_musica = _hab_musica;

	if (hab_sonido | hab_musica)
	{
		printf(_("+ Starting audio system: "));

		if (SDL_InitSubSystem( SDL_INIT_AUDIO) == -1 ||\
				Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0)
		{
			imprimir_error();
			printf("error: %s\n", SDL_GetError());
			hab_musica = hab_sonido = 0;
			return;
		}
		else
		{
			imprimir_ok();
		}
	}
	else
	{
		printf(_("- Audio system disable from options\n"));
		return;
	}

	
	if (hab_sonido)
		cargar_sonidos();
	else
		printf(_("- Sound system disable from options\n"));


	if (hab_musica)
	{
		Mix_VolumeMusic(100);
		cargar_musicas();
	}
	else
		printf(_("- Music system disable from options\n"));
}


/*!
 * \brief reproduce un sonido
 *
 * \param indice indice del sonido
 */
void audio :: play(int indice)
{
	if (hab_sonido)
		Mix_PlayChannel(0, sonidos[indice], 0);
}

/*!
 * \brief reproduce musica
 *
 * \param indice numero de musica
 */
void audio :: play_musica(int indice)
{
	if (hab_musica)
		canal_musica=Mix_FadeInMusic(musicas[indice], -1, 50);
}

/*!
 * \brief almacena los archivos de sonido en memoria
 */
void audio :: cargar_sonidos(void)
{
	char nombres[][200]= {\
		{".data/sounds/tiro.wav"},\
		{".data/sounds/mata.wav"},\
		{".data/sounds/pierde.wav"},\
		{".data/sounds/tecla1.wav"},\
		{".data/sounds/tecla2.wav"},\
		{".data/sounds/tic.wav"},\
		{".data/sounds/gancho.wav"},\
		{".data/sounds/toc.wav"},\
		{".data/sounds/romper.wav"},\
		{".data/sounds/alarma.wav"},\
		{".data/sounds/item.wav"},\
		{".data/sounds/boom.wav"}};
	int i;
	
	printf(_("+ Loading sound files: "));

	for (i=0; i<12; i++)
	{
		printf(".");
		fflush(stdin);

		sonidos[i] = Mix_LoadWAV(nombres[i]);

		if (sonidos[i] == NULL)
		{
			imprimir_error();
			printf(_("Can't load '%s': %s\n"), nombres[i],\
					SDL_GetError());
			printf(_("- Disable sound system because of"\
						" errors\n"));
			hab_sonido=0;
			return;
		}
	}
	imprimir_ok();
}

/*!
 * \brief almacena las archivos de musicas en memoria
 */
void audio :: cargar_musicas(void)
{
	char nombres[][200] = {\
		{"./data/music/menu.xm"},\
		{"./data/music/menu.xm"}}; // temp
	int i;
	
	printf(_("+ Loading music files: "));

	for (i=0; i<2; i++)
	{
		printf(".");
		fflush(stdin);

		musicas[i] = Mix_LoadMUS(nombres[i]);

		if (musicas[i] == NULL)
		{
			imprimir_error();
			printf(_("Can't load '%s': %s\n"), nombres[i], \
					SDL_GetError());
			
			printf(_("- Disable music system because of errors\n"));
			hab_musica=0;
			return;
		}
	}

	imprimir_ok();
}

/*!
 * \brief libera memoria
 */
void audio :: descargar_sonidos(void)
{
	int i;
	
	for (i=0; i < 12; i++)
		Mix_FreeChunk(sonidos[i]);
}

/*!
 * \brief libera memoria
 */
void audio :: descargar_musicas(void)
{
	Mix_Resume(canal_musica);
//	Mix_FreeMusic(musicas[0]);
//	Mix_FreeMusic(musicas[1]);
}
