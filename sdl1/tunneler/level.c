#include <stdlib.h>

#include <level.h>
#include <memalloc.h>
#include <random.h>
#include <tweak.h>
#include <types.h>
#include <drawbuffer.h>
#include <gamelib.h>

#include <level_defn.h>


Level *level_new(DrawBuffer *b, unsigned w, unsigned h) {
	Level *lvl;
	unsigned i;
	
	lvl = malloc(sizeof(Level));
	lvl->width  = w;
	lvl->height = h;
	lvl->b = b;
	
	lvl->array = (char*)malloc( sizeof(char) * w * h );
	for(i=0; i<w*h; i++) lvl->array[i] = 1;
	
	return lvl;
}

void level_destroy(Level *lvl) {
	if(!lvl) return;
	free(lvl->array);
	free(lvl);
}

void level_set(Level *lvl, unsigned x, unsigned y, char data) {
	lvl->array[ y*lvl->width + x ] = data;
	level_draw_pixel(lvl, lvl->b, x, y);
}

char level_get(Level *lvl, unsigned x, unsigned y) {
	if(x>=lvl->width || y>=lvl->height) return ROCK;
	return lvl->array[ y*lvl->width + x ];
}

void level_decorate(Level *lvl) {
	register int x, y;
	
	for(y=0; y<lvl->height; y++)
		for(x=0; x<lvl->width; x++) {
			char *spot = &lvl->array[ y*lvl->width + x ];
			if(*spot) *spot = ROCK;
			else      *spot = rand_bool(500) ? DIRT_LO : DIRT_HI;
		}
}

static void make_base(Level *lvl, unsigned bx, unsigned by, unsigned color) {
	register int x, y;
	
	if(color >= MAX_TANKS) return;
	
	for(y=-BASE_SIZE/2; y<=BASE_SIZE/2; y++) {
		for(x=-BASE_SIZE/2; x<=BASE_SIZE/2; x++) {
			if(abs(x)==BASE_SIZE/2 || abs(y)==BASE_SIZE/2) {
				if(x>=-BASE_DOOR_SIZE/2 && x<=BASE_DOOR_SIZE/2) continue;
				lvl->array[(by+y)*lvl->width + bx+x] = BASE + color;
				continue;
			}
			lvl->array[(by+y)*lvl->width + bx+x] = BLANK;
		}
	}
}

/* TODO: Rethink the method for adding bases, as the current method DEMANDS that
 *       you use MAX_TANKS tanks. */
void level_make_bases(Level *lvl) {
	unsigned i;
	for(i=0; i<MAX_TANKS; i++)
		/*level_dig_hole(lvl, lvl->spawn[i].x, lvl->spawn[i].y);*/
		make_base(lvl, lvl->spawn[i].x, lvl->spawn[i].y, i);
}

Vector level_get_spawn(Level *lvl, unsigned i) {
	return lvl->spawn[i];
}

int level_dig_hole(Level *lvl, unsigned x, unsigned y) {
	register unsigned tx, ty;
	int did_dig = 0;
	
	for(ty=y-3; ty<=y+3; ty++)
		for(tx=x-3; tx<=x+3; tx++) {
			char oldcolor;
			
			/* Don't go out-of-bounds: */
			if(tx >= lvl->width || ty >= lvl->height) continue;
			
			/* Don't take out the corners: */
			if((tx==x-3 || tx==x+3) && (ty==y-3 || ty==y+3)) continue;
			
			/* Only take out dirt: */
			oldcolor = lvl->array[ty*lvl->width + tx];
			if(oldcolor != DIRT_HI && oldcolor != DIRT_LO) continue;
			
			level_set(lvl, tx, ty, BLANK);
			did_dig = 1;
		}
	
	return did_dig;
}

void level_draw_all(Level *lvl, DrawBuffer *b) {
	register unsigned x, y;
	unsigned color;
	
	for(y=0; y<lvl->height; y++)
		for(x=0; x<lvl->width; x++) {
			char val = lvl->array[y*lvl->width + x];
			switch(val) {
				case ROCK:    drawbuffer_set_pixel(b, x, y, color_rock); break;
				case DIRT_HI: drawbuffer_set_pixel(b, x, y, color_dirt_hi); break;
				case DIRT_LO: drawbuffer_set_pixel(b, x, y, color_dirt_lo); break;
				case BLANK:   drawbuffer_set_pixel(b, x, y, color_blank); break;
				default:
					/* Else, this is most likely a base: */
					if((color=val-BASE) < MAX_TANKS)
						drawbuffer_set_pixel(b, x, y, color_tank[color][0]); break;
			}
		}
}


void level_draw_pixel(Level *lvl, DrawBuffer *b, unsigned x, unsigned y) {
	unsigned color;
	char val = lvl->array[y*lvl->width + x];
	
	switch(val) {
		case ROCK:    drawbuffer_set_pixel(b, x, y, color_rock); break;
		case DIRT_HI: drawbuffer_set_pixel(b, x, y, color_dirt_hi); break;
		case DIRT_LO: drawbuffer_set_pixel(b, x, y, color_dirt_lo); break;
		case BLANK:   drawbuffer_set_pixel(b, x, y, color_blank); break;
		default:
			/* Else, this is most likely a base: */
			if((color=val-BASE) < MAX_TANKS)
				drawbuffer_set_pixel(b, x, y, color_tank[color][0]); break;
	}
}


/* TODO: This needs to be done in a different way, as this approach will take 
 * MAX_TANKS^2 time to do all collision checks for all tanks. It should only
 * take MAX_TANKS time. */
BaseCollision level_check_base_collision(Level *lvl, unsigned x, unsigned y, unsigned color) {
	unsigned id;
	
	for(id=0; id < MAX_TANKS; id++) {
		if(abs((long)(lvl->spawn[id].x - x)) < BASE_SIZE/2 && abs((long)(lvl->spawn[id].y - y)) < BASE_SIZE/2) {
			if(id == color)
				return BASE_COLLISION_YOURS;
			return BASE_COLLISION_ENEMY;
		}
	}
	
	return BASE_COLLISION_NONE;
}


/* Dumps a level into a BMP file: */
void level_dump_bmp(Level *lvl, char *filename) {
	unsigned x, y;
	BMPFile *f = gamelib_bmp_new(lvl->width, lvl->height);
	
	for(y=0; y<lvl->height; y++)
		for(x=0; x<lvl->width; x++) {
			Color color = COLOR( 0,0,0 );
			
			char val = lvl->array[y*lvl->width + x];
			
			if     (val == DIRT_HI) color = color_dirt_hi;
			else if(val == DIRT_LO) color = color_dirt_lo;
			else if(val == ROCK)    color = color_rock;
			else if(val == BLANK)   color = color_blank;
			else if(val-BASE < MAX_TANKS && val-BASE >= 0)
				color = color_tank[val-BASE][0];
			
			gamelib_bmp_set_pixel(f, x, y, color);
		}
	
	gamelib_bmp_finalize(f, filename);
}

