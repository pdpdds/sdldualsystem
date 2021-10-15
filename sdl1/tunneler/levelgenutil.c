#include <stdlib.h>

#include <levelgenutil.h>
#include <level.h>
#include <types.h>
#include <random.h>

#include <level_defn.h>


void fill_all(Level *lvl, char c) {
	register unsigned i;
	
	for(i=0; i<lvl->width * lvl->height; i++) {
		lvl->array[i] = c;
	}
}

void rough_up(Level *lvl) {
	unsigned x, y;
	
	/* Sanitize our input: */
	for(x=0; x<lvl->width * lvl->height; x++)
		lvl->array[x] = !!lvl->array[x];
	
	/* Mark all spots that are blank, but next to spots that are marked: */
	for(x=0; x<lvl->width; x++) {
		for(y=0; y<lvl->height; y++) {
			unsigned t = 0;
			
			if(lvl->array[y*lvl->width + x]) continue;
			
			t += (x!=0            ) && lvl->array[y*lvl->width + x - 1] == 1;
			t += (x!=lvl->width-1 ) && lvl->array[y*lvl->width + x + 1] == 1;
			t += (y!=0            ) && lvl->array[(y-1)*lvl->width + x] == 1;
			t += (y!=lvl->height-1) && lvl->array[(y+1)*lvl->width + x] == 1;

			if(t) lvl->array[y*lvl->width + x] = 2;
		}
	}
	
	/* For every marked spot, randomly fill it: */
	for(x=0; x<lvl->width * lvl->height; x++)
		if(lvl->array[x] == 2)
			lvl->array[x] = rand_bool(500);
}

Vector pt_rand(unsigned w, unsigned h, unsigned border) {
	Vector out;
	out.x = rand_int(border, w - border);
	out.y = rand_int(border, h - border);
	return out;
}

/* Actually returns the distance^2, but points should still remain in the same
 * order, and this doesn't require a call to sqrt(): */
unsigned pt_dist(Vector a, Vector b) {
	return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y);
}

/* Used for point drawing: */
static void set_point(Level *lvl, unsigned x, unsigned y, char value) {
	if(x >= lvl->width || y >= lvl->height) return;
	lvl->array[y*lvl->width+x] = value;
}

void set_circle(Level *lvl, unsigned x, unsigned y, char value) {
	register int tx, ty;
	for(ty=-3; ty<=3; ty++) {
		for(tx=-3; tx<=3; tx++) {
			if((tx==-3 || tx==3) && (ty==-3 || ty==3)) continue;
			set_point(lvl, x+tx, y+ty, value);
		}
	}
}

#define SWAP(type,a,b) do { type t = (a); (a)=(b); (b)=t; } while(0)

/* New Bresenham's Algorithm-based function: */

void draw_line(Level *dest, Vector a, Vector b, char value, int fat_line) {
	int swap, dx, dy, error, stepy;
	register unsigned x, y;
	void (*pt_func)(Level *, unsigned, unsigned, char) ;
	
	/* How is this thing getting drawn? */
	pt_func = (fat_line) ? set_circle : set_point;
	
	/* Swap x and y values when the graph gets too steep to operate normally: */
	if((swap = abs(b.y - a.y) > abs(b.x - a.x))) {
		SWAP(unsigned, a.x, a.y);
		SWAP(unsigned, b.x, b.y);
	}

	/* Swap a and b so that a is to the left of b: */
	if(a.x > b.x) SWAP(Vector, a, b);

	/* A few calculations: */
	dx = b.x - a.x;
	dy = abs(b.y - a.y);
	error = dx / 2;
	stepy = (a.y < b.y) ? 1 : -1;
	
	/* Now, for every x from a.x to b.x, add the correct dot: */
	for (x = a.x, y=a.y; x <= b.x; x++) {
		if(swap) pt_func(dest, y, x, value);
		else     pt_func(dest, x, y, value);

		error -= dy;
		if(error < 0) {
			y     += stepy;
			error += dx;
		}
	}
}


/* Original DDA-based function:

#define ROUND(x) ((unsigned)((x)+0.5))
#define SWAP(a,b) do { Vector t = (a); (a)=(b); (b)=t; } while(0)

static void draw_line(Level *dest, Vector a, Vector b) {
	double x, y, dx, dy, stepx, stepy;
	
	dx = (double)a.x - (double)b.x;
	dy = (double)a.y - (double)b.y;
	
	if(dx==0 && dy==0) {
		set_circle(dest->array, dest->width, a.x, a.y);
		return;
	
	} else if(dx==0 || abs(dy)>abs(dx)) {
		if(a.y > b.y) SWAP(a,b);
		stepx = dx / dy; stepy = 1;
	 
	} else {
		if(a.x > b.x) SWAP(a,b);
		stepx = 1; stepy = dy / dx;
	}
	
	x = a.x; y = a.y;
	while ((stepx==1 && x<=b.x) || (stepy==1 && y<=b.y)) {
		set_circle(dest->array, dest->width, ROUND(x), ROUND(y));
		x += stepx; y += stepy;
	}
}
*/

