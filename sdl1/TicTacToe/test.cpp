#include "GameBoard.hpp"
#include "enums.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

void outputBoard( GameBoard *b, int rows, int columns );

int main( int argc, char** argv ){

  GameBoard *b = new GameBoard();

  int rows = b->getRows();
  int columns = b->getColumns();

  srand( time(NULL) );

  for ( int i=0; i<rows; i++ ){
    for ( int j=0; j<columns; j++ ){
      int mark = rand() % 10 + 1;
      
      switch ( mark ){
	case 1:
	case 2:
	case 3:
	case 4:
	  b->markSquare( j, i, Values::PLAYER_ONE );
	  break;

	case 5:
	case 6:
	case 7:
	case 8:
	  b->markSquare( j, i, Values::PLAYER_TWO );
	  break;
      }
    }
  }

  cout << "Board One: " << endl;
  outputBoard(b, rows, columns);

  cout << "Creating board two using copy constructor: " << endl;

  GameBoard *c = new GameBoard(*b);

  cout << "Done." << endl << endl;

  cout << "Board Two: " << endl;
  outputBoard(c, rows, columns);

  delete b;
  delete c;

}


void outputBoard( GameBoard *b, int rows, int columns ){
  cout << "+-+-+-+" << endl;
  for ( int i=0; i<rows; i++) {
    for ( int j=0; j<columns; j++) {
      cout << "|";
      Values::Players mark = b->getSquare(j,i);
      switch ( mark ){
	case Values::PLAYER_NULL:
	  cout << " ";
	  break;

	case Values::PLAYER_ONE:
	  cout << "x";
	  break;

	case Values::PLAYER_TWO:
	  cout << "o";
	  break;
      }
    }
    cout << "|" << endl;
  }
  cout << "+-+-+-+" << endl;
}
