#ifndef WL_DEF_H
#define WL_DEF_H

// Defines which version shall be built and configures supported extra features
#include "version.h"

#include <assert.h>
#include <fcntl.h>
#include <math.h>

#ifdef USE_REAL_RAND
#include <time.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(_arch_dreamcast)
#	include <string.h>
#	include "dc/dc_main.h"
#elif !defined(_WIN32)
#	include <stdint.h>
#	include <string.h>
#	include <stdarg.h>
#endif
#include <SDL.h>

#if !defined O_BINARY
#	define O_BINARY 0
#endif

#pragma pack(1)

#if defined(_arch_dreamcast)
#define YESBUTTONNAME "A"
#define NOBUTTONNAME  "B"
#elif defined(GP2X)
#define YESBUTTONNAME "Y"
#define NOBUTTONNAME  "B"
#else
#define YESBUTTONNAME "Y"
#define NOBUTTONNAME  "N"
#endif

#include "foreign.h"

#ifndef SPEAR
    #include "audiowl6.h"
    #ifdef UPLOAD
        #include "gfxv_apo.h"
    #else
		#ifdef JAPAN
			#include "gfxv_jap.h"
		#else
			#ifdef GOODTIMES
	            #include "gfxv_wl6.h"
		    #else
			    #include "gfxv_apo.h"
			#endif
        #endif
    #endif
#else
    #include "audiosod.h"
    #include "gfxv_sod.h"
    #include "f_spear.h"
#endif

#ifdef _arch_dreamcast
typedef uint8 uint8_t;
typedef uint16 uint16_t;
typedef uint32 uint32_t;
typedef int8 int8_t;
typedef int16 int16_t;
typedef int32 int32_t;
typedef int64 int64_t;
typedef ptr_t uintptr_t;
#endif

typedef uint8_t byte;
typedef uint16_t word;
typedef int32_t fixed;
typedef uint32_t longword;
typedef int8_t boolean;
typedef void * memptr;

typedef struct
{
    int x,y;
} Point;
typedef struct
{
    Point ul,lr;
} Rect;

void Quit(const char *errorStr, ...);

#include "id_pm.h"
#include "id_sd.h"
#include "id_in.h"
#include "id_vl.h"
#include "id_vh.h"
#include "id_us.h"
#include "id_ca.h"

#include "wl_menu.h"

#ifdef SPEAR
#include "f_spear.h"
#endif

#define MAPSPOT(x,y,plane) (mapsegs[plane][((y)<<mapshift)+(x)])

#define SIGN(x)         ((x)>0?1:-1)
#define ABS(x)          ((int)(x)>0?(x):-(x))
#define LABS(x)         ((int32_t)(x)>0?(x):-(x))

// causes conflict compiling with VS 2015 community
//#define abs(x) ABS(x)

/*
=============================================================================

                            GLOBAL CONSTANTS

=============================================================================
*/

#define MAXTICS 10
#define DEMOTICS        4

#define WALLSHIFT       10

#define BIT_WALL        (1<<WALLSHIFT)
#define BIT_DOOR        (1<<(WALLSHIFT+1))
#define BIT_ALLTILES    (1<<(WALLSHIFT+2))
#define LAST_WALLNUM    BIT_WALL-5
#define LAST_DOORNUM    BIT_DOOR-1 

#define MOVINGIN_N BIT_WALL-1
#define MOVINGIN_S BIT_WALL-2
#define MOVINGIN_E BIT_WALL-3
#define MOVINGIN_W BIT_WALL-4

#define MAXACTORS       1000         // max number of nazis, etc / map
#define MAXSTATS        2000         // max number of lamps, bonus, etc
#define MAXDOORS        BIT_DOOR          // max number of sliding doors
#define MAXWALLTILES    BIT_WALL         // max number of wall tiles

//
// tile constants
//

#define ICONARROWS      90
#define PUSHABLETILE    98
#define EXITTILE        99          // at end of castle
#define AREATILE        107         // first of NUMAREAS floor tiles
#define NUMAREAS        37
#define ELEVATORTILE    21
#define AMBUSHTILE      106
#define ALTELEVATORTILE 107

// -- EOD tile stuff --

#define HELLTILE 87
#define FIREPLACETILE 78 
// --- Teleporter tile stuff
#define WARPEASTTILE 100
#define WARPWESTTILE 102
#define INVISWARPTILE 160
#define WESTINSTANT 0

// --- Special triggers
#define ENDLEVELTILE 225
#define ENEMYBLOCKTILE 261

// --- Stat obj stuff
#define ARMORTILE 39
#define TOILETTILE 35
#define CLONETOILETTILE 267 // used for easy clipping
#define RADIOTILE 328
#define REGENTILE 322
#define TURRETTILE 106 // used for easy clipping

// --- Secret tile stuff
#define SECRETTILE 380
#define SUPERSECRETTILE 381

// --- Wall switch tile stuff
#define SHOOTERTILE 65

#define SWITCHOFFTILE 57
#define SWITCHONTILE 58

#define SW_LIGHTS 388

#define SW_FIREWALL_1 408
#define SW_FIREWALL_2 409
#define SW_FIREWALL_3 410
#define SW_FIREWALL_4 411
#define SW_FIREWALL_5 412
#define SW_FIREWALL_6 413
#define SW_FIREWALL_7 414
#define SW_FIREWALL_8 415

#define SW_PUSHWALL_1 416
#define SW_PUSHWALL_2 417
#define SW_PUSHWALL_3 418
#define SW_PUSHWALL_4 419

#define PUSHWALL_1_N 420
#define PUSHWALL_1_E 421
#define PUSHWALL_1_S 422
#define PUSHWALL_1_W 423
#define PUSHWALL_2_N 424
#define PUSHWALL_2_E 425
#define PUSHWALL_2_S 426
#define PUSHWALL_2_W 427
#define PUSHWALL_3_N 428
#define PUSHWALL_3_E 429
#define PUSHWALL_3_S 430
#define PUSHWALL_3_W 431
#define PUSHWALL_4_N 432
#define PUSHWALL_4_E 433
#define PUSHWALL_4_S 434
#define PUSHWALL_4_W 435

// --- Spawn trigger tile stuff
#define SPN_TRG_OFC_1 512
#define SPN_TRG_OFC_2 513
#define SPN_TRG_SS_1 514
#define SPN_TRG_SS_2 515
#define SPN_TRG_GRD_1 516
#define SPN_TRG_GRD_2 517
#define SPN_TAR_OFC_1 518
#define SPN_TAR_OFC_2 519
#define SPN_TAR_SS_1 520
#define SPN_TAR_SS_2 521
#define SPN_TAR_GRD_1 522
#define SPN_TAR_GRD_2 523

// --- Flags for handling the timer
#define MAXTIMERS 4
#define T_LEVEL 1
#define T_GOD 2
#define T_FURY 4

// ----------------------------------


#define NUMBERCHARS     9


//----------------

#define EXTRAPOINTS     40000

#define PLAYERSPEED     3000
#define RUNSPEED        6000

#define SCREENSEG       0xa000

#define SCREENBWIDE     80

#define HEIGHTRATIO     0.5            // also defined in id_mm.c

#define BORDERCOLOR     3
#define FLASHCOLOR      5
#define FLASHTICS       4

#ifndef SPEAR
    #define LRpack      8       // # of levels to store in endgame
#else
    #define LRpack      60
#endif

#define PLAYERSIZE      MINDIST         // player radius
#define MINACTORDIST    0x10000l       // minimum dist from player center
                                        // to any actor center
#define MINPLAYERDIST 0xD800l

#define NUMLATCHPICS    100

#undef M_PI
#define PI              3.1415926535897932384626433832795 //3.14159265358979323846264338327950288419716939937510  //3.141592657
#define M_PI PI

#define GLOBAL1         (1l<<16)
#define TILEGLOBAL      GLOBAL1
#define PIXGLOBAL       (GLOBAL1/64)
#define TILESHIFT       16l
#define UNSIGNEDSHIFT   8

#define ANGLES          360             // must be divisable by 4
#define ANGLEQUAD       (ANGLES/4)
#define FINEANGLES      3600
#define ANG90           (FINEANGLES/4)
#define ANG180          (ANG90*2)
#define ANG270          (ANG90*3)
#define ANG360          (ANG90*4)
#define VANG90          (ANGLES/4)
#define VANG180         (VANG90*2)
#define VANG270         (VANG90*3)
#define VANG360         (VANG90*4)

#define MINDIST         (0x5800l)

#define mapshift        7
#define MAPSIZE         (1<<mapshift)
#define maparea         MAPSIZE*MAPSIZE
#define MAPSIZE_M1      MAPSIZE-1

#define mapheight       MAPSIZE
#define mapwidth        MAPSIZE

#ifdef USE_HIRES

#define TEXTURESHIFT    7
#define TEXTURESIZE     (1<<TEXTURESHIFT)
#define TEXTUREFROMFIXEDSHIFT 2
#define TEXTUREMASK     (TEXTURESIZE*(TEXTURESIZE-1))

