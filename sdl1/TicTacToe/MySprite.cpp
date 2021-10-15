#include "MySprite.hpp"
#include <iostream>
using namespace std;

MySprite::MySprite(SDL_Surface* screen, int xpos, int ypos){
  _screen = screen;
  _x = xpos;
  _y = ypos;
  _number_of_frames = 0;
  _width_of_frame = 0;
  _height_of_frame = 0;
  _current_frame = FRAME_NULL;
  _image = NULL;

  // set source clip to all zeros
  _source_clip.w = 0;
  _source_clip.h = 0;
  _source_clip.x = 0;
  _source_clip.y = 0;

  setDestClipRect();
}

MySprite::~MySprite(){
  SDL_FreeSurface( _image );
}

void MySprite::update(){
  if ( _current_frame != -1 ){
    if ( _image != NULL ) {
      SDL_BlitSurface( _image, &_source_clip, _screen, &_dest_clip );
    }
  }
}

void MySprite::update( SDL_Rect *offset ){
  if ( _current_frame != -1 ){
    if ( _image != NULL ) {
      SDL_BlitSurface( _image, &_source_clip, _screen, offset );
    }
  }
}

void MySprite::switchFrame( int frame ){
  bool frameChanged = false;
  if ( _current_frame != frame ){
    if ( frame > _number_of_frames ){
      frame = frame%_number_of_frames;
    }
    _current_frame = frame;
    frameChanged = true;
  }

  if ( frameChanged && _current_frame != FRAME_NULL ){
    _source_clip.w = _width_of_frame;
    _source_clip.h = _height_of_frame;
    _source_clip.y = 0;
    _source_clip.x = _width_of_frame * _current_frame;
  } else if ( _current_frame == FRAME_NULL && frameChanged ) {
    _source_clip.w = 0;
    _source_clip.y = 0;
    _source_clip.x = 0;
    _source_clip.y = 0;
  }
}

void MySprite::loadImage(){
  SDL_Surface* loadedImage = NULL;
  if ( !_imageFile.empty() ){
    loadedImage = IMG_Load( _imageFile.c_str() );
    if ( loadedImage != NULL ){
      _image = SDL_DisplayFormatAlpha( loadedImage );

      if ( _image != NULL && _number_of_frames > 0 ){
	_width_of_frame = _image->w / _number_of_frames;
      }

      SDL_FreeSurface( loadedImage );
    } else {
      _image = NULL;
    }
  }
}


void MySprite::setDestClipRect(){
  _dest_clip.x = _x;
  _dest_clip.y = _y;
  _dest_clip.h = _height_of_frame;
  _dest_clip.w = _width_of_frame;
}
