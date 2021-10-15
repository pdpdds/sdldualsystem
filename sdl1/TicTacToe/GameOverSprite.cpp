#include "GameOverSprite.hpp"

GameOverSprite::GameOverSprite( SDL_Surface *screen, int xpos, int ypos, MySprite::DrawStyle style ):
  MySprite::MySprite( screen, xpos, ypos){

  _number_of_frames = 3;
  _height_of_frame = 65;
  _current_frame = FRAME_NULL;

  _imageFile = "winlose.png";
  loadImage();
  setDestClipRect();
}
