#ifndef _LEVEL_H_
#define _LEVEL_H_

typedef struct Level Level;

#include <drawbuffer.h>
#include <types.h>


/* (Con|De)structor: */
Level *level_new(DrawBuffer *b, unsigned w, unsigned h) ;
void   level_destroy(Level *lvl) ;

/* Exposes the level data: */
void level_set(Level *lvl, unsigned x, unsigned y, char data) ;
char level_get(Level *lvl, unsigned x, unsigned y) ;

/* Decorates a level for drawing. Should be called by level generators: */
void level_decorate(Level *lvl) ;
void level_make_bases(Level *lvl) ;

Vector level_get_spawn(Level *lvl, unsigned i);

int level_dig_hole(Level *lvl, unsigned x, unsigned y) ;

void level_draw_all(Level *lvl, DrawBuffer *b) ;
void level_draw_pixel(Level *lvl, DrawBuffer *b, unsigned x, unsigned y) ;

/* Will return a value indicating coll: */
typedef enum BaseCollision {
	BASE_COLLISION_NONE,
	BASE_COLLISION_YOURS,
	BASE_COLLISION_ENEMY
} BaseCollision;

BaseCollision level_check_base_collision(Level *lvl, unsigned x, unsigned y, unsigned color) ;

/* Dumps a decorated level into a color bmp file: */
void level_dump_bmp(Level *lvl, char *filename) ;

#endif /* _LEVEL_H_ */

