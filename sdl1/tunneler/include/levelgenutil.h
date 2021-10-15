#ifndef _LEVEL_GEN_UTIL_H_
#define _LEVEL_GEN_UTIL_H_

#include <level.h>
#include <types.h>

void     fill_all  (Level *lvl, char c) ;
void     rough_up(Level *lvl) ;
Vector   pt_rand   (unsigned w, unsigned h, unsigned border) ;
unsigned pt_dist   (Vector a, Vector b) ;
void     set_circle(Level *lvl, unsigned x, unsigned y, char value) ;
void     draw_line (Level *dest, Vector a, Vector b, char value, int fat_line) ;

#endif /* _LEVEL_GEN_UTIL_H_ */

