//Includes
#include 	"HeroShip.h"

HeroShip::HeroShip() {
	
	invisibility_counter = new SDLTimer();
	fire_regulator = new SDLTimer();
	Reset();
	
}

HeroShip::~HeroShip() {
	
}

void HeroShip::Reset() {
	
	//Initialize/Reset the hero ship attributes	
	score = 0;
	lives = 3;
	health_points = 100;
	shield = false;
	invisibility = false;
	
	//Initialize/Reset the hero graphics offset
	graphics.x = 0;
	graphics.y = 0;
	graphics.h = HERO_SPRITE_DIMENSION;
	graphics.w = HERO_SPRITE_DIMENSION;
	
	//Initialize/Reset the hero spatial definition
	space_definition.x = (WIDTH / 2) - (HERO_SPRITE_DIMENSION /2); //The screen resolution divided by 2 plus the sprite width divided by 2.
	space_definition.y = (HEIGHT - HERO_SPRITE_DIMENSION) - 100;
	space_definition.h = HERO_SPRITE_DIMENSION;
	space_definition.w = HERO_SPRITE_DIMENSION;
	
	//Initialize/Reset the sensible area rectangle
	sensible_area.h = (HERO_SPRITE_DIMENSION / 2);
	sensible_area.w = (HERO_SPRITE_DIMENSION / 2); 
	
	//Initialize/Reset the weapon
	weapon = TYPE_LASER;
	misiles = 0;
	plasmas = 0;
	fire_regulator->Start();
	
	//Reset the statistics
	ChangeLevel();
	
}

void HeroShip::ChangeLevel() {
	
	//Reset all the level dependant score variables
	invader_low_killed = 0;
	invader_medium_killed = 0;
	invader_high_killed = 0;
	kamikazes_killed = 0;
	mothership_killed = 0;
	amount_shields = 0;
	amount_invis = 0;
	amount_misiles = 0;
	amount_plasmas = 0;
	amount_times = 0;
}

void HeroShip::Move(int xAbsolute) {
	
	space_definition.x = xAbsolute;
	
	//Verify that the x location of the ship still in the visible area.
	if(space_definition.x < 0) {
		space_definition.x = 0;
	} else if(space_definition.x > (WIDTH - HERO_SPRITE_DIMENSION)) {
		space_definition.x = (WIDTH - HERO_SPRITE_DIMENSION);
	}
}

void HeroShip::Move(Object* final_gem) {
	
	if(final_gem != NULL) {
		
		//His variables
		int final_gem_x = final_gem->GetSpaceDefinition()->x;
		int final_gem_w = final_gem->GetSpaceDefinition()->w;
		
		//Our variables
		int center_x = (space_definition.x + (HERO_SPRITE_DIMENSION / 2));
		int center_y = (space_definition.y + (HERO_SPRITE_DIMENSION / 2));
		
		//Height variable
		int h = (center_y - final_gem->GetSpaceDefinition()->y); 
		
		if(h < 0) {
			h = 0;
		}
		
		if((center_x >= final_gem_x) && (center_x <= (final_gem_x + final_gem_w))) {
			//		*		Gem
			//
			//		|		Direction
			//		x		Hero Ship
			space_definition.x += 0;
			space_definition.y += -4;
		} else if((final_gem_x >= (center_x - h)) && (final_gem_x < center_x)) {
			//	  * 		Gem
			//	    		
			//	   \		Direction
			//	    x		Hero Ship
			space_definition.x += -4;
			space_definition.y += -4;
		} else if((final_gem_x > center_x) && (final_gem_x <= (center_x + h))) {
			//	      *		Gem
			//	    		 
			//	     /   	Direction 
			//	    x		Hero Ship 
			space_definition.x += 4;
			space_definition.y += -4;
		} else if(final_gem_x > (center_x + h)) {
			//	     	  * Gem
			//
			//				Direction
			//	    x--     Hero Ship
			space_definition.x += 4;
			space_definition.y += 0;
		} else if(final_gem_x < (center_x - h)) {
			//	  --*   	Kamikaze
			//	     		Direction
			//
			//x             Hero Ship
			space_definition.x += -4;
			space_definition.y += 0;
		}
	} else {
		printf("PANIC! Final gem is NULL! \n");
	}
}

bool HeroShip::Fire() {
	
	if(fire_regulator->Get_Ticks() > 300) {
		
		//Reset the timer
		fire_regulator->Start();
		
		//Check if we have a stock
		if(weapon == TYPE_MISILE) {
			
			if(misiles == 0) {
				weapon = TYPE_LASER;
			} else {
				misiles--;
			}
		} else if(weapon == TYPE_PLASMA) {
			
			if(plasmas == 0) {
				weapon = TYPE_LASER;
			} else {
				plasmas--;
			}
		}
		
		return true;
	}
	
	return false;
	
}

void HeroShip::AddWeapon(unsigned short param_weapon) {
	if(param_weapon == TYPE_MISILE) {
		misiles++;
	} else {
		plasmas++;
	}
}

void HeroShip::ToggleWeapon() {
	
	if(weapon == TYPE_LASER) {
		if(misiles > 0) {
			weapon = TYPE_MISILE;
		} else if(plasmas > 0) {
			weapon = TYPE_PLASMA;
		}
	} else if(weapon == TYPE_MISILE) {
		if(plasmas > 0) {
			weapon = TYPE_PLASMA;
		} else {
			weapon = TYPE_LASER;
		}
	} else {//if(weapon == TYPE_PLASMA)
		weapon = TYPE_LASER;
	}
}

