#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "string.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_mixer.h"

#include "List.h"
#include "Symbol.h"
#include "2DCMC.h"
#include "auxiliar.h"
#include "GLTile.h"
#include "2DCMC.h"
#include "keyboardstate.h"
#include "randomc.h"
#include "VirtualController.h"

#include "GLTManager.h"
#include "SoundManager.h"
#include "SFXManager.h"
#include "MusicManager.h"
#include "GObject.h"
#include "GO_character.h"
#include "GO_skulldoor.h"
#include "GO_enemy.h"
#include "GO_fratelli.h"
#include "GooniesScript.h"
#include "GMap.h"
#include "TheGoonies.h"
#include "TheGooniesApp.h"

#include "TheGooniesCtnt.h"

#include "GObjectCreator.h"

#include "font_extractor.h"

#include "debug.h"

#define MAPPAUSE_TIME 25

extern bool ambient_light;
extern int hiscore;

TheGoonies::TheGoonies(int level, int roomx, int roomy, int sfx_volume,
                       int music_volume, int ambient_volume, GLTManager *GLTM)
{
    m_sfx_volume = sfx_volume;
    m_music_volume = music_volume;
    m_ambient_volume = ambient_volume;

    m_current_map = get_map(level, roomx, roomy);
    {
        int x = (int)(216 * 1.25f);
        int y = (int)(150 * 1.25f);
        List<int> pars;
        GObject *o = m_current_map->m_map->get_object(entrydoor_symbol);
        if (o != 0)
            x = (int)(o->get_x() - 4);
        if (o != 0)
            y = (int)(o->get_y() + 13);
        pars.Add(new int(0));

        m_character = (GO_character *)GObject_create(character_symbol, x, y, sfx_volume, &pars);
        m_current_map->m_map->add_object(m_character, m_character->get_layer());
    }

    m_previous_map = 0;

    m_cycle = 0;

    m_current_music[0] = 0;
    m_current_ambient[0] = 0;

    m_gamepaused = PAUSESTATE_PLAYING;
    m_gamepaused_timmer = 0;
    m_pausemap_r_computed = true;

    m_level_complete = false;
    m_player_critical = false;

    m_current_zoom = 0;

    // Load the room/music table:
    {
        FILE *fp;
        fp = fopen("maps/musics.txt", "r");
        if (fp != 0)
        {
            int n;
            char tmp[80], tmp2[80], tmp3[80];
            do {
                n = fscanf(fp, "%s %s %s", tmp, tmp2, tmp3);
                if (n == 3) {
                    char *s = new char[strlen(tmp) + 1];
                    strcpy(s, tmp);
                    m_music_tbl_room.Add(s);

                    s = new char[strlen(tmp2) + 1];
                    strcpy(s, tmp2);
                    m_music_tbl_music.Add(s);

                    s = new char[strlen(tmp3) + 1];
                    strcpy(s, tmp3);
                    m_music_tbl_ambient.Add(s);
                } // if
            } while (n == 3);
            fclose(fp);
        }
    }

    // Load the room/fratelli table:
    {
        FILE *fp;
        fp = fopen("maps/fratellies.txt", "r");
        if (fp != 0)
        {
            int n;
            char tmp[80], tmp2[80];
            GMapHolder *m;
            GO_fratelli *o;

            do {
                n = fscanf(fp, "%s %s", tmp, tmp2);
                if (n == 2) {
                    {
                        int nl, nx, ny;
                        nl = tmp[5] - '0';
                        nx = tmp[7] - '0';
                        ny = tmp[9] - '0';
                        if (nl == level) {
                            m = get_map_not_player(nl, nx, ny);
                            m_multiroom_objects_map.Add(m);
                            m->m_map->search_for_bridges(GLTM);
                        } else {
                            m = 0;
                        } // if
                    }

                    if (m != 0) {
                        Symbol *n = new Symbol(tmp2);
                        List<int> l;
                        int i, y = 200;

                        for (i = 64;i < m->m_map->get_dy();i += 2) {
                            if (m->m_map->collision_with_background(GLTM->get
                                                                    ("ob_yellowguy-l1"), 320, i, GLTM) &&
                                    !m->m_map->collision_with_background(GLTM->get
                                                                         ("ob_yellowguy-l1"), 320, i - 2, GLTM)) y = i - 8;
                        }

                        o = (GO_fratelli *)GObject_create(n, 320, y, m_sfx_volume, &l);
                        delete n;
                        m_multiroom_objects.Add(o);
                    }
                }
            } while (n == 2);
            fclose(fp);
        }
    }

}

TheGoonies::~TheGoonies()
{	
    m_multiroom_objects_map.ExtractAll();
}

void TheGoonies::clear(void)
{
    m_current_map->m_map->remove_object(m_character, m_character->get_layer());
    m_multiroom_objects.Delete();
    m_multiroom_objects_map.ExtractAll();
    m_maps.Delete();
    m_maps_buffer.Delete();
    m_current_map = 0;
}


