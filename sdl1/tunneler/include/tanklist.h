#ifndef _TANK_LIST_H_
#define _TANK_LIST_H_

typedef struct TankList TankList;

#include <tank.h>
#include <types.h>

#define tanklist_map(tl,func) do {\
	unsigned i;\
	for(i=0; i<MAX_TANKS; i++) {\
		Tank *t = tanklist_get(tl, i);\
		if(!t) continue;\
		func;\
	}\
} while(0)

TankList *tanklist_new(Level *lvl, PList *pl) ;
void tanklist_destroy(TankList *tl) ;

Tank *tanklist_add_tank(TankList *tl, unsigned id, Vector p) ;
int   tanklist_remove_tank(TankList *tl, unsigned id) ;

Tank *tanklist_check_point(TankList *tl, unsigned x, unsigned y, unsigned ignored) ;
int tanklist_check_collision(TankList *tl, Vector p, unsigned dir, unsigned ignored) ;

Tank *tanklist_get(TankList *tl, unsigned id) ;

#endif /* _TANK_LIST_H_ */

