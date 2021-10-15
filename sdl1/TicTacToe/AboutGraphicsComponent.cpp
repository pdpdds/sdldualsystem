#include "AboutGraphicsComponent.hpp"
#include "BackgroundSprite.hpp"
#include "GameEngine.hpp"
#include "GameState.hpp"
#include "GameBoard.hpp"

AboutGraphicsComponent::AboutGraphicsComponent( GameBoard *board, GameEngine *engine ):
  GraphicsComponent( board, engine) {}

AboutGraphicsComponent::~AboutGraphicsComponent(){
  _engine = NULL;
  _board = NULL;
}

void AboutGraphicsComponent::update( GameState *obj ){
  BackgroundSprite *background = _engine->getBackground();
  background->switchFrame( BackgroundSprite::FRAME_ABOUT );
  background->update();
}
