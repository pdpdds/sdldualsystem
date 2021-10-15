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


int TheGooniesApp::editor_cycle(KEYBOARDSTATE *k)
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
		G_interface::add_element(new G_Button("back",40,380,4,1,1,m_GLTM));
		G_interface::add_element(new G_Button("load",240,380,4,1,2,m_GLTM));
		G_interface::add_element(new G_Button("save",440,380,4,1,3,m_GLTM));
		G_interface::add_element(new G_Text("the goonies level editor",320,24,true));
		G_interface::add_element(new G_Browser(60,64,360,300,11));

		G_interface::add_element(new G_SButton("rename",440,65,8,1,8,m_GLTM));
		G_interface::add_element(new G_SButton("password",440,100,8,1,9,m_GLTM));
		G_interface::add_element(new G_Button("edit",445,135,4,1,10,m_GLTM));


		G_interface::add_element(new G_SButton("add map",440,255,8,1,4,m_GLTM));
		G_interface::add_element(new G_SButton("delete map",440,290,8,1,5,m_GLTM));
		G_interface::add_element(new G_SButton("up",460,325,3,1,6,m_GLTM));
		G_interface::add_element(new G_SButton("down",520,325,4,1,7,m_GLTM));

		G_interface::disable(2);
		G_interface::disable(3);

		G_interface::disable(5);
		G_interface::disable(6);
		G_interface::disable(7);
		G_interface::disable(8);
		G_interface::disable(9);
		G_interface::disable(10);

		if (m_editor_levelpack == 0) m_editor_levelpack = new GLevelPack();
		if (m_previous_state != THEGOONIES_STATE_MAPEDITOR) m_editor_level = 0;

	} else {

		// UPDATE THE INTERFACE:
		if (m_editor_level == 0) {
			G_interface::disable(5);
			G_interface::disable(6);
			G_interface::disable(7);
			G_interface::disable(8);
			G_interface::disable(9);
			G_interface::disable(10);
		} else {
			G_interface::enable(5);
			if (m_editor_levelpack->getLevelPosition(m_editor_level)>0) G_interface::enable(6);
																   else G_interface::disable(6);
			if (m_editor_levelpack->getLevelPosition(m_editor_level)<(m_editor_levelpack->getNLevels()-1)) G_interface::enable(7);
																									  else G_interface::disable(7);
			G_interface::enable(8);
			G_interface::enable(9);
			G_interface::enable(10);
		} // if

		// Update the list of names in the browser:
		{
			G_Browser *browser = (G_Browser *)G_interface::get(11);
			if (browser!=0) {
				int i;
				int nl = m_editor_levelpack->getNLevels();
				
				browser->clear();
				for(i=0;i<nl;i++) {
					browser->addEntry(m_editor_levelpack->getLevel(i)->getName());
				} // for

				if (m_editor_level==0) browser->setSelected(-1);
								  else browser->setSelected(m_editor_levelpack->getLevelPosition(m_editor_level));
			} // if
		}

	} // if

	{
		int button_status=0;
		int ID=-1;
		button_status=SDL_GetMouseState(&m_mouse_x,&m_mouse_y);

		if (k->key_press(SDLK_SPACE) || k->key_press(SDLK_RETURN)) m_mouse_button=1;

		ID = G_interface::update_state(m_mouse_x,m_mouse_y,m_mouse_button,button_status,k);

		switch(ID) {
		case 1:	// BACK:
				m_title_current_menu = 2;
				m_title_next_menu = -1;
				m_title_state = 1;
				m_state_cycle = 0;
				m_title_option_selected = 5;
				return THEGOONIES_STATE_TITLE;
				break;

		case 2: // LOAD
				// ...
				break;

		case 3: // SAVE
				// ...
				break;

		case 4: // ADD MAP:
				{
					char tmp[80];
					GLevel *l=new GLevel();
					int i=1;
					do{
						sprintf(tmp,"map-%i",i);
						i++;
					}while(m_editor_levelpack->getLevel(tmp)!=0);
					l->setName(tmp);
					m_editor_levelpack->addNewLevel(l);
				}
				break;

		case 5: // DELETE MAP:
				G_interface::add_element(new G_Confirmation("delete map?",320,200,104,m_GLTM));
				break;

		case 6: // UP:
				{
					int pos = m_editor_levelpack->getLevelPosition(m_editor_level);
					GLevel *l;
					if (pos>0) {
						l = m_editor_levelpack->getLevel(pos-1);
						m_editor_levelpack->setLevel(pos-1,m_editor_level);
						m_editor_levelpack->setLevel(pos,l);
					} // if
				}
				break;

		case 7: // DOWN:
				{
					int pos = m_editor_levelpack->getLevelPosition(m_editor_level);
					GLevel *l;
					if (pos<(m_editor_levelpack->getNLevels()-1)) {
						l = m_editor_levelpack->getLevel(pos+1);
						m_editor_levelpack->setLevel(pos+1,m_editor_level);
						m_editor_levelpack->setLevel(pos,l);
					} // if
				}
				break;

		case 8: // RENAME
				G_interface::add_element(new G_TextInput(m_editor_level->getName(),"new name:",16,320,200,10,2,100,m_GLTM));
				break;

		case 9: // PASSWORD
				if (m_editor_level->getPassword()==0) {
					G_interface::add_element(new G_TextInput("","new password:",16,320,200,10,2,102,m_GLTM));				
				} else {
					G_interface::add_element(new G_TextInput("","old password:",16,320,200,10,2,101,m_GLTM));
				} // if
				break;

		case 10: // EDIT
				return THEGOONIES_STATE_MAPEDITOR;
				break;

		case 11: // LEVEL SELECTED:
				{
					G_Browser *browser = (G_Browser *)G_interface::get(11);
					if (browser!=0) {
						int i = browser->getSelected();
						if (i>=0) {
							m_editor_level = m_editor_levelpack->getLevel(i);
						} // if
					} // if
				}
				break;
		case 100: // NAME ENTERED:
				{
					G_TextInput *ti = (G_TextInput *)G_interface::get(100);
					if (strlen(ti->m_editing)>0) m_editor_level->setName(ti->m_editing);
				}
				break;
		case 101: // OLD PASSWORD:
				{
					G_TextInput *ti = (G_TextInput *)G_interface::get(101);
					if (strcmp(ti->m_editing,m_editor_level->getPassword())==0) {
						G_TextInput *ti2=new G_TextInput("","new password:",16,320,200,10,2,102,m_GLTM);
						G_interface::substitute_element(ti,ti2);
						delete ti;
						ti2->m_cycle=25;
					} else {
						G_Message *ti2=new G_Message("incorrect password",320,200,-1,m_GLTM);
						G_interface::substitute_element(ti,ti2);
						delete ti;
						ti2->m_cycle=25;
					} // if
				}
				break;
		case 102: // NEW PASSWORD:
				{
					G_TextInput *ti = (G_TextInput *)G_interface::get(102);
					if (strlen(ti->m_editing)>0) {
						strcpy(m_editor_tmp_password,ti->m_editing);
						G_TextInput *ti2=new G_TextInput("","retype password:",16,320,200,10,2,103,m_GLTM);
						G_interface::substitute_element(ti,ti2);
						delete ti;
						ti2->m_cycle=25;
					} // if
				}
				break;
		case 103: // NEW PASSWORD (RETYPED):
				{
					G_TextInput *ti = (G_TextInput *)G_interface::get(103);
					if (strcmp(ti->m_editing,m_editor_tmp_password)==0) {
						m_editor_level->setPassword(ti->m_editing);
					} else {
						G_Message *ti2=new G_Message("incorrect password",320,200,-1,m_GLTM);
						G_interface::substitute_element(ti,ti2);
						delete ti;
						ti2->m_cycle=25;
					} // if
				}
				break;

		case 104: // DELETE MAP:
				{
					m_editor_levelpack->deleteLevel(m_editor_level);
					delete m_editor_level;
					m_editor_level = 0;
				}
				break;

		} // switch
	}


    return THEGOONIES_STATE_EDITOR;
} /* TheGooniesApp::editor_cycle */ 

void TheGooniesApp::editor_draw(void)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
	m_GLTM->get("bg_copyright")->draw(1, 1, 1, 1);

	G_interface::draw();

	if (m_draw_mouse>0) {
		m_GLTM->get("interface_pointer_glow")->draw(1,1,1,float(sin(m_state_cycle/(4*M_PI))*m_draw_mouse),float(m_mouse_x),float(m_mouse_y),0,0,1);
		m_GLTM->get("interface_pointer")->draw(1,1,1,m_draw_mouse,float(m_mouse_x),float(m_mouse_y),0,0,1);
	}

} /* TheGooniesApp::editor_draw */ 

