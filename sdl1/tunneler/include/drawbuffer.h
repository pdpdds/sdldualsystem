#ifndef _DRAW_BUFFER_H_
#define _DRAW_BUFFER_H_

typedef struct DrawBuffer DrawBuffer;

#include <types.h>

extern Color color_dirt_hi;
extern Color color_dirt_lo;
extern Color color_rock;
extern Color color_fire_hot;
extern Color color_fire_cold;
extern Color color_blank;
extern Color color_bg;
extern Color color_bg_dot;
extern Color color_status_bg;
extern Color color_status_energy;
extern Color color_status_health;
extern Color color_primary[8];
extern Color color_tank[8][3];

DrawBuffer *drawbuffer_new(unsigned w, unsigned h) ;
void drawbuffer_destroy(DrawBuffer *b) ;

void  drawbuffer_set_default(DrawBuffer *b, Color color) ;
void  drawbuffer_set_pixel(DrawBuffer *b, unsigned x, unsigned y, Color color) ;
Color drawbuffer_get_pixel(DrawBuffer *b, unsigned x, unsigned y) ;


#endif /* _DRAW_BUFFER_H_ */

