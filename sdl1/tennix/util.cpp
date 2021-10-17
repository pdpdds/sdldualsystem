
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
#include "util.h"

#include <time.h>
#include <assert.h>

void day_night(unsigned int width, unsigned int* start, unsigned int* end)
{
    time_t t = time(NULL);
    unsigned int offset = 60; /* set this so the day/night fits for London */

    assert(start != NULL);
    assert(end != NULL);

    *start = (unsigned int)(offset - (t/60)*width/(60*24)) % width;
    *end = (unsigned int)(*start + width/2) % width;
}

