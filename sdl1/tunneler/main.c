#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <game.h>
#include <tweak.h>
#include <memalloc.h>
#include <level.h>
#include <levelgen.h>
#include <random.h>
#include <drawbuffer.h>
#include <projectile.h>
#include <types.h>
#include <gamelib.h>

/* Nothing in this file uses SDL, but we still have to include SDL.h for Macs,
 * since they do some extra magic in the file WRT the main() function: */
#include <SDL.h>

int main(int argc, char *argv[]) {
	unsigned i, is_reading_level=0, is_reading_seed=0, is_reading_file=0;
	unsigned fullscreen=0, width=1000, height=500, player_count = 2, debug = 0;
	char *id = NULL, *outfile_name = NULL;
	int seed = 0, manual_seed=0;
	GameData *gd;
	
	/* Iterate through the CLAs: */
	for(i=1; i<argc; i++) {
		if(is_reading_level) {
			id = argv[i];
			is_reading_level = 0;
		
		} else if(is_reading_seed) {
			seed = atoi(argv[i]);
			manual_seed = 1;
			is_reading_seed = 0;
		
		} else if(is_reading_file) {
			outfile_name = argv[i];
			is_reading_file = 0;
		
		} else if( !strcmp("--help", argv[i]) ) {
			gamelib_print("%s %s\n\n", WINDOW_TITLE, VERSION);
			
			gamelib_print("--version          Display version, and exit.\n");
			gamelib_print("--help             Display this help message and exit.\n\n");
			
			gamelib_print("--single           Only have one user-controlled tank.\n");
			gamelib_print("--double           Have two user-controlled tanks. (Default)\n\n");
			
			gamelib_print("--show-levels      List all available level generators.\n");
			gamelib_print("--level <GEN>      Use <GEN> as the level generator.\n");
			gamelib_print("--seed <INT>       Use <INT> as the random seed.\n");
			gamelib_print("--large            Generate a far larger level.\n");
			gamelib_print("--fullscreen       Start in fullscreen mode.\n\n");
			gamelib_print("--only-gen <FILE>  Will only write the level to a .bmp file, and exit.\n");
			gamelib_print("--debug            Write before/after .bmp's to current directory.\n");
			
			return 0;
		
		} else if( !strcmp("--version", argv[i]) ) {
			gamelib_print("%s %s\n", WINDOW_TITLE, VERSION);
			return 0;
		
		
		} else if( !strcmp("--single", argv[i]) ) {
			player_count = 1;
		
		} else if( !strcmp("--double", argv[i]) ) {
			player_count = 2;
		
		
		} else if( !strcmp("--show-levels", argv[i]) ) {
			print_levels(stdout);
			return 0;
		
		} else if( !strcmp("--level", argv[i]) ) {
			is_reading_level = 1;
		
		} else if( !strcmp("--seed", argv[i]) ) {
			is_reading_seed = 1;
		
		} else if( !strcmp("--large", argv[i]) ) {
			width = 1500; height = 750;
		
		} else if( !strcmp("--debug", argv[i]) ) {
			debug = 1;
		
		} else if( !strcmp("--fullscreen", argv[i]) ) {
			fullscreen = 1;
		
		} else if( !strcmp("--only-gen", argv[i]) ) {
			is_reading_file = 1;
		
		} else {
			gamelib_error("Unexpected argument: '%s'\n", argv[i]);
			exit(1);
		}
	}
	
	/* Seed if necessary: */
	if(manual_seed) srand(seed);
	else            rand_seed();
	
	/* If we're only writing the generated level to file, then just do that: */
	if(outfile_name) {
		Level *lvl = level_new(NULL, width, height);
	
		/* Generate our random level: */
		generate_level(lvl, id);
		level_decorate(lvl);
		level_make_bases(lvl);
		
		/* Dump it out, and exit: */
		level_dump_bmp(lvl, outfile_name);

		gamelib_exit();
		return 0;
	}
	
	/* Let's get this ball rolling: */
	gamelib_init();
	gd = game_new();
	
	game_set_level_gen   (gd, id);
	game_set_level_size  (gd, width, height);
	game_set_debug       (gd, debug);
	game_set_fullscreen  (gd, fullscreen);
	game_set_player_count(gd, player_count);
	
	game_finalize(gd);
	
	/* Play the game: */
	gamelib_main_loop(game_step, gd);
	
	/* Ok, we're done. Tear everything up: */
	game_free(gd);
	gamelib_exit();
	print_mem_stats();
	
	return 0;
}