#define SPRITESCALEFACTOR 1

#else

#define TEXTURESHIFT    6 // 6 (64) 7 (128)
#define TEXTURESIZE     (1<<TEXTURESHIFT)
#define TEXTUREFROMFIXEDSHIFT 4
#define TEXTUREMASK     (TEXTURESIZE*(TEXTURESIZE-1))

#define SPRITESCALEFACTOR 2

#endif

#define NORTH   0
#define EAST    1
#define SOUTH   2
#define WEST    3

#define NORTHANGLE 112
#define NORTHEASTANGLE 67
#define EASTANGLE 22
#define SOUTHEASTANGLE 337
#define SOUTHANGLE 292
#define SOUTHWESTANGLE 247
#define WESTANGLE 202
#define NORTHWESTANGLE 157

#define STATUSLINES     40

#define SCREENSIZE      (SCREENBWIDE*208)
#define PAGE1START      0
#define PAGE2START      (SCREENSIZE)
#define PAGE3START      (SCREENSIZE*2u)
#define FREESTART       (SCREENSIZE*3u)


#define PIXRADIUS       512

#define STARTAMMO       8


// object flag values

typedef enum
{
    FL_SHOOTABLE        = 0x00000001,
    FL_BONUS            = 0x00000002,
    FL_NEVERMARK        = 0x00000004,
    FL_VISABLE          = 0x00000008,
    FL_ATTACKMODE       = 0x00000010,
    FL_FIRSTATTACK      = 0x00000020,
    FL_AMBUSH           = 0x00000040,
    FL_NONMARK          = 0x00000080,
    FL_FULLBRIGHT       = 0x00000100,
#ifdef USE_DIR3DSPR
    // you can choose one of the following values in wl_act1.cpp
    // to make a static sprite a directional 3d sprite
    // (see example at the end of the statinfo array)
    FL_DIR_HORIZ_MID    = 0x00000200,
    FL_DIR_HORIZ_FW     = 0x00000400,
    FL_DIR_HORIZ_BW     = 0x00000600,
    FL_DIR_VERT_MID     = 0x00000a00,
    FL_DIR_VERT_FW      = 0x00000c00,
    FL_DIR_VERT_BW      = 0x00000e00,

    // these values are just used to improve readability of code
    FL_DIR_NONE         = 0x00000000,
    FL_DIR_POS_MID      = 0x00000200,
    FL_DIR_POS_FW       = 0x00000400,
    FL_DIR_POS_BW       = 0x00000600,
    FL_DIR_POS_MASK     = 0x00000600,
    FL_DIR_VERT_FLAG    = 0x00000800,
    FL_DIR_MASK         = 0x00000e00,
#endif

    FL_STEALTH          = 0x00001000,
    FL_GIB              = 0x00002000,
    FL_BOSS             = 0x00004000,
    FL_IMMUNE           = 0x00008000,
	FL_INVISIBLE		= 0x00010000
    // next free bit is   0x00001000
} objflag_t;


//
// sprite constants
//

enum
{
    SPR_DEMO,
#ifndef APOGEE_1_0
    SPR_DEATHCAM,
#endif
//
// static sprites
//
    SPR_STAT_0,SPR_STAT_1,SPR_STAT_2,SPR_STAT_3,
    SPR_STAT_4,SPR_STAT_5,SPR_STAT_6,SPR_STAT_7,

    SPR_STAT_8,SPR_STAT_9,SPR_STAT_10,SPR_STAT_11,
    SPR_STAT_12,SPR_STAT_13,SPR_STAT_14,SPR_STAT_15,

    SPR_STAT_16,SPR_STAT_17,SPR_STAT_18,SPR_STAT_19,
    SPR_STAT_20,SPR_STAT_21,SPR_STAT_22,SPR_STAT_23,

    SPR_STAT_24,SPR_STAT_25,SPR_STAT_26,SPR_STAT_27,
    SPR_STAT_28,SPR_STAT_29,SPR_STAT_30,SPR_STAT_31,

    SPR_STAT_32,SPR_STAT_33,SPR_STAT_34,SPR_STAT_35,
    SPR_STAT_36,SPR_STAT_37,SPR_STAT_38,SPR_STAT_39,

    SPR_STAT_40,SPR_STAT_41,SPR_STAT_42,SPR_STAT_43,
    SPR_STAT_44,SPR_STAT_45,SPR_STAT_46,SPR_STAT_47,

#ifdef SPEAR
    SPR_STAT_48,SPR_STAT_49,SPR_STAT_50,SPR_STAT_51,
#endif

//
// guard
//
    SPR_GRD_S_1,SPR_GRD_S_2,SPR_GRD_S_3,SPR_GRD_S_4,
    SPR_GRD_S_5,SPR_GRD_S_6,SPR_GRD_S_7,SPR_GRD_S_8,

    SPR_GRD_W1_1,SPR_GRD_W1_2,SPR_GRD_W1_3,SPR_GRD_W1_4,
    SPR_GRD_W1_5,SPR_GRD_W1_6,SPR_GRD_W1_7,SPR_GRD_W1_8,

    SPR_GRD_W2_1,SPR_GRD_W2_2,SPR_GRD_W2_3,SPR_GRD_W2_4,
    SPR_GRD_W2_5,SPR_GRD_W2_6,SPR_GRD_W2_7,SPR_GRD_W2_8,

    SPR_GRD_W3_1,SPR_GRD_W3_2,SPR_GRD_W3_3,SPR_GRD_W3_4,
    SPR_GRD_W3_5,SPR_GRD_W3_6,SPR_GRD_W3_7,SPR_GRD_W3_8,

    SPR_GRD_W4_1,SPR_GRD_W4_2,SPR_GRD_W4_3,SPR_GRD_W4_4,
    SPR_GRD_W4_5,SPR_GRD_W4_6,SPR_GRD_W4_7,SPR_GRD_W4_8,

    SPR_GRD_PAIN_1,SPR_GRD_DIE_1,SPR_GRD_DIE_2,SPR_GRD_DIE_3,
    SPR_GRD_PAIN_2,SPR_GRD_DEAD,

    SPR_GRD_SHOOT1,SPR_GRD_SHOOT2,SPR_GRD_SHOOT3,

//
// dogs
//
    SPR_SCLONE_W1_1,SPR_SCLONE_W1_2,SPR_SCLONE_W1_3,SPR_SCLONE_W1_4,
    SPR_SCLONE_W1_5,SPR_SCLONE_W1_6,SPR_SCLONE_W1_7,SPR_SCLONE_W1_8,

    SPR_SCLONE_W2_1,SPR_SCLONE_W2_2,SPR_SCLONE_W2_3,SPR_SCLONE_W2_4,
    SPR_SCLONE_W2_5,SPR_SCLONE_W2_6,SPR_SCLONE_W2_7,SPR_SCLONE_W2_8,

    SPR_SCLONE_W3_1,SPR_SCLONE_W3_2,SPR_SCLONE_W3_3,SPR_SCLONE_W3_4,
    SPR_SCLONE_W3_5,SPR_SCLONE_W3_6,SPR_SCLONE_W3_7,SPR_SCLONE_W3_8,

    SPR_SCLONE_W4_1,SPR_SCLONE_W4_2,SPR_SCLONE_W4_3,SPR_SCLONE_W4_4,
    SPR_SCLONE_W4_5,SPR_SCLONE_W4_6,SPR_SCLONE_W4_7,SPR_SCLONE_W4_8,

    SPR_SCLONE_DIE_1,SPR_SCLONE_DIE_2,SPR_SCLONE_DIE_3,SPR_SCLONE_DEAD,
    SPR_SCLONE_JUMP1,SPR_SCLONE_JUMP2,SPR_SCLONE_JUMP3,



//
// ss
//
    SPR_SS_S_1,SPR_SS_S_2,SPR_SS_S_3,SPR_SS_S_4,
    SPR_SS_S_5,SPR_SS_S_6,SPR_SS_S_7,SPR_SS_S_8,

    SPR_SS_W1_1,SPR_SS_W1_2,SPR_SS_W1_3,SPR_SS_W1_4,
    SPR_SS_W1_5,SPR_SS_W1_6,SPR_SS_W1_7,SPR_SS_W1_8,

    SPR_SS_W2_1,SPR_SS_W2_2,SPR_SS_W2_3,SPR_SS_W2_4,
    SPR_SS_W2_5,SPR_SS_W2_6,SPR_SS_W2_7,SPR_SS_W2_8,

    SPR_SS_W3_1,SPR_SS_W3_2,SPR_SS_W3_3,SPR_SS_W3_4,
    SPR_SS_W3_5,SPR_SS_W3_6,SPR_SS_W3_7,SPR_SS_W3_8,

    SPR_SS_W4_1,SPR_SS_W4_2,SPR_SS_W4_3,SPR_SS_W4_4,
    SPR_SS_W4_5,SPR_SS_W4_6,SPR_SS_W4_7,SPR_SS_W4_8,

