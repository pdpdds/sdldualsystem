#pragma once

// positon of the "current autoblock" display
#define AB_LEFT				(WINDOW_WIDTH+3)
#define AB_TOP				129
#define AB_NEXTBUTTON_X		NEXTBUTTON_X
#define AB_NEXTBUTTON_Y		(AB_TOP+(TILE_H*3)+6)
#define AB_LASTBUTTON_X		AB_NEXTBUTTON_X
#define AB_LASTBUTTON_Y		(AB_NEXTBUTTON_Y+BUTTON_H+2)

#define AB_UL			0
#define AB_U			1
#define AB_UR			2
#define AB_L			3
#define AB_C			4
#define AB_R			5
#define AB_LL			6
#define AB_D			7
#define AB_LR			8

void autoblock(int x1, int y1, int x2, int y2);
void drawcurautoblock(void);