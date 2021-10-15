#include <stdio.h>
#include <stdlib.h>

#include <game.h>
#include <controller.h>
#include <guisprites.h>
#include <level.h>
#include <levelgen.h>
#include <tanklist.h>
#include <drawbuffer.h>
#include <projectile.h>
#include <screen.h>
#include <memalloc.h>
#include <tweak.h>
#include <gamelib.h>


/*#define ERR_OUT(msg) fprintf(stderr, "PROGRAMMING ERROR: " msg "\n")*/
#define ERR_OUT(msg) gamelib_error( "PROGRAMMING ERROR: " msg )

#define ASSERT_CONFIG() do { \
	if(gd->is_active) { \
		ERR_OUT("Attempted to configure a game while it is running."); \
		exit(1); \
	} \
} while(0)

#define ASSERT_ACTIVE() do { \
	if(!gd->is_active) { \
		ERR_OUT("Attempted to start a game before finalizing."); \
		exit(1); \
	} \
} while(0)


typedef struct GameDataConfig {
	char *gen;
	unsigned w, h;
	unsigned is_fullscreen;
	unsigned player_count;
	int rand_seed;
} GameDataConfig;

typedef struct GameDataActive {
	Level      *lvl;
	TankList   *tl;
	DrawBuffer *b;
	PList      *pl;
	Screen     *s;
} GameDataActive;

struct GameData {
	int is_active;
	int is_debug;
	union {
		GameDataConfig config;
		GameDataActive active;
	} data;
};


/*----------------------------------------------------------------------------*
 * This bit is used to initialize various GUIs:                               *
 *----------------------------------------------------------------------------*/

static void twitch_fill(TankList *tl, Level *lvl, unsigned starting_id) {
	unsigned i;
	
	for(i=starting_id; i<MAX_TANKS; i++) {
		Tank *t = tanklist_add_tank(tl, i, level_get_spawn(lvl, i));
		controller_twitch_attach(t);
	}
}


/* TODO: De-uglify this crap: */
static void init_single_player(Screen *s, TankList *tl, Level *lvl) {
	Tank *t;
	Rect gui;
	unsigned gui_shift;
	
	/* Account for the GUI Controller: */
	gui = gamelib_gui_get_size();
	gui_shift = gui.w + !!gui.w * 15; /* << Shift out of way of thumb... */
	
	gamelib_debug("XYWH: %u %u %u %u", gui.x, gui.y, gui.w, gui.h);
	
	/* Ready the tank! */
	t = tanklist_add_tank(tl, 0, level_get_spawn(lvl, 0));
	gamelib_tank_attach(t, 0, 1);
	
	screen_add_window(s, RECT(2, 2, GAME_WIDTH-4, GAME_HEIGHT-6-STATUS_HEIGHT), t);
	screen_add_status(s, RECT(9 + gui_shift, GAME_HEIGHT - 2 - STATUS_HEIGHT, GAME_WIDTH-12 - gui_shift, STATUS_HEIGHT), t, 1);
	if(gui_shift)
		screen_add_controller(s, RECT(3, GAME_HEIGHT - 5 - gui.h, gui.w, gui.h));
	
	/* Add the GUI bitmaps: */
	screen_add_bitmap(s, RECT(3 + gui_shift, GAME_HEIGHT - 2 - STATUS_HEIGHT    , 4, 5), GUI_ENERGY, &color_status_energy);
	screen_add_bitmap(s, RECT(3 + gui_shift, GAME_HEIGHT - 2 - STATUS_HEIGHT + 6, 4, 5), GUI_HEALTH, &color_status_health);
	
	/* Fill up the rest of the slots with Twitches: */
	twitch_fill(tl, lvl, 1);
}

static void init_double_player(Screen *s, TankList *tl, Level *lvl) {
	Tank *t;
	
	/* Ready the tanks! */
	t = tanklist_add_tank(tl, 0, level_get_spawn(lvl, 0));
	gamelib_tank_attach(t, 0, 2);
	screen_add_window(s, RECT(2, 2, GAME_WIDTH/2-3, GAME_HEIGHT-6-STATUS_HEIGHT), t);
	screen_add_status(s, RECT(3, GAME_HEIGHT - 2 - STATUS_HEIGHT, GAME_WIDTH/2-5-2, STATUS_HEIGHT), t, 0);
	
	/* Load up two controllable tanks: */
	t = tanklist_add_tank(tl, 1, level_get_spawn(lvl, 1));
	
	/*controller_twitch_attach(t);  << Attach a twitch to a camera tank, so we can see if they're getting smarter... */
	gamelib_tank_attach(t, 1, 2);
	screen_add_window(s, RECT(GAME_WIDTH/2+1, 2, GAME_WIDTH/2-3, GAME_HEIGHT-6-STATUS_HEIGHT), t);
	screen_add_status(s, RECT(GAME_WIDTH/2+2+2, GAME_HEIGHT - 2 - STATUS_HEIGHT, GAME_WIDTH/2-5-3, STATUS_HEIGHT), t, 1);

	/* Add the GUI bitmaps: */
	screen_add_bitmap(s, RECT(GAME_WIDTH/2-2, GAME_HEIGHT - 2 - STATUS_HEIGHT    , 4, 5), GUI_ENERGY, &color_status_energy);
	screen_add_bitmap(s, RECT(GAME_WIDTH/2-2, GAME_HEIGHT - 2 - STATUS_HEIGHT + 6, 4, 5), GUI_HEALTH, &color_status_health);
	
	/* Fill up the rest of the slots with Twitches: */
	twitch_fill(tl, lvl, 2);
}



