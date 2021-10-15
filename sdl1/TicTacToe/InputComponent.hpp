#ifndef __INPUTCOMPONENT_H
#define __INPUTCOMPONENT_H

#include "SDL.h"

class GameBoard;
class GameState;
class GameEngine;

class InputComponent {

 public:
  virtual void update( GameState *obj, GameEngine *engine ){};

  InputComponent( GameBoard *board, SDL_Event *event );
  virtual ~InputComponent();

 private:

 protected:
  GameBoard *_board;
  SDL_Event *_event;
};

#endif /* __INPUTCOMPONENT_H */
