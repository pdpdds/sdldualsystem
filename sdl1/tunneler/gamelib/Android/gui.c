#include <types.h>
#include <screen.h>
#include "androiddata.h"
#include "require_android.h"

#define COLOR_OUTER COLOR(100,100,100)
#define COLOR_INNER COLOR(150,150,150)
#define COLOR_NUB   COLOR(110,110,110)

/* In multiples of radius: */
#define OUTSIDE 5

/* In pixels: */
#define INSIDE 3
#define NUB_SIZE 7

Rect gamelib_gui_get_size() { return RECT(0,0,24,24); }

/* Way too simple: */
unsigned rounded_sqrt(unsigned val) {
	unsigned i, array[] = { 0, 2, 6, 12, 20, 30, 42, 56, 72, 90, 110, 132, 156,
		182, 210, 240, 272, 306, 342, 380, 420, 462, 506, 552, 600, 650, 702,
		756, 812, 870, 930, 992 };
	
	for(i=0; i<32; i++) if(val <= array[i]) break;
	return i;
}

void draw_circle(Screen *s, unsigned x, unsigned y, int radius, Color c) {
	int tx, ty;
	
	for(ty=-radius; ty<=radius; ty++) {
		unsigned number = radius * radius - ty * ty;
		int row = rounded_sqrt(number);
		
		for(tx=-row+1; tx<row; tx++) {
			screen_draw_pixel(s, x+tx, y+ty, c);
		}
	}
}

int abs(int in) {
	return (in < 0) ? -in : in;
}

void gamelib_gui_draw(Screen *s, Rect r) {
	int x = r.x+r.w/2, y = r.y+r.w/2;
	int radius = r.w/2+1;
	
	draw_circle(s, x, y, radius,   COLOR_OUTER);
	draw_circle(s, x, y, radius-1, COLOR_INNER);
	
	/* Draw the nub: */
	if(!_DATA.c_is_touching) {
		draw_circle(s, x, y, NUB_SIZE,   COLOR_OUTER);
		draw_circle(s, x, y, NUB_SIZE-1, COLOR_NUB);
		_DATA.c_dir = VECTOR(0,0);
	
	} else {
		int tx = screen_map_x(s, _DATA.c_touch.x);
		int ty = screen_map_y(s, _DATA.c_touch.y);
		int diffx, diffy, calcx, calcy;
		
		int dist = (tx-x)*(tx-x) + (ty-y)*(ty-y);
		int outside = (radius-NUB_SIZE) * (radius-NUB_SIZE) * OUTSIDE * OUTSIDE;
		
		if(dist > outside || dist < INSIDE * INSIDE) {
			tx = x; ty = y;
			_DATA.c_dir = VECTOR(0,0);
		
		} else {
			if(dist > (radius-NUB_SIZE)*(radius-NUB_SIZE)) {
				dist = rounded_sqrt(dist);
			
				diffx  = (tx-x) * (radius-NUB_SIZE);
				diffx /= dist;
				diffy  = (ty-y) * (radius-NUB_SIZE);
				diffy /= dist;
			
				tx = x + diffx;
				ty = y + diffy;
			}
			
			calcx = (diffx==0) ? 0 : ( abs(diffy * 1000 / diffx) < 2000 );
			calcy = (diffx==0) ? 1 : ( abs(diffy * 1000 / diffx) > 500 );

			_DATA.c_dir.x = calcx * (diffx > 0 ? 1 : -1);
			_DATA.c_dir.y = calcy * (diffy > 0 ? 1 : -1);
		}
		
		draw_circle(s, tx, ty, NUB_SIZE,   COLOR_OUTER);
		draw_circle(s, tx, ty, NUB_SIZE-1, COLOR_NUB);
	}
}

