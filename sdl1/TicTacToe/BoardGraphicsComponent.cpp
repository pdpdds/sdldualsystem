#include "BoardGraphicsComponent.hpp"
#include "BackgroundSprite.hpp"
#include "GameEngine.hpp"
#include "GameState.hpp"
#include "GameBoard.hpp"

BoardGraphicsComponent::BoardGraphicsComponent( GameBoard *board, GameEngine *engine ):
  GraphicsComponent(board, engine){

  _rows = _board->getRows();
  _columns = _board->getColumns();

  _offsets = new SDL_Rect**[_rows];
  for ( int i=0; i<_rows; i++ ){
    _offsets[i] = new SDL_Rect*[_columns];

    for ( int j=0; j<_columns; j++ ) {
      _offsets[i][j] = new SDL_Rect;
      _offsets[i][j]->x = ( _tile_width * j) +
	( _tile_x_offset + ( j * ( _tile_x_offset * 2)));
      _offsets[i][j]->y = (_tile_height * i) + 
	( _tile_y_offset + ( i * ( _tile_y_offset * 2)));
      _offsets[i][j]->w = _tile_width;
      _offsets[i][j]->h = _tile_height;
    }
  }

  _sprite = new CharSprite( _engine->getBoardSurface(), 0, 0 );

}

BoardGraphicsComponent::~BoardGraphicsComponent(){
  for ( int i=0; i<_rows; i++ ){ 
    for ( int j=0; j<_columns; j++ ) {
      delete [] _offsets[i][j];
    }
    delete [] _offsets[i];
  }

  delete _offsets;

  delete _sprite;
}

void BoardGraphicsComponent::update( GameState *obj ){
  BackgroundSprite* background = _engine->getBackground();
  background->switchFrame( BackgroundSprite::FRAME_BOARD );
  background->update();

  for ( int i=0; i< _rows; i++ ){
    for ( int j=0; j<_columns; j++ ){ 
      Values::Players spot = _board->getSquare(j,i);
      if ( spot != Values::PLAYER_NULL ){
	switch( spot ){
	case Values::PLAYER_ONE:
	  _sprite->switchFrame( CharSprite::FRAME_X );
	  break;
	  
	case Values::PLAYER_TWO:
	  _sprite->switchFrame( CharSprite::FRAME_O );
	  break;
	}
	_sprite->update( _offsets[i][j] );
      }
    }
  }

}
