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
#include "GMap.h"

#include "GO_fallingwater.h"
#include "GO_watersplash.h"

#include "GObjectCreator.h"


extern bool water_reflection;

GO_fallingwater::GO_fallingwater(int x, int y, int dx, int dy, int sfx_volume) : GO_enemy(x, y, sfx_volume)
{
    m_class = new Symbol(fallingwater_symbol);

    m_dx = dx;
    m_dy = dy;
    m_speed = 0;
    m_type = 0;

}

bool GO_fallingwater::cycle(VirtualController *k, GMap *map, int layer, TheGoonies *game, GLTManager *GLTM, SFXManager *SFXM)
{
    int water_level = map->get_water_level();
    int lava_level = map->get_lava_level();
    int level;
    m_state_cycle++;

    level = (water_level < lava_level ? water_level : lava_level);

    if (water_level < lava_level)
        m_type = 0;
    else
        m_type = 1;

    m_y += m_speed;
    if (m_speed < 8)
        m_speed += 0.25;

    if (m_y + m_dy >= level + 20) {
        m_dy = (int)(level - m_y);
        int i;

        for (i = 0;i < 4;i++) {
            GO_watersplash *o = new GO_watersplash((int)(m_x + rand() % m_dx), level + 12 + rand() % 16, ( -16 + (rand() % 32)) / 8.0f, -((rand() % 8)) / 16.0f, m_sfx_volume);
            map->add_auxiliar_object(o, 4);
        }

        if (m_dy < 0) {
            return false;
		}
    }
    return true;
}

void GO_fallingwater::draw(GLTManager *GLTM)
{
	// we need different colors for the 'water'
	// based on type (0 = water, 1 = lava)
	// and if the water reflection effect is on or off
    if (m_type == 0) {
		if (water_reflection) {
			glColor4f(0.3f, 0.3f, 1.0f, 0.5f);
		} else {
			glColor4f(0.11f, 0.11f, 0.35f, 0.5f);
		}
    } else {
		if (water_reflection) {
			glColor4f(1.0f, 0.0f, 0.0f, 0.75f);
		} else {
			glColor4f(0.56f, 0.0f, 0.0f, 0.75f);
		}
	}
	
    glNormal3f(0.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    {
        glVertex3f(m_x, m_y, 0);
        glVertex3f(m_x, m_y + m_dy, 0);
        glVertex3f(m_x + m_dx, m_y + m_dy, 0);
        glVertex3f(m_x + m_dx, m_y, 0);
    }
    glEnd();

	if (water_reflection) {
		bool tmp;
		GLuint tname = m_map->get_water_info_texture();
		
		tmp = (glIsEnabled(GL_TEXTURE_2D) ? true : false);
		if (!tmp)
			glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tname);
		glBegin(GL_QUADS);
		{
			float fx = m_x / 640.0f, fy = (400 - m_y) / 400.0f;
			float fdx = m_dx / 640.0f, fdy = m_dy / 400.0f;
			float fsx;
			float fsx2;
			float fsy;
			float fsy2;

			if (m_type == 0) {
				fsx = float(sin(m_x + m_state_cycle * 0.1) * 0.02f);
				fsx2 = float(sin(m_x + m_dx + m_state_cycle * 0.1) * 0.02f);
				fsy = float(sin(m_x + m_state_cycle * 0.15) * 0.01f);
				fsy2 = float(sin(m_x + m_dy + m_state_cycle * 0.15) * 0.01f);
			} else {
				fsx = float(sin(m_x + m_state_cycle * 0.025) * 0.01f);
				fsx2 = float(sin(m_x + m_dx + m_state_cycle * 0.025) * 0.01f);
				fsy = float(sin(m_x + m_state_cycle * 0.0375) * 0.005f);
				fsy2 = float(sin(m_x + m_dy + m_state_cycle * 0.0375) * 0.005f);
			}

			glTexCoord2f(fx + fsx, fy + fsy);
			glVertex3f(m_x, m_y, 0);

			glTexCoord2f(fx + fsx, fy - fdy + fsy2);
			glVertex3f(m_x, m_y + m_dy, 0);

			glTexCoord2f(fx + fdx + fsx2, fy - fdy + fsy2);
			glVertex3f(m_x + m_dx, m_y + m_dy, 0);

			glTexCoord2f(fx + fdx + fsx2, fy + fsy);
			glVertex3f(m_x + m_dx, m_y, 0);
		}
		glEnd();

		if (!tmp)
			glDisable(GL_TEXTURE_2D);
	}
}

bool GO_fallingwater::is_a(Symbol *c)
{
    if (c->cmp(fallingwater_symbol))
        return true;

    return GO_enemy::is_a(c);
}

bool GO_fallingwater::is_a(char *c)
{
    bool retval;
    Symbol *s = new Symbol(c);

    retval = is_a(s);

    delete s;

    return retval;
}

bool GO_fallingwater::collision(GObject *o)
{
    float x[2];
    float y[2];
    C2DCMC *cmc;
    bool col;

    if (o->get_last_tile_used() == 0)
        return false;

    x[0] = 0;
    y[0] = 0;
    x[1] = float(m_dx);
    y[1] = float(m_dy);
    cmc = new C2DCMC(x, y, 2);

    col = o->get_last_tile_used()->get_cmc()->collision_no_rotation(o->get_x(), o->get_y(), cmc, m_x, m_y);

    delete cmc;

    return col;
}

int GO_fallingwater::enemy_hit(void)
{
    return 1;
}
