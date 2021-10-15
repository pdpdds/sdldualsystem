#ifndef _GAME_H_
#define _GAME_H_

typedef struct GameData GameData;

/* Create a default game structure: */
GameData *game_new             () ;

/* Configure a game structure: */
void      game_set_level_gen   (GameData *gd, char *gen) ;
void      game_set_level_size  (GameData *gd, unsigned w, unsigned h) ;
void      game_set_debug       (GameData *gd, int is_debugging) ;
void      game_set_fullscreen  (GameData *gd, int is_fullscreen) ;
void      game_set_player_count(GameData *gd, unsigned num) ;

/* Ready a game structure for actual use: */
void      game_finalize        (GameData *gd) ;

/* Using a finalized game structure: */
int       game_step            (void *gd) ;

/* Done with a game structure: */
void      game_free            (GameData *gd) ;

#endif /* _GAME_H_ */

