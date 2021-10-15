#include "keen.h"

extern void box(int x, int y, int w, int h, uchar c);
extern void font_draw(unsigned char *text, int xs, int y, void(*drawingfunc)());

void drawbutton(char *txt, int x, int y)
{
	box(x, y, BUTTON_W, BUTTON_H, 14);
	font_draw(txt,x+2,y+2,drawcharacter_clear);
}

char intersectbutton(int bx, int by, int mx, int my)
{
	if (mx >= bx && my >= by)
	{
		if (mx <= bx+BUTTON_W && my <= by+BUTTON_H)
		{
			return 1;
		}
	}
	return 0;
}
