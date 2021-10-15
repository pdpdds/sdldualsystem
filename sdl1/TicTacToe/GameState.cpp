#include "GameState.hpp"
#include "InputComponent.hpp"
#include "GraphicsComponent.hpp"
#include "GameEngine.hpp"

#include <iostream>

using namespace std;

GameState::GameState( InputComponent *player_one,
		      InputComponent *player_two,
		      GraphicsComponent *graphics,
		      GameEngine *engine ):
  _player_one(player_one),
  _player_two(player_two),
  _graphics(graphics),
  _engine(engine)
{}

GameState::~GameState(){
  delete _player_one;
  delete _player_two;
  delete _graphics;
}

void GameState::handleInput(){
  Values::CurrentTurn turn = _engine->getCurrentTurn();
  if ( turn == Values::TURN_ONE && _player_one != NULL){
    _player_one->update( this, _engine );
  } else if ( turn == Values::TURN_TWO && _player_two != NULL ){
    _player_two->update( this, _engine );
  }
}

void GameState::draw(){
  _graphics->update( this );
}
