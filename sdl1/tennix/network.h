
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

#ifndef __NETWORK_H
#define __NETWORK_H

#include "game.h"
#include "input.h"

#include <SDL_net.h>

/* Vanity ports "STNX" and "MTNX" */
#define TENNIXNET_PORT_SLAVE 7869
#define TENNIXNET_PORT_MASTER 6869

typedef struct {
    Uint32 x;
    Uint32 y;
    Uint32 z;
    Uint32 move_x;
    Uint32 move_y;
    Uint32 move_z;
    bool ground_hit;
    char last_hit_by;
    bool inhibit_gravity;
} NetworkBall;

typedef struct {
    Uint32 x;
    Uint32 y;
    Uint32 power;
    bool use_power;
    unsigned char score;
    unsigned char desire;
    char game;
    unsigned char sets[SETS_TO_WIN*2];
    Uint32 accelerate;
} NetworkPlayer;

typedef struct {
    NetworkBall ball;
    NetworkPlayer players[MAXPLAYERS];
    unsigned char serving_player;
    referee_t referee;
    unsigned char current_set;
    winner_t winner;
    soundevent_t sound_events;
    scoreevent_t score_event;
    unsigned char score_time;
    eventcounter_t ec_game;
    eventcounter_t ec_sets;
    statusmessage_t status_message;
} NetworkGameState;

void
init_network();

void
uninit_network();

TennixNet*
network_connect(const char* host, bool master);

void
network_disconnect(TennixNet* connection);

void
network_send_input(TennixNet* connection, NetworkInputData* src);

void
network_send_state(TennixNet* connection, GameState* src);

void
network_receive(TennixNet* connection);

void
network_get_input(TennixNet* connection, NetworkInputData* dest);

void
network_get_gamestate(TennixNet* connection, GameState* dest);

void
net_serialize_ball(const Ball* src, NetworkBall* dest);

void
net_unserialize_ball(NetworkBall* src, Ball* dest);

void
net_serialize_player(const Player* src, NetworkPlayer* dest);

void
net_unserialize_player(const NetworkPlayer* src, Player* dest);

void
net_serialize_gamestate(const GameState* src, NetworkGameState* dest);

void
net_unserialize_gamestate(NetworkGameState* src, GameState* dest);

#endif

