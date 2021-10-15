#include "InputComponent.hpp"
#include <iostream>
using namespace std;

InputComponent::InputComponent( GameBoard *board, SDL_Event *event ):
  _board(board),
  _event(event){}

InputComponent::~InputComponent(){
  _board = NULL;
  _event = NULL;
}
