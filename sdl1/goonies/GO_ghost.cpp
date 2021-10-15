#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include "windows.h"
#endif

#include "math.h"
#include "stdlib.h"
#include "string.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include "List.h"

#include "auxiliar.h"
#include "2DCMC.h"
#include "Symbol.h"
#include "GLTile.h"
#include "keyboardstate.h"

#include "GLTManager.h"
#include "SoundManager.h"
#include "SFXManager.h"
#include "GObject.h"
#include "GO_enemy.h"
#include "GO_character.h"
#include "GO_ghost.h"

#include "GMap.h"

#include "TheGooniesCtnt.h"
#include "GObjectCreator.h"


GO_ghost::GO_ghost(int x, int y, int sfx_volume) : GO_enemy(x, y, sfx_volume)
{
    m_class = new Symbol(ghost_symbol);

    m_original_x = m_x;
    m_original_y = m_y;
    m_state = 0;
    m_state_cycle = 0;
    m_energy = 5;
}

bool GO_ghost::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    switch (m_state) {
        case 0:  // waiting for the player
            {
                GO_character *c = (GO_character *)map->get_object(character_symbol);
                m_x = m_original_x;
                m_y = m_original_y;

                // ghosts should be drawn in layer 3:
                if (layer != 3)
                {
                    if (map != 0)
                        map->remove_object(this, layer);
                    if (map != 0)
                        map->add_object(this, 3);
                }
                if (c != 0)
                {
                    if (c->get_x() >= m_x - 10 && c->get_x() < m_x + 30 &&
                            c->get_y() >= m_y && c->get_y() < m_y + 100) {
                        m_state = 1;
                        m_state_cycle = 0;
                        SFXM->SFX_play("sfx/ghost_appear", m_sfx_volume, get_angle(map), get_distance(map));

                    } else {
                        if (c->player_has("GO_bluebadbook") &&
                                c->get_bluebadbook_nghosts() > 0) {
                            c->set_bluebadbook_nghosts(c->get_bluebadbook_nghosts() - 1);
                            m_state = 1;
                            m_state_cycle = 0;
                        }
                    }
                }
            }
            break;
        case 1:  // initial pause
            if (m_state_cycle > 64) {
                m_state = 2;
                m_state_cycle = 0;
                m_hit_timmer = 0;
                {
                    GO_character *c = (GO_character *)map->get_object(character_symbol);
                    if (c != 0) {
                        m_movement_state = 0;
                        m_radius = 0;
                        m_destination_x = m_x;
                        m_destination_y = m_y;
                    }
                }
            }
            break;
        case 2:  // moving
            {
                m_movement_state++;
                m_x = float(m_destination_x + cos(m_movement_state * 0.02f) * m_radius);
                m_y = float(m_destination_y + sin(m_movement_state * 0.02f) * m_radius * 0.8f);

                if (m_radius < 60)
                    m_radius += 0.33f;
                {
                    GO_character *c = (GO_character *)map->get_object(character_symbol);
                    if (c != 0) {
                        if (c->get_x() - 10 < m_destination_x)
                            m_destination_x -= 0.5;
                        if (c->get_x() - 10 > m_destination_x)
                            m_destination_x += 0.5;
                        if (c->get_y() - 30 < m_destination_y)
                            m_destination_y -= 0.5;
                        if (c->get_y() - 30 > m_destination_y)
                            m_destination_y += 0.5;
                    }
                }

            }
		break;
        case 3:  // dying
            if (m_state_cycle > 32) {
                m_state = 0;
                m_state_cycle = 0;
            }
            break;
    }
    m_state_cycle++;

    if (m_hit_timmer > 0)
        m_hit_timmer--;
    return true;
}

void GO_ghost::draw(GLTManager *GLTM)
{
    int s = (m_state_cycle / 16) % 4;
    int s2 = (m_state_cycle / 8);
    float f = 1;

    switch (m_state) {
        case 0:
            m_last_tile_used = 0;
            break;
        case 1:
            f = (m_state_cycle) / 64.0f;
        case 2:
            if (s == 0) m_last_tile_used = GLTM->get ("ob_ghost1");
            if (s == 1) m_last_tile_used = GLTM->get ("ob_ghost2");
            if (s == 2) m_last_tile_used = GLTM->get ("ob_ghost3");
            if (s == 3) m_last_tile_used = GLTM->get ("ob_ghost2");
            if (m_hit_timmer > 0)
                f = float(0.5F + 0.5F * sin(m_hit_timmer * 0.6f));
            break;
        case 3:
			f = 1.0f;
            if (s2 == 0) m_last_tile_used = GLTM->get ("ob_ghost-death1");
            if (s2 == 1) m_last_tile_used = GLTM->get ("ob_ghost-death2");
            if (s2 == 2) m_last_tile_used = GLTM->get ("ob_ghost-death3");
            if (s2 == 3) m_last_tile_used = GLTM->get ("ob_ghost-death4");
			if (s2 >= 4) m_last_tile_used = 0;
            break;
    }

    if (m_last_tile_used != 0)
        m_last_tile_used->draw(1, 1, 1, f, m_x, m_y, 0, 0, 1);
}

bool GO_ghost::is_a(Symbol *c)
{
    if (c->cmp(ghost_symbol))
        return true;

    return GO_enemy::is_a(c);
}

bool GO_ghost::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
}

bool GO_ghost::player_hit(int *experience, int *score)
{
    if (m_state == 2) {
        m_energy--;
        m_hit_timmer = 32;
        if (m_energy <= 0) {
            m_energy = 5;
            m_state = 3;
            m_state_cycle = 0;
            *experience = 1;
            *score = 400;
        } else {
            *experience = 0;
            *score = 0;
        }
        return true;
    }
    return false;
}

int GO_ghost::enemy_hit(void)
{
    if (m_state == 2)
        return 8;
    return 0;
}
