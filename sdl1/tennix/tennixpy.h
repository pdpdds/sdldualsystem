

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

#ifndef __TENNIXPY_H
#define __TENNIXPY_H

#ifdef TENNIX_PYTHON

#include "archive.hh"

extern PyThreadState* _py_save;

PyObject* tennixpy_register_bot(PyObject* self, PyObject* bot_class);
void tennixpy_unregister_bot(PyObject* bot);
void tennixpy_get_bot_name(PyObject* bot_class, char* dest, int maxlen);

PyObject* tennixpy_create_bot(PyObject* bot_class, GameState* s, int player_id);
void tennixpy_destroy_bot(PyObject* bot);

float tennixpy_bot_get_axis(PyObject* bot, int axis);
char tennixpy_bot_get_key(PyObject* bot, int key);

PyObject* tennixpy_get_ball_pos(PyObject* self, PyObject* gamestate);
PyObject* tennixpy_get_power(PyObject* self, PyObject* args);
PyObject* tennixpy_get_position(PyObject* self, PyObject* args);

PyObject* tennixpy_create_module(void);

void tennixpy_init(TennixArchive& tnxar);
void tennixpy_uninit();

#endif

#endif
