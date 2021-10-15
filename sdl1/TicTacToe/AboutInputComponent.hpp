#ifndef __ABOUTINPUTCOMPONENT_H
#define __ABOUTINPUTCOMPONENT_H

#include "InputComponent.hpp"

#include "SDL.h"

class AboutInputComponent : public InputComponent {
public:
  void update( GameState *obj, GameEngine *engine );

  AboutInputComponent( GameBoard *board, SDL_Event *event );
  ~AboutInputComponent();

private:
  SDL_Rect _back;
  int _mouse_x;
  int _mouse_y;

};

#endif /* __ABOUTINPUTCOMPONENT_H */
