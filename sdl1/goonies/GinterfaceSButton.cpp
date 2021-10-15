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

extern Symbol *button_left_symbol;
extern Symbol *button_center_symbol;
extern Symbol *button_right_symbol;


G_SButton::G_SButton(char *text,float x,float y,float dx,float dy,int ID, GLTManager *GLTM) : G_Button(text,x,y,dx,dy,ID,GLTM)
{
	m_height = 0;
	m_width = 0;
	if (m_dy == 1) {
		m_height = 32;
		m_width = int(32+(m_dx-2)*24);
	} else {
		if (m_dy == 2) {
			m_height = 64;
			m_width = int(64+(m_dx-2)*48);
		} // if
	} // if

} /* G_SButton::G_SButton */


G_SButton::G_SButton(GLTile *icon,float x,float y,float dx,float dy,int ID, GLTManager *GLTM) : G_Button(icon,x,y,dx,dy,ID,GLTM)
{
	m_height = 0;
	m_width = 0;
	if (m_dy == 1) {
		m_height = 32;
		m_width = int(32+(m_dx-2)*24);
	} else {
		if (m_dy == 2) {
			m_height = 64;
			m_width = int(64+(m_dx-2)*48);
		} // if
	} // if
} /* G_SButton::G_SButton */


G_SButton::~G_SButton()
{
	// Only delete the tile if it has been created here:
	if (m_text!=0) {
		if (m_tile!=0) delete m_tile;
		m_tile=0;
	} // if

	if (m_text!=0) delete []m_text;
	m_text=0;

} /* G_SButton::~G_SButton */


void G_SButton::draw(float alpha)
{
	int i;

	if (!m_enabled) alpha*=0.33f;

	if (m_dy == 1) {
		m_GLTM->get(button_left_symbol)->draw(1,1,1,alpha,m_x,m_y,0,0,0.25f);
		for(i=0;i<m_dx-2;i++) {
			m_GLTM->get(button_center_symbol)->draw(1,1,1,alpha,m_x+16+i*24,m_y,0,0,0.25f);
		} // for
		m_GLTM->get(button_right_symbol)->draw(1,1,1,alpha,m_x+16+(m_dx-2)*24,m_y,0,0,0.25f);
	} else {
		if (m_dy == 2) {
			m_GLTM->get(button_left_symbol)->draw(1,1,1,alpha,m_x,m_y,0,0,0.5f);
			for(i=0;i<m_dx-2;i++) {
				m_GLTM->get(button_center_symbol)->draw(1,1,1,alpha,m_x+32+i*48,m_y,0,0,0.5f);
			} // for
			m_GLTM->get(button_right_symbol)->draw(1,1,1,alpha,m_x+32+(m_dx-2)*48,m_y,0,0,0.5f);
		} // if
	} // if

	if (m_status==0) font_print_c(int(m_x+m_width/2), int(m_y+m_height/2), 0, 0, 0.5f, alpha, "font", m_text, -2);
				else font_print_c(int(m_x+m_width/2)-2, int(m_y+m_height/2)-2, 0, 0, 0.5f, alpha, "font_hl", m_text, -4);
} /* G_SButton::draw */

