#ifndef _TANK_H_
#define _TANK_H_


typedef struct Tank Tank;

/* For the controllers/AIs: */
#include <levelslice.h>

/* Put inside a structure, so we are protected from casual AI cheating: */
typedef struct PublicTankInfo {
	unsigned health, energy;
	int x, y; /* relative from home base */
	LevelSlice *slice;
} PublicTankInfo;

typedef void (*TankController)(PublicTankInfo *, void *, int *, int *, unsigned *) ;


#include <level.h>
#include <screen.h>
#include <drawbuffer.h>
#include <projectile.h>
#include <tanklist.h>

Tank *tank_new(Level *lvl, PList *pl, unsigned x, unsigned y, unsigned color) ;
void tank_destroy(Tank *t) ;

unsigned tank_get_color(Tank *t) ;

unsigned tank_get_dir(Tank *t) ;
void tank_get_stats(Tank *t, unsigned *energy, unsigned *health) ;
void tank_get_position(Tank *t, unsigned *x, unsigned *y) ;
void tank_move(Tank *t, TankList *tl) ;
void tank_try_base_heal(Tank *t) ;

void tank_clear(Tank *t, DrawBuffer *b) ;
void tank_draw(Tank *t, DrawBuffer *b) ;

void tank_return_bullet(Tank *t) ;
void tank_alter_energy(Tank *t, int diff) ;
void tank_alter_health(Tank *t, int diff) ;
void tank_trigger_explosion(Tank *t) ;

void tank_set_controller(Tank *t, TankController func, void *data) ;

int tank_is_dead(Tank *t) ;

#endif /* _TANK_H_ */

