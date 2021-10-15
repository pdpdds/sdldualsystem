#ifndef __ABOUTGRAPHICSCOMPONENT_H
#define __ABOUTGRAPHICSCOMPONENT_H

#include "GraphicsComponent.hpp"

class AboutGraphicsComponent : public GraphicsComponent {
public:
  void update( GameState *obj );

  AboutGraphicsComponent( GameBoard *board, GameEngine *engine );
  ~AboutGraphicsComponent();

};

#endif /* __ABOUTGRAPHICSCOMPONENT_H */
