#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include "windows.h"
#endif

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
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

#include "GLTManager.h"
#include "SoundManager.h"
#include "SFXManager.h"
#include "MusicManager.h"
#include "GObject.h"
#include "GO_enemy.h"
#include "GO_fratelli.h"
#include "GMap.h"
#include "TheGoonies.h"
#include "TheGooniesApp.h"
#include "Level.h"
#include "LevelPack.h"

#include "Ginterface.h"

extern bool fullscreen;
extern SDL_Surface *screen_sfc;
extern SDL_Surface *toogle_video_mode(bool fullscreen);


int TheGooniesApp::mapeditor_cycle(KEYBOARDSTATE *k)
{
	int old_mouse_x = m_mouse_x;
	int old_mouse_y = m_mouse_y;

	SDL_GetMouseState(&m_mouse_x,&m_mouse_y);
	if (!m_mouse_click_x.EmptyP()) {
		m_mouse_click_x.Delete();
		m_mouse_click_y.Delete();
		m_mouse_button=1;
	} else {
		m_mouse_button=0;
	} // if

	if (m_mouse_x==old_mouse_x && m_mouse_y==old_mouse_y) {
		m_mouse_movement_timmer++;
		
		if (m_mouse_movement_timmer>150 && m_draw_mouse_status==2) {		
			m_draw_mouse_status=3;
			m_draw_mouse=1.0f;
		} // if
	} else {
		m_mouse_movement_timmer=0;

		if (m_draw_mouse_status==0) {
			m_draw_mouse_status=1;
			m_draw_mouse=0.0f;
		} // if
	} // if

	switch(m_draw_mouse_status) {
	case 0:
			break;
	case 1: m_draw_mouse+=0.05f;
			if (m_draw_mouse>=1.0f) {
				m_draw_mouse=1.0f;
				m_draw_mouse_status=2;
			} // if
			break;
	case 2:
			break;
	case 3: m_draw_mouse-=0.05f;
			if (m_draw_mouse<=0.0f) {
				m_draw_mouse=0.0f;
				m_draw_mouse_status=0;
			} // if 			
			break;
	} // switch

	if (m_state_cycle == 0){
		// CREATE THE INTERFACE:
		G_interface::reset();
		G_interface::add_element(new G_SButton("back",10,440,4,1,1,m_GLTM));
	} else {

	} // if

	{
		int button_status=0;
		int ID=-1;
		button_status=SDL_GetMouseState(&m_mouse_x,&m_mouse_y);

		if (k->key_press(SDLK_SPACE) || k->key_press(SDLK_RETURN)) m_mouse_button=1;

		ID = G_interface::update_state(m_mouse_x,m_mouse_y,m_mouse_button,button_status,k);

		switch(ID) {
		case 1:	// BACK:
				return THEGOONIES_STATE_EDITOR;
				break;

		} // switch
	}


    return THEGOONIES_STATE_MAPEDITOR;
} /* TheGooniesApp::mapeditor_cycle */ 

void TheGooniesApp::mapeditor_draw(void)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

	G_interface::draw();

	if (m_draw_mouse>0) {
		m_GLTM->get("interface_pointer_glow")->draw(1,1,1,float(sin(m_state_cycle/(4*M_PI))*m_draw_mouse),float(m_mouse_x),float(m_mouse_y),0,0,1);
		m_GLTM->get("interface_pointer")->draw(1,1,1,m_draw_mouse,float(m_mouse_x),float(m_mouse_y),0,0,1);
	}

} /* TheGooniesApp::mapeditor_draw */ 

