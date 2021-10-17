/*============================================================================||
||       __  ___  _______    ___  ____  __         ___                        ||
||      /  \/  /_/__  __/_ _/ __\/_/  \/ /   _____/   \                       ||
||     / /\_/ / ___/ / __ `/ /  __/ __/  \  / ___/  `_/                       ||
||    / /  / / __// / /_/ / /__/ / / / /\ \/ __// /\ \                        ||
||   /_/  /_/____/_/\__,_/\___/_/_/ /_/ /_/____/_/ /_/                        ||
||                                                                            ||
||============================================================================||
|| Define																	  ||
|| Copyright Tim Jones. All Rights Reserved.                                  ||
||============================================================================*/


#ifndef _DEFINE_H_
	#define _DEFINE_H_

//==============================================================================

#define		WWIDTH				640
#define		WHEIGHT				480
#define		WBIT				32
#define		WFLAGS				(SDL_HWSURFACE | SDL_DOUBLEBUF)
#define		WCAPTION			"MetaPacman 1.0"

#define		TILE_SIZE			32

#define 	MAP_WIDTH			15
#define		MAP_HEIGHT			15

#define		MAP_AREA			(MAP_WIDTH * MAP_HEIGHT)

#define		MAP_WIDTH_PIXELS	(MAP_WIDTH * TILE_SIZE)
#define		MAP_HEIGHT_PIXELS	(MAP_HEIGHT * TILE_SIZE)

#define		MAP_AREA_PIXELS		(MAP_WIDTH_PIXELS * MAP_HEIGHT_PIXELS)

#define		TILE_TYPE_GROUND	0
#define		TILE_TYPE_WALL		1
#define		TILE_TYPE_EDGE		2

#define		DIR_LEFT			0
#define		DIR_UP				1
#define		DIR_RIGHT			2
#define		DIR_DOWN			3

#define		NIBBLET_NONE		-1
#define		NIBBLET_SMALL		0
#define		NIBBLET_LARGE		1

#define		SKILL_EASY			0
#define		SKILL_MEDIUM		1
#define		SKILL_HARD			2

#define		DEFAULT_DIFFICULTY	SKILL_MEDIUM
#define		DEFAULT_LIVES		2
#define 	DEFAULT_LEVEL		"maps/1.map"

#define		MAX_GHOSTS			4

#define		SCORE_FOR_LIFE		500

//==============================================================================

#endif

