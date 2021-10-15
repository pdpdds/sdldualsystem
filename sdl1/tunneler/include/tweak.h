#ifndef _TWEAK_H_
#define _TWEAK_H_

/*
 * Most of the game engine's arbitrary limits are stored in here:
 */

/* The default size of the window: */
#define SCREEN_WIDTH                   640
#define SCREEN_HEIGHT                  400
#define SCREEN_MAX_WINDOWS             4
#define SCREEN_MAX_STATUS              4
#define SCREEN_MAX_BITMAPS             4

/* Various attributes of the status bar: */
#define STATUS_HEIGHT                  11
#define STATUS_BORDER                  1

/* Window title / version string: */
#define WINDOW_TITLE                   "TunnelTanks"
#define VERSION                        "0.5 alpha"

/* The virtual resolution of the game. (IE: How many blocks tall/wide) */
#define GAME_WIDTH                     160
#define GAME_HEIGHT                    100

/* The desired speed in frames per second: */
#define GAME_FPS                       24
#define GAME_FPS_WAIT                  ((unsigned)(1000/GAME_FPS))

/* The minimum distance between two tanks in the world. If this is set too high,
 * then the level generator may start throwing exceptions: */
#define MIN_SPAWN_DIST                 150

/* The number of frames to wait in between shots: */
#define TANK_BULLET_DELAY              3

/* The maximum number of bullets allowed from a given tank: */
#define TANK_BULLET_MAX                6

/* The speed in pixels/frame of bullets: */
#define TANK_BULLET_SPEED              3

/* The maximum number of tanks: */
#define MAX_TANKS                      8

/* Various constants for energy calculation: */
#define TANK_STARTING_FUEL             24000
#define TANK_SHOOT_COST                -160
#define TANK_MOVE_COST                 -8
#define TANK_IDLE_COST                 -3
#define TANK_HOME_CHARGE               120
#define TANK_ENEMY_CHARGE              45

/* Various constants for health calculation: */
#define TANK_STARTING_SHIELD           1000
#define TANK_SHOT_DAMAGE               -160
#define TANK_HOME_HEAL                 3

/* Constants for drawing static: (The bottom 3 constants are out of 1000) */
#define STATIC_THRESHOLD               (TANK_STARTING_FUEL/5)
#define STATIC_TRANSPARENCY            200
#define STATIC_BLACK_BAR_ODDS          500
#define STATIC_BLACK_BAR_SIZE          500

/* Various base sizes: */
#define BASE_SIZE                      35
#define BASE_DOOR_SIZE                 7

/* The starting number of items in the Projectile Buffer's cache: */
#define PROJECTILE_BUFFER_START_SIZE   32

/* The radius/number/ttl of particles in various special effects: */
/* TODO: radius would probably be better named as 'speed' or the like... */

#define EXPLOSION_DIRT_COUNT           10
#define EXPLOSION_DIRT_RADIUS          12
#define EXPLOSION_DIRT_TTL             10

#define EXPLOSION_HURT_COUNT           14
#define EXPLOSION_HURT_RADIUS          18
#define EXPLOSION_HURT_TTL             13

#define EXPLOSION_DEATH_COUNT          100
#define EXPLOSION_DEATH_RADIUS         8
#define EXPLOSION_DEATH_TTL            72

/* Characters used in level structures for things: */
#define DIRT_HI                        'D'
#define DIRT_LO                        'd'
#define ROCK                           'r'
#define BLANK                          ' '
#define BASE                           '0'

/* Default to keeping memory stats: */
#ifndef _MEM_STATS
#define _MEM_STATS
#endif

#endif /* _TWEAK_H_ */
