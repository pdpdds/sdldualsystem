#ifndef __GAMEOVERSPRITE_H
#define __GAMEOVERSPRITE_H

#include "MySprite.hpp"

class GameOverSprite : public MySprite {

public:
  enum GameOverFrame {
    FRAME_NULL = -1,
    FRAME_PLAYER_ONE = 0,
    FRAME_PLAYER_TWO = 1,
    FRAME_PLAYER_TIE = 2
  };

  GameOverSprite( SDL_Surface *screen, int xpos, int ypos , DrawStyle style = MySprite::DRAW_TOPLEFT );
  ~GameOverSprite();

};

#endif /* __GAMEOVERSPRITE_H */