    SPR_SS_PAIN_1,SPR_SS_DIE_1,SPR_SS_DIE_2,SPR_SS_DIE_3,
    SPR_SS_PAIN_2,SPR_SS_DEAD,

    SPR_SS_SHOOT1,SPR_SS_SHOOT2,SPR_SS_SHOOT3,

//
// mutant
//
    SPR_MUT_S_1,SPR_MUT_S_2,SPR_MUT_S_3,SPR_MUT_S_4,
    SPR_MUT_S_5,SPR_MUT_S_6,SPR_MUT_S_7,SPR_MUT_S_8,

    SPR_MUT_W1_1,SPR_MUT_W1_2,SPR_MUT_W1_3,SPR_MUT_W1_4,
    SPR_MUT_W1_5,SPR_MUT_W1_6,SPR_MUT_W1_7,SPR_MUT_W1_8,

    SPR_MUT_W2_1,SPR_MUT_W2_2,SPR_MUT_W2_3,SPR_MUT_W2_4,
    SPR_MUT_W2_5,SPR_MUT_W2_6,SPR_MUT_W2_7,SPR_MUT_W2_8,

    SPR_MUT_W3_1,SPR_MUT_W3_2,SPR_MUT_W3_3,SPR_MUT_W3_4,
    SPR_MUT_W3_5,SPR_MUT_W3_6,SPR_MUT_W3_7,SPR_MUT_W3_8,

    SPR_MUT_W4_1,SPR_MUT_W4_2,SPR_MUT_W4_3,SPR_MUT_W4_4,
    SPR_MUT_W4_5,SPR_MUT_W4_6,SPR_MUT_W4_7,SPR_MUT_W4_8,

    SPR_MUT_PAIN_1,SPR_MUT_DIE_1,SPR_MUT_DIE_2,SPR_MUT_DIE_3,
    SPR_MUT_PAIN_2,SPR_MUT_DIE_4,SPR_MUT_DEAD,

    SPR_MUT_SHOOT1,SPR_MUT_SHOOT2,SPR_MUT_SHOOT3,SPR_MUT_SHOOT4,

//
// officer
//
    SPR_OFC_S_1,SPR_OFC_S_2,SPR_OFC_S_3,SPR_OFC_S_4,
    SPR_OFC_S_5,SPR_OFC_S_6,SPR_OFC_S_7,SPR_OFC_S_8,

    SPR_OFC_W1_1,SPR_OFC_W1_2,SPR_OFC_W1_3,SPR_OFC_W1_4,
    SPR_OFC_W1_5,SPR_OFC_W1_6,SPR_OFC_W1_7,SPR_OFC_W1_8,

    SPR_OFC_W2_1,SPR_OFC_W2_2,SPR_OFC_W2_3,SPR_OFC_W2_4,
    SPR_OFC_W2_5,SPR_OFC_W2_6,SPR_OFC_W2_7,SPR_OFC_W2_8,

    SPR_OFC_W3_1,SPR_OFC_W3_2,SPR_OFC_W3_3,SPR_OFC_W3_4,
    SPR_OFC_W3_5,SPR_OFC_W3_6,SPR_OFC_W3_7,SPR_OFC_W3_8,

    SPR_OFC_W4_1,SPR_OFC_W4_2,SPR_OFC_W4_3,SPR_OFC_W4_4,
    SPR_OFC_W4_5,SPR_OFC_W4_6,SPR_OFC_W4_7,SPR_OFC_W4_8,

    SPR_OFC_PAIN_1,SPR_OFC_DIE_1,SPR_OFC_DIE_2,SPR_OFC_DIE_3,
    SPR_OFC_PAIN_2,SPR_OFC_DIE_4,SPR_OFC_DEAD,

    SPR_OFC_SHOOT1,SPR_OFC_SHOOT2,SPR_OFC_SHOOT3,

#ifndef SPEAR
//
// ghosts
//
    SPR_BLINKY_W1,SPR_BLINKY_W2,SPR_PINKY_W1,SPR_PINKY_W2,
    SPR_CLYDE_W1,SPR_CLYDE_W2,SPR_INKY_W1,SPR_INKY_W2,

//
// hans
//
    SPR_BOSS_W1,SPR_BOSS_W2,SPR_BOSS_W3,SPR_BOSS_W4,
    SPR_BOSS_SHOOT1,SPR_BOSS_SHOOT2,SPR_BOSS_SHOOT3,SPR_BOSS_DEAD,

    SPR_BOSS_DIE1,SPR_BOSS_DIE2,SPR_BOSS_DIE3,

//
// schabbs
//
    SPR_SCHABB_W1,SPR_SCHABB_W2,SPR_SCHABB_W3,SPR_SCHABB_W4,
    SPR_SCHABB_SHOOT1,SPR_SCHABB_SHOOT2,

    SPR_SCHABB_DIE1,SPR_SCHABB_DIE2,SPR_SCHABB_DIE3,SPR_SCHABB_DEAD,
    SPR_HYPO1,SPR_HYPO2,SPR_HYPO3,SPR_HYPO4,

//
// fake
//
    SPR_FAKE_W1,SPR_FAKE_W2,SPR_FAKE_W3,SPR_FAKE_W4,
    SPR_FAKE_SHOOT,SPR_FIRE1,SPR_FIRE2,

    SPR_FAKE_DIE1,SPR_FAKE_DIE2,SPR_FAKE_DIE3,SPR_FAKE_DIE4,
    SPR_FAKE_DIE5,SPR_FAKE_DEAD,

//
// hitler
//
    SPR_MECHA_W1,SPR_MECHA_W2,SPR_MECHA_W3,SPR_MECHA_W4,
    SPR_MECHA_SHOOT1,SPR_MECHA_SHOOT2,SPR_MECHA_SHOOT3,SPR_MECHA_DEAD,

    SPR_MECHA_DIE1,SPR_MECHA_DIE2,SPR_MECHA_DIE3,

    SPR_HITLER_W1,SPR_HITLER_W2,SPR_HITLER_W3,SPR_HITLER_W4,
    SPR_HITLER_SHOOT1,SPR_HITLER_SHOOT2,SPR_HITLER_SHOOT3,SPR_HITLER_DEAD,

    SPR_HITLER_DIE1,SPR_HITLER_DIE2,SPR_HITLER_DIE3,SPR_HITLER_DIE4,
    SPR_HITLER_DIE5,SPR_HITLER_DIE6,SPR_HITLER_DIE7,

//
// giftmacher
//
    SPR_GIFT_W1,SPR_GIFT_W2,SPR_GIFT_W3,SPR_GIFT_W4,
    SPR_GIFT_SHOOT1,SPR_GIFT_SHOOT2,

    SPR_GIFT_DIE1,SPR_GIFT_DIE2,SPR_GIFT_DIE3,SPR_GIFT_DEAD,
#endif
//
// Rocket, smoke and small explosion
//
    SPR_ROCKET_1,SPR_ROCKET_2,SPR_ROCKET_3,SPR_ROCKET_4,
    SPR_ROCKET_5,SPR_ROCKET_6,SPR_ROCKET_7,SPR_ROCKET_8,

    SPR_SMOKE_1,SPR_SMOKE_2,SPR_SMOKE_3,SPR_SMOKE_4,
    SPR_BOOM_1,SPR_BOOM_2,SPR_BOOM_3,

//
// Angel of Death's DeathSparks(tm)
//
#ifdef SPEAR
    SPR_HROCKET_1,SPR_HROCKET_2,SPR_HROCKET_3,SPR_HROCKET_4,
    SPR_HROCKET_5,SPR_HROCKET_6,SPR_HROCKET_7,SPR_HROCKET_8,

    SPR_HSMOKE_1,SPR_HSMOKE_2,SPR_HSMOKE_3,SPR_HSMOKE_4,
    SPR_HBOOM_1,SPR_HBOOM_2,SPR_HBOOM_3,

    SPR_STAT_SNOWYWELL,SPR_STAT_SPEARRACK_3D,SPR_STAT_WOODBARRIER_3D,SPR_STAT_RADIO,
#endif

#ifndef SPEAR
//
// gretel
//
    SPR_GRETEL_W1,SPR_GRETEL_W2,SPR_GRETEL_W3,SPR_GRETEL_W4,
    SPR_GRETEL_SHOOT1,SPR_GRETEL_SHOOT2,SPR_GRETEL_SHOOT3,SPR_GRETEL_DEAD,

    SPR_GRETEL_DIE1,SPR_GRETEL_DIE2,SPR_GRETEL_DIE3,

//
// fat face
//
    SPR_FAT_W1,SPR_FAT_W2,SPR_FAT_W3,SPR_FAT_W4,
    SPR_FAT_SHOOT1,SPR_FAT_SHOOT2,SPR_FAT_SHOOT3,SPR_FAT_SHOOT4,

