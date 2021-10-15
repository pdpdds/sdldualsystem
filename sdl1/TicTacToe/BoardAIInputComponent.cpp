#include "BoardAIInputComponent.hpp"
#include "enums.hpp"
#include "GameEngine.hpp"
#include "GameState.hpp"
#include "GameBoard.hpp"
#include "SDL_collide.h"

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

BoardAIInputComponent::BoardAIInputComponent( GameBoard *board, SDL_Event *event ):
  InputComponent(board,event) {

  _rows = _board->getRows();
  _columns = _board->getColumns();

  _current_board = initMap();
  _friends = initMap();
  _blocks = initMap();
  _combined = initMap();
}

BoardAIInputComponent::~BoardAIInputComponent(){
  deleteMap(_current_board);
  deleteMap(_friends);
  deleteMap(_blocks);
  deleteMap(_combined);
}

void BoardAIInputComponent::update( GameState *obj, GameEngine *engine ){
  zeroMap(_current_board);
  zeroMap(_friends);
  zeroMap(_blocks);
  zeroMap(_combined);

  for ( int i=0; i<_rows; i++ ){
    for ( int j=0; j<_columns; j++ ) {
      _current_board[i][j] = _board->getSquare( j, i );
    }
  }

  Values::CurrentTurn turn = engine->getCurrentTurn();

  markFriends(turn);
  markBlocks(turn);
  combine(turn);

  cout << "'Combined' Board:" << endl;
  printBoard(_combined);

  chooseSquare(turn,engine);
}

void BoardAIInputComponent::markFriends( Values::CurrentTurn t ){
  Values::Players me = ( t == Values::TURN_ONE ) ? 
    Values::PLAYER_ONE : Values::PLAYER_TWO;

  mark( _friends, me );
}

void BoardAIInputComponent::markBlocks( Values::CurrentTurn t ){
  Values::Players other = ( t != Values::TURN_ONE ) ? 
    Values::PLAYER_ONE : Values::PLAYER_TWO;

  mark( _blocks, other );
}

void BoardAIInputComponent::mark( int **board, int check){
  // look for columns, rows, and diagonals that are contain at least
  // one of 'check' marks, but none of the oposing players.
  // mark that square with the number of 'check' found in that row,
  // column, or diagonal.

  bool foundOther = false; // used if the other players mark is found
			   // in a column, row, or diagonal
  int foundCheck = 0; // number of 'check' marks found in a column, row, or diagonal

  // check rows
  for ( int i=0; i<_rows; i++ ){ 
    foundOther = false;
    foundCheck = 0;

    //first, check to see if there are any of mine in this row ( and
    //how many ), and if there are any of the opposing players in this row
    for ( int j=0; j<_columns; j++) {
      if ( _current_board[i][j] != Values::PLAYER_NULL ){
	if ( _current_board[i][j] == check ) {
	  foundCheck++;
	} else {
	  //one of the other players marks has been found, ignore this row
	  foundOther = true;
	  break;
	}
      }
    }

    // if we haven't found any marks for the other player in this row,
    // add the number of 'check' marks in this row to each cell
    if ( !foundOther ){
      for ( int j=0; j<_columns; j++ ){
	if ( _current_board[i][j] == Values::PLAYER_NULL && 
	  board[i][j] < foundCheck )
	  board[i][j] = foundCheck;
      }
    }
  }

  // check columns

  for ( int j=0; j<_columns; j++ ){
    foundOther = false;
    foundCheck = 0;

    //same as above, but this time, for columns.
    for ( int i=0; i<_rows; i++ ) {
      if ( _current_board[i][j] != Values::PLAYER_NULL ){
	if ( _current_board[i][j] == check ){
	  foundCheck++;
	} else {
	  foundOther = true;
	  break;
	}
      }
    }

    if ( !foundOther ){
      for ( int i=0; i<_rows; i++ ){ 
	if ( _current_board[i][j] == Values::PLAYER_NULL &&
	  board[i][j] < foundCheck )
	  board[i][j] = foundCheck;
      }
    }
  }

  // check diagonal ( top left to bottom right )

  foundOther = false;
  foundCheck = 0;
  // go through the diagonal once, to check for others and how many of
  // 'check' marks there are
  for ( int i=0, j=0; j<_columns, i<_rows; i++, j++ ){
    if ( _current_board[i][j] != Values::PLAYER_NULL ){
      if ( _current_board[i][j] == check ){
	foundCheck++;
      } else {
	foundOther = true;
	break;
      }
    }
  }

  if ( !foundOther ){
    // go through the diagonal, adding the number of 'check' marks 
    for ( int i=0, j=0; j<_columns, i<_rows; i++, j++ ){
      if ( _current_board[i][j] == Values::PLAYER_NULL &&
	board[i][j] < foundCheck )
	board[i][j] = foundCheck;
    }
  }

  // now the other diagonal ( lower left to top right )
  foundOther = false;
  foundCheck = 0;
  for ( int i=(_rows-1), j=0; i>=0, j<_columns; i--, j++ ){
    if ( _current_board[i][j] != Values::PLAYER_NULL ){
      if ( _current_board[i][j] == check ){
	foundCheck++;
      } else {
	foundOther = true;
	break;
      }
    }
  }

  if ( !foundOther ){
    for ( int i=(_rows-1), j=0; i>=0, j<_columns; i--, j++ ){
      if ( _current_board[i][j] == Values::PLAYER_NULL && 
	board[i][j] < foundCheck )
	board[i][j] = foundCheck;
    }
  }

}

