#ifndef __ENUMS_H
#define __ENUMS_H

struct BoardSpot {
  int x;
  int y;
} ;

namespace Values {
  enum Players{
    PLAYER_NULL = 1,
    PLAYER_ONE = 2,
    PLAYER_TWO = 3,
  };

  enum WinState{
    WIN_NULL,
    WIN_ONE,
    WIN_TWO,
    WIN_TIE
  };

  enum CurrentTurn{
    TURN_ONE,
    TURN_TWO
  };

  enum States{
    STATE_NULL,
    STATE_TITLE,
    STATE_ABOUT,
    STATE_BOARD,
    STATE_PLAYER_1_WIN,
    STATE_PLAYER_2_WIN,
    STATE_PLAYER_TIE,
    STATE_QUIT
  };
};

#endif /* __ENUMS_H */
