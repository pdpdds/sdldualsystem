//Includes
#include 	"Invader.h"

Invader::Invader() {
	//Set the Invader is a Low invader by default
	SetType(INVADER_LOW);
}

Invader::Invader(const unsigned short param_type) {
	//Set the Invader is a Low invader by default
	SetType(param_type);
}

Invader::~Invader() {
	
}

void Invader::SetType(const unsigned short param_type) {
	
	//Load the default offset
	graphics.h = INVADER_SPRITE_DIMENSION;
	graphics.w = INVADER_SPRITE_DIMENSION;
	space_definition.x = 0;
	space_definition.y = 0;
	space_definition.h = INVADER_SPRITE_DIMENSION;
	space_definition.w = INVADER_SPRITE_DIMENSION;
	
	//Initialize the variables
	frames_counter = 0;
	speed = 0;
	
	//The invader is alive
	alive = true;
	//Set the type value
	type = param_type;
	
	//Animation start point
	if(type == INVADER_MOTHERSHIP) {
		graphics.x = 0;
	} else {
		graphics.x = ((rand()%3) * INVADER_SPRITE_DIMENSION);
	}
	
	switch(type) {
		case INVADER_LOW:
			graphics.y = (INVADER_SPRITE_DIMENSION * 0);
			fire_probability = 1;
			resistance = 1;
			break;
		case INVADER_MEDIUM:
			graphics.y = (INVADER_SPRITE_DIMENSION * 1);
			fire_probability = 2;
			resistance = 2;
			break;
		case INVADER_HIGH:
			graphics.y = (INVADER_SPRITE_DIMENSION * 2);
			fire_probability = 3;
			resistance = 3;
			break;
		case INVADER_MOTHERSHIP:
			graphics.y = (INVADER_SPRITE_DIMENSION * 3);
			fire_probability = 2;
			resistance = 6;
			space_definition.y = 10;
			//Override the default space and graphic offset
			speed = 4;
			graphics.w = (INVADER_SPRITE_DIMENSION * 2);
			space_definition.w = (INVADER_SPRITE_DIMENSION * 2);
			break;
	}
}

int Invader::GetType() {
	return type;
}

bool Invader::StillAlive() {
	return alive;
}

void Invader::Hit() {
	resistance--;
	if(resistance <= 0) {
		alive = false;
	}
}

void Invader::Kill() {
	alive = false;
}

void Invader::Revive() {
	SetType(type);
}

bool Invader::Fire() {
	
	if((rand()%1000) <= fire_probability) {
		return true;
	}
	
	return false;
	
}

void Invader::Move() {
	if(type == INVADER_MOTHERSHIP) {
		
		space_definition.x += speed;
		
		if((space_definition.x < 0) || (space_definition.x > (WIDTH - (INVADER_SPRITE_DIMENSION * 2)))) {
			speed *= -1;
		}
		
	}
}

int Invader::FreeItem() {
	
	int item = (rand()%50); 
	
	if(item <= 5) {
		if(item <= 1) {
			return TYPE_POWERUP_SHIELD;
		} else if(item == 2) {
			return TYPE_POWERUP_INVI;
		} else if(item == 3) {
			return TYPE_POWERUP_MISILES;
		} else if(item == 4) {
			return TYPE_POWERUP_PLASMA;
		} else if(item == 5) {
			return TYPE_POWERUP_TIME;
		}
	}
	
	return TYPE_NONE;
}

SDL_Rect* Invader::Next() {
	
	//Update the frames counter
	frames_counter++;
	
	if((frames_counter / 4) == 1) {
		frames_counter = 0;
		
		//Step the animation
		if(type != INVADER_MOTHERSHIP) {
			graphics.x += INVADER_SPRITE_DIMENSION;
		} else {
			graphics.x += (INVADER_SPRITE_DIMENSION * 2);
		}
	}
	
	//If last animation frame reached, reset the x value
	if(graphics.x >= (INVADER_SPRITE_DIMENSION * IMAGES_PER_INVADER)) {
		graphics.x = 0;
	}
		
	return &graphics;
}

SDL_Rect* Invader::GetSpaceDefinition() {
	return &space_definition;
}

string Invader::GetHash() {
	
	stringstream sstr;
	sstr << type << DIVISION_CHAR;
	sstr << fire_probability << DIVISION_CHAR;
	sstr << alive << DIVISION_CHAR;
	sstr << resistance << DIVISION_CHAR;
	sstr << speed << DIVISION_CHAR;
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

