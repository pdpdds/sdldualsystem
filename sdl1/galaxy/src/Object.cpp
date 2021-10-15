//Includes
#include "Object.h"

Object::Object(int param_from, int param_type, int x, int y) {
	
	//Status variables
	resistance = 0;
	alive = true;
	
	//Assign the destination and type variables
	from = param_from;
	type = param_type;
	
	space_definition.x = x;
	space_definition.y = y;
	space_definition.h = OBJECT_SPRITE_DIMENSION;
	space_definition.w = OBJECT_SPRITE_DIMENSION;
		
	//The frame counter for animation frames control
	frames_counter = 0;
	//The horizonal speed, only need by the kamikase
	horizontal_speed = 0;
	
	//The animation start point
	graphics.x = 0;
	graphics.w = OBJECT_SPRITE_DIMENSION;
	graphics.h = OBJECT_SPRITE_DIMENSION;
	
	//Configure the speed and the graphics offsets	
	int graphical_object_number = 0;
	
	if(param_from == NOBODY) {			//From no one, explosions
		
		switch(param_type) {
			case TYPE_SMALL_EXPLOSION:
				vertical_speed = 0;
				graphical_object_number = 12;
				break;
			case TYPE_BIG_EXPLOSION:
				graphics.w = BIG_EXPLOSION_SPRITE_DIMENSION;
				graphics.h = BIG_EXPLOSION_SPRITE_DIMENSION;
				vertical_speed = 0;
				graphical_object_number = 0;
				break;
		}
		
	}else if(param_from == SHIP) {			//From the hero ship
		//Determine if the object is a shoot from laser, misile or plasma
		switch(param_type) {
			case TYPE_LASER:
				vertical_speed = -5;
				graphical_object_number = 0;
				break;
			case TYPE_MISILE:
				vertical_speed = -8;
				graphical_object_number = 1;
				break;
			case TYPE_PLASMA:
				vertical_speed = -3;
				graphical_object_number = 2;
				break;
		}
	} else {								//From an invader
		//Determine if is a laser shoot (from who), power-up or kamikase
		switch(param_type) {
			case TYPE_LASER:
				switch(param_from) {
					case INVADER_LOW:
						vertical_speed = 3;
						graphical_object_number = 3;
						break;
					case INVADER_MEDIUM:
						vertical_speed = 5;
						graphical_object_number = 4;
						break;
					case INVADER_HIGH:
						vertical_speed = 8;
						graphical_object_number = 5;
						break;
				}
				break;
			case TYPE_KAMIKAZE:
				vertical_speed = 8;
				resistance = 3;
				graphical_object_number = 6;
				break;
			case TYPE_POWERUP_SHIELD:
				vertical_speed = 3;
				graphical_object_number = 7;
				break;
			case TYPE_POWERUP_INVI:
				vertical_speed = 3;
				graphical_object_number = 8;
				break;
			case TYPE_POWERUP_MISILES:
				vertical_speed = 3;
				graphical_object_number = 9;
				break;
			case TYPE_POWERUP_PLASMA:
				vertical_speed = 3;
				graphical_object_number = 10;
				break;
			case TYPE_POWERUP_TIME:
				vertical_speed = 3;
				graphical_object_number = 11;
				break;
			case TYPE_GEM:
				vertical_speed = 0;
				graphical_object_number = 13;
				break;
		}
	}
	
	//Set the correct offset
	graphics.y = (OBJECT_SPRITE_DIMENSION * graphical_object_number);
}

Object::~Object() {
	
}

//Move the object;
void Object::Move() {
	
	if(type != TYPE_KAMIKAZE) {
		space_definition.y += vertical_speed;
	}
}

