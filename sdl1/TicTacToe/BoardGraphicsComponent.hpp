#ifndef __BOARDGRAPHICSCOMPONENT_H
#define __BOARDGRAPHICSCOMPONENT_H

#include "GraphicsComponent.hpp"
#include "CharSprite.hpp"

#include "SDL.h"

class BoardGraphicsComponent : public GraphicsComponent {
 public:
  void update( GameState *obj );

  BoardGraphicsComponent( GameBoard *board, GameEngine *engine );
  ~BoardGraphicsComponent();

 private:
  int _rows;
  int _columns;
  static const int _tile_x_offset = 4;
  static const int _tile_y_offset = 4;
  static const int _tile_width = 142;
  static const int _tile_height =142;

  CharSprite *_sprite;

  SDL_Rect ***_offsets;

  
};

#endif /* __BOARDGRAPHICSCOMPONENT_H */
