#ifndef _SCREEN_H_
#define _SCREEN_H_

typedef struct Screen Screen;

#include <level.h>
#include <types.h>
#include <tank.h>
#include <drawbuffer.h>


/* (Con|De)structor: */
Screen  *screen_new(int is_fullscreen) ;
void     screen_destroy(Screen *s) ;

/* Resizing the screen: */
void screen_set_fullscreen(Screen *s, int is_fullscreen) ;
int screen_resize(Screen *s, unsigned width, unsigned height) ;

/* Set the current drawing mode: */
void screen_set_mode_level(Screen *s, DrawBuffer *b) ;
/*
void screen_set_mode_menu(Screen *s, Menu *m) ;
void screen_set_mode_map(Screen *s, Map *m) ;
*/

/* A few useful functions for external drawing: */
void screen_draw_pixel(Screen *s, unsigned x, unsigned y, Color color) ;
int  screen_map_x(Screen *s, int x) ;
int  screen_map_y(Screen *s, int y) ;

/* Window creation/removal: */

void screen_add_window(Screen *s, Rect r, Tank *t) ;
void screen_add_status(Screen *s, Rect r, Tank *t, int decreases_to_left) ;
void screen_add_bitmap(Screen *s, Rect r, char *bitmap, Color *color) ;
void screen_add_controller(Screen *s, Rect r) ;

/*
void     screen_remove_window(Screen *s, WindowID id) ;
*/
/* Draw the structure: */
void screen_draw(Screen *s) ;

#endif /* _SCREEN_H_ */

