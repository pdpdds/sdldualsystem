#ifndef __GAMEOVERGRAPHICSCOMPONENT_H
#define __GAMEOVERGRAPHICSCOMPONENT_H

#include "GraphicsComponent.hpp"
#include "GameOverSprite.hpp"

class GameOverGraphicsComponent : public GraphicsComponent {
public:
  void update( GameState *obj );

  GameOverGraphicsComponent( GameBoard *board, GameEngine *engine );
  ~GameOverGraphicsComponent();

private:

  GameOverSprite *_text;

};

#endif /* __GAMEOVERGRAPHICSCOMPONENT_H */
