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

GLevelPack::GLevelPack()
{
} /* GLevelPack::GLevelPack */


GLevelPack::~GLevelPack()
{
} /* GLevelPack::~GLevelPack */


void GLevelPack::addNewLevel(GLevel *level)
{
	m_levels.Add(level);
} /* GLevelPack::addNewLevel */


void GLevelPack::deleteLevel(GLevel *level)
{
	m_levels.DeleteElement(level);
} /* GLevelPack::deleteLevel */


int GLevelPack::getNLevels(void)
{
	return m_levels.Length();
} /* GLevelPack::getNLevels */


GLevel *GLevelPack::getLevel(int n)
{
	return m_levels[n];
} /* GLevelPack::getLevel */


GLevel *GLevelPack::getLevel(char *name)
{
	GLevel *l;
	m_levels.Rewind();
	while(m_levels.Iterate(l)) if (strcmp(l->getName(),name)==0) return l;
	return 0;
} /* GLevelPack::getLevel */


int GLevelPack::getLevelPosition(GLevel *level)
{
	return m_levels.PositionRef(level);
} /* GLevelPack::getLevelPosition */


void GLevelPack::setLevel(int n,GLevel *level)
{
	if (m_levels.Length()>n) {
		m_levels.SetObj(n,level);
	} // if
} /* GLevelPack::setLevel */