void TheGoonies::level_change(int level, int roomx, int roomy, GLTManager *GLTM)
{

    m_current_map = get_map(level, roomx, roomy);
    {
        int x = (int)(216 * 1.25f);
        int y = (int)(150 * 1.25f);
        List<int> pars;
        GObject *o = m_current_map->m_map->get_object(entrydoor_symbol);
        if (o != 0)
            x = (int)(o->get_x() - 4);
        if (o != 0)
            y = (int)(o->get_y() + 13);
        pars.Add(new int(0));

        if (m_character != 0)
            delete m_character;
		
        m_character = (GO_character *)GObject_create(character_symbol, x, y, m_sfx_volume, &pars);
        m_current_map->m_map->add_object(m_character, m_character->get_layer());
    }

    m_cycle = 0;

    m_current_music[0] = 0;
    m_current_ambient[0] = 0;

    m_gamepaused = PAUSESTATE_PLAYING;
    m_gamepaused_timmer = 0;
    m_pausemap_r_computed = true;

    m_level_complete = false;

    m_current_zoom = 0;

    // Load the room/fratelli table:
    {
        FILE *fp;
        fp = fopen("maps/fratellies.txt", "r");
        if (fp != 0)
        {
            int n;
            char tmp[80], tmp2[80];
            GMapHolder *m;
            GO_fratelli *o;

            do {
                n = fscanf(fp, "%s %s", tmp, tmp2);
                if (n == 2) {
                    {
                        int nl, nx, ny;
                        nl = tmp[5] - '0';
                        nx = tmp[7] - '0';
                        ny = tmp[9] - '0';
                        if (nl == level) {
                            m = get_map_not_player(nl, nx, ny);
                            m_multiroom_objects_map.Add(m);
                            m->m_map->search_for_bridges(GLTM);
                        } else {
                            m = 0;
                        }
                    }

                    if (m != 0) {
                        Symbol *n = new Symbol(tmp2);
                        List<int> l;
                        int i, y = 200;

                        for (i = 64;i < m->m_map->get_dy();i += 2) {
                            if (m->m_map->collision_with_background(GLTM->get
                                                                    ("ob_yellowguy-l1"), 320, i, GLTM) &&
                                    !m->m_map->collision_with_background(GLTM->get
                                                                         ("ob_yellowguy-l1"), 320, i - 2, GLTM)) y = i - 8;
                        }

                        o = (GO_fratelli *)GObject_create(n, 320, y, m_sfx_volume, &l);
                        delete n;
                        m_multiroom_objects.Add(o);
                    }
                }
            } while (n == 2);
            fclose(fp);
        }
    }

}

void TheGoonies::map_filename(char *fn, int level, int roomx, int roomy)
{
    sprintf(fn, "maps/level%i-%i-%i.gmp", level, roomx, roomy);
}

