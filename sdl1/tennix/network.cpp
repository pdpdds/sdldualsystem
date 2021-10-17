
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

#include "game.h"
#include "network.h"

#include <assert.h>
#include <SDL_net.h>

/* HELPER FUNCTIONS */

Uint32
pack_float(float v, float min, float max)
{
    assert(v >= min && v < max);
    return (Uint32)((1U<<31) * (v-min) / (max-min));
}

float
unpack_float(Uint32 v, float min, float max)
{
    assert(v < (1U<<31));
    return v * (max-min) / (1U<<31) + min;
}

void
init_network()
{
    SDLNet_Init();
}

void
uninit_network()
{
    SDLNet_Quit();
}

TennixNet*
network_connect(const char* host, bool master)
{
    TennixNet* connection = (TennixNet*)malloc(sizeof(TennixNet));
    assert(connection != NULL);

    assert(SDLNet_ResolveHost(&(connection->peer), (const char*)host, 0) == 0);
    connection->base_port_local = (master)?(TENNIXNET_PORT_MASTER):(TENNIXNET_PORT_SLAVE);
    connection->base_port_remote = (master)?(TENNIXNET_PORT_SLAVE):(TENNIXNET_PORT_MASTER);
    connection->input_packet = SDLNet_AllocPacket(sizeof(NetworkInputData));
    connection->state_packet = SDLNet_AllocPacket(sizeof(NetworkGameState));
    connection->input_available = false;
    connection->state_available = false;
    connection->send_socket = SDLNet_UDP_Open(0);
    connection->recv_input_socket = SDLNet_UDP_Open(connection->base_port_local);
    connection->recv_state_socket = SDLNet_UDP_Open(connection->base_port_local+1);

    /* Fill with invalid data for first-time update */
    connection->input_data.x = 0xFF;
    connection->input_data.y = 0xFF;
    connection->input_data.keys = 0xFF;

    connection->master = master;

    return connection;
}

void
network_disconnect(TennixNet* connection)
{
    if (connection != NULL)
    {
        SDLNet_UDP_Close(connection->send_socket);
        SDLNet_UDP_Close(connection->recv_input_socket);
        SDLNet_UDP_Close(connection->recv_state_socket);
        SDLNet_FreePacket(connection->input_packet);
        SDLNet_FreePacket(connection->state_packet);
        free(connection);
    }
}

void
network_send_input(TennixNet* connection, NetworkInputData* src)
{
    assert(src != NULL);
    if (connection != NULL && memcmp(&(connection->input_data),
                src, sizeof(NetworkInputData)) != 0) {
        memcpy(connection->input_packet->data, src, sizeof(NetworkInputData));
        connection->input_packet->address.host = connection->peer.host;
        SDLNet_Write16(connection->base_port_remote, &(connection->input_packet->address.port));
        connection->input_packet->channel = -1;
        connection->input_packet->len = sizeof(NetworkInputData);
        SDLNet_UDP_Send(connection->send_socket, -1, connection->input_packet);
        /* Remember what we sent (what the remote end "sees") right now */
        memcpy(&(connection->input_data), src, sizeof(NetworkInputData));
    }
}

void
network_send_state(TennixNet* connection, GameState* src)
{
    assert(src != NULL);
    if (connection != NULL && connection->master) {
        net_serialize_gamestate(src, (NetworkGameState*)(connection->state_packet->data));
        connection->state_packet->address.host = connection->peer.host;
        SDLNet_Write16(connection->base_port_remote+1, &(connection->state_packet->address.port));
        connection->state_packet->channel = -1;
        connection->state_packet->len = sizeof(NetworkGameState);
        assert(SDLNet_UDP_Send(connection->send_socket, -1, connection->state_packet)!=0);
    }
}

void
network_receive(TennixNet* connection)
{
    if (connection != NULL) {
        connection->input_packet->len = sizeof(NetworkInputData);
        while (SDLNet_UDP_Recv(connection->recv_input_socket, connection->input_packet)) {
            connection->input_available = true;
        }

        if (!(connection->master)) {
            connection->state_packet->len = sizeof(NetworkGameState);
            while (SDLNet_UDP_Recv(connection->recv_state_socket, connection->state_packet)) {
                connection->state_available = true;
            }
        }
    }
}

void
network_get_input(TennixNet* connection, NetworkInputData* dest)
{
    assert(dest != NULL);
    if (connection != NULL && connection->input_available) {
        memcpy(dest, connection->input_packet->data, sizeof(NetworkInputData));
        connection->input_available = false;
    }
}

void
network_get_gamestate(TennixNet* connection, GameState* dest)
{
    assert(dest != NULL);
    if (connection != NULL && connection->state_available) {
        net_unserialize_gamestate((NetworkGameState*)
                (connection->state_packet->data), dest);
        connection->state_available = false;
    }
}


