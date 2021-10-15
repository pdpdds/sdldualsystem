#ifndef __BACKGROUNDSPRITE_H
#define __BACKGROUNDSPRITE_H

#include "MySprite.hpp"

class BackgroundSprite : public MySprite {

 public:
  enum BackgroundSpriteFrame{
    FRAME_NULL = -1,
    FRAME_TITLE = 0,
    FRAME_BOARD = 1,
    FRAME_ABOUT = 2,
    FRAME_GAMEOVER = 3
  };

  BackgroundSprite(SDL_Surface* screen);
  ~BackgroundSprite(){}
 private:

 protected:

};

#endif /* __BACKGROUNDSPRITE_H */
