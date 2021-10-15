#include <stdlib.h>

#include <level.h>
#include <levelslice.h>
#include <memalloc.h>
#include <tweak.h>
#include <tank.h>


struct LevelSlice {
	Tank  *t;
	Level *lvl;
};


LevelSlice *level_slice_new(Level *lvl, Tank *t) {
	LevelSlice *ls;
	
	ls = malloc(sizeof(LevelSlice));
	ls->t = t; ls->lvl = lvl;
	
	return ls;
}


void level_slice_free(LevelSlice *ls) {
	free(ls);
}


LevelSliceQuery level_slice_query_point(LevelSlice *ls, int x, int y) {
	unsigned tx, ty;
	char c;
	
	tank_get_position(ls->t, &tx, &ty);
	
	if(abs(x) >= LS_WIDTH/2 || abs(y) >= LS_HEIGHT/2) return LSQ_OUT_OF_BOUNDS;
	c = level_get(ls->lvl, tx+x, ty+y);

	if(c==DIRT_HI || c==DIRT_LO || c==BLANK) return LSQ_OPEN;
	return LSQ_COLLIDE;
}


LevelSliceQuery level_slice_query_circle(LevelSlice *ls, int x, int y) {
	int dx, dy;
	
	for(dy=y-3; dy<=y+3; dy++)
		for(dx=x-3; dx<=x+3; dx++) {
			LevelSliceQuery out;
			
			/* Don't take out the corners: */
			if((dx==x-3 || dx==x+3) && (dy==y-3 || dy==y+3)) continue;
			
			out = level_slice_query_point(ls, dx, dy);
			if(out==LSQ_OUT_OF_BOUNDS || out==LSQ_COLLIDE) return out;
		}
	
	return LSQ_OPEN;
}


void level_slice_copy(LevelSlice *ls, LevelSliceCopy *lsc) {
	unsigned tx, ty;
	int x, y;
	
	tank_get_position(ls->t, &tx, &ty);
	
	for(y=-LS_HEIGHT/2; y<=LS_HEIGHT/2; y++)
		for(x=-LS_WIDTH/2; x<=LS_WIDTH/2; x++)
			lsc->data[y*LS_WIDTH+x] = level_get(ls->lvl, tx+x, ty+y);
}

