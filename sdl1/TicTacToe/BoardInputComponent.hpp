#ifndef __BOARDINPUTCOMPONENT_H
#define __BOARDINPUTCOMPONENT_H

#include "InputComponent.hpp"

#include "SDL.h"

class BoardInputComponent : public InputComponent {
 public:
  virtual void update( GameState *obj, GameEngine *engine );

  BoardInputComponent( GameBoard *board, SDL_Event *event );
  ~BoardInputComponent();

 protected:
  SDL_Rect **_squares;
  int _rows;
  int _columns;
  static const int _tile_x_offset = 4;
  static const int _tile_y_offset = 4;
  static const int _tile_width = 142;
  static const int _tile_height =142;

  int _mouse_x;
  int _mouse_y;

};

#endif /* __BOARDINPUTCOMPONENT_H */
