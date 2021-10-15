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

#include "font_extractor.h"


G_Text::G_Text(char *text,float x,float y,bool centered) : G_InterfaceElement()
{
	m_text=new char[strlen(text)+1];
	strcpy(m_text,text);
	m_x=x;
	m_y=y;
	m_ID=-1;
	m_enabled=true;
	m_active=false;
	m_centered=centered;
} /* G_Text::G_Text */


G_Text::~G_Text()
{
	if (m_text!=0) delete []m_text;
	m_text=0;

} /* G_Text::~G_Text */


void G_Text::draw(void)
{
	draw(1);
} /* G_Text::draw */


void G_Text::draw(float alpha)
{
	if (m_centered) font_print_c(int(m_x), int(m_y), 0, 0, 1, alpha, "font", m_text, -4);
			   else font_print(int(m_x), int(m_y), 0, 0, 1, alpha, "font", m_text, -4);
} /* G_Text::draw */


