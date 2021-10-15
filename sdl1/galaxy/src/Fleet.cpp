//Includes
#include 	"Fleet.h"

Fleet::Fleet() {
	//Initialize all the variables
	fleet_matrix = new Matrix<Invader*>(FLEET_ROWS, FLEET_COLUMNS);
	
	for(short row = 0, column = 0; row != FLEET_ROWS; column++) {
			
			//Reset the columns when reached the limit of the matrix
			if(column == FLEET_COLUMNS) {
				column = -1;
				row++;
			} else {
				//Revive the Invader
				fleet_matrix->Set(row, column, new Invader());
			}
		}
	
	space_definition.h = (FLEET_ROWS * INVADER_SPRITE_DIMENSION);
	space_definition.w = (FLEET_COLUMNS * INVADER_SPRITE_DIMENSION) + ((FLEET_COLUMNS - 1) * INVADERS_SEPARATION);
	
	LoadLevel(1);
}

Fleet::~Fleet() {
	
}

void Fleet::Move() {
	
	space_definition.x += horizontal_speed;
	
	//If the fleet has reached the borders of the screen
	if((space_definition.x < 0) || (space_definition.x > (WIDTH - space_definition.w))) {
		horizontal_speed = (horizontal_speed * -1);
		space_definition.y += vertical_speed;
	}
	
	//If the fleet has reached the upper or bottom border
	if((space_definition.y < TOP_LIMIT) || (space_definition.y > (HEIGHT - space_definition.h - BOTTOM_LIMIT))) {
		vertical_speed = (vertical_speed * -1);
		space_definition.y += vertical_speed;
	}
	
}

//Get the status of a specific invader
Invader* Fleet::Get(short unsigned row, short unsigned column) {
	return fleet_matrix->Get(row, column);
}

//Getter of the space definition
SDL_Rect* Fleet::GetSpaceDefinition() {
	return &space_definition;
}

//Load the new level organization
void Fleet::LoadLevel(short unsigned level) {
	
	space_definition.x = ((WIDTH / 2) - ((FLEET_COLUMNS * INVADER_SPRITE_DIMENSION) / 2) - ((INVADERS_SEPARATION * FLEET_COLUMNS) / 2));
	space_definition.y = TOP_LIMIT;
	
	if(level == 1) {
		
		total_invaders = 40;
		
		horizontal_speed = 3;
		vertical_speed = 10;
		
		for(short row = 0, column = 0; row != FLEET_ROWS; column++) {
			
			//Reset the columns when reached the limit of the matrix
			if(column == FLEET_COLUMNS) {
				column = -1;
				row++;
			} else {
				//Revive the Invader
				(fleet_matrix->Get(row, column))->Revive();
				
				//Load the type
				if(row == 0 || row == 4) {
					(fleet_matrix->Get(row, column))->SetType(INVADER_MEDIUM);
				} else {
					(fleet_matrix->Get(row, column))->SetType(INVADER_LOW);
				}	
			}
		}
		
		
	} else if(level == 2) {
		
		total_invaders = 40;
		horizontal_speed = 5;
		vertical_speed = 13;
		GenerateRandomLevel();
		
	} else if(level == 3) {
		
		total_invaders = 40;
		horizontal_speed = 7;
		vertical_speed = 16;
		GenerateRandomLevel();
		
	} else if(level == 4) {
		
		total_invaders = 40;
		horizontal_speed = 9;
		vertical_speed = 19;
		GenerateRandomLevel();
		
	} else if(level == 5) {
		
		total_invaders = 40;
		horizontal_speed = 11;
		vertical_speed = 22;
		GenerateRandomLevel();
		
	} else  {
		
		total_invaders = 40;
		horizontal_speed = 13;
		vertical_speed = 25;
		
		for(short row = 0, column = 0; row != FLEET_ROWS; column++) {
			
			//Reset the columns when reached the limit of the matrix
			if(column == FLEET_COLUMNS) {
				column = -1;
				row++;
			} else {
				//Revive the Invader
				(fleet_matrix->Get(row, column))->Revive();
				
				//Load the type
				(fleet_matrix->Get(row, column))->SetType(INVADER_HIGH);
			}
		}
		
	}
}

bool Fleet::StillAlive() {
	return (total_invaders > 0);
}

void Fleet::KillAnInvader() {
	total_invaders--;
}

void Fleet::GenerateRandomLevel() {
	
	for(short row = 0, column = 0; row != FLEET_ROWS; column++) {
			
		//Reset the columns when reached the limit of the matrix
		if(column == FLEET_COLUMNS) {
			column = -1;
			row++;
		} else {
			
			//Load a random level
			int random_sprite = (rand() % 3);
			int type = INVADER_LOW;
			
			if(random_sprite == 0) {
				type = INVADER_LOW;
			} else if(random_sprite == 1) {
				type = INVADER_MEDIUM;
			} else if(random_sprite == 2) {
				type = INVADER_HIGH;
			}
			
			(fleet_matrix->Get(row, column))->SetType(type);
		}
	}
}

string Fleet::GetHash() {
	
	stringstream sstr;
	
	sstr << space_definition.x << DIVISION_CHAR;
	sstr << space_definition.y << DIVISION_CHAR;
	sstr << space_definition.h << DIVISION_CHAR;
	sstr << space_definition.w << DIVISION_CHAR;
	sstr << horizontal_speed << DIVISION_CHAR;
	sstr << vertical_speed << DIVISION_CHAR;
	sstr << total_invaders << DIVISION_CHAR;

	return sstr.str();	
}
