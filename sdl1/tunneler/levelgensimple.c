#include <stdio.h>
#include <math.h>

#include <level.h>
#include <levelgensimple.h>
#include <levelgenutil.h>
#include <random.h>
#include <types.h>

#include <level_defn.h>


#define BORDER         80
#define STEP_MIN       2       
#define STEP_MAX       8
#define MAX_SLOPE      100
#define RARE_SLOPE     600
#define RARITY         40


/* This just adds random points for now: */
typedef enum Side {
	SIDE_TOP    = 2,
	SIDE_RIGHT  = 3,
	SIDE_BOTTOM = 4,
	SIDE_LEFT   = 5
} Side;


static void add_rock_lines(Level *lvl, Side s) {
	int xstep = 0, ystep = 0;
	Vector cur, prev;
	int is_rare, needs_flip = 0;
	unsigned x, y, minx = 0, maxx = 0, miny = 0, maxy = 0;
	
	/* Configuration based on what side the rock is on: */
	if(s == SIDE_TOP) {
		minx = 0; maxx = lvl->width - 1;
		miny = 0; maxy = BORDER;
	} else if(s == SIDE_BOTTOM) {
		minx = 0; maxx = lvl->width - 1;
		miny = lvl->height - BORDER - 1; maxy = lvl->height - 1;
	} else if(s == SIDE_LEFT) {
		minx = 0; maxx = lvl->height - 1;
		miny = 0; maxy = BORDER;
		needs_flip = 1;
	} else if(s == SIDE_RIGHT) {
		minx = 0; maxx = lvl->height - 1;
		miny = lvl->width - BORDER - 1; maxy = lvl->width - 1;
		needs_flip = 1;
	}
	
	/* Let's get this party started: */
	prev = cur = VECTOR(minx, rand_int(miny, maxy));
	
	do {
		/* Advance the current x position so it doesn't go over the edge: */
		cur.x += (xstep = rand_int(STEP_MIN, STEP_MAX));
		if(cur.x > maxx) {
			xstep = cur.x - maxx;
			cur.x = maxx;
		}
		
		/* Advance the y position so that it is within bounds: */
		is_rare = rand_bool(RARITY);
		do {
			int slope = is_rare ? RARE_SLOPE : MAX_SLOPE;
			ystep =  (rand_int(0, slope*2) - slope) * xstep;
			ystep /= 100;
		} while( (cur.y + ystep) < miny || (cur.y + ystep) > maxy );
		
		cur.y += ystep;
		
		/* Draw this in whatever way is needed: */
		if(needs_flip)
			draw_line(lvl, VECTOR(cur.y, cur.x), VECTOR(prev.y, prev.x), s, 0);
		else
			draw_line(lvl, cur, prev, s, 0);
		
		prev = cur;
		
	} while(cur.x != maxx);
	
	/* Do the correct fill sequence, based on side: */
	#define _LOOKUP_ ((lvl)->array[(y)*(lvl)->width + (x)])
	
	if(s == SIDE_TOP)
		for(x=0; x<lvl->width; x++)
			for(y=0; _LOOKUP_ != s; y++) _LOOKUP_ = 1;
	
	else if(s == SIDE_RIGHT)
		for(y=0; y<lvl->height; y++)
			for(x=lvl->width-1; _LOOKUP_ != s; x--) _LOOKUP_ = 1;
	
	else if(s == SIDE_BOTTOM)
		for(x=0; x<lvl->width; x++)
			for(y=lvl->height-1; _LOOKUP_ != s; y--) _LOOKUP_ = 1;
	
	else if(s == SIDE_LEFT)
		for(y=0; y<lvl->height; y++)
			for(x=0; _LOOKUP_ != s; x++) _LOOKUP_ = 1;
}
#undef _LOOKUP_


static void add_spawns(Level *lvl) {
	unsigned i, j;
	
	lvl->spawn[0] = pt_rand(lvl->width, lvl->height, BORDER);
	
	for(i=1; i<MAX_TANKS; i++) {
		int done = 0;
		while(!done) {
			/* Try adding a new point: */
			lvl->spawn[i] = pt_rand(lvl->width, lvl->height, BORDER);
			
			/* Make sure that point isn't too close to others: */
			for(j=0; j<i; j++) {
				if(pt_dist(lvl->spawn[i],lvl->spawn[j]) < MIN_SPAWN_DIST*MIN_SPAWN_DIST)
					break;
			}
			
			/* We're done if we were able to get through that list: */
			done = (j == i);
		}
	}
}

void simple_generator(Level *lvl) {
	/* Levels default to all rock. Set this to all dirt: */
	fill_all(lvl, 0);
	
	/* Add rock walls on all sides: */
	add_rock_lines(lvl, SIDE_TOP);
	add_rock_lines(lvl, SIDE_BOTTOM);
	add_rock_lines(lvl, SIDE_LEFT);
	add_rock_lines(lvl, SIDE_RIGHT);
	
	/* Rough it up a little: */
	rough_up(lvl);
	
	/* Add a few spawns, and we're good to go! */
	add_spawns(lvl);
}

