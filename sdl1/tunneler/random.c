#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <gamelib.h>
#include <random.h>


int rand_bool(unsigned odds) {
	return rand_int(0,999) < odds;
}

unsigned rand_int(unsigned min, unsigned max) {
	unsigned range = max - min + 1;
	
	if(max <= min) return min;
	
	/* I know that using the % isn't entirely accurate, but it only uses
	 * integers, so w/e: */
	return (rand() % range) + min;
}

void rand_seed() {
	int seed = 0;
	FILE *urand;
	
	/* Try to load out of /dev/urandom, but it isn't fatal if we can't: */
	urand = fopen("/dev/urandom", "r");
	if(urand) {
		if(fread(&seed, sizeof(seed), 1, urand) != 1) seed = 0;
		fclose(urand);
	}
	
	/* Throw in the time, so that computers w/o the urandom source don't get
	 * screwed... plus... it doesn't hurt. :) */
	seed ^= time(NULL);
	
	gamelib_print("Using seed: %d\n", seed);
	
	srand(seed);
}

