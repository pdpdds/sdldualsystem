#include <stdio.h>
#include <stdlib.h>

#include <projectile.h>
#include <level.h>
#include <random.h>
#include <tank.h>
#include <tweak.h>
#include <memalloc.h>
#include <drawbuffer.h>
#include <tanklist.h>


struct Projectile {
	unsigned x, y;       /* The x,y of the 'hot' portion.  (#ff3408) */
	unsigned oldx, oldy; /* The x,y of the 'cold' portion. (#ba0000) */
	
	int      xstep, ystep;
	unsigned life  : 7;
	
	unsigned is_effect : 1;
	
	struct Tank *tank;
};

struct PListNode {
	Projectile p;
	struct PListNode *prev, *next;
	struct PList     *parent;
};

struct PList {
	PListNode *active;
	PListNode *dead;
};


/* Register a new plistnode object: */
static PListNode *plistnode_new(PList *parent, Projectile payload) {
	PListNode *n = malloc(sizeof(PListNode));
	n->p = payload; n->parent = parent;
	n->prev = n->next = NULL;
	return n;
}


/* Push a new projectile, and return a pointer to it: */
static Projectile *plist_activate(PList *pl) {
	PListNode *n;
	Projectile payload = {0, 0, 0, 0, 0, 0, 0, 0, NULL};
	
	/* If there is something in the 'dead' list... */
	if(pl->dead) {
		/* ... then we pull an item from it: */
		n = pl->dead;
		if(n->next) n->next->prev = NULL;
		pl->dead = n->next;
		n->prev = n->next = NULL;
		n->p = payload;
	
	/* Else, we just make a new one: */
	} else n = plistnode_new(pl, payload);
	
	/* Push p onto the 'active' list: */
	if(pl->active) pl->active->prev = n;
	n->next = pl->active;
	pl->active = n;
	
	/* Return a pointer to the Projectile structure: */
	return &n->p;
}


/* Remove an object from the 'active' set, and push it into the 'dead' set: */
void plistnode_remove(PListNode *n) {
	PList *pl = n->parent;
	
	/* Drop from the 'active' list: */
	if(n->prev) n->prev->next = n->next;
	else        pl->active = n->next;
	if(n->next) n->next->prev = n->prev;
	
	/* Push to the 'dead' list: */
	n->prev = NULL;
	n->next = n->parent->dead;
	if(n->parent->dead) n->parent->dead->prev = n;
	n->parent->dead = n;
}


PList *plist_new() {
	unsigned i;
	PList *out;
	
	out = malloc(sizeof(PList));
	out->active = out->dead = NULL;
	
	/* Throw in a bunch of dead objects: */
	for(i=0; i<PROJECTILE_BUFFER_START_SIZE; i++) {
		Projectile p = {0, 0, 0, 0, 0, 0, 0, 0, NULL};
		PListNode *n = plistnode_new(out, p);
		
		if(out->dead) out->dead->prev = n;
		n->next = out->dead;
		out->dead = n;
	}
	
	return out;
}


void plist_destroy(PList *pl) {
	PListNode *n, *next;
	
	if(!pl) return;
	
	/* Free the active list: */
	next = pl->active;
	while((n = next)) {
		next = n->next;
		free(n);
	}
	
	/* Free the dead list: */
	next = pl->dead;
	while((n = next)) {
		next = n->next;
		free(n);
	}
	
	free(pl);
}


void plist_push_bullet(PList *pl, Tank *t) {
	Projectile payload;
	unsigned dir;
	
	tank_get_position(t, &payload.x, &payload.y);
	payload.oldx = payload.x; payload.oldy = payload.y;

	dir = tank_get_dir(t);
	payload.xstep = dir % 3 - 1;
	payload.ystep = dir / 3 - 1;
	payload.life = TANK_BULLET_SPEED;
	payload.is_effect = 0;
	payload.tank = t;
	
	*plist_activate(pl) = payload;
}