    SPR_FAT_DIE1,SPR_FAT_DIE2,SPR_FAT_DIE3,SPR_FAT_DEAD,

//
// bj
//
#ifdef APOGEE_1_0
    SPR_BJ_W1=360,
#elif defined(APOGEE_1_1) && defined(UPLOAD)
    SPR_BJ_W1=406,
#else
    SPR_BJ_W1,
#endif
    SPR_BJ_W2,SPR_BJ_W3,SPR_BJ_W4,
    SPR_BJ_JUMP1,SPR_BJ_JUMP2,SPR_BJ_JUMP3,SPR_BJ_JUMP4,
#else
//
// THESE ARE FOR 'SPEAR OF DESTINY'
//

//
// Trans Grosse
//
    SPR_GRAU_W1,SPR_GRAU_W2,SPR_GRAU_W3,SPR_GRAU_W4,
    SPR_GRAU_SHOOT1,SPR_GRAU_SHOOT2,SPR_GRAU_SHOOT3,SPR_GRAU_DEAD,

    SPR_GRAU_DIE1,SPR_GRAU_DIE2,SPR_GRAU_DIE3,

//
// Wilhelm
//
    SPR_OTTO_W1,SPR_OTTO_W2,SPR_OTTO_W3,SPR_OTTO_W4,
    SPR_OTTO_SHOOT1,SPR_OTTO_SHOOT2,SPR_OTTO_SHOOT3,SPR_OTTO_SHOOT4,

    SPR_OTTO_DIE1,SPR_OTTO_DIE2,SPR_OTTO_DIE3,SPR_OTTO_DEAD,

//
// UberMutant
//
    SPR_SUPERCLONE_W1,SPR_SUPERCLONE_W2,SPR_SUPERCLONE_W3,SPR_SUPERCLONE_W4,
    SPR_SUPERCLONE_SHOOT1,SPR_SUPERCLONE_SHOOT2,SPR_SUPERCLONE_SHOOT3,SPR_SUPERCLONE_SHOOT4,

    SPR_SUPERCLONE_DIE1,SPR_SUPERCLONE_DIE2,SPR_SUPERCLONE_DIE3,SPR_SUPERCLONE_DIE4,
    SPR_SUPERCLONE_DEAD,

//
// Death Knight
//
    SPR_BARON_W1,SPR_BARON_W2,SPR_BARON_W3,SPR_BARON_W4,
    SPR_BARON_SHOOT1,SPR_BARON_SHOOT2,SPR_BARON_SHOOT3,SPR_BARON_SHOOT4,

    SPR_BARON_DIE1,SPR_BARON_DIE2,SPR_BARON_DIE3,SPR_BARON_DIE4,
    SPR_BARON_DIE5,SPR_BARON_DIE6,SPR_BARON_DEAD,

//
// Ghost
//
    SPR_STAT_BARBF_3D,SPR_STAT_BARBS_3D,SPR_STAT_IRONGATE_3D,SPR_STAT_METALGRATE_3D,
    SPR_STAT_GOODBERRY,SPR_STAT_BADBERRY,SPR_STAT_BRNBRKA_3DNS,SPR_STAT_BRNBRKA_3DEW,

//
// Angel of Death
//
    SPR_ANGEL_W1,SPR_ANGEL_W2,SPR_ANGEL_W3,SPR_ANGEL_W4,
    SPR_ANGEL_SHOOT1,SPR_ANGEL_SHOOT2,SPR_ANGEL_TIRED1,SPR_ANGEL_TIRED2,

    SPR_ANGEL_DIE1,SPR_ANGEL_DIE2,SPR_ANGEL_DIE3,SPR_ANGEL_DIE4,
    SPR_ANGEL_DIE5,SPR_ANGEL_DIE6,SPR_ANGEL_DIE7,SPR_ANGEL_DEAD,

#endif

//
// player attack frames
//
    SPR_KNIFEREADY,SPR_KNIFEATK1,SPR_KNIFEATK2,SPR_KNIFEATK3,
    SPR_KNIFEATK4,

    SPR_PISTOLREADY,SPR_PISTOLATK1,SPR_PISTOLATK2,SPR_PISTOLATK3,
    SPR_PISTOLATK4,

    SPR_MACHINEGUNREADY,SPR_MACHINEGUNATK1,SPR_MACHINEGUNATK2,MACHINEGUNATK3,
    SPR_MACHINEGUNATK4,

    SPR_CHAINREADY,SPR_CHAINATK1,SPR_CHAINATK2,SPR_CHAINATK3,
    SPR_CHAINATK4,

    // BEGIN EXTRA

    SPR_MK4READY,SPR_MK4ATK1,SPR_MK4ATK2,SPR_MK4ATK3,SPR_MK4ATK4,

    SPR_STAT_MK4,SPR_STAT_BLOODKEY,SPR_STAT_ELEKEY,SPR_STAT_LANDMINE,
    SPR_STAT_COLUMNDEAD,SPR_STAT_SECAMMOCLIP,SPR_STAT_BLACKCOLUMN,SPR_STAT_BUTCHERTABLE,
    SPR_STAT_BLACKDIAMOND,SPR_STAT_ROCKS1,SPR_STAT_ROCKS2,SPR_STAT_BLOODYSKELE,
    SPR_STAT_HANGBLOODSKULL,
    
    // Directional sprites
    SPR_STAT_SOF_3DNS,SPR_STAT_SOF_3DEW,

    SPR_STAT_LIGHTDEAD,
    
    // Directional sprites
    SPR_STAT_BRNBRKW_3DNS, SPR_STAT_BRNBRKW_3DEW,

    SPR_STAT_HANGPLANT,SPR_STAT_HITLERSTATUE,SPR_STAT_REDBARREL,
    SPR_BARRELEXP1,SPR_BARRELEXP2,SPR_BARRELEXP3,SPR_BARRELDEAD,

    // Animated outdoor torch
    SPR_STAT_OTORCH1,SPR_STAT_OTORCH2,SPR_STAT_OTORCH3,SPR_STAT_OTORCH4,

    SPR_STAT_HANGCLEANSKULL,SPR_STAT_SKULLPOLE1,SPR_STAT_SKULLPOLE2,

    //Animated indoor torch
    SPR_STAT_ITORCH1,SPR_STAT_ITORCH2,SPR_STAT_ITORCH3,SPR_STAT_ITORCH4,

    SPR_STAT_SECAMMOBOX,

    // Column breaking animation
    SPR_COLUMNBREAK1,SPR_COLUMNBREAK2,SPR_COLUMNBREAK3,SPR_COLUMNBREAK4,

    // Silenced rifle
    SPR_RIFLEREADY,SPR_RIFLEATK1,SPR_RIFLEATK2,SPR_RIFLEATK3,SPR_RIFLEATK4,

    // Launcher
    SPR_LAUNCHERREADY,SPR_LAUNCHERATK1,SPR_LAUNCHERATK2,SPR_LAUNCHERATK3,SPR_LAUNCHERATK4,

    // Directional stuff
    SPR_STAT_FLAG_3D,SPR_STAT_RAIL_3D,

    SPR_STAT_SNOWYPINE,SPR_STAT_PEDVASE,SPR_STAT_DESK1,SPR_STAT_FLAG1,
    SPR_STAT_BARETREE,SPR_STAT_WELLFULL,SPR_STAT_ROCKS3,SPR_STAT_BONES1,
    SPR_STAT_CLEANSKULL,SPR_STAT_BLOODYSKULL,SPR_STAT_POISON,SPR_STAT_TELE,

    // Animated newlife
    SPR_STAT_NEWLIFE1,SPR_STAT_NEWLIFE2,SPR_STAT_NEWLIFE3,SPR_STAT_NEWLIFE4,

    SPR_STAT_BACKPACK,

    SPR_STAT_STEEL_3DNS,SPR_STAT_STEEL_3DEW,

    SPR_STAT_UNIPOLE,SPR_STAT_SKULLS,SPR_STAT_SKELE2,SPR_STAT_RIFLE,
    SPR_STAT_LAUNCHER,SPR_STAT_WELLEMPTY,SPR_STAT_RUSTBARREL,SPR_STAT_SPENTBOX,
    SPR_STAT_VINES,SPR_STAT_RIFLEAMMO,SPR_STAT_LAUNCHERAMMOBOX,SPR_STAT_PISTOL,

    // TURRET
    SPR_TURRET_S1,SPR_TURRET_S2,SPR_TURRET_S3,SPR_TURRET_S4,
    SPR_TURRET_S5,SPR_TURRET_S6,SPR_TURRET_S7,SPR_TURRET_S8,
    SPR_TURRET_FIRE,

    SPR_STAT_HATPOLE,SPR_STAT_CANDLES,

    SPR_TURRET_DIE1,SPR_TURRET_DIE2,SPR_TURRET_DIE3,SPR_TURRET_DIE4,

    SPR_STAT_WEB1_3D,
    
    // Animated bonus objects
    SPR_STAT_SAVEGAME1,SPR_STAT_STAT_SAVEGAME2,SPR_STAT_SAVEGAME3,
    SPR_STAT_SAVEGAME4,

