//==============================================================================
#include "Path.h"

//==============================================================================
CPath::CPath() {
	Map = 0;

	OpenSize 	= 0;
	ClosedSize 	= 0;

	for(int i = 0;i < MAP_AREA;i++) {
		OpenList[i] 	= -1;
		ClosedList[i] 	= -1;
	}
}

//------------------------------------------------------------------------------
void CPath::AddToOpen(int ID) {
	OpenList[OpenSize] = ID;
	OpenSize++;
};

//------------------------------------------------------------------------------
void CPath::OpenToClosed(int ID) {
	int Start = 0;
	int i = 0;

	//Find the ID in the open list, and then 'void' it
	for(i = OpenSize - 1; i >= 0; --i) {
		if(OpenList[i] == ID) { 
			OpenList[i] = -1;
			Start = i;
			break;
		}
	}

	//Add to closed
	ClosedList[ClosedSize] = ID;
	ClosedSize++;
}

//------------------------------------------------------------------------------
bool CPath::IsOpen(int ID) {
	//Check if the ID is on the open list
	for(int i = OpenSize - 1; i >= 0; --i) {
		if(IsClosed(i) == false && OpenList[i] == ID) return true;
	}

	return false;
}

//------------------------------------------------------------------------------
bool CPath::IsClosed(int ID) {
	//Check if the ID is on the closed list
	for(int i = ClosedSize - 1; i >= 0; --i) {
		if(ClosedList[i] == ID) return true;
	}

	return false;
}

//------------------------------------------------------------------------------
int CPath::FindNextID() {
	int LowestScoreID 	= -1;
	int LowestScore 	= 10000;

	//Find the next ID with the lowest score, which means the 'best' guess
	//for finding the shortest path
	for(int i = 0;i < OpenSize;i++) {
		if(OpenList[i] != -1 && Map[OpenList[i]].F < LowestScore) {
			LowestScoreID 	= OpenList[i];
			LowestScore 	= Map[OpenList[i]].F;
		}
	}

	return LowestScoreID;
}

//------------------------------------------------------------------------------
bool CPath::ValidID(int ID) {
	//The ID must be within the map range, not a wall, and not closed
	if(ID >= 0 && ID < MAP_AREA && Map[ID].TypeID != TILE_TYPE_WALL && IsClosed(ID) == false) return true;
	return false;
}