void BoardAIInputComponent::combine( Values::CurrentTurn t ){
  for ( int i=0; i<_rows; i++ ){
    for ( int j=0; j<_columns; j++ ){
      _combined[i][j] = _friends[i][j] + _blocks[i][j];
    }
  }
}

void BoardAIInputComponent::chooseSquare( Values::CurrentTurn t, GameEngine *engine ){
  bool highestFound = false;
  int highest = 0;

  int squaresChecked = 0;
  int squaresToCheck = _rows * _columns;

  do{
    for ( int i=0; i<_rows; i++ ) {
      for ( int j=0; j<_columns; j++ ){
	if ( _combined[i][j] > highest && _combined[i][j] != 0 ){
	  // found a number higher than our current highest, so we
	  // reset our squares checked counter, and clear out all the
	  // previous spots that matched the highest
	  squaresChecked = 1;
	  highest = _combined[i][j];
	  _spots.clear();

	  BoardSpot spot;
	  spot.x = j;
	  spot.y = i;
	  _spots.push_back(spot);
	} else if ( _combined[i][j] == highest && _combined[i][j] != 0 ){
	  BoardSpot spot;
	  spot.x = j;
	  spot.y = i;
	  _spots.push_back(spot);

	  squaresChecked++;
	} else {
	  squaresChecked++;
	}
      }
    }

    if ( squaresChecked == squaresToCheck ){
      highestFound = true;
    } else {
      cout << "Entire board run through, resetting squaresChecked to 0" << endl;
      squaresChecked = 0;
      _spots.clear();
    }

  } while ( !highestFound );

  Values::Players other = ( engine->getCurrentTurn() != Values::TURN_ONE ) ? 
    Values::PLAYER_ONE : Values::PLAYER_TWO;

  Values::CurrentTurn otherTurn = ( other == Values::PLAYER_ONE ) ?
    Values::TURN_ONE : Values::TURN_TWO;

  Values::Players me = ( other == Values::PLAYER_ONE ) ?
    Values::PLAYER_TWO : Values::PLAYER_ONE;


  if ( _spots.size() > 0 ){ // it will be zero if there is a draw
    int choose = rand() % _spots.size();

    BoardSpot chosen = _spots.at(choose);

    _spots.clear();

    bool marked = _board->markSquare( chosen.x, chosen.y, me );

    if ( !marked ){
      cout << "Position already picked?!?! Sean's AI fails at picking spots." << endl;

      // IS IT A DRAW?!? Probably not, seems to be caught by code below

    } else {
      engine->setCurrentTurn( otherTurn );
      // we've marked our spot, no need for any more processing
    }

  } else {
    //DRAW, BITCHES
    engine->setCurrentTurn( otherTurn );
  }

}

int ** BoardAIInputComponent::initMap(){

  int **_map = new int*[_rows];

  for ( int i=0; i<_rows; i++ ){
    _map[i] = new int[_columns];
  }

  return _map;
}

void BoardAIInputComponent::deleteMap( int **_map ){
  for ( int i=0; i<_rows; i++ ){
    delete [] _map[i];
  }

  delete _map;
}

void BoardAIInputComponent::zeroMap( int **_map ){
  for ( int i=0; i<_rows; i++ ) {
    for ( int j=0; j<_columns; j++ ){
      _map[i][j] = 0;
    }
  }
}

void BoardAIInputComponent::printBoard( int **_map ){
  cout << "Board[" << _rows << "][" << _columns << "]" << endl;

  cout << "+-+-+-+" << endl;
  for ( int i=0; i<_rows; i++) {
    for ( int j=0; j<_columns; j++) {
      cout << "|" << _map[i][j];
    }
    cout << "|" << endl << "+-+-+-+" << endl;
  }

  cout << endl;
}
