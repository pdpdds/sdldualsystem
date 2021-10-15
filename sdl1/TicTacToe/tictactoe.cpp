#include <iostream>
#include <string>
#include "GameEngine.hpp"

using namespace std;

#undef main
int main( int argc, char** argv ){

  GameEngine *engine = new GameEngine();

  engine->gameLoop();

  return 0;
}
