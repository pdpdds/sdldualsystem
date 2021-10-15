#include "TitleInputComponent.hpp"
#include "GameState.hpp"
#include "GameBoard.hpp"
#include "GameEngine.hpp"

#include "SDL_collide.h"

#include <iostream>

using namespace std;

TitleInputComponent::TitleInputComponent( GameBoard *board, SDL_Event *event ):
  InputComponent(board,event){
  _new_game.x = 158;
  _new_game.y = 258;
  _new_game.w = 133;
  _new_game.h = 18;
  
  _about.x = 186;
  _about.y = 290;
  _about.w = 78;
  _about.h = 22;

  _quit.x = 199;
  _quit.y = 326;
  _quit.w = 54;
  _quit.h = 25;

  _mouse_x = 0;
  _mouse_y = 0;
}

TitleInputComponent::~TitleInputComponent(){
  _board = NULL;
  _event = NULL;
}

void TitleInputComponent::update( GameState *obj, GameEngine *engine ){
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
    if ( SDL_CollideBoundingBox( mouse, _new_game ) ){
      cout << "New Game clicked!" << endl;
      engine->setCurrentState( Values::STATE_BOARD );
    }

    if ( SDL_CollideBoundingBox( mouse, _about ) ){
      cout << "About clicked!" << endl;
    }

    if ( SDL_CollideBoundingBox( mouse, _quit ) ){
      cout << "Quit clicked!" << endl;
    }
  }
}
