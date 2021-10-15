#ifndef __GAMESTATE_H
#define __GAMESTATE_H

#include "SDL.h"
#include "SDL_image.h"

class InputComponent;
class GraphicsComponent;
class GameEngine;

class GameState {

 public:
  void handleInput();
  void draw();

  GameState( InputComponent *player_one,
	     InputComponent *player_two,
	     GraphicsComponent *graphics,
	     GameEngine *engine);

  ~GameState();

 private:
  InputComponent *_player_one;
  InputComponent *_player_two;
  GraphicsComponent *_graphics;
  GameEngine *_engine;

 protected:
  
};

#endif /* __GAMESTATE_H */
