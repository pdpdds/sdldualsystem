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

G_Confirmation::G_Confirmation(char *message,float x,float y,int ID,GLTManager *GLTM) : G_InterfaceElement()
{
	m_message=new char[strlen(message)+1];
	strcpy(m_message,message);
	m_x=x;
	m_y=y;
	m_ID=ID;
	m_enabled=true;
	m_active=true;
	m_modal=true;
	m_GLTM = GLTM;
	m_state=0;
	m_cycle=0;

	m_ok_button = new G_SButton("ok",x-170,y+40,4,2,-1,GLTM);
	m_cancel_button = new G_SButton("cancel",x+10,y+40,4,2,-1,GLTM);
} /* G_Confirmation::G_Confirmation */


G_Confirmation::~G_Confirmation()
{
	if (m_message!=0) delete []m_message;
	m_message=0;

	delete m_ok_button;
	delete m_cancel_button;

} /* G_Confirmation::~G_Confirmation */


void G_Confirmation::draw(void)
{
	draw(1);
} /* G_Confirmation::draw */


void G_Confirmation::draw(float alpha)
{
	if (!m_enabled) alpha*=0.33f;

	if (m_state==0 && m_cycle<25) alpha*=(m_cycle/25.0f);
	if (m_state==1) {
		if (m_cycle<25) {
			alpha*=((25-m_cycle)/25.0f);
		} else {
			alpha=0;
		} // if
	} // if

    {
        glEnable(GL_COLOR_MATERIAL);
        {
            float f = alpha*0.75f;
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

	font_print_c(int(m_x),int(m_y), 0, 0, 1, alpha, "font", m_message, -2);

	m_ok_button->draw(alpha);
	m_cancel_button->draw(alpha);

} /* G_Confirmation::draw */



bool G_Confirmation::check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k)
{
	if (!m_enabled) {
		return false;
	} // if

	m_cycle++;

	if (m_ok_button->check_status(mousex,mousey,button,button_status,k)) {
		m_state = 1;
		m_cycle=0;
		return true;
	} // if
	if (m_cancel_button->check_status(mousex,mousey,button,button_status,k)) {
		m_state = 1;
		m_cycle=0;
	} // if

	if (m_state == 1 && m_cycle>25) m_to_be_deleted = true;

	return false;
} /* G_Confirmation::check_status */