void HeroShip::Reallocate() {
	space_definition.x = (WIDTH / 2) - (HERO_SPRITE_DIMENSION /2);
	space_definition.y = (HEIGHT - HERO_SPRITE_DIMENSION) - 100;
}

int HeroShip::GetLocation() {
	
	return space_definition.x;
	
}

int HeroShip::GetGameLine() {
	
	return space_definition.y;
	
}

int HeroShip::GetWeapon() {
	return weapon;
}

void HeroShip::SetWeapon(unsigned short param_weapon) {
	weapon = param_weapon;
}

SDL_Rect* HeroShip::GetStatus() {
	
	//Check for visibility status
	if(invisibility_counter->Is_Started()) {
		if(invisibility_counter->Get_Ticks() >= 10000) {
			invisibility = false;
			invisibility_counter->Stop();
		}
	}
	
	if(shield && invisibility) {		
		graphics.x = (HERO_SPRITE_DIMENSION * 3);
	} else if(invisibility) {
		graphics.x = (HERO_SPRITE_DIMENSION * 2);
	} else if(shield) {
		graphics.x = (HERO_SPRITE_DIMENSION * 1);
	} else {
		graphics.x = (HERO_SPRITE_DIMENSION * 0);
	}
	
	return &graphics;
}

SDL_Rect* HeroShip::GetSpaceDefinition() {
	
	return &space_definition;
}

SDL_Rect* HeroShip::GetSensibleArea() {
	
	sensible_area.x = space_definition.x + (HERO_SPRITE_DIMENSION / 4); 
	sensible_area.y = space_definition.y + (HERO_SPRITE_DIMENSION / 4);
	
	return &sensible_area;
}

//NOT_HIT if not hit (shield), HIT if hit but not dead and LIVES_LEFT if hit and dead, GAME_OVER if dead and no more lives left
int HeroShip::Hit(unsigned short type) {
	
	if(shield) {
		
		shield = false;
		return NOT_HIT;
		
	} else {
		
		if(type == TYPE_LASER) {
			health_points -= 25;
		} else if(type == TYPE_KAMIKAZE) {
			health_points = 0;
		}
		
		if(health_points == 0) {
			lives--;
			if(lives == 0) {
				return GAME_OVER;
			} else {
				health_points = 100;
				return LIVES_LEFT;
			}
		} else {
			return HIT;
		}
	}
}

void HeroShip::SetVisibility(bool status) {
	invisibility = !status;
	if(invisibility) {
		invisibility_counter->Start();
	}
}

bool HeroShip::IsInvisible() {
	return invisibility;
}

void HeroShip::SetShield(bool status) {
	shield = status;
	
}

unsigned int HeroShip::GetLives() {
	return lives;
}

void HeroShip::AddLive() {
	lives++;
}

unsigned int HeroShip::GetHP() {
	return health_points;
}

unsigned int HeroShip::GetScore() {
	return score;
}

unsigned int HeroShip::GetMisiles() {
	return misiles;
}

unsigned int HeroShip::GetPlasmas() {
	return plasmas;
}

void HeroShip::UpdateScore(unsigned int type) {
	
	switch(type) {
		case INVADER_LOW:
			invaders_killed++;
			invader_low_killed++;
			score += 20;
			break;
		case INVADER_MEDIUM:
			invaders_killed++;
			invader_medium_killed++;
			score += 40;
			break;
		case INVADER_HIGH:
			invaders_killed++;
			invader_high_killed++;
			score += 60;
			break;
		case INVADER_KAMIKAZE:
			invaders_killed++;
			kamikazes_killed++;
			score += 50;
			break;
		case INVADER_MOTHERSHIP:
			invaders_killed++;
			mothership_killed++;
			score += 100;
			break;		
		case TYPE_POWERUP_SHIELD:
			amount_shields++;
			score += 20;
			break;
		case TYPE_POWERUP_INVI:
			amount_invis++;
			score += 20;
			break;
		case TYPE_POWERUP_MISILES:
			amount_misiles++;
			score += 10;
			break;
		case TYPE_POWERUP_PLASMA:
			amount_plasmas++;
			score += 50;
			break;
		case TYPE_POWERUP_TIME:
			amount_times++;
			score += 50;
			break;
	}
}

string HeroShip::GetHash() {
	
	stringstream sstr;
	
	sstr << score << DIVISION_CHAR;
	sstr << lives << DIVISION_CHAR;
	sstr << health_points << DIVISION_CHAR;
	sstr << weapon << DIVISION_CHAR;
	sstr << misiles << DIVISION_CHAR;
	sstr << plasmas << DIVISION_CHAR;
	sstr << shield << DIVISION_CHAR;
	sstr << invisibility << DIVISION_CHAR;
	sstr << space_definition.x << DIVISION_CHAR;
	sstr << space_definition.y << DIVISION_CHAR;
	sstr << space_definition.h << DIVISION_CHAR;
	sstr << space_definition.w << DIVISION_CHAR;
	sstr << sensible_area.x << DIVISION_CHAR;
	sstr << sensible_area.y << DIVISION_CHAR;
	sstr << sensible_area.h << DIVISION_CHAR;
	sstr << sensible_area.w << DIVISION_CHAR;
	sstr << graphics.x << DIVISION_CHAR;
	sstr << graphics.y << DIVISION_CHAR;
	sstr << graphics.h << DIVISION_CHAR;
	sstr << graphics.w;

	return sstr.str();
	
}
