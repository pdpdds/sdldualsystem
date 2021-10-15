#include "GameOverInputComponent.hpp"
#include "GameState.hpp"
#include "GameBoard.hpp"
#include "GameEngine.hpp"

#include "SDL_collide.h"

#include <iostream>

using namespace std;

GameOverInputComponent::GameOverInputComponent(GameBoard *board, SDL_Event *event ):
  InputComponent(board,event){

  _title.x = 120;
  _title.y = 325;
  _title.w = 208;
  _title.h = 33;

  _quit.x = 180;
  _quit.y = 378;
  _quit.w = 74;
  _quit.h = 34;

  _mouse_x =0;
  _mouse_y =0;

}

GameOverInputComponent::~GameOverInputComponent(){
  _board = NULL;
  _event = NULL;
}

void GameOverInputComponent::update( GameState *obj, GameEngine *engine ){
  switch ( _event->type ){
    case SDL_MOUSEMOTION:
      _mouse_y = _event->motion.y;
      _mouse_x = _event->motion.x;
      break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      _mouse_y = _event->button.y;
      _mouse_x = _event->button.x;
      break;
  }

  SDL_Rect mouse;
  mouse.x = _mouse_x;
  mouse.y = _mouse_y;
  mouse.w = 1;
  mouse.h = 1;

  if ( _event->type == SDL_MOUSEBUTTONDOWN ){
    cout << "Click!" << endl;
    cout << "x: " << mouse.x << ", y: " << mouse.y << endl;
    if ( SDL_CollideBoundingBox( mouse, _title )){
      engine->setCurrentState( Values::STATE_TITLE );
    }

    if ( SDL_CollideBoundingBox( mouse, _quit )){
      engine->setCurrentState( Values::STATE_QUIT );
    }
  }
  
}