bool TheGoonies::cycle(VirtualController *v, GLTManager *GLTM, SFXManager *SFXM, MusicManager *MusicM)
{
    GMapHolder *old_map = m_current_map;

    if (m_level_complete)
        return false;
    {
        // check if the music has to be changed:
        int i;
        char tmp[80], *music, *ambient;
        sprintf(tmp, "level%i-%i-%i.gmp", m_current_map->m_l, m_current_map->m_x, m_current_map->m_y);
        i = m_music_tbl_room.Position(tmp);
        if (i != -1) {
            music = m_music_tbl_music[i];
            ambient = m_music_tbl_ambient[i];

            if (strcmp(music, m_current_music) != 0) {
                strcpy(m_current_music, music);
				MusicM->music_fadein(MUSIC_ID,m_current_music,m_music_volume,-1,50);
            }
            if (strcmp(ambient, m_current_ambient) != 0) {
                strcpy(m_current_ambient, ambient);
				MusicM->music_play(AMBIENT_ID,m_current_ambient,m_ambient_volume,-1);
            }
        }
    }

    // Pause the game until the "level indicator" sign has disappeared:
    m_cycle++;
    if (m_cycle < 150 && m_current_map->m_l > 0) {
        if (m_cycle > 50 && v->m_button[0])
            m_cycle = 150;
        return true;
    }

    if (m_current_zoom > 0 && m_character->get_state() != CSTATE_ENTERING_DOOR) {
        m_current_zoom--;
    }

    if (m_character->get_state() == CSTATE_ENTERING_DOOR && m_current_zoom < 50) {
        m_current_zoom++;
    }

    if (m_character->get_state() == CSTATE_DYING) {
		MusicM->music_stop(MUSIC_ID);
		MusicM->music_stop(AMBIENT_ID);
    }

    m_gamepaused_timmer++;
    if (v->m_pause && !v->m_old_pause && m_current_zoom == 0 && m_previous_map == 0) {
        switch (m_gamepaused) {
            case PAUSESTATE_PLAYING:
                m_gamepaused = PAUSESTATE_PAUSING;
                m_gamepaused_timmer = 0;
				SFXM->SFX_play("sfx/pause",m_sfx_volume);
				SFXM->SFX_pause_continuous();
				MusicM->music_pause(MUSIC_ID);
//				MusicM->music_resume(AMBIENT_ID);
                break;
            case PAUSESTATE_PAUSING:
                m_gamepaused = PAUSESTATE_UNPAUSING;
                m_gamepaused_timmer = MAPPAUSE_TIME - m_gamepaused_timmer;
				SFXM->SFX_resume_continuous();
				MusicM->music_resume(MUSIC_ID);
//				MusicM->music_resume(AMBIENT_ID);
                break;
            case PAUSESTATE_PAUSED:
                m_gamepaused = PAUSESTATE_UNPAUSING;
                m_gamepaused_timmer = 0;
                SFXM->SFX_resume_continuous();
				MusicM->music_resume(MUSIC_ID);
//				MusicM->music_resume(AMBIENT_ID);
                break;
            case PAUSESTATE_UNPAUSING:
                m_gamepaused = PAUSESTATE_PAUSING;
                m_gamepaused_timmer = MAPPAUSE_TIME - m_gamepaused_timmer;
				SFXM->SFX_play("sfx/pause",m_sfx_volume);
                SFXM->SFX_pause_continuous();
				MusicM->music_pause(MUSIC_ID);
//				MusicM->music_pause(AMBIENT_ID);
                break;
        } 
    }
	
    if (m_gamepaused == PAUSESTATE_PLAYING) {

        // Temporarily add the multi-room objects to the current map:
        {
            GObject *f;
            GMapHolder *m;
            m_multiroom_objects.Rewind();
            m_multiroom_objects_map.Rewind();
            while (m_multiroom_objects.Iterate(f) && m_multiroom_objects_map.Iterate(m))
            {
                if (m_current_map == m) {
                    m->m_map->add_object(f, f->get_layer());
                }
            }
		}

        // Execute player map:
        if (m_character->m_clock_timer > 0) {
            m_current_map->m_map->cycle_clock(v, this, GLTM, SFXM);
        } else {
            m_current_map->m_map->cycle(v, this, GLTM, SFXM);
            m_current_map->m_map->m_script->execute(m_character, m_current_map->m_map, GLTM, SFXM, m_sfx_volume);

            // Execute multiroom objects cycles:
            {
                GObject *f;
                GMapHolder *m;
                m_multiroom_objects.Rewind();
                m_multiroom_objects_map.Rewind();
                while (m_multiroom_objects.Iterate(f) && m_multiroom_objects_map.Iterate(m))
                {
                    if (m != m_current_map) {
                        f->cycle(0, m->m_map, f->get_layer(), this, GLTM, 0);                        
                    }

                    // remove again the fratellis from the current map:
                    m->m_map->remove_object(f, f->get_layer());

                    // Multiroom object room change:
                    // right:
                    if (f->get_x() >= m->m_map->get_dx()) {
                        // does the next map exist?
                        if (get_map_not_player(m->m_l, m->m_x + 1, m->m_y) != 0) {
                            m = get_map_not_player(m->m_l, m->m_x + 1, m->m_y);
                            m->m_map->search_for_bridges(GLTM);
                            m_multiroom_objects_map.SetObj(m_multiroom_objects.PositionRef(f), m);
                            f->set_x(0);
                        } else {
                            f->set_x(float(m->m_map->get_dx() - 1));
                        }
                    }
                    // left:
                    if (f->get_x() < 0) {
                        // does the next map exist?
                        if (get_map_not_player(m->m_l, m->m_x - 1, m->m_y) != 0) {
                            m = get_map_not_player(m->m_l, m->m_x - 1, m->m_y);
                            m->m_map->search_for_bridges(GLTM);
                            m_multiroom_objects_map.SetObj(m_multiroom_objects.PositionRef(f), m);
                            f->set_x(float(m->m_map->get_dx() - 1));
                        } else {
                            f->set_x(0);
                        }
                    }
                    // down:
                    if (f->get_y() >= m_current_map->m_map->get_dy()) {
                        // does the next map exist?
                        if (get_map_not_player(m->m_l, m->m_x, m->m_y + 1) != 0) {
                            m = get_map_not_player(m->m_l, m->m_x, m->m_y + 1);
                            m->m_map->search_for_bridges(GLTM);
                            m_multiroom_objects_map.SetObj(m_multiroom_objects.PositionRef(f), m);
                            f->set_y(8);
                        } else {
                            f->set_y(float(m->m_map->get_dy() - 1));
                        }
                    }
                    // up:
                    if (f->get_y() < 8) {
                        // does the next map exist?
                        if (get_map_not_player(m->m_l, m->m_x, m->m_y - 1) != 0) {
                            m = get_map_not_player(m->m_l, m->m_x, m->m_y - 1);
                            m->m_map->search_for_bridges(GLTM);
                            m_multiroom_objects_map.SetObj(m_multiroom_objects.PositionRef(f), m);
                            f->set_y(float(m->m_map->get_dy() - 1));
                        } else {
                            f->set_y(0);
                        }
                    }
                }
            }
        }

        // check for room change:
        // by a skull door:
        if (m_character->room_change_request()) {
            int x, y, d;
            x = m_character->get_requested_room_x();
            y = m_character->get_requested_room_y();
            d = m_character->get_requested_room_door();
            if (x == -1 && y == -1) {
                // Level complete!!!!
				MusicM->music_stop(MUSIC_ID);
				MusicM->music_stop(AMBIENT_ID);
                m_level_complete = true;
                m_character->inc_score(10000);
                return false;
            }
            if (get_map(m_current_map->m_l, x, y) != 0) {
                m_current_map->m_map->reset(m_sfx_volume);
                SFXM->SFX_stop_continuous();
                m_current_map->m_map->remove_object(m_character, m_character->get_layer());
                m_current_map = get_map(m_current_map->m_l, x, y);
                m_current_map->m_map->add_object(m_character, m_character->get_layer());

                // compute new position:
                {
                    int i = 0;
                    List<GObject> *l = m_current_map->m_map->get_objects(skulldoor_symbol);
                    GO_skulldoor *sd;
                    while (!l->EmptyP())
                    {
                        sd = (GO_skulldoor *)l->ExtractIni();
                        if (i == d) {
                            m_character->set_x(sd->get_x() - 4);
                            m_character->set_y(sd->get_y() + 16);
                        }
                        i++;
                    }
                }
            }
            m_character->clear_change_request();
            m_previous_map = 0;
        }
        // right:
        if (m_character->get_x() >= m_current_map->m_map->get_dx()) {
            // does the next map exist?
            if (get_map(m_current_map->m_l, m_current_map->m_x + 1, m_current_map->m_y) != 0) {
				SFXM->SFX_stop_continuous();
                m_current_map->m_map->reset(m_sfx_volume);
                m_current_map->m_map->remove_object(m_character, m_character->get_layer());
                m_previous_map = m_current_map;
                m_previous_map_direction = 0;
                m_previous_map_cycle = ROOM_TRANSITION_TIME;
                m_current_map = get_map(m_current_map->m_l, m_current_map->m_x + 1, m_current_map->m_y);
                m_current_map->m_map->add_object(m_character, m_character->get_layer());
                m_character->set_x(0);
                m_character->set_camefrom("left");
            } else {
                m_character->set_x(float(m_current_map->m_map->get_dx() - 1));
            }
        }
        // left:
        if (m_character->get_x() < 0) {
            // does the next map exist?
            if (get_map(m_current_map->m_l, m_current_map->m_x - 1, m_current_map->m_y) != 0) {
				SFXM->SFX_stop_continuous();
                m_current_map->m_map->reset(m_sfx_volume);
                m_current_map->m_map->remove_object(m_character, m_character->get_layer());
                m_previous_map = m_current_map;
                m_previous_map_direction = 1;
                m_previous_map_cycle = ROOM_TRANSITION_TIME;
                m_current_map = get_map(m_current_map->m_l, m_current_map->m_x - 1, m_current_map->m_y);
                m_current_map->m_map->add_object(m_character, m_character->get_layer());
                m_character->set_x(float(m_current_map->m_map->get_dx() - 1));
                m_character->set_camefrom("right");
            } else {
                m_character->set_x(0);
            }
        }
        // down:
        if (m_character->get_y() >= m_current_map->m_map->get_dy()) {
            // does the next map exist?
            if (get_map(m_current_map->m_l, m_current_map->m_x, m_current_map->m_y + 1) != 0) {
				SFXM->SFX_stop_continuous();
                m_current_map->m_map->reset(m_sfx_volume);
                m_current_map->m_map->remove_object(m_character, m_character->get_layer());
                m_previous_map = m_current_map;
                m_previous_map_direction = 2;
                m_previous_map_cycle = ROOM_TRANSITION_TIME;
                m_current_map = get_map(m_current_map->m_l, m_current_map->m_x, m_current_map->m_y + 1);
                m_current_map->m_map->add_object(m_character, m_character->get_layer());
                m_character->set_y(8);
                m_character->set_camefrom("up");
            } else {
                m_character->set_y(float(m_current_map->m_map->get_dy() - 1));
            }
        }
        // up:
        if (m_character->get_y() < 8) {
            // does the next map exist?
            if (get_map(m_current_map->m_l, m_current_map->m_x, m_current_map->m_y - 1) != 0) {
				SFXM->SFX_stop_continuous();
                m_current_map->m_map->reset(m_sfx_volume);
                m_current_map->m_map->remove_object(m_character, m_character->get_layer());
                m_previous_map = m_current_map;
                m_previous_map_direction = 3;
                m_previous_map_cycle = ROOM_TRANSITION_TIME;
                m_current_map = get_map(m_current_map->m_l, m_current_map->m_x, m_current_map->m_y - 1);
                m_current_map->m_map->add_object(m_character, m_character->get_layer());
                m_character->set_y(float(m_current_map->m_map->get_dy() - 1));
                m_character->set_camefrom("down");
            } else {
                m_character->set_y(0);
            }
        }

        // Delete requested multiroom objects:
        {
            GObject *o;

            while (!m_multiroom_objects_todelete.EmptyP()) {
                int pos;
				
                o = m_multiroom_objects_todelete.ExtractIni();
				SFXM->object_destroyed_notification(o);
                pos = m_multiroom_objects.PositionRef(o);
                if (pos != -1) {
                    m_multiroom_objects.DeletePosition(pos);
                    m_multiroom_objects_map.DeletePosition(pos);
                }
                delete o;
            }
		}

        if (m_current_map != old_map &&
                m_character->player_has("GO_bluebadbook"))
            m_character->m_bluebadbook_nghosts = 2;

    } else {
        if (m_gamepaused == PAUSESTATE_PAUSING && m_gamepaused_timmer >= MAPPAUSE_TIME)
            m_gamepaused = PAUSESTATE_PAUSED;
        if (m_gamepaused == PAUSESTATE_UNPAUSING && m_gamepaused_timmer >= MAPPAUSE_TIME)
            m_gamepaused = PAUSESTATE_PLAYING;
    }

    if (m_previous_map != 0) {
        if (m_previous_map_cycle > 0)
            m_previous_map_cycle--;
        else
            m_previous_map = 0;
    }

    assert(!m_current_map->m_map->get_object(fratelli_symbol));

    // nearly dead sfx
    if ((m_character->get_energy() <= PLAYER_CRITICAL_ENERGY) && !m_player_critical) {
        m_player_critical = true;
		SFXM->SFX_play("sfx/player_critical",m_sfx_volume);
    }
    if (m_character->get_energy() > PLAYER_CRITICAL_ENERGY) {
        m_player_critical = false;
    }

    if (m_character->get_state() == CSTATE_DEAD)
        return false;
    return true;
}

