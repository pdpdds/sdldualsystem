#include "CharSprite.hpp"

CharSprite::CharSprite(SDL_Surface* screen, int xpos, int ypos) :
  MySprite::MySprite(screen,xpos,ypos) {

  _number_of_frames = 2;
  _height_of_frame = 150;
  _current_frame = FRAME_NULL;

  _imageFile = "letters.png";

  loadImage();
  setDestClipRect();
}

