#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <gamelib.h>
#include <levelgen.h>
#include <levelgenutil.h>
#include <level.h>
#include <memalloc.h>
#include <random.h>
#include <queue.h>

#include <level_defn.h>


/* Configuration Constants: */
#define BORDER    30
#define FILTER    70
#define ODDS      300
#define FILLRATIO 65
#define TREESIZE  150

typedef struct Pairing {
	unsigned dist, a, b;
} Pairing;

#ifdef _TESTING
static void level_draw_ascii(Level *lvl) {
	unsigned x,y;
	
	for(y=0; y<lvl->height; y++) {
		for(x=0; x<lvl->width; x++)
			printf("%c", lvl->array[ y*lvl->width + x ]?'#':' ');
		printf("\n");
	}
}
#endif /* _TESTING */

/*----------------------------------------------------------------------------*
 * STAGE 1: Generate a random tree                                            *
 *----------------------------------------------------------------------------*/

static int pairing_cmp(const void *a, const void *b) {
	return ((Pairing *)a)->dist - ((Pairing *)b)->dist;
}

static void generate_tree(Level *lvl) {
	unsigned *dsets, paircount;
	register unsigned i, j, k;
	Vector *points;
	Pairing *pairs;
	
	/* Get an array of disjoint set IDs: */
	dsets = malloc( sizeof(unsigned) * TREESIZE );
	for(i=0; i<TREESIZE; i++) dsets[i] = i;
	
	/* Randomly generate all points: */
	points = malloc( sizeof(Vector) * TREESIZE );
	for(i=0; i<TREESIZE; i++) points[i] = pt_rand(lvl->width, lvl->height, BORDER);
	
	/* While we're here, copy in some of those points: */
	lvl->spawn[0] = points[0];
	for(i=1,j=1; i<TREESIZE && j<MAX_TANKS; i++) {
		for(k=0; k<j; k++) {
			if(pt_dist(points[i],lvl->spawn[k]) < MIN_SPAWN_DIST*MIN_SPAWN_DIST)
				break;
		}
		
		if(k!=j) continue;
		lvl->spawn[j++] = points[i];
	}
	if(j!=MAX_TANKS) {
		/* TODO: More robust error handling. */
		gamelib_error("OH SHUCKS OH SHUCKS OH SHUCKS\n");
		exit(1);
	}
	/* Get an array of all point-point pairings: */
	paircount = TREESIZE*(TREESIZE-1) / 2;
	pairs = malloc( sizeof(Pairing) * paircount );

	/* Set up all the pairs, and sort them: */
	for (k = i = 0; i < TREESIZE; i++)
		for (j = i + 1; j < TREESIZE; j++, k++) {
			pairs[k].a = i; pairs[k].b = j;
			pairs[k].dist = pt_dist(points[i], points[j]);
		}
	qsort(pairs, paircount, sizeof(Pairing), pairing_cmp);

	for (i = j = 0; i < paircount; i++) {
		unsigned aset, bset;

		/* Trees only have |n|-1 edges, so call it quits if we've selected that
		 * many: */
		if(j>=TREESIZE-1) break;
		
		aset = dsets[pairs[i].a]; bset = dsets[pairs[i].b];
		if(aset == bset) continue;
		
		/* Else, these points are in different disjoint sets. "Join" them by
		 * drawing them, and merging the two sets: */
		j+=1;
		for(k=0; k<TREESIZE; k++) if(dsets[k] == bset) dsets[k] = aset;
		draw_line(lvl, points[pairs[i].a], points[pairs[i].b], 0, 0);
	}
	
	/* We don't need this data anymore: */
	free(pairs);
	free(points);
	free(dsets);
}


/*----------------------------------------------------------------------------*
 * STAGE 2: Randomly expand upon the tree                                     *
 *----------------------------------------------------------------------------*/

/* Some cast-to-int tricks here may be fun... ;) */
static int has_neighbor(Level *lvl, unsigned x, unsigned y) {
	if(!lvl->array[ (y-1)*lvl->width + (x-1) ]) return 1;
	if(!lvl->array[ (y-1)*lvl->width + (x  ) ]) return 1;
	if(!lvl->array[ (y-1)*lvl->width + (x+1) ]) return 1;
	if(!lvl->array[ (y  )*lvl->width + (x-1) ]) return 1;
	if(!lvl->array[ (y  )*lvl->width + (x+1) ]) return 1;
	if(!lvl->array[ (y+1)*lvl->width + (x-1) ]) return 1;
	if(!lvl->array[ (y+1)*lvl->width + (x  ) ]) return 1;
	if(!lvl->array[ (y+1)*lvl->width + (x+1) ]) return 1;
	return 0;
}

static void set_outside(Level *lvl, char val) {
	register unsigned i;
	unsigned w = lvl->width, h = lvl->height;
	
	for(i=0; i<w; i++)   lvl->array[i]           = val;
	for(i=0; i<w; i++)   lvl->array[(h-1)*w + i] = val;
	for(i=1; i<h-1; i++) lvl->array[i*w]         = val;
	for(i=1; i<h-1; i++) lvl->array[i*w + w - 1] = val;
}

