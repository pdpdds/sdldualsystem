#ifndef _LEVEL_SLICE_H_
#define _LEVEL_SLICE_H_

#define LS_WIDTH  159
#define LS_HEIGHT 99

typedef struct LevelSlice LevelSlice;

typedef struct LevelSliceCopy {
	char data[LS_WIDTH*LS_HEIGHT];
} LevelSliceCopy;
#define LSP_LOOKUP(lsp,x,y) ((lsp).data[(y)*LS_WIDTH+(x)])

typedef enum LevelSliceQuery {
	LSQ_OPEN,
	LSQ_COLLIDE,
	LSQ_OUT_OF_BOUNDS
} LevelSliceQuery;

#include <level.h>
#include <tank.h>

/* Used by tank.c to init a LevelSlice structure: */
LevelSlice *level_slice_new(Level *lvl, Tank *t) ;
void        level_slice_free(LevelSlice *ls) ;

/* Some quick queries for use in AIs: */
LevelSliceQuery level_slice_query_point (LevelSlice *ls, int x, int y) ;
LevelSliceQuery level_slice_query_circle(LevelSlice *ls, int x, int y) ;

/* A way to copy level data into the controller, for when you NEED that kind of
 * resolution: */
void level_slice_copy(LevelSlice *ls, LevelSliceCopy *lsc) ;

#endif /* _LEVEL_SLICE_H_ */

