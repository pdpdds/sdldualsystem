
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

#ifndef __INPUT_H
#define __INPUT_H

#include "archive.hh"
#include "graphics.h"

#ifdef TENNIX_PYTHON
#include <Python.h>
#endif

#define JOYSTICK_PERCENTIZE(x) ((float)(x)/(float)(32768))

#define MAX_INPUT_DEVICES 16

enum {
    INPUT_COORD_TYPE_PLAYER,
    INPUT_COORD_TYPE_BALL
};

enum {
    INPUT_TYPE_KEYBOARD,
    INPUT_TYPE_JOYSTICK,
    INPUT_TYPE_MOUSE,
    INPUT_TYPE_NETWORK,
#ifdef TENNIX_PYTHON
    INPUT_TYPE_AI_PYTHON,
#endif
    INPUT_TYPE_MAX
};

enum {
    INPUT_AXIS_X,
    INPUT_AXIS_Y
};

enum {
    INPUT_KEY_HIT,
    INPUT_KEY_TOPSPIN,
    INPUT_KEY_SMASH,
    INPUT_KEY_COUNT
};

enum {
    NET_KEY_HIT     = 1 << INPUT_KEY_HIT,
    NET_KEY_TOPSPIN = 1 << INPUT_KEY_TOPSPIN,
    NET_KEY_SMASH   = 1 << INPUT_KEY_SMASH
};

#define INPUT_DEVICE_NAME_MAX 100

typedef struct {
    unsigned char type;
    char name[INPUT_DEVICE_NAME_MAX];

    /* Joystick-specific items */
    SDL_Joystick* joystick;
    int x_axis;
    int y_axis;

    /* Joystick, keyboard and mouse */
    int input_keys[INPUT_KEY_COUNT];

    /* Keyboard-specific items */
    int up_key;
    int down_key;
    int left_key;
    int right_key;

    /* Mouse-specific items */
    int mx;
    int my;
    int player_x;
    int player_y;

    /* Network-specific items */
    NetworkInputData net;

#ifdef TENNIX_PYTHON
    /* Python-specific items */
    PyObject* py_bot_class;
    PyObject* py_bot;
#endif

    /* The picture for this input device */
    image_id icon;

    /* If nonzero, be exclusive to this player ID */
    int exclusive_to_player;
} InputDevice;

void init_input(TennixArchive& tnxar);
void uninit_input();

#ifdef TENNIX_PYTHON
void input_add_python_bot(PyObject* pyclass);
#endif

InputDevice* find_input_devices(unsigned int*);

const char* input_device_get_name(InputDevice*);
float input_device_get_axis(InputDevice*, unsigned const char);
char input_device_get_key(InputDevice*, unsigned const char);

void input_device_join_game(InputDevice* device, void* gamestate, int player_id);
void input_device_part_game(InputDevice*);


#endif

