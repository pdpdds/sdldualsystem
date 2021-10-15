#include "BoardInputComponent.hpp"
#include "GameEngine.hpp"
#include "GameState.hpp"
#include "GameBoard.hpp"
#include "SDL_collide.h"
#include "enums.hpp"

#include <iostream>

using namespace std;

BoardInputComponent::BoardInputComponent( GameBoard *board, SDL_Event *event ):
  InputComponent(board,event){

  _rows = _board->getRows();
  _columns = _board->getColumns();

  // initialize the offset sqaures for the board
  _squares = new SDL_Rect*[_rows];
  for ( int i=0; i<_rows; i++ ){
    _squares[i] = new SDL_Rect[_columns];
    
    for ( int j=0; j<_columns; j++) {
      _squares[i][j].x = (_tile_width * j) + 
	( _tile_x_offset + ( j * ( _tile_x_offset * 2)));
      _squares[i][j].y = (_tile_height * i) + 
	( _tile_y_offset + ( i * ( _tile_y_offset * 2)));
      _squares[i][j].w = _tile_width;
      _squares[i][j].h = _tile_height;
    }
  }

}

BoardInputComponent::~BoardInputComponent(){
  for ( int i=0; i<_rows; i++) {
    delete [] _squares[i];
  }

  delete _squares;
}


void BoardInputComponent::update( GameState *obj, GameEngine *engine ){
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

  int board_tile_clicked_x = 0;
  int board_tile_clicked_y = 0;
  bool turnTaken = false;

  if ( _event->type == SDL_MOUSEBUTTONDOWN ){
    for ( int i=0; i<_rows; i++) {
      for ( int j=0; j<_columns; j++ ){
	if ( SDL_CollideBoundingBox( mouse, _squares[i][j] ) ){
	  // handle player one or player two input, depending on whose
	  // turn it currently is
	  switch ( engine->getCurrentTurn() ){
	    case Values::TURN_ONE:
	      turnTaken = _board->markSquare( j, i, Values::PLAYER_ONE );
	      if ( turnTaken )
		engine->setCurrentTurn( Values::TURN_TWO );
	      break;
	    case Values::TURN_TWO:
	      turnTaken = _board->markSquare( j, i, Values::PLAYER_TWO );
	      if ( turnTaken )
		engine->setCurrentTurn( Values::TURN_ONE );
	      break;
	  }
	}
	if ( turnTaken ) break;
      }
      if ( turnTaken ) break;
    }
  }
}