void TheGoonies::draw(GLTManager *GLTM)
{

    glPushMatrix();
    glTranslatef(0, 40, 0);

    if (m_current_map != 0) {
        if (m_gamepaused != PAUSESTATE_PLAYING && !m_pausemap_r_computed) {
            List<int> stack;
            List<int> explored;
            int *p;
            int i;
            int max_dx = 256;
            char fname[256];
            int offss[4] = { -max_dx, 1, max_dx, -1};
            FILE *fp;

            stack.Insert(new int(m_current_map->m_x + m_current_map->m_y*max_dx));

            m_pausemap_rx1 = m_pausemap_rx2 = m_current_map->m_x;
            m_pausemap_ry1 = m_pausemap_ry2 = m_current_map->m_y;

            while (!stack.EmptyP()) {
                p = stack.ExtractIni();
                explored.Add(p);

                for (i = 0;i < 4;i++) {
                    int np = (*p) + offss[i];
                    if (!explored.MemberP(&np)) {
                        map_filename(fname, m_current_map->m_l, np % max_dx, np / max_dx);
                        fp = fopen(fname, "r");
                        if (fp != 0) {
                            fclose(fp);
                            stack.Insert(new int(np));
                        }
					}
				}
			}
			
            while (!explored.EmptyP()) {
                p = explored.ExtractIni();
                if (((*p) % max_dx) < m_pausemap_rx1)
                    m_pausemap_rx1 = ((*p) % max_dx);
                if (((*p) % max_dx) > m_pausemap_rx2)
                    m_pausemap_rx2 = ((*p) % max_dx);
                if (((*p) / max_dx) < m_pausemap_ry1)
                    m_pausemap_ry1 = ((*p) / max_dx);
                if (((*p) / max_dx) > m_pausemap_ry2)
                    m_pausemap_ry2 = ((*p) / max_dx);
                delete p;
            }
            m_pausemap_r_computed = true;
        }
		
        // Temporaryly add the multiroom objectss to the current map:
        {
            GObject *f;
            GMapHolder *m;
            m_multiroom_objects.Rewind();
            m_multiroom_objects_map.Rewind();
            while (m_multiroom_objects.Iterate(f) && m_multiroom_objects_map.Iterate(m))
            {
                if (m_current_map == m) {
                    m->m_map->add_object(f, f->get_layer());
                }
			}
		}
		

        switch (m_gamepaused) {
            case PAUSESTATE_PLAYING: {
                    if (m_previous_map != 0) {
                        float xnew = 0, xold = 0, ynew = 0, yold = 0;
                        float f = ((m_previous_map_cycle) / float(ROOM_TRANSITION_TIME));
                        f = f * f;

                        switch (m_previous_map_direction) {
                            case 0:  // left
                                xnew = f * 640;
                                ynew = 0;
                                xold = xnew - 640;
                                yold = 0;
                                break;
                            case 1:  // right
                                xnew = (1 - f) * 640 - 640;
                                ynew = 0;
                                xold = xnew + 640;
                                yold = 0;
                                break;
                            case 2:  // up
                                xnew = 0;
                                ynew = f * 400;
                                xold = 0;
                                yold = ynew - 400;
                                break;
                            case 3:  // down
                                xnew = 0;
                                ynew = (1 - f) * 400 - 400;
                                xold = 0;
                                yold = ynew + 400;
                                break;
                        }
						
                        glPushMatrix();
                        glTranslatef(xnew, ynew, 0);
                        m_current_map->m_map->draw(GLTM, true);
                        if (strcmp(m_current_music, "cave2") == 0 && ambient_light)
                            m_current_map->m_map->draw_darkened_area(m_character->get_x(), m_character->get_y());
                        glPopMatrix();

                        glPushMatrix();
                        glTranslatef(xold, yold, 0);
                        m_previous_map->m_map->draw(GLTM, false);
                        if (strcmp(m_current_music, "cave2") == 0 && ambient_light)
                            m_current_map->m_map->draw_darkened_area(m_character->get_x() + int(xnew - xold), m_character->get_y() + int(ynew - yold));
                        glPopMatrix();
                    } else {
                        if (m_current_zoom == 0) {
                            m_current_map->m_map->draw(GLTM, true);
                            if (strcmp(m_current_music, "cave2") == 0 && ambient_light)
                                m_current_map->m_map->draw_darkened_area(m_character->get_x(), m_character->get_y());
                        } else {
                            float f = (m_current_zoom < 25 ? 0 : m_current_zoom - 25) / 25.0f;
                            glMatrixMode(GL_MODELVIEW);
                            glPushMatrix();
                            glTranslatef( -m_character->get_x()*f*10,
                                          -(m_character->get_y() - 40)*f*10, 0);
                            glScalef(10*f + (1 - f), 10*f + (1 - f), 10*f + (1 - f));
                            m_current_map->m_map->draw(GLTM, true);
							
                            if (strcmp(m_current_music, "cave2") == 0 && ambient_light)
                                m_current_map->m_map->draw_darkened_area(m_character->get_x(), m_character->get_y());
                            glPopMatrix();
                        }
					}

                    m_pausemap_r_computed = false;
                }
                break;
            case PAUSESTATE_PAUSING:
            case PAUSESTATE_UNPAUSING:
            case PAUSESTATE_PAUSED: {
                    int j, k;
                    GMapHolder *m;
                    int dx = m_pausemap_rx2 + 1 - m_pausemap_rx1;
                    int dy = m_pausemap_ry2 + 1 - m_pausemap_ry1;
                    int dmax = (dx > dy ? dx : dy);
                    float z = 1.0F / dmax;
                    float fact = 1;
                    FILE *fp;
                    char fname[256];

                    if (m_gamepaused == PAUSESTATE_PAUSING)
                        fact = m_gamepaused_timmer / float(MAPPAUSE_TIME);
                    if (m_gamepaused == PAUSESTATE_UNPAUSING)
                        fact = (MAPPAUSE_TIME - m_gamepaused_timmer) / float(MAPPAUSE_TIME);
                    glMatrixMode(GL_MODELVIEW);
                    glPushMatrix();
                    {
                        float start_x = ((1.0F / z) - dx) * 0.5f;
                        float start_y = ((1.0F / z) - dy) * 0.5f;
                        glTranslatef(((m_current_map->m_x - m_pausemap_rx1) + start_x)*m_current_map->m_map->get_dx()*fact*z,
                                     ((m_current_map->m_y - m_pausemap_ry1) + start_y)*m_current_map->m_map->get_dy()*fact*z, 0);
                    }
                    glScalef((1 - fact) + z*fact, (1 - fact) + z*fact, (1 - fact) + z*fact);

                    for (k = m_pausemap_rx1;k <= m_pausemap_rx2;k++) {
                        for (j = m_pausemap_ry1;j <= m_pausemap_ry2;j++) {
                            m = get_map_noload(m_current_map->m_l, k, j);

                            glPushMatrix();
                            glTranslatef(float(k - m_current_map->m_x)*m_current_map->m_map->get_dx(),
                                         float(j - m_current_map->m_y)*m_current_map->m_map->get_dy(), 0);
                            if (m == 0) {
                                map_filename(fname, m_current_map->m_l, k, j);
                                fp = fopen(fname, "r");
                                if (fp != 0) {
                                    fclose(fp);
                                    glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
                                    glBegin(GL_QUADS);
                                    glVertex3f(4, 4, 0);
                                    glVertex3f(4, float(m_current_map->m_map->get_dy() - 4), 0);
                                    glVertex3f(float(m_current_map->m_map->get_dx() - 5), float(m_current_map->m_map->get_dy() - 4), 0);
                                    glVertex3f(float(m_current_map->m_map->get_dx() - 4), 4, 0);
                                    glEnd();
                                }
                            } else {
                                if (m == m_current_map) {
                                    if (strcmp(m_current_music, "cave2") == 0) {
                                        GLint bb[4];
                                        float f = (1 - fact) + z * fact;
                                        glGetIntegerv(GL_SCISSOR_BOX, bb);
                                        // This might seem a bit complex, but simply computes the bounding box in which the current
                                        // map is drawn, so that the "darkened_area" doesn't go beyond the bounding box
                                        glScissor(GLint(((m_current_map->m_x - m_pausemap_rx1) + ((1.0F / z) - dx)*0.5f)*m_current_map->m_map->get_dx()*fact*z +
                                                        int(k - m_current_map->m_x)*m_current_map->m_map->get_dx()*f),
                                                  GLint((440 - int(m_current_map->m_map->get_dy()*f)) -
                                                        (((m_current_map->m_y - m_pausemap_ry1) + ((1.0F / z) - dy)*0.5f)*m_current_map->m_map->get_dy()*fact*z +
                                                         int(j - m_current_map->m_y)*m_current_map->m_map->get_dy()*f)),
                                                  int(m_current_map->m_map->get_dx()*f),
                                                  int(m_current_map->m_map->get_dy()*f));
                                        glEnable(GL_SCISSOR_TEST);
                                        m_current_map->m_map->draw(GLTM, false);
										if (ambient_light) {
											m_current_map->m_map->draw_darkened_area(m_character->get_x(), m_character->get_y());
										}
                                        glDisable(GL_SCISSOR_TEST);
                                        glScissor(bb[0], bb[1], bb[2], bb[3]);
                                    } else {
                                        m_current_map->m_map->draw(GLTM, false);
                                    }
                                } else {
                                    m->m_map->draw(GLTM, false);
                                    glColor4f(0.1f, 0.1f, 0.1f, 0.6f);
                                    glBegin(GL_QUADS);
                                    glVertex3f(0, 0, 0);
                                    glVertex3f(0, float(m_current_map->m_map->get_dy()), 0);
                                    glVertex3f(float(m_current_map->m_map->get_dx()), float(m_current_map->m_map->get_dy()), 0);
                                    glVertex3f(float(m_current_map->m_map->get_dx()), 0, 0);
                                    glEnd();
                                }
                            }
                            glPopMatrix();

                        }
					}
					glPopMatrix();
                }
                // pause message
                if (m_cycle > 200) {
					float f = (sin(m_cycle / (8*M_PI)) + 1.0f)/2.0f;
					GLTM->get("gamepaused")->draw(1, 1, 1, f, 75, 60, 0, 0, 1);
                    char tmp[32];
                    sprintf(tmp, "current stage: %i", m_current_map->m_l);
                    font_print_c(320, 210, 0, 0, 0.5f, "font", tmp);

                    sprintf(tmp, "goonies rescued: %i/7", m_character->get_rescued_goonies());
                    font_print_c(320, 230, 0, 0, 0.5f, "font", tmp);
                    break;
                }
        }
		
        // Remove again the multiroom objects from the current map:
        {
            GObject *f = 0;
            GMapHolder *m = 0;
            m_multiroom_objects.Rewind();
            m_multiroom_objects_map.Rewind();
            while (m_multiroom_objects.Iterate(f) && m_multiroom_objects_map.Iterate(m))
            {
                m->m_map->remove_object(f, f->get_layer());
            }
        }


        if (m_cycle < 150 && m_current_map->m_l > 0) {
            char tmp[16];
            sprintf(tmp, "stage%i", m_current_map->m_l);

            {
                float f = 0;
                if (m_cycle >= 0 && m_cycle < 50)
                    f = m_cycle / 75.0f;
                if (m_cycle >= 50 && m_cycle < 100)
                    f = 0.66f;
                if (m_cycle >= 100 && m_cycle < 150)
                    f = (150 - m_cycle) / 75.0f;
                glColor4f(0, 0, 0, f);
                glNormal3f(0.0, 0.0, 1.0);
                glBegin(GL_QUADS);
                glVertex3f(0, 0, 0);
                glVertex3f(0, 480, 0);
                glVertex3f(640, 480, 0);
                glVertex3f(640, 0, 0);
                glEnd();
            }

            // 1: text appearing
            if (m_cycle >= 0 && m_cycle < 50) {
                int x;
                float f = (1 - (m_cycle / 50.0f));
                f *= f;
                x = (int)(320 + f * 640);
                GLTM->get
                (tmp)->draw(1, 1, 1, 1, float(x - 150), 200 - 90, 0, 0, 1);
            }
			
            // 2: text braking
            if (m_cycle >= 50 && m_cycle < 100) {
                GLTM->get
                (tmp)->draw(1, 1, 1, 1, 320 - 150, 200 - 90, 0, 0, 1);
            }
			
            // 3: text disappearing
            if (m_cycle >= 100 && m_cycle < 150) {
                int x;
                float f = (((m_cycle - 100) / 50.0f));
                f *= f;
                x = (int)(320 - f * 640);
                GLTM->get
                (tmp)->draw(1, 1, 1, 1, float(x - 150), 200 - 90, 0, 0, 1);
            }
		}
	}
    glPopMatrix();

    // zoom in-out when entering skull doors:
    if (m_current_zoom > 0) {
        {
            float f = 0;
            f = float(m_current_zoom) / 50;
            glColor4f(0, 0, 0, f);
        }
        glNormal3f(0.0, 0.0, 1.0);

        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 480, 0);
        glVertex3f(640, 480, 0);
        glVertex3f(640, 0, 0);
        glEnd();
    }
	
    // display hud:
    {
        GLTM->get
        ("hud-top")->draw(0, 0, 0, 0, 1);
        GLTM->get
        ("hud-bottom")->draw(0, 421, 0, 0, 1);

        // energy:
        {
            float w = float(m_character->get_energy()) / PLAYER_MAX_ENERGY;
            int x, i;
            x = (int)(342 + w * 200);
            for (i = 342;i < x;i++) {
                // nearly dead blinking effect
                if (m_character->get_energy() <= PLAYER_CRITICAL_ENERGY) {
                    float f = float(0.5f + 0.5f * sin(m_cycle * 0.2));
                    GLTM->get
                    ("hud-vitality-critical")->draw(1, 1, 1, f, float(i), 9, 0, 0, 1);
                } else {
                    GLTM->get
                    ("hud-vitality")->draw(float(i), 9, 0, 0, 1);
                }
            }
        }

        // experience:
        {
            float w = float(m_character->get_experience()) / PLAYER_MAX_EXPERIENCE;
            int x, i;
            x = (int)(342 + w * 200);
            for (i = 342;i < x;i++) {
                GLTM->get
                ("hud-experience")->draw(float(i), 24, 0, 0, 1);
            }
        }

        // scene
        if (m_current_map != 0)
        {
            char tmp[7];
            sprintf(tmp, "%02i-%02i", m_current_map->m_x, m_current_map->m_y);
            font_print(566, 24, 0, 0, 1, "font_hud", tmp, -2, true);
        }
		
        // score
        if (m_character != 0)
        {
            char tmp[7];
            sprintf(tmp, "%06i", m_character->get_score());
            font_print(19, 24, 0, 0, 1, "font_hud", tmp, -2);

            // hiscore
            if (m_character->get_score() > hiscore) {
                hiscore = m_character->get_score();
            }
            sprintf(tmp, "%06i", hiscore);
            font_print(148, 24, 0, 0, 1, "font_hud", tmp, -2);
        }
		
        // saved kids:
        {
            float x[7] = {475, 496, 516, 537, 557, 577, 600};
            float y[7] = {450, 450, 450, 450, 450, 450, 450};
            int i, n = m_character->get_rescued_goonies();
            char tile[16];
            if (n == 7)
            {
                float f = float(0.5f + 0.5f * sin(m_cycle * 0.2));
                for (i = 0;i < 7;i++) {
                    sprintf(tile, "hud-goonie%i", i + 1);
                    GLTM->get
                    (tile)->draw(1, 1, 1, f, x[i], y[i], 0, 0, 1);
                }
            } else
            {
                for (i = 0;i < 7;i++) {
					if (!m_character->get_goonie_is_rescued(i))
					{
						sprintf(tile, "hud-goonie%i", i + 1);
						GLTM->get
							(tile)->draw(x[i], y[i], 0, 0, 1);
					}
                }
            }
        }

        // keys:
        if (m_character->has_key())
            GLTM->get
            ("ob_key")->draw(417, 451, 0, 0, 1);

        // items:
        {
            int i, n_items = 23;
            char *item_names[] = {
                                     "GO_hammer",
                                     "GO_yellowcoat",
                                     "GO_greycoat",
                                     "GO_shoes",
                                     "GO_yellowhelmet",

                                     "GO_lamp",
                                     "GO_greenbook",
                                     "GO_redbook",
                                     "GO_lightbluebook",

                                     "GO_bluebook",
                                     "GO_greencoat",
                                     "GO_whitebook",
                                     "GO_yellowshield",

                                     "GO_lightbluecoat",
                                     "GO_whiteshield",
                                     "GO_redbadbook",
                                     "GO_purplebook",
                                     "GO_lightbluehelmet",

                                     "GO_yellowbook",
                                     "GO_purplebadbook",
                                     "GO_purpleshield",
                                     "GO_clock",
                                     "GO_bluebadbook"
                                 };
            char *sprite_names[] = {
                                       "it_hammer",
                                       "it_yellowcoat",
                                       "it_greycoat",
                                       "it_shoes",
                                       "it_yellowhelmet",

                                       "it_lamp",
                                       "it_greenbook",
                                       "it_redbook",
                                       "it_lightbluebook",

                                       "it_bluebook",
                                       "it_greencoat",
                                       "it_whitebook",
                                       "it_yellowshield",

                                       "it_lightbluecoat",
                                       "it_whiteshield",
                                       "it_redanti",
                                       "it_purplebook",
                                       "it_lightbluehelmet",

                                       "it_yellowbook",
                                       "it_purpleanti",
                                       "it_purpleshield",
                                       "it_clock",
                                       "it_blueanti"
                                   };
            float sprite_alpha[] = {
                                       m_character->m_hammer_status / 30.0f,
                                       (m_character->m_yellowcoat_timer / 500.0f),
                                       m_character->m_greycoat_status / 5.0f,
                                       1,
                                       m_character->m_yellowhelmet_status / 5.0f,

                                       1,
                                       m_character->m_greenbook_status / 5.0f,
                                       m_character->m_redbook_status / 4.0f,
                                       m_character->m_lightbluebook_status / 5.0f,

                                       m_character->m_bluebook_status / 5.0f,
                                       m_character->m_greenbook_status / 5.0f,
                                       m_character->m_whitebook_status / 5.0f,
                                       m_character->m_yellowshield_status / 5.0f,

                                       m_character->m_lightbluecoat_timer / 500.0f,
                                       m_character->m_whiteshield_status / 5.0f,
                                       1,
                                       1,
                                       m_character->m_lightbluehelmet_status / 5.0f,

                                       m_character->m_yellowbook_status / 5.0f,
                                       1,
                                       m_character->m_purpleshield_status / 5.0f,
                                       m_character->m_clock_timer / 1500.0f,
                                       1
                                   };
            int x = 21;
            int x_inc = 31 ;

            for (i = 0;i < n_items;i++) {
                if (m_character->player_has(item_names[i]) && sprite_alpha[i] > 0) {
                    GLTM->get
                    (sprite_names[i])->draw(1, 1, 1, sprite_alpha[i], float(x), 449, 0, 0, 1.0f);
                    x += x_inc;
                }
            }
        }
    }
}

