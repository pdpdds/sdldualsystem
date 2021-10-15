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

G_TextInput::G_TextInput(char *initial_text,char *message,int max_characters,float x,float y,float dx,float dy,int ID,GLTManager *GLTM) : G_InterfaceElement()
{
	m_message=new char[strlen(message)+1];
	strcpy(m_message,message);
	m_editing=new char[max_characters+1];
	strcpy(m_editing,initial_text);
	m_editing_position=strlen(m_editing);
	m_max_length = max_characters;
	m_x=x;
	m_y=y;
	m_dx=dx;
	m_max_dx=dx;
	m_dy=dy;
	m_ID=ID;
	m_enabled=true;
	m_active=true;
	m_modal=true;
	m_GLTM = GLTM;
	m_state=0;
	m_cycle=0;

	m_height = 0;
	m_width = 0;
	{
		int x1=0,y1=0,x2=0,y2=0;
		font_box_c("font",m_editing,&x1,&y1,&x2,&y2,-2);

		if (m_dy == 1) {
			m_height = 64;
			m_width = int(64+(m_max_dx-2)*48);
			m_dx = m_max_dx;
			while(m_width-48>x2 && m_dx>2) {
				m_width-=48;
				m_dx--;
			} // while
		} else {
			if (m_dy == 2) {
				m_height = 128;
				m_width = int(128+(m_max_dx-2)*96);
				m_dx = m_max_dx;
				while(m_width-96>x2 && m_dx>2) {
					m_width-=96;
					m_dx--;
				} // while
			} // if
		} // if
	}
} /* G_TextInput::G_TextInput */


G_TextInput::~G_TextInput()
{
	if (m_editing!=0) delete []m_editing;
	m_editing=0;
	if (m_message!=0) delete []m_message;
	m_message=0;

} /* G_TextInput::~G_TextInput */


void G_TextInput::draw(void)
{
	draw(1);
} /* G_TextInput::draw */


void G_TextInput::draw(float alpha)
{
	int i;
	int text_width=0;

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

	if (m_dy == 1) {
		m_GLTM->get(button_left_symbol)->draw(1,1,1,alpha,m_x-m_width/2,m_y-m_height/2,0,0,0.5f);
		for(i=0;i<m_dx-2;i++) {
			m_GLTM->get(button_center_symbol)->draw(1,1,1,alpha,m_x+32+i*48-m_width/2,m_y-m_height/2,0,0,0.5f);
		} // for
		m_GLTM->get(button_right_symbol)->draw(1,1,1,alpha,m_x+32+(m_dx-2)*48-m_width/2,m_y-m_height/2,0,0,0.5f);
	} else {
		if (m_dy == 2) {
			m_GLTM->get(button_left_symbol)->draw(1,1,1,alpha,m_x-m_width/2,m_y-m_height/2,0,0,1);
			for(i=0;i<m_dx-2;i++) {
				m_GLTM->get(button_center_symbol)->draw(1,1,1,alpha,m_x+64+i*96-m_width/2,m_y-m_height/2,0,0,1);
			} // for
			m_GLTM->get(button_right_symbol)->draw(1,1,1,alpha,m_x+64+(m_dx-2)*96-m_width/2,m_y-m_height/2,0,0,1);
		} // if
	} // if

	{
		int x1,y1,y2;
		font_box_c("font",m_editing,&x1,&y1,&text_width,&y2,-2);
		font_print(int(m_x)-text_width/2, int(m_y)-16, 0, 0, 1, alpha, "font", m_editing, -2);
	}

	font_print_c(int(m_x),int(m_y)-(m_height/2+24), 0, 0, 1, alpha, "font", m_message, -2);
	
	// Draw cursor
	{
		char tmp[256];
		int x1,y1,x2,y2;
		strcpy(tmp,m_editing);
		tmp[m_editing_position]=0;
		font_box_c("font",tmp,&x1,&y1,&x2,&y2,-2);

		x1 = (int(m_x)-text_width/2) + x2;

		glColor4f(0, 1, 0, float(0.5f+0.3*sin(m_cycle*0.2)));
        glNormal3f(0.0, 0.0, 1.0);
        glBegin(GL_QUADS);
        glVertex3f(float(x1), m_y-12, 0);
        glVertex3f(float(x1), m_y+24, 0);
        glVertex3f(float(x1)+4, m_y+24, 0);
        glVertex3f(float(x1)+4, m_y-12, 0);
        glEnd();
	}

} /* G_TextInput::draw */



bool G_TextInput::check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k)
{
	if (!m_enabled) {
		return false;
	} // if

	m_cycle++;

	if (m_state==0) {
        int i;
        char characters[] = "1234567890qwertyuiopasdfghjklzxcvbnmñ!%?,.-;: ";

		SDL_keysym *ke;

		while(!k->keyevents.EmptyP()) {
			ke=k->keyevents.ExtractIni();
			if (ke->unicode>=0x20 && ke->unicode<=0x7E && int(strlen(m_editing))<m_max_length) {
				i=0;
				while(characters[i]!=ke->unicode && characters[i]!=0) i++;

				if (characters[i]!=0) {
					if (m_editing_position<strlen(m_editing)) {
						for(unsigned int i=strlen(m_editing)+1;i>m_editing_position;i--) m_editing[i]=m_editing[i-1];
						m_editing[m_editing_position]=(char)ke->unicode;
						m_editing_position++;
					} else {
						m_editing[m_editing_position]=(char)ke->unicode;
						m_editing_position++;
						m_editing[m_editing_position]=0;
					} /* if */
				} // if
			} /* if */

			if (ke->sym==SDLK_BACKSPACE) {
				if (m_editing_position>0) {
					strcpy(m_editing+m_editing_position-1,m_editing+m_editing_position);
					m_editing_position--;
				} /* if */
			} /* if */

			if (ke->sym==SDLK_DELETE) {
				if (m_editing_position<strlen(m_editing)) {
					strcpy(m_editing+m_editing_position,m_editing+m_editing_position+1);
				} /* if */
			} /* if */ 		

			if (ke->sym==SDLK_LEFT) {
				if (m_editing_position>0) {
					m_editing_position--;
				} /* if */
			} /* if */
			if (ke->sym==SDLK_RIGHT) {
				if (m_editing_position<strlen(m_editing)) {
					m_editing_position++;
				} /* if */
			} /* if */

			if (ke->sym==SDLK_HOME) m_editing_position=0;
			if (ke->sym==SDLK_END) m_editing_position=strlen(m_editing);
			
			delete ke;
		} /* while */
	}

	if (k->key_press(SDLK_RETURN)) {
		m_state = 1;
		m_cycle=0;
		return true;
	} // if
	if (k->key_press(SDLK_ESCAPE)) {
		m_state = 1;
		m_cycle=0;
	} // if

	if (m_state == 1 && m_cycle>25) m_to_be_deleted = true;

	{
		int x1=0,y1=0,x2=0,y2=0;
		font_box_c("font",m_editing,&x1,&y1,&x2,&y2,-2);

		if (m_dy == 1) {
			m_height = 64;
			m_width = int(64+(m_max_dx-2)*48);
			m_dx = m_max_dx;
			while(m_width-(48+64)>x2 && m_dx>2) {
				m_width-=48;
				m_dx--;
			} // while
		} else {
			if (m_dy == 2) {
				m_height = 128;
				m_width = int(128+(m_max_dx-2)*96);
				m_dx = m_max_dx;
				while(m_width-(96+128)>x2 && m_dx>2) {
					m_width-=96;
					m_dx--;
				} // while
			} // if
		} // if
	}

	return false;
} /* G_TextInput::check_status */



