#include <stdlib.h>

#include <tanklist.h>
#include <tank.h>
#include <types.h>
#include <memalloc.h>
#include <level.h>
#include <projectile.h>
#include <tweak.h>
#include <tanksprites.h>


struct TankList {
	Tank  *list[MAX_TANKS];
	Level *lvl;
	PList *pl;
};


TankList *tanklist_new(Level *lvl, PList *pl) {
	unsigned i;
	//TankList *out = get_object(TankList);
	TankList *out = malloc(sizeof(TankList));
	out->lvl = lvl; out->pl = pl;
	for(i=0; i<MAX_TANKS; i++) out->list[i] = NULL;
	return out;
}

void tanklist_destroy(TankList *tl) {
	unsigned i;
	for(i=0; i<MAX_TANKS; i++) tank_destroy(tl->list[i]);
	free(tl);
}


Tank *tanklist_add_tank(TankList *tl, unsigned id, Vector p) {
	if(id >= MAX_TANKS) return NULL;
	if(tl->list[id]) return NULL;
	tl->list[id] = tank_new(tl->lvl, tl->pl, p.x, p.y, id);
	return tl->list[id];
}

int tanklist_remove_tank(TankList *tl, unsigned id) {
	if(id >= MAX_TANKS) return 1;
	free(tl->list[id]);
	tl->list[id] = NULL;
	return 0;
}

Tank *tanklist_check_point(TankList *tl, unsigned x, unsigned y, unsigned ignored) {
	unsigned i;
	for(i=0; i<MAX_TANKS; i++) {
		unsigned tx, ty;
		if(i==ignored || !tl->list[i] || tank_is_dead(tl->list[i])) continue;
		
		tank_get_position(tl->list[i], &tx, &ty);
		tx = x - tx + 3; ty = y - ty + 3;
		if(tx > 6 || ty > 6) continue;
		
		if(TANK_SPRITE[ tank_get_dir(tl->list[i]) ][ty][tx])
			return tl->list[i];
	}
	return NULL;
}

/* Note: change that vector to two unsigned's eventually... */
int tanklist_check_collision(TankList *tl, Vector p, unsigned pdir, unsigned ignored) {
	unsigned i;
	for(i=0; i<MAX_TANKS; i++) {
		unsigned x, y, dir, tx, ty, lx, ly, ux, uy;
		
		if( i==ignored || !tl->list[i] || tank_is_dead(tl->list[i]) ) continue;
		
		/* Let's see if these two tanks are ANYWHERE near each other: */
		tank_get_position(tl->list[i], &x, &y);
		if(abs(p.x-x)>6 || abs(p.y-y)>6) continue;
		
		/* Ok, if we're here, the two tanks are real close. Now it's time for
		 * brute-force pixel checking: */
		dir = tank_get_dir(tl->list[i]);
		
		/* Find the bounds of the two sprite's overlap: */
		if(x<p.x) { lx=p.x-3; ux=x+3;   }
		else      { lx=x-3;   ux=p.x+3; }
		if(y<p.y) { ly=p.y-3; uy=y+3;   }
		else      { ly=y-3;   uy=p.y+3; }
		
		/* Check the overlap for collisions: */
		for(ty=ly; ty<=uy; ty++)
			for(tx=lx; tx<=ux; tx++)
				if(TANK_SPRITE[dir] [ty-y+3]  [tx-x+3] &&
				   TANK_SPRITE[pdir][ty-p.y+3][tx-p.x+3])
					return 1;
	}
	
	return 0;
}

/* This shouldn't be needed, in theory, but that theory is a cruel mistress: */
Tank *tanklist_get(TankList *tl, unsigned id) {
	if(id >= MAX_TANKS) return NULL;
	return tl->list[id];
}