GMapHolder *TheGoonies::get_map_noload(int l, int x, int y)
{
    GMapHolder *m;

    m_maps.Rewind();
    while (m_maps.Iterate(m)) {
        if (m->m_l == l && m->m_x == x && m->m_y == y)
            return m;
    }
	
    return 0;
}

GMapHolder *TheGoonies::get_map(int l, int x, int y)
{
    GMapHolder *m;

    m_maps.Rewind();
    while (m_maps.Iterate(m)) {
        if (m->m_l == l && m->m_x == x && m->m_y == y)
            return m;
    }
	
    m_maps_buffer.Rewind();
    while (m_maps_buffer.Iterate(m)) {
        if (m->m_l == l && m->m_x == x && m->m_y == y) {
            m_maps_buffer.DeleteElement(m);
            m_maps.Add(m);
            return m;
        }
	}
	
    {
        FILE *fp;
        GMap *map;

        char fname[256];
        map_filename(fname, l, x, y);
        fp = fopen(fname, "rb");
        if (fp == 0)
            return 0;
        map = new GMap(fp);
        map->reset_first(m_sfx_volume);
        fclose(fp);

        m = new GMapHolder();
        m->m_map = map;
        m->m_l = l;
        m->m_x = x;
        m->m_y = y;
        m_maps.Add(m);

        return m;
    }
}