static void expand_init(Level *lvl, Queue *q) {
	register unsigned x, y;
	
	for(y=1; y<lvl->height-1; y++)
		for(x=1; x<lvl->width-1; x++)
			if(lvl->array[y*lvl->width+x] && has_neighbor(lvl, x, y)) {
				lvl->array[y*lvl->width+x] = 2;
				queue_enqueue(q, &(Vector){x,y});
			}
}

#define MIN2(a,b)   ((a<b) ? a : b)
#define MIN3(a,b,c) ((a<b) ? a : (b<c) ? b : c)
static unsigned expand_once(Level *lvl, Queue *q) {
	Vector temp;
	unsigned i, j, total, count = 0;
	
	total = queue_length(q);
	for(i=0; i<total; i++) {
		unsigned xodds, yodds, odds;
		
		temp = queue_dequeue(q);
		
		xodds = ODDS * MIN2(lvl->width - temp.x,  temp.x) / FILTER;
		yodds = ODDS * MIN2(lvl->height - temp.y, temp.y) / FILTER;
		odds  = MIN3(xodds, yodds, ODDS);
		
		if(rand_bool(odds)) {
			lvl->array[ temp.y*lvl->width + temp.x ] = 0;
			count++;
			
			/* Now, queue up any neighbors that qualify: */
			for(j=0; j<9; j++) {
				char *c;
				unsigned tx, ty;
				
				if(j==4) continue;
				
				tx = temp.x + (j%3) - 1; ty = temp.y + (j/3) - 1;
				c = &lvl->array[ty*lvl->width + tx];
				if(*c == 1) {
					*c = 2;
					queue_enqueue(q, &(Vector){tx, ty});
				}
			}
		} else
			queue_enqueue(q, &temp);
	}
	return count;
}

static void expand_cleanup(Level *lvl) {
	register unsigned x, y;
	
	for(y=0; y<lvl->height; y++)
		for(x=0; x<lvl->width; x++)
			lvl->array[y*lvl->width+x] = !! lvl->array[y*lvl->width+x];
}

static void randomly_expand(Level *lvl) {
	unsigned cur = 0, goal = lvl->width * lvl->height * FILLRATIO / 100;
	Queue *q;
	
	/* Experimentally, the queue never grew to larger than 3/50ths of the level
	 * size, so we can use that to save quite a bit of memory: */
	q = queue_new(lvl->width * lvl->height * 3 / 50);
	
	expand_init(lvl, q);
	while( (cur += expand_once(lvl, q)) < goal );
	expand_cleanup(lvl);
	
	queue_destroy(q);
}


/*----------------------------------------------------------------------------*
 * STAGE 3: Smooth out the graph with a cellular automaton                    *
 *----------------------------------------------------------------------------*/

static int count_neighbors(Level *lvl, unsigned x, unsigned y) {
	unsigned w = lvl->width;
	return lvl->array[ (y-1)*w + (x-1) ] +
	       lvl->array[ (y-1)*w + (x  ) ] +
	       lvl->array[ (y-1)*w + (x+1) ] +
	       lvl->array[ (y  )*w + (x-1) ] +
	       lvl->array[ (y  )*w + (x+1) ] +
	       lvl->array[ (y+1)*w + (x-1) ] +
	       lvl->array[ (y+1)*w + (x  ) ] +
	       lvl->array[ (y+1)*w + (x+1) ];
}

#define MIN2(a,b)   ((a<b) ? a : b)
#define MIN3(a,b,c) ((a<b) ? a : (b<c) ? b : c)
static unsigned smooth_once(Level *lvl) {
	register unsigned x, y, count = 0;
	
	for(y=1; y<lvl->height-1; y++)
		for(x=1; x<lvl->width-1; x++) {
			int n;
			char oldbit = lvl->array[ y*lvl->width + x ];
			
			n = count_neighbors(lvl, x, y);
			lvl->array[ y*lvl->width + x ] = oldbit ? (n>=3) : (n>4);
			
			count += lvl->array[ y*lvl->width + x ] != oldbit;
		}
	return count;
}

static void smooth_cavern(Level *lvl) {
	set_outside(lvl, 0);
	while(smooth_once(lvl));
	set_outside(lvl, 1);
}


/*----------------------------------------------------------------------------*
 * MAIN FUNCTIONS:                                                            *
 *----------------------------------------------------------------------------*/

void toast_generator(Level *lvl) {
	generate_tree(lvl);
	randomly_expand(lvl);
	smooth_cavern(lvl);
}

#ifdef _TESTING

int main() {
	clock_t t, all;
	Level lvl;
	unsigned i;
	
	rand_seed();
	
	/* We don't need a full-fledged Level object, so let's just half-ass one: */
	lvl.width = 1000; lvl.height = 500;
	lvl.array = malloc(sizeof(char) * lvl.width * lvl.height);
	for(i=0; i<lvl.width * lvl.height; i++) lvl.array[i] = 1;
	
	TIMER_START(all);
	TIMER_START(t);
	generate_tree(&lvl);
	TIMER_STOP(t);
	
	TIMER_START(t);
	randomly_expand(&lvl);
	TIMER_STOP(t);
	
	TIMER_START(t);
	smooth_cavern(&lvl);
	TIMER_STOP(t);
	
	printf("-----------\nTotal: ");
	TIMER_STOP(all);
	
	level_draw_ascii(&lvl);
	
	free_mem(lvl.array);
	
	print_mem_stats();
	return 0;
}
#endif /* _TESTING */