    SPR_STAT_FURY1,SPR_STAT_FURY2,SPR_STAT_FURY3,SPR_STAT_FURY4,
    SPR_STAT_DEATH1,SPR_STAT_DEATH2,SPR_STAT_DEATH3,SPR_STAT_DEATH4,

    SPR_STAT_TELE1,SPR_STAT_TELE2,SPR_STAT_TELE3,SPR_STAT_TELE4,
    SPR_STAT_GOD1,SPR_STAT_GOD2,SPR_STAT_GOD3,SPR_STAT_GOD4,

    // Flame enemy?
    SPR_FLAME1,SPR_FLAME2,SPR_FLAME3,SPR_FLAME4,
    SPR_FLAME_DIE1,SPR_FLAME_DIE2,SPR_FLAME_DIE3,SPR_FLAME_DIE4,

    SPR_STAT_WEB2_3D,SPR_STAT_MINESIGN_3D,SPR_STAT_CONCRETE_3DNS,SPR_STAT_CONCRETE_3DEW,

    SPR_FLAME2_DIE1,SPR_FLAME2_DIE2,SPR_FLAME2_DIE3,SPR_FLAME2_DIE4,

    SPR_STAT_REGEN1,SPR_STAT_REGEN2,SPR_STAT_REGEN3,SPR_STAT_REGEN4,

    SPR_STAT_FLAME1,SPR_STAT_FLAME2,SPR_STAT_FLAME3,SPR_STAT_FLAME4,

    // Ultra clone
    SPR_ULTRACLONE_W1,SPR_ULTRACLONE_W2,SPR_ULTRACLONE_W3,SPR_ULTRACLONE_W4,
    SPR_ULTRACLONE_AIM,SPR_ULTRACLONE_SHOOT1,SPR_ULTRACLONE_SHOOT2,
    SPR_ULTRACLONE_DIE1,SPR_ULTRACLONE_DIE2,SPR_ULTRACLONE_DIE3,SPR_ULTRACLONE_DIE4,
    SPR_ULTRACLONE_DEAD,

    // Heinrich
    SPR_HEINRICH_W1,SPR_HEINRICH_W2,SPR_HEINRICH_W3,SPR_HEINRICH_W4,
    SPR_HEINRICH_AIM,SPR_HEINRICH_SHOOT1,SPR_HEINRICH_SHOOT2,SPR_HEINRICH_SHOOT3,
    SPR_HEINRICH_DIE1,SPR_HEINRICH_DIE2,SPR_HEINRICH_DIE3,SPR_HEINRICH_DIE4,
    SPR_HEINRICH_DIE5,SPR_HEINRICH_DEAD,

    // Spear weapon
    SPR_SPEARREADY,SPR_SPEARATK1,SPR_SPEARATK2,SPR_SPEARATK3,SPR_SPEARATK4,

    // Fireball
    SPR_FIREBALL_1,SPR_FIREBALL_2,SPR_FIREBALL_3,SPR_FIREBALL_4,
    SPR_FIREBALL_5,SPR_FIREBALL_6,SPR_FIREBALL_7,SPR_FIREBALL_8,

    SPR_FIREBALL_TRAIL1,SPR_FIREBALL_TRAIL2,SPR_FIREBALL_TRAIL3,SPR_FIREBALL_TRAIL4,
    SPR_FIREBALL_EXP1,SPR_FIREBALL_EXP2,SPR_FIREBALL_EXP3,

    SPR_EXPLOSIVEDEATH1,SPR_EXPLOSIVEDEATH2,SPR_EXPLOSIVEDEATH3,SPR_EXPLOSIVEDEATH4,
    SPR_FIREYDEATH1,SPR_FIREYDEATH2,SPR_FIREYDEATH3,

    SPR_STAT_PORTMED,SPR_TOILETCLONE,

    SPR_ROBOT_W1,SPR_ROBOT_W2,SPR_ROBOT_W3,SPR_ROBOT_W4,
    SPR_ROBOT_AIM,SPR_ROBOT_SHOOT1,SPR_ROBOT_SHOOT2,
    SPR_ROBOT_DIE1,SPR_ROBOT_DIE2,SPR_ROBOT_DIE3,SPR_ROBOT_DIE4,
    SPR_ROBOT_DEAD,

    SPR_STAT_SNOWBRICK_3DNS,SPR_STAT_SNOWBRICK_3DEW,
    SPR_STAT_GREYBRICK_3DNS,SPR_STAT_GREYBRICK_3DEW,

    // SNOWMAN!!
    SPR_SNOWMAN_W1,SPR_SNOWMAN_W2,
    SPR_SNOWMAN_DIE1,SPR_SNOWMAN_DIE2,SPR_SNOWMAN_DIE3,SPR_SNOWMAN_DEAD,

    // Spear weapon projectile
    SPR_SPEARPROJ_1,SPR_SPEARPROJ_2,SPR_SPEARPROJ_3,SPR_SPEARPROJ_4,
    SPR_SPEARPROJ_5,SPR_SPEARPROJ_6,SPR_SPEARPROJ_7,SPR_SPEARPROJ_8,
    SPR_SPEARPROJ_TRAIL1,SPR_SPEARPROJ_TRAIL2,SPR_SPEARPROJ_TRAIL3,SPR_SPEARPROJ_TRAIL4,
    SPR_SPEARPROJ_EXP1,SPR_SPEARPROJ_EXP2,SPR_SPEARPROJ_EXP3,

    SPR_STAT_XMASPINE
};


/*
=============================================================================

                               GLOBAL TYPES

=============================================================================
*/

typedef enum {
    di_north,
    di_east,
    di_south,
    di_west
} controldir_t;

typedef enum {
    dr_normal,
    dr_lock1,
    dr_lock2,
    dr_lock3,
    dr_lock4,
    dr_elevator,
    dr_rust,
    dr_wood,
    dr_auto
} door_t;

typedef enum {
    ac_badobject = -1,
    ac_no,
    ac_yes,
    ac_allways
} activetype;

typedef enum {
    nothing,
    playerobj,
    inertobj,
    guardobj,
    officerobj,
    ssobj,
    suicideobj,
    mutantobj,
    ghostobj,
    rocketobj,

    spectreobj,
    angelobj,
    grauobj,
    supercloneobj,
    ottoobj,
    baronobj,
    hrocketobj,
    sparkobj,
    ultracloneobj,
    robotobj,
    heinrichobj,
    turretobj,
    barrelobj,
    columnobj,
    snowmanobj,
    fireballobj,
    spearprojobj,
    toiletobj
} classtype;

typedef enum {
    none,
    block,
    bo_gibs,
    bo_alpo,
    bo_firstaid,
    bo_key1,
    bo_key2,
    bo_key3,
    bo_key4,
    bo_cross,
    bo_chalice,
    bo_bible,
    bo_crown,
    bo_clip,
    bo_clip2,
    bo_machinegun,
    bo_mk3,
    bo_food,
    bo_fullheal,
    bo_25clip,
    bo_spear,
    bo_sclip,
    bo_diamond,
    bo_s25clip,
    bo_mk4,
    bo_landmine,
    bo_backpack,
    bo_goodberry,
    bo_badberry,
    bo_poison,
    bo_pistol,
    bo_rifle,
    bo_launcher,
    bo_rifleclip,
    bo_rocketbox,
    bo_save,
    bo_fury,
    bo_death,
    bo_god,
    bo_portmed,
	bo_secret,
	bo_supersecret,
	bo_grdspn1,
	bo_grdspn2,
	bo_ssspn1,
	bo_ssspn2,
	bo_ofcspn1,
	bo_ofcspn2
    // TODO: add drop clips for ustom ammo?
} wl_stat_t;

typedef enum {
    east,
    northeast,
    north,
    northwest,
    west,
    southwest,
    south,
    southeast,
    nodir
} dirtype;


#define NUMENEMIES  29
typedef enum {
    en_guard,
    en_officer,
    en_ss,
    en_suicide,
#ifndef SPEAR
    en_boss,
    en_schabbs,
    en_fake,
    en_hitler,
#endif
    en_mutant,
#ifndef SPEAR
    en_blinky,
    en_clyde,
    en_pinky,
    en_inky,
    en_gretel,
    en_gift,
    en_fat,
#endif
    en_spectre, // flame
    en_angel,
    en_grau,
    en_superclone,
    en_otto,
    en_baron,
    en_ultraclone,
    en_robot,
    en_heinrich,
    en_turret,
    en_barrel,
    en_column,
    en_snowman
} enemy_t;

typedef void (* statefunc) (void *);

typedef struct statestruct
{
    boolean rotate;
    short   shapenum;           // a shapenum of -1 means get from ob->temp1
    short   tictime;
    void    (*think) (void *),(*action) (void *);
    struct  statestruct *next;
    boolean fullbright;
} statetype;


//---------------------
//
// trivial actor structure
//
//---------------------

