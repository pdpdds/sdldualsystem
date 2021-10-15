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

#include "TheGooniesCtnt.h"

#include "Ginterface.h"

#include "font_extractor.h"


#define ROW_HEIGHT	36

G_Browser::G_Browser(float x,float y,float dx,float dy,int ID) : G_InterfaceElement()
{
	m_x=x;
	m_y=y;
	m_dx=dx;
	m_dy=dy;
	m_enabled=true;
	m_active=false;
	m_ID = ID;

	m_mouse_over = -1;
	m_selected = -1;

	m_slider_pos = 0.0f;
	m_slider_height = 0.0f;

	m_old_mouse_x = 0;
	m_old_mouse_y = 0;
} /* G_Browser::G_Browser */


G_Browser::~G_Browser()
{
} /* G_Browser::~G_Browser */


void G_Browser::draw(void)
{
	draw(1);
} /* G_Browser::draw */


void G_Browser::draw(float alpha)
{
	glBegin(GL_QUADS);
	glColor4f(0,0,0,alpha*0.5f);
	glVertex3f(m_x,m_y,0);
	glVertex3f(m_x+m_dx,m_y,0);
	glVertex3f(m_x+m_dx,m_y+m_dy,0);
	glVertex3f(m_x,m_y+m_dy,0);

	glVertex3f(m_x,m_y,0);
	glVertex3f(m_x+m_dx,m_y,0);
	glVertex3f(m_x+m_dx,m_y+4,0);
	glVertex3f(m_x,m_y+4,0);

	glVertex3f(m_x,m_y,0);
	glVertex3f(m_x+4,m_y,0);
	glVertex3f(m_x+4,m_y+m_dy,0);
	glVertex3f(m_x,m_y+m_dy,0);

	glVertex3f(m_x+m_dx-4,m_y,0);
	glVertex3f(m_x+m_dx,m_y,0);
	glVertex3f(m_x+m_dx,m_y+m_dy,0);
	glVertex3f(m_x+m_dx-4,m_y+m_dy,0);

	glVertex3f(m_x,m_y+m_dy-4,0);
	glVertex3f(m_x+m_dx,m_y+m_dy-4,0);
	glVertex3f(m_x+m_dx,m_y+m_dy,0);
	glVertex3f(m_x,m_y+m_dy,0);	
	glEnd();

	// Draw the slider:
	{
		if (m_entries.Length()>0) {
			m_slider_height = (m_dy-8)/(m_entries.Length()*ROW_HEIGHT);
			if (m_slider_height>=1) {
				m_slider_pos = 0;
				m_slider_height = -1;
			} else {
				m_slider_height = m_slider_height*(m_dy-8);
			    if (m_slider_pos + m_slider_height>m_dy-8) {
					m_slider_pos = m_dy-8 - m_slider_height;
				} // if
			} // if

		} else {
			m_slider_pos = 0;
			m_slider_height = -1;
		} // if

		if (m_slider_height>0) {
			glBegin(GL_QUADS);
			glColor4f(0.5f,1,0.5f,alpha*0.5f);
			glVertex3f(m_x+m_dx-20,m_y+4+m_slider_pos,0);
			glVertex3f(m_x+m_dx-4,m_y+4+m_slider_pos,0);
			glVertex3f(m_x+m_dx-4,m_y+4+m_slider_pos+m_slider_height,0);
			glVertex3f(m_x+m_dx-20,m_y+4+m_slider_pos+m_slider_height,0);
			glEnd();
		} // if
	}

	// Draw the entries:
	{
        GLint bb[4];
        glGetIntegerv(GL_SCISSOR_BOX, bb);
		glScissor(int(m_x+4),int(SCREEN_Y-(m_y+4+(m_dy-8))),int(m_dx-(8 + 20)),int(m_dy-8));
        glEnable(GL_SCISSOR_TEST);

		int i=0;
		int y;
		char *entry;

		y = int(int(m_y) - (m_slider_pos/(m_dy-8))*(m_entries.Length()*ROW_HEIGHT));

		m_entries.Rewind();
		while(m_entries.Iterate(entry) && (y<m_y+m_dy)) {

			if (m_mouse_over == i) {
				glBegin(GL_QUADS);
				glColor4f(0.5f,1,0.5f,alpha*0.5f);
				glVertex3f(m_x+4,float(y+4),0);
				glVertex3f(m_x+m_dx-4,float(y+4),0);
				glVertex3f(m_x+m_dx-4,float(y+ROW_HEIGHT+4),0);
				glVertex3f(m_x+4,float(y+ROW_HEIGHT+4),0);
				glEnd();
			} // if

			if (m_selected == i) font_print(int(m_x+8), y-2, 0, 0, 1, alpha, "font_hl", entry, -2);
							else font_print(int(m_x+8), y, 0, 0, 1, alpha, "font", entry, -4);
			y+=ROW_HEIGHT;
			i++;
		} // while

        glDisable(GL_SCISSOR_TEST);
		glScissor(bb[0],bb[1],bb[2],bb[3]);
	}
} /* G_Browser::draw */


bool G_Browser::check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k)
{
	if (mousex>m_x && mousex<m_x+(m_dx-20) && mousey>m_y && mousey<m_y+m_dy) {
		int starty = int(int(m_y) - (m_slider_pos/(m_dy-8))*(m_entries.Length()*ROW_HEIGHT));
		int s = int((mousey-starty)/ROW_HEIGHT);

		if (s<m_entries.Length()) {
			m_mouse_over = s;
		} else {
			m_mouse_over = -1;
		} // if
	} else {
		m_mouse_over = -1;
	} // if

	if (button_status!=0 && mousex>m_x+(m_dx-20) && mousex<m_x+m_dx && mousey>m_y && mousey<m_y+m_dy) {
		float p = (mousey-m_y-(m_slider_height/2))/(m_dy-8-m_slider_height);
		if (p<0) p=0;
		if (p>1) p=1;
		m_slider_pos = p*(m_dy-8-m_slider_height);
	} // if

	if (m_mouse_over != -1 && button!=0) {
		m_selected = m_mouse_over;
		
		m_old_mouse_x = mousex;
		m_old_mouse_y = mousey;

		return true;
	} // if

	m_old_mouse_x = mousex;
	m_old_mouse_y = mousey;

	return false;
} /* G_Browser::check_status */


void G_Browser::clear(void)
{
	m_entries.Delete();
} /* G_Browser::clear */


void G_Browser::addEntry(char *e)
{
	char *tmp = new char[strlen(e)+1];
	strcpy(tmp,e);
	m_entries.Add(tmp);
} /* G_Browser::addEntry */


char *G_Browser::getEntry(int i)
{
	return m_entries[i];
} /* G_Browser::addEntry */


void G_Browser::deleteEntry(int i)
{
	m_entries.DeletePosition(i);
} /* G_Browser::addEntry */



void G_Browser::setSelected(int i)
{
	if (i<-1) i=-1;
	if (i>=m_entries.Length()) i = m_entries.Length()-1;
	m_selected = i;
} /* G_Browser::setSelected */


int G_Browser::getSelected(void)
{
	return m_selected;
} /* G_Browser::getSelected */
