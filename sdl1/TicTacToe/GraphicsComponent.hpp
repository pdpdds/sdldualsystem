#ifndef __GRAPHICSCOMPONENT_H
#define __GRAPHICSCOMPONENT_H

class GameState;
class GameBoard;
class GameEngine;

class GraphicsComponent {
 public:
  virtual void update( GameState *obj ){};

  GraphicsComponent( GameBoard *board, GameEngine *engine );
  virtual ~GraphicsComponent();
 private:

 protected:
  GameBoard *_board;
  GameEngine *_engine;
};

#endif /* __GRAPHICSCOMPONENT_H */