//------------------------------------------------------------------------------
int CPath::FindPath(int StartX, int StartY, int EndX, int EndY) {
	ClosedSize = 0;
	OpenSize = 0;

	//Reset
	for(int i = 0;i < MAP_AREA;i++) {
		Map[i].G = Map[i].F = Map[i].H = 0;
		Map[i].ParentID = -1;
	}

	//Add the start point to the open list
	AddToOpen(GetID(StartX, StartY));

	//Starting ID
	int CurrentID = GetID(StartX, StartY);
	
	//Check real quick if the start == end
	if(CurrentID == GetID(EndX, EndY)) return -1;

	//The tile ID we are checking, and calculating F,G,H
	register int ID = 0;

	//Forever... (until not path is found, or we reach the end)
	while(true) {

		//We found the end of our path
		if(CurrentID == GetID(EndX, EndY)) {
			break;
		}
		
		ID = CurrentID - MAP_WIDTH;

		//Top
		//The last part checks to see if we are on the edge
		if(ValidID(ID) == true && CurrentID > MAP_WIDTH) {
			//If this ID is already open...
			if(IsOpen(ID) == true) {
				//This seems like a better path, so change the parent
				if(Map[ID].G < Map[CurrentID].G) {
					Map[ID].ParentID = CurrentID;
				}
			}else{
				AddToOpen(ID);

				//Calculate the costs...
				Map[ID].G += Map[CurrentID].G + G_COST;
				Map[ID].H = Distance(ID, EndX, EndY);

				Map[ID].F  = Map[ID].G;
				Map[ID].F += Map[ID].H;

				//Set parent
				Map[ID].ParentID = CurrentID;
			}
		}

		ID = CurrentID + MAP_WIDTH;

		//Bottom
		//(Comments same as above)
		if(ValidID(ID) == true && CurrentID < MAP_AREA - MAP_WIDTH) {
			if(IsOpen(ID) == true) {
				if(Map[ID].G < Map[CurrentID].G) {
					Map[ID].ParentID = CurrentID;
				}
			}else{
				AddToOpen(ID);

				Map[ID].G += Map[CurrentID].G + G_COST;
				Map[ID].H = Distance(ID, EndX, EndY);
				Map[ID].F  = Map[ID].G;
				Map[ID].F += Map[ID].H;

				Map[ID].ParentID = CurrentID;
			}
		}

		ID = CurrentID - 1;

		//Left
		if(ValidID(ID) == true && CurrentID % MAP_WIDTH != 0) {
			if(IsOpen(ID) == true) {
				if(Map[ID].G < Map[CurrentID].G) {
					Map[ID].ParentID = CurrentID;
				}
			}else{
				AddToOpen(ID);

				Map[ID].G += Map[CurrentID].G + G_COST;
				Map[ID].H = Distance(ID, EndX, EndY);
				Map[ID].F  = Map[ID].G;
				Map[ID].F += Map[ID].H;

				Map[ID].ParentID = CurrentID;
			}
		}

		ID = CurrentID + 1;

		//Right
		if(ValidID(ID) == true && (CurrentID + 1) % MAP_WIDTH != 0) {
			if(IsOpen(ID) == true) {
				if(Map[ID].G < Map[CurrentID].G) {
					Map[ID].ParentID = CurrentID;
				}
			}else{
				AddToOpen(ID);

				Map[ID].G += Map[CurrentID].G + G_COST;
				Map[ID].H = Distance(ID, EndX, EndY);
				Map[ID].F  = Map[ID].G;
				Map[ID].F += Map[ID].H;

				Map[ID].ParentID = CurrentID;
			}
		}

		//Top Left
		//We don't add corners to the open list, because pacman and ghosts
		//can't move diagonally, but we'll need to set it's G cost, because
		//moving these tiles are more costly
		ID = CurrentID - MAP_WIDTH - 1;
		if(ValidID(ID) == true && Map[ID].G != G_COST_CORNER) {
			Map[ID].G = G_COST_CORNER;
		}

		//Top Right
		//(Same as comment above)
		ID = CurrentID - MAP_WIDTH + 1;
		if(ValidID(ID) == true && Map[ID].G != G_COST_CORNER) {
			Map[ID].G = G_COST_CORNER;
		}

		//Bottom Left
		ID = CurrentID + MAP_WIDTH - 1;
		if(ValidID(ID) == true && Map[ID].G != G_COST_CORNER) {
			Map[ID].G = G_COST_CORNER;
		}

		//Bottom Right
		ID = CurrentID + MAP_WIDTH + 1;
		if(ValidID(ID) == true && Map[ID].G != G_COST_CORNER) {
			Map[ID].G = G_COST_CORNER;
		}

		//We're done with this ID, so close it
		OpenToClosed(CurrentID);

		//No more tiles to check! (No path found)
		if(OpenSize <= 0) { 
			return -1;
		}

		//Grab the next ID
		CurrentID = FindNextID();

		//No path found
		if(CurrentID < 0) { 
			return -1;
		}
	}
	
	int tID = 0;

	//Find the next ID to move to. If you wanted to grab all the IDs on the path,
	//create a vector or list, and push_back all the CurrentIDs. Remember to ignore
	//the last ID like I did, because the Start has no parent. :P
	while(Map[CurrentID].ParentID != -1) {
		tID = CurrentID;
		CurrentID = Map[CurrentID].ParentID;
	}
	
	return tID;
}

//------------------------------------------------------------------------------
int Distance(int ID, int EndX, int EndY) {
	int StartX = (ID % MAP_WIDTH);
	int StartY = (ID / MAP_WIDTH);

	//Find the distance from the start to end, which is the number of tiles
	//to move left/right, and the number of tiles to move up/down.
	int Difference = abs(StartX - EndX);
		Difference = Difference + abs(StartY - EndY);

	return Difference * G_COST;
}

//==============================================================================