GMapHolder *TheGoonies::get_map_not_player(int l, int x, int y)
{
    GMapHolder *m;

    m_maps.Rewind();
    while (m_maps.Iterate(m)) {
        if (m->m_l == l && m->m_x == x && m->m_y == y)
            return m;
    }
	
    m_maps_buffer.Rewind();
    while (m_maps_buffer.Iterate(m)) {
        if (m->m_l == l && m->m_x == x && m->m_y == y) {
            return m;
        }
	}
	
    {
        FILE *fp;
        GMap *map;

        char fname[256];
        map_filename(fname, l, x, y);
        fp = fopen(fname, "rb");
        if (fp == 0)
            return 0;
        map = new GMap(fp);
        map->reset_first(m_sfx_volume);
        fclose(fp);

        m = new GMapHolder();
        m->m_map = map;
        m->m_l = l;
        m->m_x = x;
        m->m_y = y;
        m_maps_buffer.Add(m);

        return m;
    }
}

void TheGoonies::set_music_volume(int v, MusicManager *MusicM)
{
    m_music_volume = v;
	MusicM->music_volume(MUSIC_ID,v);
}

void TheGoonies::set_sfx_volume(int v)
{
    m_sfx_volume = v;
}

void TheGoonies::set_ambient_volume(int v, MusicManager *MusicM)
{
    m_ambient_volume = v;
	MusicM->music_volume(AMBIENT_ID,v);
}

void TheGoonies::add_multiroom_object(GObject *o, GMapHolder *map)
{
    m_multiroom_objects.Add(o);
    m_multiroom_objects_map.Add(map);
}

void TheGoonies::add_multiroom_object(GObject *o, GMap *map)
{
    List<GMapHolder> l;
    GMapHolder *mh;

    l.Instance(m_maps);
    l.Rewind();
    while (l.Iterate(mh)) {
        if (mh->m_map == map) {
            add_multiroom_object(o, mh);
            return ;
        }
	}
	
    l.Instance(m_maps_buffer);
    l.Rewind();
    while (l.Iterate(mh)) {
        if (mh->m_map == map) {
            add_multiroom_object(o, mh);
            return ;
        }
	}
}

void TheGoonies::delete_multiroom_object(GObject *o)
{
    m_multiroom_objects_todelete.Add(o);
}


bool TheGoonies::object_exists(GObject *o)
{
	if (m_current_map!=0) return m_current_map->m_map->object_exists(o);
	return false;
} /* TheGoonies::object_exists */ 


bool TheGoonies::game_paused()
{
	return (m_gamepaused == PAUSESTATE_PAUSING || m_gamepaused == PAUSESTATE_PAUSED);
} /* TheGoonies::game_paused */ 
