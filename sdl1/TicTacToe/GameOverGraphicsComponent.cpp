#include "GameOverGraphicsComponent.hpp"
#include "GameOverSprite.hpp"
#include "BackgroundSprite.hpp"
#include "GameEngine.hpp"
#include "GameState.hpp"
#include "GameBoard.hpp"

GameOverGraphicsComponent::GameOverGraphicsComponent( GameBoard *board, GameEngine *engine ):
  GraphicsComponent( board, engine ){

  _text = new GameOverSprite( engine->getBoardSurface(),
			      engine->getBoardWidth()/2 ,
			      engine->getBoardHeight()/3 ,
			      MySprite::DRAW_ORGIN );

  switch ( board->checkGameWon() ){
  case Values::WIN_ONE:
    _text->switchFrame( GameOverSprite::FRAME_PLAYER_ONE );
    break;

  case Values::WIN_TWO:
    _text->switchFrame( GameOverSprite::FRAME_PLAYER_TWO );
    break;

  case Values::WIN_TIE:
    _text->switchFrame( GameOverSprite::FRAME_PLAYER_TIE );
    break;
  }


}

GameOverGraphicsComponent::~GameOverGraphicsComponent(){
  _board = NULL;
  _engine = NULL;
}

void GameOverGraphicsComponent::update( GameState *obj ){
  BackgroundSprite *background = _engine->getBackground();
  background->switchFrame( BackgroundSprite::FRAME_GAMEOVER );
  background->update();

  _text->update();
}