void Object::Move(SDL_Rect* heroShip) {
	
	int center_x = (space_definition.x + (OBJECT_SPRITE_DIMENSION / 2));
	int center_y = (space_definition.y + (OBJECT_SPRITE_DIMENSION / 2));
	int h = (heroShip->y - center_y); 
	
	if(h < 0) {
		h = 0;
	}
	
	if((center_x >= heroShip->x) && (center_x <= (heroShip->x + heroShip->w))) {
		//		*		Kamikaze
		//		|		Direction
		//
		//		x		Hero Ship
		vertical_speed = 3;
		horizontal_speed = 0;
	} else if((heroShip->x >= (center_x - h)) && (heroShip->x < center_x)) {
		//		*		Kamikaze
		//	   /		Direction
		//
		//	  x 		Hero Ship
		vertical_speed = 3;
		horizontal_speed = -3;
	} else if((heroShip->x > center_x) && (heroShip->x <= (center_x + h))) {
		//		*		Kamikaze
		//	     \		Direction
		//
		//	       x	Hero Ship
		vertical_speed = 3;
		horizontal_speed = 3;
	} else if(heroShip->x > (center_x + h)) {
		//		* --	Kamikaze
		//	     		Direction
		//
		//	          x Hero Ship
		vertical_speed = 0;
		horizontal_speed = 3;
	} else if(heroShip->x < (center_x - h)) {
		//	  --*   	Kamikaze
		//	     		Direction
		//
		//x             Hero Ship
		vertical_speed = 0;
		horizontal_speed = -3;
	}
		
	space_definition.y += vertical_speed;
	space_definition.x += horizontal_speed;
}

//Get the SDL_Rect related to their spatial position to check collision
SDL_Rect* Object::GetSpaceDefinition() {
	return &space_definition;
}

//Step to the next animation frame
SDL_Rect* Object::Next() {
	
	//Animation speed regulation
	frames_counter++;
	
	if(frames_counter == 2) {
		
		//Reset the speed regulation
		frames_counter = 0;
		
		//Step the animation
		graphics.x += OBJECT_SPRITE_DIMENSION;
		
		//Reset if last frame is reached 
		if(graphics.x >= (OBJECT_SPRITE_DIMENSION * IMAGES_PER_OBJECT)) {
			
			if(type == TYPE_SMALL_EXPLOSION) {
				alive = false;
			} else {
				graphics.x = 0;
			}
		}	
	}
	
	return &graphics;
	
}

SDL_Rect* Object::BigNext() {
	
	//Animation speed regulation
	frames_counter++;
	
	if(frames_counter == 3) {
		
		//Reset the speed regulation
		frames_counter = 0;
		
		//Step the animation
		graphics.x += BIG_EXPLOSION_SPRITE_DIMENSION;
		
		//Reset if last frame is reached 
		if(graphics.x >= (BIG_EXPLOSION_SPRITE_DIMENSION * IMAGES_PER_EXPLOSION)) {
			
			graphics.y += BIG_EXPLOSION_SPRITE_DIMENSION;
			
			if(graphics.y >= (BIG_EXPLOSION_SPRITE_DIMENSION * IMAGES_PER_EXPLOSION)) {
				alive = false;
			}
		}	
	}
	
	return &graphics;
	
}

bool Object::StillAlive() {
	return alive;
}

void Object::Hit() {
	resistance--;
	if(resistance <= 0) {
		alive = false;
	}
}

void Object::Kill() {
	alive = false;
}

string Object::GetHash() {
	
	stringstream sstr;
	sstr << from << DIVISION_CHAR;
	sstr << type << DIVISION_CHAR;
	sstr << alive << DIVISION_CHAR;
	sstr << resistance << DIVISION_CHAR;
	sstr << horizontal_speed << DIVISION_CHAR;
	sstr << vertical_speed << DIVISION_CHAR;
	sstr << graphics.x << DIVISION_CHAR;
	sstr << graphics.y << DIVISION_CHAR;
	sstr << graphics.h << DIVISION_CHAR;
	sstr << graphics.w << DIVISION_CHAR;
	sstr << space_definition.x << DIVISION_CHAR;
	sstr << space_definition.y << DIVISION_CHAR;
	sstr << space_definition.h << DIVISION_CHAR;
	sstr << space_definition.w;
	
	return sstr.str();
}
