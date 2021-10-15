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
#include "auxiliar.h"
#include "GLTile.h"
#include "keyboardstate.h"

#include "Ginterface.h"

#include "GLTManager.h"
#include "font_extractor.h"

Symbol *button_left_symbol = new Symbol("interface_button_left");
Symbol *button_center_symbol = new Symbol("interface_button_center");
Symbol *button_right_symbol = new Symbol("interface_button_right");


G_Button::G_Button(char *text,float x,float y,float dx,float dy,int ID, GLTManager *GLTM) : G_InterfaceElement()
{
	m_text=new char[strlen(text)+1];
	strcpy(m_text,text);
	m_x=x;
	m_y=y;
	m_dx=dx;
	m_dy=dy;
	m_ID=ID;
	m_status=0;
	m_enabled=true;
	m_tile=0;
	m_active=true;
	m_GLTM = GLTM;

	m_height = 0;
	m_width = 0;
	if (m_dy == 1) {
		m_height = 64;
		m_width = int(64+(m_dx-2)*48);
	} else {
		if (m_dy == 2) {
			m_height = 128;
			m_width = int(128+(m_dx-2)*96);
		} // if
	} // if

} /* G_Button::G_Button */


G_Button::G_Button(GLTile *icon,float x,float y,float dx,float dy,int ID, GLTManager *GLTM) : G_InterfaceElement()
{
	m_text=0;
	m_x=x;
	m_y=y;
	m_dx=dx;
	m_dy=dy;
	m_ID=ID;
	m_status=0;
	m_enabled=true;
	m_tile=icon;
	m_active=true;
	m_GLTM = GLTM;

	m_height = 0;
	m_width = 0;
	if (m_dy == 1) {
		m_height = 64;
		m_width = int(64+(m_dx-2)*48);
	} else {
		if (m_dy == 2) {
			m_height = 128;
			m_width = int(128+(m_dx-2)*96);
		} // if
	} // if
} /* G_Button::G_Button */


G_Button::~G_Button()
{
	// Only delete the tile if it has been created here:
	if (m_text!=0) {
		if (m_tile!=0) delete m_tile;
		m_tile=0;
	} // if

	if (m_text!=0) delete []m_text;
	m_text=0;

} /* G_Button::~G_Button */


void G_Button::draw(void)
{
	draw(1);
} /* G_Button::draw */


void G_Button::draw(float alpha)
{
	int i;

	if (!m_enabled) alpha*=0.33f;

	if (m_dy == 1) {
		m_GLTM->get(button_left_symbol)->draw(1,1,1,alpha,m_x,m_y,0,0,0.5f);
		for(i=0;i<m_dx-2;i++) {
			m_GLTM->get(button_center_symbol)->draw(1,1,1,alpha,m_x+32+i*48,m_y,0,0,0.5f);
		} // for
		m_GLTM->get(button_right_symbol)->draw(1,1,1,alpha,m_x+32+(m_dx-2)*48,m_y,0,0,0.5f);
	} else {
		if (m_dy == 2) {
			m_GLTM->get(button_left_symbol)->draw(1,1,1,alpha,m_x,m_y,0,0,1);
			for(i=0;i<m_dx-2;i++) {
				m_GLTM->get(button_center_symbol)->draw(1,1,1,alpha,m_x+64+i*96,m_y,0,0,1);
			} // for
			m_GLTM->get(button_right_symbol)->draw(1,1,1,alpha,m_x+64+(m_dx-2)*96,m_y,0,0,1);
		} // if
	} // if

	if (m_status==0) font_print_c(int(m_x+m_width/2), int(m_y+m_height/2), 0, 0, 1, alpha, "font", m_text, -2);
				else font_print_c(int(m_x+m_width/2)-2, int(m_y+m_height/2)-2, 0, 0, 1, alpha, "font_hl", m_text, -4);
} /* G_Button::draw */


bool G_Button::check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k)
{
	if (!m_enabled) {
		m_status=0;
		return false;
	} // if

	if (mousex>=m_x && mousex<m_x+m_width &&
		mousey>=m_y && mousey<m_y+m_height) {
		if (button==0) {
			m_status=1;
		} else {
			m_status=2;
			return true;
		} // if
	} else {
		m_status=0;
	} // if

	return false;
} /* G_Button::check_status */

