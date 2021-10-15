#include "keen.h"
#include "autoblock.h"

extern void plotmap(int x, int y, int t);
extern void box(int x, int y, int w, int h, uchar c);
extern void font_draw(unsigned char *text, int xs, int y, void(*drawingfunc)());
extern void drawtile(int x, int y, unsigned int t);

extern int curblock;

// list of tiles which form blocks when pieced together
// ul, u, ur, l, c, r, ll, d, lr
// if l = 0, block is only 2 tiles tall, and if l = 0 and c != 0,
// block must be C tiles wide
int blocks_ep1[200] = { 183,182,187, 184,185,186, 184,185,186,
						359,358,363, 360,361,362, 360,361,362,
						403,405,404, 406,407,419, 416,418,417,
						254,255,256, 0,0,0, 0,0,0,
						225,226,227, 0,0,0, 0,0,0,
						208,212,216, 0,0,0, 0,0,0,
						143,229,230, 0,3,0, 231,232,233,
						476,477,478, 0,0,0, 489,490,491,
						-1 };

int blocks_ep2[200] = { 183,182,187, 184,185,186, 184,185,186,
						290,291,293, 295,185,296, 295,185,296,
						359,358,363, 360,361,362, 373,372,371,
						403,407,405, 409,411,422, 416,420,418,
						254,255,256, 0,0,0, 0,0,0,
						225,226,227, 0,0,0, 0,0,0,
						433,0,434, 0,0,0, 0,0,0,
						435,436,437, 0,0,0, 0,0,0,
						476,477,478, 489,490,491, 502,503,504,
						-1 };

int blocks_ep3[200] = { 247,248,249, 260,261,262, 273,274,275,
						312,312,312, 0,0,0, 260,261,262,
						400,401,402, 413,414,415, 426,427,428,
						429,430,431, 442,443,444, 455,456,457,
						468,469,470, 481,482,483, 494,495,496,
						308,296,298, 309,143,309, 321,296,320,
						295,296,297, 0,0,0, 0,0,0,
						311,0,0, 309,0,0, 310,0,0,
						562,563,564, 0,0,0, 559,560,561,
						484,485,486, 0,0,0, 0,0,0,
						475,476,477, 488,489,490, 501,502,503,
						378,379,380, 391,392,393, 404,405,406,
						416,417,418, 423,383,422, 419,420,421,
						-1 };


int *getautoblock(int whichblock)
{
	int *block = NULL;
	int index = 0;
	switch (levelcontrol.episode)
	{
	case 1: block = blocks_ep1; break;
	case 2: block = blocks_ep2; break;
	case 3: block = blocks_ep3; break;
	}
	index = (whichblock * 9);
	return &block[index];
}

// draw an "autoblock" over the given coords
void autoblock(int x1, int y1, int x2, int y2)
{
	int x, y;
	int *block;
	int t;

	block = getautoblock(curblock);

	// handle blocks that are only 1 or 2 tiles tall
	if (!block[AB_L])
	{
		if (block[AB_LL]) { y2 = y1 + 1; }
		else { y2 = y1; }
		if (block[AB_C]) x2 = x1 + (block[AB_C] - 1);
	}

	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			if (y == y1)
			{
				if (x == x1) t = block[AB_UL];
				else if (x == x2) t = block[AB_UR];
				else t = block[AB_U];
			}
			else if (y == y2)
			{
				if (x == x1) t = block[AB_LL];
				else if (x == x2) t = block[AB_LR];
				else t = block[AB_D];
			}
			else
			{
				if (x == x1) t = block[AB_L];
				else if (x == x2) t = block[AB_R];
				else t = block[AB_C];
			}
			if (t) plotmap(x, y, t);
		}
	}
}



void drawcurautoblock(void)
{
	int *block = getautoblock(curblock);
	char txt[40];
	int blank;
	int l, c, ll, d, x, u;
	int i;

	blank = (levelcontrol.episode != 3) ? 155 : 489;
	l = block[AB_L];
	c = block[AB_C];
	ll = block[AB_LL];
	d = block[AB_D];
	u = block[AB_U];
	// handle blocks that are only 1 or 2 tiles tall
	if (!l)
	{
		if (block[AB_LL]) { l = ll; c = d; }
		else { l = c = blank; }
		ll = blank; d = blank;
	}
	// platforms of width 2
	if (!u) u = block[AB_UR];

	drawtile(AB_LEFT, AB_TOP, block[AB_UL]);
	drawtile(AB_LEFT + TILE_W, AB_TOP, u);

	drawtile(AB_LEFT, AB_TOP + TILE_H, l);
	drawtile(AB_LEFT + TILE_W, AB_TOP + TILE_H, c);

	drawtile(AB_LEFT, AB_TOP + TILE_H + TILE_H, ll);
	drawtile(AB_LEFT + TILE_W, AB_TOP + TILE_H + TILE_H, d);

	for (i = curblock + 1;; i++)
	{
		block = getautoblock(i);
		if (block[0] == -1) break;
	}
	sprintf(txt, " %d", curblock);
	x = (AB_LASTBUTTON_X + BUTTON_W) - (8 * strlen(txt)) + 2;
	font_draw(txt, x, AB_LASTBUTTON_Y + BUTTON_H + 3, drawcharacter_clear_erasebg);
}
