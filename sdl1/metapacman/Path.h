/*============================================================================||
||       __  ___  _______    ___  ____  __         ___                        ||
||      /  \/  /_/__  __/_ _/ __\/_/  \/ /   _____/   \                       ||
||     / /\_/ / ___/ / __ `/ /  __/ __/  \  / ___/  `_/                       ||
||    / /  / / __// / /_/ / /__/ / / / /\ \/ __// /\ \                        ||
||   /_/  /_/____/_/\__,_/\___/_/_/ /_/ /_/____/_/ /_/                        ||
||                                                                            ||
||============================================================================||
|| CPacman																	  ||
|| Copyright Tim Jones. All Rights Reserved.                                  ||
||============================================================================*/

#ifndef _PATH_H_
	#define _PATH_H_

#include "CMap.h"
#include "Util.h"

//==============================================================================

#define	G_COST			10		//The cost for moving to another tile
#define	G_COST_CORNER	4		//The cost for checking a corner tile

//==============================================================================

using namespace std;

//==============================================================================
class CPath {
	public:
		int	OpenList[MAP_AREA];		//The tiles that are waiting to be checked
		int	ClosedList[MAP_AREA];	//The tiles that have been checked

		int OpenSize;
		int ClosedSize;

		CTile*		Map;	//The tiles

	public:
		CPath();

		void AddToOpen(int ID);

		void OpenToClosed(int ID);

		bool IsOpen(int ID);

		bool IsClosed(int ID);

		int FindNextID();

		bool ValidID(int ID);

		int FindPath(int StartX, int StartY, int EndX, int EndY);
};


//------------------------------------------------------------------------------
int 	Distance(int ID, int EndX, int EndY);

//==============================================================================

#endif

