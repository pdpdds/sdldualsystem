#include <stdio.h>
#include <string.h>
#include <time.h>

#include <gamelib.h>
#include <levelgen.h>


typedef struct LevelGenerator {
	char              *id;
	LevelGeneratorFunc gen;
	char              *desc;
} LevelGenerator;
#define LEVEL_GENERATOR(id, gen, desc) ((LevelGenerator){(id), (gen), (desc)})


/* === All the generator headers go here: =================================== */

#include <levelgentoast.h>
#include <levelgensimple.h>
#include <levelgenmaze.h>
#include <levelgenbraid.h>

/* Add an entry for every generator: */
LevelGenerator GENERATOR_LIST[] =
{
	{"toast",  toast_generator,  "Twisty, cavernous maps."},
	
	{ "braid",  braid_generator,  "Maze-like maps with no dead ends."},
	{ "maze",   maze_generator,   "Complicated maps with a maze surrounding the bases."},
	{ "simple", simple_generator, "Simple rectangular maps with ragged sides."},
	
	/* This needs to be the last item in the list: */
	{ NULL, NULL, NULL}
};

/* ========================================================================== */

#define TIMER_START(t) \
	(t) = clock()

#define TIMER_STOP(t) do { \
	unsigned temp = ((clock() - (t)) * 100) / CLOCKS_PER_SEC; \
	gamelib_print("%u.%02u sec\n", temp/100, temp%100); \
} while(0)

/* Linear search is ok here, since there aren't many level generators: */

void generate_level(Level *lvl, char *id) {
	LevelGeneratorFunc func = NULL;
	unsigned i;
	clock_t t;
	
	/* If 'id' is null, go with the default: */
	if(!id) id = GENERATOR_LIST[0].id;
	
	/* Look for the id: */
	for(i=0; GENERATOR_LIST[i].id; i++) {
		if(!strcmp(id, GENERATOR_LIST[i].id)) {
			gamelib_print("Using level generator: '%s'\n", GENERATOR_LIST[i].id);
			func = GENERATOR_LIST[i].gen;
			goto generate_level;
		}
	}
	
	/* Report what level generator we found: */
	gamelib_print("Couldn't find level generator: '%s'\n", id);
	gamelib_print("Using default level generator: '%s'\n", GENERATOR_LIST[0].id);
	
	/* If we didn't find the id, then we select the default: */
	if(!func) func = GENERATOR_LIST[0].gen;
	
generate_level:

	TIMER_START(t);
	
	/* Ok, now generate the level: */
	func(lvl);
	
	gamelib_print("Level generated in: ");
	TIMER_STOP(t);
}

/* Will print a specified number of spaces to the file: */
static void put_chars(int i, char c) {
	while( i --> 0 )
		gamelib_print("%c", c);
}

void print_levels(FILE *out) {
	unsigned i, max_id = 7, max_desc = strlen("Description:");
	
	/* Get the longest ID/Description length: */
	for(i=0; GENERATOR_LIST[i].id; i++) {
		if(strlen(GENERATOR_LIST[i].id) > max_id)
			max_id = strlen(GENERATOR_LIST[i].id);
		if(strlen(GENERATOR_LIST[i].desc) > max_desc)
			max_desc = strlen(GENERATOR_LIST[i].desc);
	}
	
	/* Print the header: */
	gamelib_print("ID:  ");
	put_chars(max_id - strlen("ID:"), ' ');
	gamelib_print("Description:\n");
	put_chars(max_id + max_desc + 2, '-');
	gamelib_print("\n");
	
	/* Print all things: */
	for(i=0; GENERATOR_LIST[i].id; i++) {
		gamelib_print("%s  ", GENERATOR_LIST[i].id);
		put_chars(max_id - strlen(GENERATOR_LIST[i].id), ' ');
		gamelib_print("%s%s\n", GENERATOR_LIST[i].desc, i==0 ? " (Default)":"");
	}
	gamelib_print("\n");
}

