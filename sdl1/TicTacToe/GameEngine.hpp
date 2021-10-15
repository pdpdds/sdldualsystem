#ifndef __GAMEENGINE_H
#define __GAMEENGINE_H

#include "enums.hpp"

#include "SDL.h"

class GameBoard;
class GameState;
class BackgroundSprite;

class GameEngine {

public:
	GameEngine();
	~GameEngine();
	void gameLoop();
	void switchState(Values::WinState w);
	void render();
	void checkForEvents();

	Values::CurrentTurn getCurrentTurn();
	void setCurrentTurn(Values::CurrentTurn turn);

	Values::States getCurrentState();
	void setCurrentState(Values::States state);

	SDL_Surface* getBoardSurface();
	BackgroundSprite* getBackground();
	GameBoard* getGameBoard();

	int getBoardWidth() { return _board_width; }
	int getBoardHeight() { return _board_height; }

private:
	// variables
	static const int _screen_width = 450;
	static const int _screen_height = 550;
	static const int _screen_bpp = 32;

	static const int _board_height = 450;
	static const int _board_width = 450;
	SDL_Rect _board_offset;

	int _wins;
	int _losses;

	bool _switch_state;

	// what our current state is
	Values::States _current_state;
	// what our last state was
	Values::States _last_state;
	// whose turn it is
	Values::CurrentTurn _current_turn;
	// game state object
	GameState* _state;
	// SDL event
	SDL_Event* _event;
	// board object
	GameBoard* _board;

	// screen ( with window manager trimmings, etc )
	SDL_Surface* _screen;

	// just the game board. game state blits to this, engine blits this
	// to the screen
	SDL_Surface* _board_surface;

	// the sprite holding the background
	BackgroundSprite* _background;

	// methods
	void getTitleState();
	void getAboutState();
	void getBoardState();
	void getGameOverState(Values::WinState w);

	void adjustEventForBoard();
protected:

};
#endif /*__GAMEENGINE_H */
