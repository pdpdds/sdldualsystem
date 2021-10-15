#ifndef __MYSPRITE_H
#define __MYSPRITE_H

#include "SDL.h"
#include "SDL_image.h"
#include <string>

class MySprite {
  
 public:
     enum DrawStyle {
         DRAW_TOPLEFT = 0,
         DRAW_ORGIN = 1,
     };

  enum SpriteFrame{
    FRAME_NULL = -1
  };

  MySprite(SDL_Surface* screen, int xpos, int ypos);
  ~MySprite();
  virtual void update();
  virtual void update( SDL_Rect *offset );
  virtual void switchFrame(int frame);
 private:

 protected:
  // position on screen
  int _x;
  int _y;

  // screen that this sprite is blitted to
  SDL_Surface* _screen;

  // information about the sprite
  int _number_of_frames;
  int _width_of_frame;
  int _height_of_frame;
  int _current_frame;

  // the sprite image
  SDL_Surface* _image;
  std::string _imageFile;

  // clipping information
  SDL_Rect _dest_clip;
  SDL_Rect _source_clip;

  void setDestClipRect();
  void loadImage();
};

#endif /* __MYSPRITE_H */