typedef struct statstruct
{
    byte      tilex,tiley;
    short     shapenum;           // if shapenum == -1 the obj has been removed
    byte      *visspot;
    uint32_t  flags;
    byte      itemnumber;
    byte      numruns;          // death artifact
} statobj_t;

//---------------------
//
// door actor structure
//
//---------------------

typedef enum
{
    dr_open,dr_closed,dr_opening,dr_closing
} doortype;

typedef struct doorstruct
{
    byte     tilex,tiley;
    boolean  vertical;
    byte     lock;
    doortype action;
    short    ticcount;
} doorobj_t;

//--------------------
//
// thinking actor structure
//
//--------------------

typedef struct objstruct
{
    activetype  active;
    short       ticcount;
    classtype   obclass;
    classtype   creator; // projectile checker
    statetype   *state;

    uint32_t    flags;              // FL_SHOOTABLE, etc

    int32_t     distance;           // if negative, wait for that door to open
    dirtype     dir;
    dirtype     olddir;

    fixed       x,y;
    word        tilex,tiley;
    byte        areanumber;

    short       viewx;
    word        viewheight;
    fixed       transx,transy;      // in global coord

    short       angle;
    short       hitpoints;
    int32_t     speed;

    short       temp1,temp2,hidden;
    struct objstruct *next,*prev;
} objtype;

enum
{
    bt_nobutton=-1,
    bt_attack=0,
    bt_strafe,
    bt_altspeed,
    bt_use,
    bt_readyknife,
    bt_readypistol,
    bt_readymachinegun,
    bt_readychaingun,
    bt_readymk4,
    bt_readyrifle,
    bt_readylauncher,
    bt_readymedkit,
    bt_nextweapon,
    bt_prevweapon,
    bt_esc,
    bt_pause,
    bt_strafeleft,
    bt_straferight,
    bt_moveforward,
    bt_movebackward,
    bt_turnleft,
    bt_turnright,
    NUMBUTTONS
};


#define NUMWEAPONS      9
typedef enum
{
    wp_knife,
    wp_pistol,
    wp_machinegun,
    wp_mk3,
    wp_mk4,
    wp_rifle,
    wp_launcher,
    wp_spear,
    wp_medkit
} weapontype;

#define WP_KNIFE 1
#define WP_PISTOL 2
#define WP_MACHINEGUN 4
#define WP_MK3 8
#define WP_MK4 16
#define WP_RIFLE 32
#define WP_LAUNCHER 64
#define WP_SPEAR 128

enum
{
    gd_baby,
    gd_easy,
    gd_medium,
    gd_hard,
    gd_extreme
};

//---------------
//
// gamestate structure
//
//---------------

#define MAXPRIMARYAMMO 100
#define MAXSECONDARYAMMO 100
#define MAXRIFLEAMMO 18
#define MAXROCKETAMMO 10
#define MAXMEDKITS 2

#define MAXPRIMARYAMMO_B MAXPRIMARYAMMO<<1
#define MAXSECONDARYAMMO_B MAXSECONDARYAMMO<<1
#define MAXRIFLEAMMO_B MAXRIFLEAMMO<<1
#define MAXROCKETAMMO_B MAXROCKETAMMO<<1
#define MAXMEDKITS_B MAXMEDKITS<<1

#define GODMODELENGTH 30
#define FURYLENGTH 30

#define RELOADDELAY 70

// Kyle: Multiple Timers
typedef struct {
    short type;
    int color;
    int32_t tics;
} timer_t;

typedef struct
{
    short       difficulty;
    short       mapon;
    int32_t     oldscore,score,nextextra;
    short       lives;
    short       health;
    short       primaryammo;
    short       secondaryammo;
    short       rifleammo;
    short       rocketammo;
    short       medkits;
    short       keys;
    boolean     backpack;
    weapontype  weapon,nextweapon,lastweapon,lastweaponfury;
    boolean     changingweapon;
    boolean     goingdown;
    int         weapchange;
    int         reloadtics;

    short       weapons;

    // handles multiple timers
    byte        activetimers;
    timer_t     timers[4];

   // boolean     lightson; // special thing for level 35
    
    // Power up status
    boolean     godmode;
    boolean     godcheat;
    int         godmodetics;
    boolean     fury;
    boolean     furycheat;
    int         furytics;

    int32_t     levelTimer; // level 34 timer

    short       weatherpoints;
    short       weatherchecktics;
    short       weatherdelaytics;
    short       weatherchange;

    short       faceframe;
    short       attackframe,attackcount,weaponframe;

    short       ratiolevels; // for correct "Victory" ratio calculations

    short       episode,secretcount,treasurecount,killcount,
                secrettotal,treasuretotal,killtotal;
    int32_t     TimeCount;
    int32_t     killx,killy;
    boolean     victoryflag;            // set during victory animations
    char        message[40];

    boolean lightson;
    short       saves;

#ifdef EODSPECIAL
    boolean gong;
    short enemytreasurecount;
#endif

    boolean cheated;
} gametype;

/*
 * Feature flags for levels
 */
typedef struct {
    short shadestr;
    uint8_t shadeR;
    uint8_t shadeG;
    uint8_t shadeB;
    short weather;
    short parallax;
    short secretlevel;
    short backtolevel;
    short ceiling1;
    short ceiling2;
    short floor1;
    short floor2;
    short parMin;
    short parSec;
    short timerMin;
    short timerSec;
    short readthischunk;
    short music;
} levelinfo_t;

extern levelinfo_t levelinfo;

#ifdef NEWMENU
typedef struct {
    byte textures;
    byte shading;
    byte weather;
    byte clouds;
    byte parallax;
    byte messages;
    byte strafe;
    byte alwaysrun;
} switchtype;

extern switchtype switches;
#endif

typedef enum
{
    ex_stillplaying,
    ex_completed,
    ex_died,
    ex_warped,
    ex_resetgame,
    ex_loadedgame,
    ex_victorious,
    ex_abort,
    ex_demodone,
    ex_secretlevel
} exit_t;


extern word *mapsegs[MAPPLANES];
extern int mapon;

/*
=============================================================================

                             WL_MAIN DEFINITIONS

=============================================================================
*/

extern  boolean  loadedgame;
extern  fixed    focallength;
extern  int      viewscreenx, viewscreeny;
extern  int      viewwidth;
extern  int      viewheight;
extern  int      halfviewheight;
extern  int      halfviewwidth;
extern  int      gu_global;
extern  int      gv_global;

extern  int      msgPrintX;
extern  int      msgPrintY;
extern  int      timerPrintX;
extern  int      timerPrintY;
extern  int      ratioPrintX;
extern  int      ratioPrintY;
extern  short    centerx;
extern  int32_t  heightnumerator;
extern  fixed    scale;

extern  int      dirangle[9];

extern  int      mouseadjustment;
extern  int      shootdelta;
extern  unsigned screenofs;

extern  boolean  startgame;
extern  char     str[80];
extern  char     configdir[256];
extern  char     configname[13];

//
// Command line parameter variables
//
extern  boolean  param_debugmode;
extern  boolean  param_nowait;
extern  int      param_difficulty;
extern  int      param_tedlevel;
extern  int      param_joystickindex;
extern  int      param_joystickhat;
extern  int      param_samplerate;
extern  int      param_audiobuffer;
extern  int      param_mission;
extern  boolean  param_goodtimes;
extern  boolean  param_ignorenumchunks;


void            NewGame (int difficulty,int episode);
void            CalcProjection (int32_t focal);
void            NewViewSize (int width);
boolean         SetViewSize (unsigned width, unsigned height);
boolean         LoadTheGame(FILE *file,int x,int y);
boolean         SaveTheGame(FILE *file,int x,int y);
void            ShowViewSize (int width);
void            ShutdownId (void);


/*
=============================================================================

                         WL_GAME DEFINITIONS

=============================================================================
*/

extern  gametype        gamestate;
extern  byte            bordercol;
extern  SDL_Surface     *latchpics[NUMLATCHPICS];
extern  char            demoname[13];

void    SetupGameLevel (void);
void    GameLoop (void);
void    DrawPlayBorder (void);
void    DrawStatusBorder (byte color);
void    DrawPlayScreen (void);
void    DrawPlayBorderSides (void);
void    DrawFullscreenHUD(void);

void    ResetGamestate(void);

void    PlayDemo (int demonumber);
void    RecordDemo (void);


#ifdef SPEAR
extern  int32_t            spearx,speary;
extern  unsigned        spearangle;
extern  boolean         spearflag;
#endif


#define ClearMemory SD_StopDigitized


// JAB
#define PlaySoundLocTile(s,tx,ty)       PlaySoundLocGlobal(s,(((int32_t)(tx) << TILESHIFT) + (1L << (TILESHIFT - 1))),(((int32_t)ty << TILESHIFT) + (1L << (TILESHIFT - 1))))
#define PlaySoundLocActor(s,ob)         PlaySoundLocGlobal(s,(ob)->x,(ob)->y)
void    PlaySoundLocGlobal(word s,fixed gx,fixed gy);
void UpdateSoundLoc(void);
void ResetFWalls(void);


