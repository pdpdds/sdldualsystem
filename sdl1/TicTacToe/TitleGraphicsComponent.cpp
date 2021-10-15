#include "TitleGraphicsComponent.hpp"
#include "BackgroundSprite.hpp"
#include "GameEngine.hpp"
#include "GameState.hpp"
#include "GameBoard.hpp"

TitleGraphicsComponent::TitleGraphicsComponent( GameBoard *board, GameEngine *engine ):
  GraphicsComponent( board, engine ){}

TitleGraphicsComponent::~TitleGraphicsComponent(){
  _engine = NULL;
  _board = NULL;
}

void TitleGraphicsComponent::update( GameState *obj ){
  BackgroundSprite* background = _engine->getBackground();
  background->switchFrame( BackgroundSprite::FRAME_TITLE );
  background->update();
}
