
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

#ifndef __UTIL_H
#define __UTIL_H

void day_night(unsigned int width, unsigned int* start, unsigned int* end);

/**
 * On Maemo, we somehow don't get these defined in any of the header files,
 * so we define the prototypes here - if we don't do that, bad things will
 * happen.
 **/
#ifdef MAEMO
float fminf(float, float);
float fmaxf(float, float);
float fabsf(float);
float sinf(float);
float cosf(float);
#endif


#endif

