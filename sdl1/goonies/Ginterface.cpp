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


List<G_InterfaceElement> G_interface::m_elements;


void G_interface::add_element(G_InterfaceElement *e)
{
	m_elements.Add(e);
} /* G_interface::add_element */


void G_interface::remove_element(G_InterfaceElement *e)
{
	m_elements.DeleteElement(e);
} /* G_interface::remove_element */


void G_interface::substitute_element(G_InterfaceElement *old,G_InterfaceElement *e)
{
	int pos=m_elements.PositionRef(old);
	if (pos>=0) {
		m_elements.SetObj(pos,e);
	} // if
} /* G_interface::substitute_element */


void G_interface::reset(void)
{
	m_elements.Delete();
} /* G_interface::reset */


int G_interface::update_state(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k)
{
	int ret_val=-1;
	G_InterfaceElement *e,*modal=0;
	List<G_InterfaceElement> to_delete;

	if (k->key_press(SDLK_TAB)) {
		G_InterfaceElement *found=0;
		bool any_active=false;
		int pos=0;

		// Find if the mouse is currently over one of the active components:
		m_elements.Rewind();
		while(m_elements.Iterate(e) && found==0) {
			if (e->m_active) {
				if (mousex>=e->m_x && mousex<e->m_x+e->m_dx &&
					mousey>=e->m_y && mousey<e->m_y+e->m_dy) {
					found=e;
				} // if
				any_active=true;
			} // if
		} // while

		if (any_active) {
			if (found==0) pos=-1;
					 else pos=m_elements.PositionRef(found);

			 do {
				 pos++;
				 if (pos>=m_elements.Length()) pos=0;
				 e=m_elements[pos];
			 } while(!e->m_active);

			 SDL_WarpMouse(int(e->m_x+e->m_dx/2),int(e->m_y+e->m_dy/2));
		} // if
	} // if

	m_elements.Rewind();
	while(m_elements.Iterate(e) && modal==0) {		
		if (e->m_modal==true) modal=e;
	} // while

	if (modal!=0) {
		if (modal->check_status(mousex,mousey,button,button_status,k)) ret_val=modal->m_ID;
		if (modal->m_to_be_deleted) to_delete.Add(e);
	} else {
		m_elements.Rewind();
		while(m_elements.Iterate(e)) {		
			if (e->check_status(mousex,mousey,button,button_status,k)) ret_val=e->m_ID;
			if (e->m_to_be_deleted) to_delete.Add(e);
		} // while
	} // if

	while(!to_delete.EmptyP()) {
		e = to_delete.ExtractIni();
		m_elements.DeleteElement(e);
		delete e;
	} // while

	return ret_val;
} /* G_interface::update_state */


void G_interface::draw(void)
{
	G_InterfaceElement *e;

	m_elements.Rewind();
	while(m_elements.Iterate(e)) {
		e->draw();
	} // while

} /* G_interface::draw */


void G_interface::draw(float alpha)
{
	G_InterfaceElement *e;

	m_elements.Rewind();
	while(m_elements.Iterate(e)) {
		e->draw(alpha);
	} // while

} /* G_interface::draw */


void G_interface::enable(int ID)
{
	G_InterfaceElement *e;

	m_elements.Rewind();
	while(m_elements.Iterate(e)) {
		if (e->m_ID==ID) e->m_enabled=true;
	} // while
} /* G_interface::enable */


void G_interface::disable(int ID)
{
	G_InterfaceElement *e;

	m_elements.Rewind();
	while(m_elements.Iterate(e)) {
		if (e->m_ID==ID) e->m_enabled=false;
	} // while
} /* G_interface::disable */


G_InterfaceElement *G_interface::get(int ID)
{
	G_InterfaceElement *e;

	m_elements.Rewind();
	while(m_elements.Iterate(e)) {
		if (e->m_ID==ID) return e;
	} // while
	return 0;
} /* G_interface::get */


/* ------------------------------------------------------------------------------------------------------ */



G_InterfaceElement::G_InterfaceElement()
{
	m_ID=-1;
	m_enabled=false;
	m_active=false;
	m_modal=false;
	m_to_be_deleted=false;
	m_x=m_y=m_dx=m_dy=0;
} /* G_InterfaceElement::G_InterfaceElement */


G_InterfaceElement::~G_InterfaceElement()
{
} /* G_InterfaceElement::~G_InterfaceElement */


bool G_InterfaceElement::check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k)
{
	return false;
} /* G_InterfaceElement::check_status */


void G_InterfaceElement::draw(void)
{
} /* G_InterfaceElement::draw */


void G_InterfaceElement::draw(float alpha)
{
} /* G_InterfaceElement::draw */


