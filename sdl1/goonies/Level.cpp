#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif


#ifdef _WIN32
#include "windows.h"
#endif

#include "stdio.h"
#include "stdarg.h"
#include "List.h"
#include "Symbol.h"

#include "Level.h"
#include "LevelPack.h"

GLevel::GLevel()
{
	m_name = 0;
	m_password = 0;
} /* GLevel::GLevel */


GLevel::~GLevel()
{
	if (m_name!=0) delete m_name;
	if (m_password!=0) delete m_password;
} /* GLevel::~GLevel */


void GLevel::setName(char *name)
{
	if (m_name!=0) delete m_name;
	m_name=new Symbol(name);
} /* GLevel::setName */


void GLevel::setPassword(char *pwd)
{
	if (m_password!=0) delete m_password;
	m_password=new Symbol(pwd);
} /* GLevel::setPassword */


char *GLevel::getName(void)
{
	if (m_name!=0) return m_name->get();
	return 0;
} /* GLevel::getName */


char *GLevel::getPassword(void)
{
	if (m_password!=0) return m_password->get();
	return 0;
} /* GLevel::getPassword */

