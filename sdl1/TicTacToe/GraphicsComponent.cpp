#include "GraphicsComponent.hpp"
#include <iostream>
using namespace std;

GraphicsComponent::GraphicsComponent( GameBoard *board, GameEngine *engine ):
  _board(board),
  _engine(engine){}

GraphicsComponent::~GraphicsComponent(){
  _board = NULL;
  _engine = NULL;
}
