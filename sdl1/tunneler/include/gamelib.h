#ifndef _GAME_LIB_H_
#define _GAME_LIB_H_

/* This file defines an interface that is implemented by one of this folder's
 * subdirectories. Most functions in here are normally provided by SDL, but are
 * abstracted so that this game can be used in non-SDL environments. (Namely:
 * Android.) */

#include <types.h>
#include <tank.h>


/* If the gamelib needs initialization, this'll do it: */
int gamelib_init();

/* If the gamelib needs to free resources before exiting, this'll do it: */
int gamelib_exit();

/* Gives a way to poll the gamelib for the capabilities provided by the
 * underlying system: */
int gamelib_get_max_players() ;    /* Returns 1 or 2. */
int gamelib_get_can_resize() ;     /* Returns 0 or 1. */
int gamelib_get_can_fullscreen() ; /* Returns 0 or 1. */
int gamelib_get_can_window() ;     /* Returns 0 or 1. */
int gamelib_get_target_fps() ;     /* Usually returns 24. */

/* Some platforms (Android) will be acting as the game loop, so the game loop
 * needs to happen in the gamelib: */
typedef int (*draw_func)(void *data);
void gamelib_main_loop(draw_func func, void *data);

/* This lets you attach controllers to a tank: */
int gamelib_tank_attach(Tank *t, int tank_num, int num_players) ;

/* TODO: This will need a means for configuring the controller... */

/* Allow us to handle events in a fairly platform-neutral way: */
typedef enum EventType {
	GAME_EVENT_NONE = 0,
	GAME_EVENT_EXIT,
	GAME_EVENT_RESIZE,
	GAME_EVENT_TOGGLE_FULLSCREEN,
} EventType;

EventType gamelib_event_get_type() ;
Rect      gamelib_event_resize_get_size() ; /* Returns {0,0,0,0} on fail. */
void      gamelib_event_done() ;

/* We need to be able to switch resolutions: */
int  gamelib_set_fullscreen() ;
int  gamelib_set_window(unsigned w, unsigned h) ;
Rect gamelib_get_resolution() ;
int  gamelib_get_fullscreen() ;

/* We need a way to draw: */
/* TODO: Implement an API for locking/unlocking the pixel array. */
int  gamelib_draw_box(Rect *rect, Color c) ;

/* Now, for a way to draw a bitmap, if the platform wants to... */
typedef struct BMPFile BMPFile;
BMPFile *gamelib_bmp_new      (unsigned width, unsigned height) ;
void     gamelib_bmp_set_pixel(BMPFile *f, unsigned x, unsigned y, Color c) ;
void     gamelib_bmp_finalize (BMPFile *f, char *filename) ;

/* A few outputting commands: */
void     gamelib_print (char *str, ...) ;
void     gamelib_debug (char *str, ...) ;
void     gamelib_error (char *str, ...) ;

/* Gamelib needs to be able to tell the outside that it needs a control GUI: */
Rect     gamelib_gui_get_size() ;
void     gamelib_gui_draw(Screen *s, Rect r) ;


#endif /* _GAME_LIB_H_ */

