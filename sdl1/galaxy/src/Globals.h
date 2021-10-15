#ifndef GLOBALS_H_
#define GLOBALS_H_

//Includes
#include 	<string>					//For string use
#include 	<sstream>					//For stringstream use

using namespace std;

	//Screen related
	static const unsigned short HEIGHT = 768;
	static const unsigned short WIDTH = 1024;
	static const unsigned short BPP = 32;

	//Graphics related
	static const unsigned short FRAMES_PER_SECOND = 30;
	static const unsigned short HERO_SPRITE_DIMENSION = 64;
	static const unsigned short INVADER_SPRITE_DIMENSION = 64;
	static const unsigned short IMAGES_PER_INVADER = 6;
	static const unsigned short OBJECT_SPRITE_DIMENSION = 32;
	static const unsigned short IMAGES_PER_OBJECT = 8;
	static const unsigned short BIG_EXPLOSION_SPRITE_DIMENSION = 64;
	static const unsigned short IMAGES_PER_EXPLOSION = 4;
	static const unsigned short NUMBERS_SPRITE_DIMENSION = 256;

	//Menu related
	static const unsigned short MODE_MENU = 0;
	static const unsigned short MODE_SCORE = 1;
	static const unsigned short MODE_CREDITS = 2;
	static const unsigned short MODE_LEVEL = 3;
	static const unsigned short MODE_GAME_OVER = 4;

	//Object related
	static const unsigned short NOBODY = 5;
	static const unsigned short SHIP = 6;
	static const unsigned short INVADER_LOW = 7;
	static const unsigned short INVADER_MEDIUM = 8;
	static const unsigned short INVADER_HIGH = 9;
	static const unsigned short INVADER_KAMIKAZE = 10;
	static const unsigned short INVADER_MOTHERSHIP = 11;

	static const unsigned short TYPE_LASER = 12;
	static const unsigned short TYPE_MISILE = 13;
	static const unsigned short TYPE_PLASMA = 14;
	static const unsigned short TYPE_KAMIKAZE = 15;
	static const unsigned short TYPE_POWERUP_SHIELD = 16;
	static const unsigned short TYPE_POWERUP_INVI = 17;
	static const unsigned short TYPE_POWERUP_MISILES = 18;
	static const unsigned short TYPE_POWERUP_PLASMA = 19;
	static const unsigned short TYPE_POWERUP_TIME = 20;
	static const unsigned short TYPE_SMALL_EXPLOSION = 21;
	static const unsigned short TYPE_BIG_EXPLOSION = 22;
	static const unsigned short TYPE_NONE = 23;
	static const unsigned short TYPE_GEM = 24;

	//Fleet related
	static const unsigned short FLEET_ROWS = 5;
	static const unsigned short FLEET_COLUMNS = 8;
	static const unsigned short BOTTOM_LIMIT = 200;
	static const unsigned short TOP_LIMIT = 64;
	static const unsigned short INVADERS_SEPARATION = 10;

	//Hero ship collision detection related
	static const unsigned short NOT_HIT = 0;
	static const unsigned short HIT = 1;
	static const unsigned short GAME_OVER = 2;
	static const unsigned short LIVES_LEFT = 3;

	//Font related
	static const short FONT_SMALL = -1;
	static const short FONT_MEDIUM = -2;

	//Files related
	static const string FILE_CONFIG = "data/config.ini";
	static const string FILE_SCORES = "data/scores.dat";

	//Hash related
	static const char DIVISION_CHAR = '|';

	//Global funtions
	string IntegerToString(int integer);

#endif /*GLOBALS_H_*/
