//============================================================================
// Name        : Main.cpp
// Author      : Created by Carlos Jenkins (Havok) <hastciberneo@gmail.com>
// Version     :
// Copyright   : This program is free software; you can redistribute it and/or modify
//				 it under the terms of the GNU General Public License.
//				 This program is distributed in the hope that it will be useful,
//				 but WITHOUT ANY WARRANTY.
// Description : Galaxy GPL Game in C++ with SDL, Ansi-style?
//============================================================================

 #include 	"Game.h"
 int main(int argc, char* args[]) {

	//Instance the game
	Game* game_instance = new Game();
	game_instance->Start();
	delete game_instance;

	return 0;

}