/*
=============================================================================

                            WL_PLAY DEFINITIONS

=============================================================================
*/

#define BASEMOVE                35
#define BASEMOVEDIAG            24

#define RUNMOVE                 70
#define RUNMOVEDIAG             49
#define BASETURN                35
#define RUNTURN                 70

#define JOYSCALE                2

extern  uint16_t            tilemap[MAPSIZE][MAPSIZE];      // wall values only
extern  byte            spotvis[MAPSIZE][MAPSIZE];
#ifdef REVEALMAP
extern byte             spotsaw[MAPSIZE][MAPSIZE];
#endif
extern  objtype         *actorat[MAPSIZE][MAPSIZE];
extern  statobj_t       *statat[MAPSIZE][MAPSIZE];

extern  objtype         *player;

extern  int        tics;
extern  int             viewsize;

extern  int             lastgamemusicoffset;

extern int DebugOk;

//
// current user input
//
extern  int         controlx,controly,controlh;              // range from -100 to 100
extern  boolean     buttonstate[NUMBUTTONS];
extern  objtype     objlist[MAXACTORS];
extern  boolean     buttonheld[NUMBUTTONS];
extern  exit_t      playstate;
extern  boolean     madenoise;
extern  statobj_t   statobjlist[MAXSTATS];
extern  statobj_t   *laststatobj;
extern  objtype     *newobj,*killerobj;
extern  doorobj_t   doorobjlist[MAXDOORS];
extern  doorobj_t   *lastdoorobj;

extern  boolean     demorecord,demoplayback;
extern  int8_t      *demoptr, *lastdemoptr;
extern  memptr      demobuffer;

//
// control info
//
extern  boolean     mouseenabled,joystickenabled;
extern  int         dirscan[4];
extern  int         buttonscan[NUMBUTTONS];
#ifdef MOUSEWHEEL
extern  int         buttonmouse[5];
#else
extern  int         buttonmouse[4];
#endif
extern  int         buttonjoy[32];

void    InitActorList (void);
void    GetNewActor (void);
void    PlayLoop (void);

void    CenterWindow(word w,word h);

void    InitRedShifts (void);
void    FinishPaletteShifts (void);

void    RemoveObj (objtype *gone);
void    PollControls (void);
int     StopMusic(void);
void    StartMusic(void);
void    ContinueMusic(int offs);
void    StartDamageFlash (int damage);
void    StartBonusFlash (void);

#ifdef SPEAR
extern  int32_t     funnyticount;           // FOR FUNNY BJ FACE
#endif

extern  objtype     *objfreelist;     // *obj,*player,*lastobj,

extern  boolean     noclip,ammocheat;
extern  int         singlestep, extravbls;
extern byte viewingmap;
extern boolean ratiocheat;

/*
=============================================================================

                                WL_INTER

=============================================================================
*/

void IntroScreen (void);
void PG13(void);
void DrawHighScores(void);
void CheckHighScore (int32_t score,word other);
void Victory (void);
void LevelCompleted (void);
void ClearSplitVWB (void);

extern char mapname[17];
void PreloadGraphics(void);


/*
=============================================================================

                                WL_DEBUG

=============================================================================
*/

int DebugKeys (void);
void ViewMap(byte exposed);
#define MAXMAPSIGHT 10

/*
=============================================================================

                            WL_DRAW DEFINITIONS

=============================================================================
*/

//
// math tables
//
extern  short *pixelangle;
extern  int32_t finetangent[FINEANGLES/4];
extern  fixed sintable[];
extern  fixed *costable;
extern  int *wallheight;
extern  word horizwall[],vertwall[];
extern  int32_t    lasttimecount;
extern  int32_t    frameon;
extern  int messagetime;
void   GetMessage (char *lastmessage, int color); // WSJ's message feature
void   DrawMessage (void);    
void    GetTimer(int seconds, int eventtype);
void    DrawTimer(void);
extern  int lastsec;
extern  boolean fs_drawflag;

#define DEF_MSG_CLR 0x35
#define SPC_MSG_CLR 0x45

#define LVL_TMR_CLR 0xB8
#define GOD_TMR_CLR 0x45
#define FUR_TMR_CLR 0x35

extern  unsigned screenloc[3];

extern  boolean fizzlein, fpscounter;

extern  fixed   viewx,viewy;                    // the focal point
extern  fixed   viewsin,viewcos;

void    ThreeDRefresh (void);
void    CalcTics (void);

typedef struct
{
    word leftpix,rightpix;
    word dataofs[64];
// table data after dataofs[rightpix-leftpix+1]
} t_compshape;

/*
=============================================================================

                             WL_STATE DEFINITIONS

=============================================================================
*/
#define TURNTICS        10
#define SPDPATROL       512

void    InitHitRect (objtype *ob, unsigned radius);
void    SpawnNewObj (unsigned tilex, unsigned tiley, statetype *state);
void    NewState (objtype *ob, statetype *state);

boolean TryWalk (objtype *ob);
void    SelectChaseDir (objtype *ob);
void    SelectDodgeDir (objtype *ob);
void    SelectRunDir (objtype *ob);
#ifdef EODSPECIAL
void    SelectLootDir (objtype *ob);
#endif
void    MoveObj (objtype *ob, int32_t move);
boolean SightPlayer (objtype *ob);

void    KillActor (objtype *ob);
void    DamageActor (objtype *ob, unsigned damage);

boolean CheckLine (objtype *ob);
boolean CheckSight (objtype *ob);
boolean CheckLine2 (objtype *ob1, objtype *ob2);

/*
=============================================================================

                             WL_AGENT DEFINITIONS

=============================================================================
*/

#define NEXTWEAPON(w) gamestate.nextweapon=w; gamestate.changingweapon = gamestate.goingdown = true

extern  short    anglefrac;
extern  int      facecount, facetimes;
extern  word     plux,pluy;         // player coordinates scaled to unsigned
extern  int32_t  thrustspeed;
extern  objtype  *LastAttacker;

extern int warpex,warpey,warpwx,warpwy; // teleporter stuff

void SwitchToNextWeapon(void);
void SwitchToPreviousWeapon(void);

void    Thrust (int angle, int32_t speed);
void    SpawnPlayer (int tilex, int tiley, int dir);
void    TakeDamage (int points,objtype *attacker);
void    GivePoints (int32_t points);
void    GetBonus (statobj_t *check);
void    GiveWeapon (weapontype newWeapon);
void    GiveAmmo (byte type, short amount);
void    GiveKey (int key);

//
// player state info
//

void    StatusDrawFace(unsigned picnum);
void    DrawFace (void);
void    DrawHealth (void);
void    HealSelf (int points);
void    DrawLevel (void);
void    DrawLives (void);
void    GiveExtraMan (void);
void    DrawScore (void);
void    DrawWeapon (void);
void    DrawKeys (void);
void    DrawAmmo (void);

int     GetPlayerAngle(void);


/*
=============================================================================

                             WL_ACT1 DEFINITIONS

=============================================================================
*/

extern  doorobj_t doorobjlist[MAXDOORS];
extern  doorobj_t *lastdoorobj;
extern  short     doornum;

extern  word      doorposition[MAXDOORS];

extern  byte      areaconnect[NUMAREAS][NUMAREAS];

extern  boolean   areabyplayer[NUMAREAS];

typedef struct
{
    bool     active;
    word     state;
    word     pos;        // amount a pushable wall has been moved (0-63)
    word     x;
    word     y;
    word    dir;
    word     tile;
} pwall_t;

#define MAXPWALLS 32

extern pwall_t  pwall[MAXPWALLS];
extern short     pw;

// Defines for the purpose of simplifying the draw loop
#define pwallstate  (pwall[pw].state)
#define pwallpos    (pwall[pw].pos)
#define pwallx      (pwall[pw].x)
#define pwally      (pwall[pw].y)
#define pwalldir    (pwall[pw].dir)
#define pwalltile   (pwall[pw].tile) 

#define MAXFIREWALLS 8
#define FIREWALLDELAY 45 // in tics (70 tics = 1 sec)
typedef struct {
    bool active; // whether or not the fire wall is on/off
    int tics;    // keeps track of the current tic count for delayed firing
    int tilex;   // the tilex of the shooting wall
    int tiley;   // the tiley of the shooting wall
} fwall_t;

extern fwall_t fwall[MAXFIREWALLS];

void InitDoorList (void);
void InitStaticList (void);
void SpawnStatic (int tilex, int tiley, int type);
void SpawnDoor (int tilex, int tiley, boolean vertical, int lock);
void MoveDoors (void);
void MovePWalls (void);
void OpenDoor (int door);
void PlaceItemType (int itemtype, int tilex, int tiley);
void PushWall (int checkx, int checky, int dir);
void OperateDoor (int door);
void InitAreas (void);

/*
=============================================================================

                             WL_ACT2 DEFINITIONS

=============================================================================
*/

