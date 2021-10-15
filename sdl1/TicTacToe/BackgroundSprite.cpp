#include "BackgroundSprite.hpp"
#include <iostream>

using namespace std;

BackgroundSprite::BackgroundSprite(SDL_Surface* screen) :
  MySprite::MySprite(screen, 0, 0){

  _number_of_frames = 4;
  _height_of_frame = 450;
  _current_frame = FRAME_NULL;

  // loadImage() defined in MySprite.h
  _imageFile = "screens.png";

  loadImage();
  setDestClipRect();
}
