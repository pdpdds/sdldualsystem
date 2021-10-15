#ifndef __TITLEINPUTCOMPONENT_H
#define __TITLEINPUTCOMPONENT_H

#include "InputComponent.hpp"

#include "SDL.h"

class TitleInputComponent : public InputComponent {
 public:
  void update( GameState *obj, GameEngine *engine );

  TitleInputComponent( GameBoard *board, SDL_Event *event );
  ~TitleInputComponent();

 private:
  SDL_Rect _new_game;
  SDL_Rect _about;
  SDL_Rect _quit;
  int _mouse_x;
  int _mouse_y;
};

#endif /* __TITLEINPUTCOMPONENT_H */
