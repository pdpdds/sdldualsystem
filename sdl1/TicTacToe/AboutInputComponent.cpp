#include "AboutInputComponent.hpp"
#include "GameState.hpp"
#include "GameBoard.hpp"
#include "GameEngine.hpp"

#include "SDL_collide.h"

#include <iostream>

using namespace std;

AboutInputComponent::AboutInputComponent(GameBoard *board, SDL_Event *event):
  InputComponent(board,event){

  _back.x = 180;
  _back.y = 350;
  _back.w = 87;
  _back.h = 32;

  _mouse_x = 0;
  _mouse_y = 0;
}


AboutInputComponent::~AboutInputComponent(){
  _board = NULL;
  _event = NULL;
}

void AboutInputComponent::update( GameState *obj, GameEngine *engine ){
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
    if ( SDL_CollideBoundingBox( mouse, _back )){
      engine->setCurrentState( Values::STATE_TITLE );
    }
  }
}
