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


#include <stdio.h>
#include <stdlib.h>
#include "mundo.h"
#include "int.h"

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif


/*!
 * \brief funcion inicial de todo el programa
 */
int main (int argc, char * argv [])
{

#ifdef ENABLE_NLS
	printf ("Locale: %s\n", setlocale (LC_ALL, ""));
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "");
	textdomain (GETTEXT_PACKAGE);
#else
	printf ("Locale: disabled\n");
#endif

	mundo mundo;
	
	printf("\n");
	printf(" Don Ceferino Haza? - version 1.0"  "\n");
	printf(" (c) - Hugo Ruscitti - www.losersjuegos.com.ar\n");
	printf(_("Remember: run 'ceferinosetup' for more options, and "\
				"'ceferinoeditor' to edit any level.\n"));
	printf("\n");
	
	if (mundo.cargar_opciones(".ceferino"))
		return 1;
	
	if (mundo.iniciar())
		return 1;
	
	mundo.correr();
	mundo.eliminar();

	printf(_("\nThanks for playing\n\n"));
	
	return 0;
}
