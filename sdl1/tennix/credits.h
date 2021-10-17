
/**
 *
 * Tennix! SDL Port
 * Copyright (C) 2003, 2007, 2008, 2009 Thomas Perl <thp@thpinfo.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA  02110-1301, USA.
 *
 **/

#include "tennix.h"

#define CREDITS_EMPTY_TEXT " "
#define CREDITS_DURATION 3000
#define CREDITS_OFFSET 500

const char* credits_text[] = {
    "Tennix Classic Championship Tour 2011",
    "Version 2 (2011-02-10)",
    CREDITS_EMPTY_TEXT,
    "Code, graphics and idea by",
    "Thomas Perl (thp.io)",
    CREDITS_EMPTY_TEXT,
    "Public domain artwork by",
    "http://en.wikipedia.org/wiki/User:Ivan",
    "http://fr.wikipedia.org/wiki/Utilisateur:Arnaud_25",
    "http://en.wikipedia.org/wiki/User:Aleksmot",
    "http://commons.wikimedia.org/wiki/User:Alexisrael",
    "http://commons.wikimedia.org/wiki/User:Vardion",
#ifdef NONFREE_LOCATIONS
    CREDITS_EMPTY_TEXT,
    "Additional artwork by",
    "Martin Third (http://flickr.com/photos/mthird3rd/471720012/)",
#endif
    CREDITS_EMPTY_TEXT,
    "Using SDL_rotozoom by",
    "A. Schiffler",
    CREDITS_EMPTY_TEXT,
    "Visit the Tennix website:",
    URL,
    CREDITS_EMPTY_TEXT,
    CREDITS_EMPTY_TEXT,
    "Thanks for playing and send some feedback.",
    CREDITS_EMPTY_TEXT,
    CREDITS_EMPTY_TEXT,
    NULL
};

