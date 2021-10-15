#ifndef __BOARDAIINPUTCOMPONENT_H
#define __BOARDAIINPUTCOMPONENT_H

#include "InputComponent.hpp"
#include "enums.hpp"

#include "SDL.h"

#include <vector>

using namespace std;

class GameBoard;
class GameEngine;

class BoardAIInputComponent : public InputComponent {
 public:
  void update( GameState *obj, GameEngine *engine );

  BoardAIInputComponent( GameBoard *board, SDL_Event *event );
  ~BoardAIInputComponent();

 private:
  int **_current_board;
  int **_friends;
  int **_blocks;
  int **_combined;

  int _rows;
  int _columns;

  vector<BoardSpot> _spots;

  void markFriends( Values::CurrentTurn );
  void markBlocks( Values::CurrentTurn );

  void mark( int **, int);
  void combine( Values::CurrentTurn );
  void chooseSquare( Values::CurrentTurn, GameEngine *engine );

  int **initMap();
  void deleteMap( int **map );
  void zeroMap( int **map );

  void printBoard( int **map );

};


#endif /* __BOARDAIINPUTCOMPONENT_H */
