#ifndef _TYPES_H_
#define _TYPES_H_

/* Generic types that are used all over the place: */

/* A very simple struct used to store spawn locations of tanks: */
typedef struct Vector {
	unsigned x, y;
} Vector;
#define VECTOR(x,y) ((Vector){(x),(y)})

/* A simple struct for quads: */
typedef struct Rect {
	int x, y;
	unsigned w, h;
} Rect;
#define RECT(x,y,w,h) ((Rect){(x),(y),(w),(h)})

/* A simple way to reference a color: */
typedef struct Color {
	unsigned char r, g, b;
} Color;
#define COLOR(r,g,b) {(r),(g),(b)}

#endif /* _TYPES_H_ */

