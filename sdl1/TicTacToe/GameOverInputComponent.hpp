#ifndef __GAMEOVERINPUTCOMPONENT_H
#define __GAMEOVERINPUTCOMPONENT_H

#include "InputComponent.hpp"

#include "SDL.h"

class GameOverInputComponent : public InputComponent {
public:
  void update( GameState *obj, GameEngine *engine);

  GameOverInputComponent( GameBoard *board, SDL_Event *event );
  ~GameOverInputComponent();

private:
  SDL_Rect _title;
  SDL_Rect _quit;

  int _mouse_x;
  int _mouse_y;

};

#endif /* __GAMEOVERINPUTCOMPONENT_H */