void
net_serialize_ball(const Ball* src, NetworkBall* dest)
{
    assert(src != NULL && dest != NULL);
    SDLNet_Write32(pack_float(src->x, -WIDTH, WIDTH*2), &(dest->x));
    SDLNet_Write32(pack_float(src->y, -HEIGHT, HEIGHT*2), &(dest->y));
    SDLNet_Write32(pack_float(src->z, -50, 50), &(dest->z));
    SDLNet_Write32(pack_float(src->move_x, -50, 50), &(dest->move_x));
    SDLNet_Write32(pack_float(src->move_y, -50, 50), &(dest->move_y));
    SDLNet_Write32(pack_float(src->move_z, -50, 50), &(dest->move_z));
    dest->ground_hit = src->ground_hit;
    dest->last_hit_by = src->last_hit_by;
    dest->inhibit_gravity = src->inhibit_gravity;
}

void
net_unserialize_ball(NetworkBall* src, Ball* dest)
{
    assert(src != NULL && dest != NULL);
    dest->x = unpack_float(SDLNet_Read32(&(src->x)), -WIDTH, WIDTH*2);
    dest->y = unpack_float(SDLNet_Read32(&(src->y)), -HEIGHT, HEIGHT*2);
    dest->z = unpack_float(SDLNet_Read32(&(src->z)), -50, 50);
    dest->move_x = unpack_float(SDLNet_Read32(&(src->move_x)), -50, 50);
    dest->move_y = unpack_float(SDLNet_Read32(&(src->move_y)), -50, 50);
    dest->move_z = unpack_float(SDLNet_Read32(&(src->move_z)), -50, 50);
    dest->ground_hit = src->ground_hit;
    dest->last_hit_by = src->last_hit_by;
    dest->inhibit_gravity = src->inhibit_gravity;
}

void
net_serialize_player(const Player* src, NetworkPlayer* dest)
{
    assert(src != NULL && dest != NULL);
    SDLNet_Write32(pack_float(src->x, 0, WIDTH*1.2), &(dest->x));
    SDLNet_Write32(pack_float(src->y, 0, HEIGHT*1.2), &(dest->y));
    SDLNet_Write32(pack_float(src->power, 0, 110), &(dest->power));
    dest->use_power = src->use_power;
    dest->score = src->score;
    dest->desire = src->desire;
    dest->game = src->game;
    memcpy(dest->sets, src->sets, sizeof(unsigned char)*(SETS_TO_WIN*2));
    SDLNet_Write32(pack_float(src->accelerate, 0, 200), &(dest->accelerate));
}

void
net_unserialize_player(NetworkPlayer* src, Player* dest)
{
    assert(src != NULL && dest != NULL);
    dest->x = unpack_float(SDLNet_Read32(&(src->x)), 0, WIDTH*1.2);
    dest->y = unpack_float(SDLNet_Read32(&(src->y)), 0, HEIGHT*1.2);
    dest->power = unpack_float(SDLNet_Read32(&(src->power)), 0, 110);
    dest->use_power = src->use_power;
    dest->score = src->score;
    dest->desire = src->desire;
    dest->game = src->game;
    memcpy(dest->sets, src->sets, sizeof(unsigned char)*(SETS_TO_WIN*2));
    dest->accelerate = unpack_float(SDLNet_Read32(&(src->accelerate)), 0, 200);
}

void
net_serialize_gamestate(const GameState* src, NetworkGameState* dest)
{
    int p;

    assert(src != NULL && dest != NULL);

    net_serialize_ball(&(src->ball), &(dest->ball));
    for (p=0; p<MAXPLAYERS; p++) {
        net_serialize_player(&(src->players[p]), &(dest->players[p]));
    }
    dest->serving_player = src->serving_player;
    dest->referee = src->referee;
    dest->current_set = src->current_set;
    dest->winner = src->winner;
    dest->sound_events = src->sound_events;
    dest->score_event = src->score_event;
    dest->ec_game = src->ec_game;
    dest->ec_sets = src->ec_sets;
    dest->status_message = src->status_message;
}

void
net_unserialize_gamestate(NetworkGameState* src, GameState* dest)
{
    int p;

    assert(src != NULL && dest != NULL);

    net_unserialize_ball(&(src->ball), &(dest->ball));
    for (p=0; p<MAXPLAYERS; p++) {
        net_unserialize_player(&(src->players[p]), &(dest->players[p]));
    }
    dest->serving_player = src->serving_player;
    dest->referee = src->referee;
    dest->current_set = src->current_set;
    dest->winner = src->winner;
    dest->sound_events = src->sound_events;
    dest->score_event = src->score_event;
    dest->ec_game = src->ec_game;
    dest->ec_sets = src->ec_sets;
    dest->status_message = src->status_message;
}

