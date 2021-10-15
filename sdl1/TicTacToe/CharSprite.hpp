#ifndef __CHARSPRITE_H
#define __CHARSPRITE_H

#include "MySprite.hpp"

class CharSprite : public MySprite {

 public:
  enum CharSpriteFrame{
    FRAME_NULL = -1,
    FRAME_X = 0,
    FRAME_O = 1,
  };

  CharSprite(SDL_Surface* screen, int xpos, int ypos);
  ~CharSprite(){}
 private:

 protected:
  CharSpriteFrame _current_frame;
};

#endif /* __CHARSPRITE_H */
