#ifndef _LEVEL_GEN_H_
#define _LEVEL_GEN_H_

#include <stdio.h>

#include <level.h>

/* Every level generator needs to conform to this definition: */
typedef void (*LevelGeneratorFunc)(Level *lvl);

/* Generate a level based on an id: */
void generate_level(Level *lvl, char *id) ;
void print_levels(FILE *out) ;

#endif /* _CAVEGEN_H_ */