void plist_push_explosion(PList *pl, unsigned x, unsigned y, unsigned count, unsigned r, unsigned ttl) {
	unsigned i;
	
	/* Add all of the effect particles: */
	for(i=0; i<count; i++) {
		Projectile p = { x*16+8, y*16+8, x, y,
			rand_int(0,r)-r/2, rand_int(0,r)-r/2, rand_int(0,ttl), 1, NULL};
		*plist_activate(pl) = p;
	}
}


/* Step up the animation: */
void plist_step(PList *pl, Level *lvl, TankList *tl) {
	PListNode *n, *next;
	char c;
	
	/* Iterate over the living objects: */
	next = pl->active;
	while((n = next)) {
		
		/* Store the next value immediately, just in case this object moves: */
		next = n->next;
		
		/* Is this a bullet, or an effect? */
		if(n->p.is_effect) {
			unsigned x, y;
			
			/* Effect: */
			
			/* Did this expire? */
			if(!n->p.life) {
				plistnode_remove(n);
				continue;
			}
			
			/* Move the effect: */
			n->p.life--;
			n->p.x += n->p.xstep; n->p.y += n->p.ystep;
			x = n->p.x/16; y = n->p.y/16;
			
			/* Make sure we didn't hit a level detail: */
			c = level_get(lvl, x, y);
			if(c!=DIRT_HI && c!= DIRT_LO && c != BLANK) {
				plistnode_remove(n);
				continue;
			}
			
			/* Effects blank everything out in their paths: */
			level_set(lvl, x, y, BLANK);
			
		} else {
			unsigned i;
			
			/* Bullet: */
			
			for(i=0; i<n->p.life; i++) {
				Tank *t;
				unsigned clr;
				
				n->p.oldx = n->p.x;     n->p.oldy = n->p.y;
				n->p.x   += n->p.xstep; n->p.y   += n->p.ystep;
				
				/* Did we hit another tank? */
				clr = tank_get_color(n->p.tank);
				t = tanklist_check_point(tl, n->p.x, n->p.y, clr);
				if(t) {
					/* If we have an associated tank, return the shot: */
					tank_return_bullet(n->p.tank);
					
					/* Hurt the tank we hit: */
					tank_alter_health(t, TANK_SHOT_DAMAGE);
					
					/* Add all of the effect particles: */
					plist_push_explosion(pl, n->p.oldx, n->p.oldy,
						EXPLOSION_HURT_COUNT,
						EXPLOSION_HURT_RADIUS,
						EXPLOSION_HURT_TTL);
					
					/* Finally, remove it: */
					plistnode_remove(n);
					
					break;
				}
				
				/* Else, did we hit something in the level? */
				c = level_get(lvl, n->p.x, n->p.y);
				if(c != BLANK) {
					/* If we have an associated tank, return the shot: */
					tank_return_bullet(n->p.tank);
					
					/* Add all of the effect particles: */
					plist_push_explosion(pl, n->p.oldx, n->p.oldy,
						EXPLOSION_DIRT_COUNT,
						EXPLOSION_DIRT_RADIUS,
						EXPLOSION_DIRT_TTL);
					
					/* Finally, remove it: */
					plistnode_remove(n);
					
					break;
				}
			}
		}
	}
}

void plist_clear(PList *pl, DrawBuffer *b) {
	PListNode *n, *next;
	
	next = pl->active;
	while((n = next)) {
		next = n->next;
		
		if(n->p.is_effect)
			drawbuffer_set_pixel(b, n->p.x/16, n->p.y/16, color_blank);
		else {
			drawbuffer_set_pixel(b, n->p.x, n->p.y, color_blank);
			drawbuffer_set_pixel(b, n->p.oldx, n->p.oldy, color_blank);
		}
	}
}

void plist_draw(PList *pl, DrawBuffer *b) {
	PListNode *n, *next;
	
	next = pl->active;
	while((n = next)) {
		next = n->next;
		
		if(n->p.is_effect)
			drawbuffer_set_pixel(b, n->p.x/16, n->p.y/16, color_fire_hot);
		else {
			drawbuffer_set_pixel(b, n->p.x, n->p.y, color_fire_hot);
			drawbuffer_set_pixel(b, n->p.oldx, n->p.oldy, color_fire_cold);
		}
	}
}