/* Create a default game structure: */
GameData *game_new() {
	GameData *out = malloc(sizeof(GameData));
	
	/* Copy in all the default values: */
	out->is_active = out->is_debug = 0;
	out->data.config.gen = NULL;
	/* The hell was I thinking?
	out->data.config.w = GAME_WIDTH;
	out->data.config.h = GAME_HEIGHT;
	*/
	out->data.config.w = 1000;
	out->data.config.h = 500;
	out->data.config.player_count = gamelib_get_max_players();
	
	if(gamelib_get_can_window())          out->data.config.is_fullscreen = 0;
	else if(gamelib_get_can_fullscreen()) out->data.config.is_fullscreen = 1;
	else {
		/* The hell!? */
		ERR_OUT("gamelib can't run fullscreen or in a window.");
		exit(1);
	}
	
	return out;
}


/* Configure a game structure: */
void game_set_level_gen(GameData *gd, char *gen) {
	ASSERT_CONFIG();
	
	gd->data.config.gen = gen;
}

void game_set_level_size(GameData *gd, unsigned w, unsigned h) {
	ASSERT_CONFIG();
	
	gd->data.config.w = w; gd->data.config.h = h;
}

void game_set_debug(GameData *gd, int is_debugging) {
	ASSERT_CONFIG();
	
	gd->is_debug = is_debugging;
}

void game_set_fullscreen(GameData *gd, int is_fullscreen) {
	ASSERT_CONFIG();
	
	gd->data.config.is_fullscreen = is_fullscreen;
}

void game_set_player_count(GameData *gd, unsigned num) {
	ASSERT_CONFIG();
	
	if(!num || num > gamelib_get_max_players()) {
		ERR_OUT("Tried to use more players than the platform can support.");
		exit(1);
	}
	
	gd->data.config.player_count = num;
}


/* Ready a game structure for actual use: */
void game_finalize(GameData *gd) {
	Level      *lvl;
	TankList   *tl;
	DrawBuffer *b;
	PList      *pl;
	Screen     *s;
	
	ASSERT_CONFIG();
	
	/* Initialize most of the structures: */
	s   = screen_new    (gd->data.config.is_fullscreen);
	pl  = plist_new     ();
	b   = drawbuffer_new(gd->data.config.w, gd->data.config.h);
	lvl = level_new     (b, gd->data.config.w, gd->data.config.h);
	tl  = tanklist_new  (lvl, pl);
	
	/* Generate our random level: */
	generate_level(lvl, gd->data.config.gen);
	level_decorate(lvl);
	level_make_bases(lvl);
	
	/* Debug the starting data, if we're debugging: */
	if(gd->is_debug)
		level_dump_bmp(lvl, "debug_start.bmp");
	
	/* Start drawing! */
	drawbuffer_set_default(b, color_rock);
	level_draw_all(lvl, b);
	screen_set_mode_level(s, b);
	
	/* Set up the players/GUI: */
	if     (gd->data.config.player_count == 1) init_single_player(s, tl, lvl);
	else if(gd->data.config.player_count == 2) init_double_player(s, tl, lvl);
	else {
		ERR_OUT("Don't know how to draw more than 2 players at once...");
		exit(1);
	}
	
	/* Copy all of our variables into the GameData struct: */
	gd->is_active = 1;
	gd->data.active.s   = s;
	gd->data.active.pl  = pl;
	gd->data.active.b   = b;
	gd->data.active.lvl = lvl;
	gd->data.active.tl  = tl;
}

/* Step the game simulation by handling events, and drawing: */
int game_step(void *input) {
	GameData *gd = input;
	ASSERT_ACTIVE();
	
	EventType temp;
	
	/* Handle all queued events: */
	while( (temp=gamelib_event_get_type()) != GAME_EVENT_NONE ) {
		
		/* Trying to resize the window? */
		if(temp == GAME_EVENT_RESIZE) {
			Rect r = gamelib_event_resize_get_size();
			screen_resize(gd->data.active.s, r.w, r.h);
		
		/* Trying to toggle fullscreen? */
		} else if(temp == GAME_EVENT_TOGGLE_FULLSCREEN) {
			screen_set_fullscreen(gd->data.active.s, -1);
		
		/* Trying to exit? */
		} else if(temp == GAME_EVENT_EXIT) {
			return 1;
		}
		
		/* Done with this event: */
		gamelib_event_done();
	}
	
	/* Clear everything: */
	tanklist_map(gd->data.active.tl, tank_clear(t, gd->data.active.b));
	plist_clear (gd->data.active.pl, gd->data.active.b);

	/* Charge a small bit of energy for life: */
	tanklist_map(gd->data.active.tl, tank_alter_energy(t, TANK_IDLE_COST));

	/* See if we need to be healed: */
	tanklist_map(gd->data.active.tl, tank_try_base_heal(t));
	
	/* Move everything: */
	plist_step  (gd->data.active.pl, gd->data.active.lvl, gd->data.active.tl);
	tanklist_map(gd->data.active.tl, tank_move(t, gd->data.active.tl));
	
	/* Draw everything: */
	plist_draw  (gd->data.active.pl, gd->data.active.b);
	tanklist_map(gd->data.active.tl, tank_draw(t, gd->data.active.b));
	screen_draw (gd->data.active.s);
	
	return 0;
}

/* Done with a game structure: */
void game_free(GameData *gd) {
	if(!gd) return;
	
	if(gd->is_active) {
		/* Debug if we need to: */
		if(gd->is_debug)
			level_dump_bmp(gd->data.active.lvl, "debug_end.bmp");
		
		drawbuffer_destroy(gd->data.active.b);
		plist_destroy     (gd->data.active.pl);
		tanklist_destroy  (gd->data.active.tl);
		level_destroy     (gd->data.active.lvl);
		screen_destroy    (gd->data.active.s);
	}
	
	free(gd);
}