#define s_nakedbody s_static10

extern  statetype s_grddie1;
extern  statetype s_suicidedie1;
extern  statetype s_ofcdie1;
extern  statetype s_mutdie1;
extern  statetype s_ssdie1;
extern  statetype s_bossdie1;
extern  statetype s_schabbdie1;
extern  statetype s_fakedie1;
extern  statetype s_mechadie1;
extern  statetype s_hitlerdie1;
extern  statetype s_greteldie1;
extern  statetype s_giftdie1;
extern  statetype s_fatdie1;

extern  statetype s_spectredie1;
extern  statetype s_spectre2die1;
extern  statetype s_angeldie1;
extern  statetype s_graudie0;
extern  statetype s_superclonedie1;
extern  statetype s_ottodie1;
extern  statetype s_barondie1;
extern  statetype s_ultraclonedie1;
extern  statetype s_robotdie1;
extern  statetype s_heinrichdie1;
extern  statetype s_turretdie1;
extern  statetype s_barreldie1;
extern  statetype s_columndie1;
extern  statetype s_splatter1;
extern  statetype s_snowdie1;
extern statetype s_snowreallydead;


extern  statetype s_grdchase1;
extern  statetype s_suicidechase1;
extern  statetype s_ofcchase1;
extern  statetype s_sschase1;
extern  statetype s_mutchase1;
extern  statetype s_bosschase1;
extern  statetype s_schabbchase1;
extern  statetype s_fakechase1;
extern  statetype s_mechachase1;
extern  statetype s_gretelchase1;
extern  statetype s_giftchase1;
extern  statetype s_fatchase1;

extern  statetype s_spectrechase1;
extern  statetype s_spectre2chase1;
extern  statetype s_angelchase1;
extern  statetype s_grauchase1;
extern  statetype s_superclonechase1;
extern  statetype s_ottochase1;
extern  statetype s_baronchase1;
extern  statetype s_ultraclonechase1;
extern  statetype s_robotchase1;
extern  statetype s_heinrichchase1;
extern  statetype s_turretscan1;
extern statetype s_snowchase1;

extern  statetype s_blinkychase1;
extern  statetype s_hitlerchase1;

extern  statetype s_grdpain;
extern  statetype s_grdpain1;
extern  statetype s_ofcpain;
extern  statetype s_ofcpain1;
extern  statetype s_sspain;
extern  statetype s_sspain1;
extern  statetype s_mutpain;
extern  statetype s_mutpain1;

extern  statetype s_deathcam;

extern  statetype s_schabbdeathcam2;
extern  statetype s_hitlerdeathcam2;
extern  statetype s_giftdeathcam2;
extern  statetype s_fatdeathcam2;

void SpawnStand (enemy_t which, int tilex, int tiley, int dir);
void SpawnPatrol (enemy_t which, int tilex, int tiley, int dir);
dirtype GetCardinalDirection(objtype *ob);

void SpawnDeadGuy (enemy_t which, int tilex, int tiley);

void SpawnGrau (int tilex, int tiley);
void SpawnSuperClone (int tilex, int tiley);
void SpawnOtto (int tilex, int tiley);
void SpawnBaron (int tilex, int tiley);
void SpawnAngel (int tilex, int tiley);
void SpawnSpectre (int tilex, int tiley);
void SpawnSpectre2 (int tilex, int tiley);
void SpawnRobot (int tilex, int tiley);
void SpawnHeinrich (int tilex, int tiley);
void SpawnUltraClone (int tilex, int tiley);
void SpawnTurret (int tilex, int tiley);
void SpawnBarrel (int tilex, int tiley);
void SpawnColumn (int tilex, int tiley);
void SpawnBlinkingLight (int tilex, int tiley);
void SpawnToiletClone (int tilex, int tiley);
void SpawnSnowman (int tilex, int tiley);

void SpawnBoss (int tilex, int tiley);
void SpawnGretel (int tilex, int tiley);
void SpawnGhosts (int which, int tilex, int tiley);
void SpawnSchabbs (int tilex, int tiley);
void SpawnGift (int tilex, int tiley);
void SpawnFat (int tilex, int tiley);
void SpawnFakeHitler (int tilex, int tiley);
void SpawnHitler (int tilex, int tiley);

void A_DeathScream (objtype *ob);
void SpawnBJVictory (void);

void A_Explode(objtype *ob);
void SpawnTriggeredActors(enemy_t which, int targetTile);
void ShootFireballs(void);

void TakeRandomDamage(int x, int y);

#ifdef EODSPECIAL
void CheckGong(void);
#endif
/*
=============================================================================

                             WL_TEXT DEFINITIONS

=============================================================================
*/

extern  char    helpfilename[],endfilename[];

extern  void    HelpScreens(void);
extern  void    EndText(void);
extern  void    LevelText(int map);


/*
=============================================================================

                            DREAMCAST DEFINITIONS

=============================================================================
*/

#ifdef _arch_dreamcast

// defined in dc_main.cpp
void DC_Main(void);
void DC_CheckParameters(void);
int DC_MousePresent(void);

// defined in dc_vmu.cpp
void StatusDrawLCD(int lcd_index);
int DC_SaveToVMU(char *src, int tp);
int DC_LoadFromVMU(char *dst);

#else

// ignore calls to this function
#define StatusDrawLCD(x)

#endif


/*
=============================================================================

                               GP2X DEFINITIONS

=============================================================================
*/

#if defined(GP2X)

#if defined(GP2X_940)
void GP2X_MemoryInit(void);
void GP2X_Shutdown(void);
#endif
void GP2X_ButtonDown(int button);
void GP2X_ButtonUp(int button);

#endif


/*
=============================================================================

                             MISC DEFINITIONS

=============================================================================
*/

static inline fixed FixedMul(fixed a, fixed b)
{
	return (fixed)(((int64_t)a * b + 0x8000) >> 16);
}

#ifdef PLAYDEMOLIKEORIGINAL
    #define DEMOCHOOSE_ORIG_SDL(orig, sdl) ((demorecord || demoplayback) ? (orig) : (sdl))
    #define DEMOCOND_ORIG                  (demorecord || demoplayback)
    #define DEMOIF_SDL                     if(DEMOCOND_SDL)
#else
    #define DEMOCHOOSE_ORIG_SDL(orig, sdl) (sdl)
    #define DEMOCOND_ORIG                  false
    #define DEMOIF_SDL
#endif
#define DEMOCOND_SDL                   (!DEMOCOND_ORIG)

#define GetTicks() ((SDL_GetTicks()*7)/100)

#define ISPOINTER(x) ((((uintptr_t)(x)) & ~0xffff) != 0)

#define CHECKMALLOCRESULT(x) if(!(x)) Quit("Out of memory at %s:%i", __FILE__, __LINE__)

#ifdef _WIN32
    #define strcasecmp stricmp
    #define strncasecmp strnicmp
    #define snprintf _snprintf
#else
    static inline char* itoa(int value, char* string, int radix)
    {
	    sprintf(string, "%d", value);
	    return string;
    }

    static inline char* ltoa(long value, char* string, int radix)
    {
	    sprintf(string, "%ld", value);
	    return string;
    }
#endif

#define lengthof(x) (sizeof(x) / sizeof(*(x)))
#define endof(x)    ((x) + lengthof(x))

static inline word READWORD(byte *&ptr)
{
    word val = ptr[0] | ptr[1] << 8;
    ptr += 2;
    return val;
}

static inline longword READLONGWORD(byte *&ptr)
{
    longword val = ptr[0] | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24;
    ptr += 4;
    return val;
}


/*
=============================================================================

                           FEATURE DEFINITIONS

=============================================================================
*/

#ifdef USE_FEATUREFLAGS
    // The currently available feature flags
    #define FF_STARSKY      0x0001
    #define FF_CLOUDSKY     0x0002
    #define FF_SNOW         0x0004
    #define FF_RAIN         0x0008

    // The ffData... variables contain the 16-bit values of the according corners of the current level.
    // The corners are overwritten with adjacent tiles after initialization in SetupGameLevel
    // to avoid interpretation as e.g. doors.

    /*************************************************************
     * Current usage of ffData... variables:
     * ffDataTopLeft:     lower 8-bit: ShadeDefID
     * ffDataTopRight:    FeatureFlags
     * ffDataBottomLeft:  CloudSkyDefID or ParallaxStartTexture
     * ffDataBottomRight: unused
     *************************************************************/

#endif

#ifdef USE_FLOORCEILINGTEX
    void DrawFloorAndCeiling(byte *vbuf, unsigned vbufPitch, int min_wallheight);
#endif

#ifdef USE_PARALLAX
    void DrawParallax(byte *vbuf, unsigned vbufPitch);
#endif

#ifdef USE_DIR3DSPR
    void Scale3DShape(byte *vbuf, unsigned vbufPitch, statobj_t *ob);
#endif

#endif

// Map name display @ Get Psyched fix
extern char currentmapname[16];

extern void InitLevelShadeTable(void);
