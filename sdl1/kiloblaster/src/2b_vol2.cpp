//	2B_VOL2.C
//
// Kiloblaster v.2 Volume specific information
//
// Written by Allen W. Pilgrim

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/gr.h"
#include "include/keyboard.h"
#include "include/windows.h"
#include "include/gamectrl.h"
#include "include/uncrunch.h"
#include "include/music.h"
#include "include/2blaster.h"
#include "include/begin.h"
#include <synchapi.h>

int16_t first_e3 = 1;
int16_t first_e4 = 1;
int16_t first_e6 = 1;
int16_t first_e7 = 1;
int16_t first_ea = 1;
int16_t first_eb = 1;
int16_t first_ei = 1;
int16_t level_flg;

char cfgfname[] = "config.kb2";
char gamename[] = "kb2_save";
char audioname[] = "audio.kb2";
char shapename[] = "shapes.kb2";
char ext[] = ".kb2";
char demolvl[3]={1,8,14};
char *demoname[3]={"demo1.kb2","demo2.kb2","demo3.kb2"};
int16_t demonum;
char k_msg[]="Epic MegaGames presents . . . Kiloblaster!";
char k_msg2[]="Kiloblaster: No Way Out";
wintype msg_win, menuwin, waitwin;

int16_t b_len = K_BEGIN_LENGTH;
extern unsigned char K_BEGIN[];

const char *rnd_msg[10]={
	"You are doomed!",
	"I'm surprised you made it this far",
	"* Say your last goodbyes *",
	"If you choose to continue you will die",
	"You can't win against our superior forces!",
	"The end is near . . . Your's!",
	"EPIC is the new leader in shareware!",
	"Are you ready to surrender yet?",
	"< < < You'll never make it rookie > > >",
	"Your skills are weak young Jedi!"
	};

const char *obj_msg[4]={
	"It takes 2 hits to kill this enemy or 1 bomb",
	"It takes 3 hits to kill this enemy or 1 bomb",
	"It takes 4 hits to kill this enemy or 1 bomb",
	"10 hits to kill this enemy--bombs count as 2",
	};

const char *end_msg[5]={
	"Game Over . . . Are you ready to try again?",
	"Game Over . . . Don't give up without a fight!",
	"Game Over . . . Did you think it would be easy?",
	"Game Over . . . Give it another shot Captain",
	"Game Over . . . Your skills seem to be rusty!"
	};

char *song1[32]={
	"song_8.kb2",		// butterfly
	"song_1.kb2",		// robot
	"song_1.kb2",
	"song_1.kb2",
	"song_2.kb2",		// doom
	"song_2.kb2",
	"song_2.kb2",
	"song_3.kb2",		// creepy
	"song_3.kb2",
	"song_3.kb2",
	"song_4.kb2",		// nemo
	"song_4.kb2",
	"song_4.kb2",
	"song_5.kb2",		// oblong
	"song_5.kb2",
	"song_5.kb2",
	"song_6.kb2",		// waiting
	"song_6.kb2",
	"song_6.kb2",
	"song_7.kb2",		// funkit
	"song_7.kb2",
	"song_7.kb2",
	"song_1.kb2",		// robot
	"song_1.kb2",
	"song_1.kb2",
	"song_2.kb2",		// doom
	"song_2.kb2",
	"song_2.kb2",
	"song_4.kb2",		// nemo
	"song_4.kb2",
	"song_4.kb2",
	"song_9.kb2"		// hero2
	};

void init_objinfo (void) {		// initialize object info array
	int16_t c;
	for (c = 0; c < maxobjkinds; c++) {
		objinfo [c].na    = "";
		objinfo [c].xl    = 0;
		objinfo [c].yl    = 0;
		objinfo [c].msg   = msg_killme;
		objinfo [c].flags = 0;
		objinfo [c].class_ = 0;
		};

		objinfo [obj_player].na      = "PLAYER";
		objinfo [obj_player].xl      = 12;
		objinfo [obj_player].yl      = 18;
		objinfo [obj_player].msg     = msg_player;
		objinfo [obj_player].flags   = 0;
		objinfo [obj_player].class_   = class_player;

		objinfo [obj_killme].na      = "KILLME";
		objinfo [obj_killme].xl      = 0;
		objinfo [obj_killme].yl      = 0;
		objinfo [obj_killme].msg     = msg_killme;
		objinfo [obj_killme].flags   = 1;
		objinfo [obj_killme].class_   = 0;

		objinfo [obj_bullet].na      = "BULLET";
		objinfo [obj_bullet].xl      = 2;
		objinfo [obj_bullet].yl      = 4;
		objinfo [obj_bullet].msg     = msg_bullet;
		objinfo [obj_bullet].flags   = 0;
		objinfo [obj_bullet].class_   = class_weapon1;

		objinfo [obj_platinum].na    = "PLATINUM";
		objinfo [obj_platinum].xl    = 6;
		objinfo [obj_platinum].yl    = 10;
		objinfo [obj_platinum].msg   = msg_platinum;
		objinfo [obj_platinum].flags = 0;
		objinfo [obj_platinum].class_ = class_weapon1;

		objinfo [obj_bomb].na        = "BOMB";
		objinfo [obj_bomb].xl        = 6;
		objinfo [obj_bomb].yl        = 8;
		objinfo [obj_bomb].msg       = msg_bomb;
		objinfo [obj_bomb].flags     = 0;
		objinfo [obj_bomb].class_     = class_weapon2;

		objinfo [obj_missile].na     = "MISSILE";
		objinfo [obj_missile].xl     = 10;
		objinfo [obj_missile].yl     = 20;
		objinfo [obj_missile].msg    = msg_missile;
		objinfo [obj_missile].flags  = 0;
		objinfo [obj_missile].class_  = class_weapon2;

		objinfo [obj_bullet2].na     = "BULLET2";
		objinfo [obj_bullet2].xl     = 6;
		objinfo [obj_bullet2].yl     = 4;
		objinfo [obj_bullet2].msg    = msg_bullet2;
		objinfo [obj_bullet2].flags  = 0;
		objinfo [obj_bullet2].class_  = 0;

		objinfo [obj_spinner].na     = "SPINNER";
		objinfo [obj_spinner].xl     = 8;
		objinfo [obj_spinner].yl     = 8;
		objinfo [obj_spinner].msg    = msg_spinner;
		objinfo [obj_spinner].flags  = 0;
		objinfo [obj_spinner].class_  = 0;

		objinfo [obj_enemy1].na      = "ENEMY1";	// Colonial Warrior
		objinfo [obj_enemy1].xl      = 12;
		objinfo [obj_enemy1].yl      = 8;
		objinfo [obj_enemy1].msg     = msg_enemy1;
		objinfo [obj_enemy1].flags   = 0;
		objinfo [obj_enemy1].class_   = class_enemy;

		objinfo [obj_enemy2].na      = "ENEMY2";	// Small Orzot Fighter
		objinfo [obj_enemy2].xl      = 14;
		objinfo [obj_enemy2].yl      = 12;
		objinfo [obj_enemy2].msg     = msg_enemy2;
		objinfo [obj_enemy2].flags   = 0;
		objinfo [obj_enemy2].class_   = class_enemy;

		objinfo [obj_enemy3].na      = "ENEMY3";	// Planet Smasher
		objinfo [obj_enemy3].xl      = 14;
		objinfo [obj_enemy3].yl      = 20;
		objinfo [obj_enemy3].msg     = msg_enemy3;
		objinfo [obj_enemy3].flags   = 0;
		objinfo [obj_enemy3].class_   = class_enemy;

		objinfo [obj_enemy4].na      = "ENEMY4";	// M-15 Abrams
		objinfo [obj_enemy4].xl      = 16;
		objinfo [obj_enemy4].yl      = 24;
		objinfo [obj_enemy4].msg     = msg_enemy4;
		objinfo [obj_enemy4].flags   = 0;
		objinfo [obj_enemy4].class_   = class_enemy;

		objinfo [obj_enemy5].na      = "ENEMY5";	// Ionian Assault Ship
		objinfo [obj_enemy5].xl      = 12;
		objinfo [obj_enemy5].yl      = 20;
		objinfo [obj_enemy5].msg     = msg_enemy5;
		objinfo [obj_enemy5].flags   = 0;
		objinfo [obj_enemy5].class_   = class_enemy;

		objinfo [obj_enemy6].na      = "ENEMY6";	// Widow Maker
		objinfo [obj_enemy6].xl      = 14;
		objinfo [obj_enemy6].yl      = 20;
		objinfo [obj_enemy6].msg     = msg_enemy6;
		objinfo [obj_enemy6].flags   = 0;
		objinfo [obj_enemy6].class_   = class_enemy;

		objinfo [obj_enemy7].na      = "ENEMY7";	// Thunderbolt
		objinfo [obj_enemy7].xl      = 12;
		objinfo [obj_enemy7].yl      = 20;
		objinfo [obj_enemy7].msg     = msg_enemy7;
		objinfo [obj_enemy7].flags   = 0;
		objinfo [obj_enemy7].class_   = class_enemy;

		objinfo [obj_enemy8].na      = "ENEMY8";	// Death Sting 2
		objinfo [obj_enemy8].xl      = 14;
		objinfo [obj_enemy8].yl      = 18;
		objinfo [obj_enemy8].msg     = msg_enemy8;
		objinfo [obj_enemy8].flags   = 0;
		objinfo [obj_enemy8].class_   = class_enemy;

		objinfo [obj_enemy9].na      = "ENEMY9";	// Bird of Prey
		objinfo [obj_enemy9].xl      = 12;
		objinfo [obj_enemy9].yl      = 16;
		objinfo [obj_enemy9].msg     = msg_enemy9;
		objinfo [obj_enemy9].flags   = 0;
		objinfo [obj_enemy9].class_   = class_enemy;

		objinfo [obj_enemya].na      = "ENEMYA";	// SS Blizzard
		objinfo [obj_enemya].xl      = 12;
		objinfo [obj_enemya].yl      = 20;
		objinfo [obj_enemya].msg     = msg_enemya;
		objinfo [obj_enemya].flags   = 0;
		objinfo [obj_enemya].class_   = class_enemy;

		objinfo [obj_enemyb].na      = "ENEMYB";	// Antagon's Revenge
		objinfo [obj_enemyb].xl      = 16;
		objinfo [obj_enemyb].yl      = 18;
		objinfo [obj_enemyb].msg     = msg_enemyb;
		objinfo [obj_enemyb].flags   = 0;
		objinfo [obj_enemyb].class_   = class_enemy;

		objinfo [obj_enemyc].na      = "ENEMYC";	// Alpha Xyntari
		objinfo [obj_enemyc].xl      = 12;
		objinfo [obj_enemyc].yl      = 24;
		objinfo [obj_enemyc].msg     = msg_enemyc;
		objinfo [obj_enemyc].flags   = 0;
		objinfo [obj_enemyc].class_   = class_enemy;

		objinfo [obj_enemyd].na      = "ENEMYD";	// Electra Flame
		objinfo [obj_enemyd].xl      = 10;
		objinfo [obj_enemyd].yl      = 16;
		objinfo [obj_enemyd].msg     = msg_enemyd;
		objinfo [obj_enemyd].flags   = 0;
		objinfo [obj_enemyd].class_   = class_enemy;

		objinfo [obj_enemye].na      = "ENEMYE";	// Sentinel
		objinfo [obj_enemye].xl      = 12;
		objinfo [obj_enemye].yl      = 16;
		objinfo [obj_enemye].msg     = msg_enemye;
		objinfo [obj_enemye].flags   = 0;
		objinfo [obj_enemye].class_   = class_enemy;

		objinfo [obj_enemyf].na      = "ENEMYF";	// Apocalypse
		objinfo [obj_enemyf].xl      = 10;
		objinfo [obj_enemyf].yl      = 16;
		objinfo [obj_enemyf].msg     = msg_enemyf;
		objinfo [obj_enemyf].flags   = 0;
		objinfo [obj_enemyf].class_   = class_enemy;

		objinfo [obj_enemyg].na      = "ENEMYG";	// Blue Spearhead
		objinfo [obj_enemyg].xl      = 10;
		objinfo [obj_enemyg].yl      = 16;
		objinfo [obj_enemyg].msg     = msg_enemyg;
		objinfo [obj_enemyg].flags   = 0;
		objinfo [obj_enemyg].class_   = class_enemy;

		objinfo [obj_enemyh].na      = "ENEMYH";	// Javelin
		objinfo [obj_enemyh].xl      = 12;
		objinfo [obj_enemyh].yl      = 18;
		objinfo [obj_enemyh].msg     = msg_enemyh;
		objinfo [obj_enemyh].flags   = 0;
		objinfo [obj_enemyh].class_   = class_enemy;

		objinfo [obj_enemyi].na      = "ENEMYI";	// X-15 Destroyer
		objinfo [obj_enemyi].xl      = 20;
		objinfo [obj_enemyi].yl      = 22;
		objinfo [obj_enemyi].msg     = msg_enemyi;
		objinfo [obj_enemyi].flags   = 0;
		objinfo [obj_enemyi].class_   = class_enemy;

		objinfo [obj_enemyj].na      = "ENEMYJ";	// Strike Fighter
		objinfo [obj_enemyj].xl      = 12;
		objinfo [obj_enemyj].yl      = 18;
		objinfo [obj_enemyj].msg     = msg_enemyj;
		objinfo [obj_enemyj].flags   = 0;
		objinfo [obj_enemyj].class_   = class_enemy;

		objinfo [obj_enemyk].na      = "ENEMYK";	// Kilofire Interceptor
		objinfo [obj_enemyk].xl      = 14;
		objinfo [obj_enemyk].yl      = 14;
		objinfo [obj_enemyk].msg     = msg_enemyk;
		objinfo [obj_enemyk].flags   = 0;
		objinfo [obj_enemyk].class_   = class_enemy;

		objinfo [obj_ejected].na     = "EJECTED";
		objinfo [obj_ejected].xl     = 4;
		objinfo [obj_ejected].yl     = 6;
		objinfo [obj_ejected].msg    = msg_ejected;
		objinfo [obj_ejected].flags  = 1;
		objinfo [obj_ejected].class_  = 0;

		objinfo [obj_explode1].na    = "EXPLODE1";
		objinfo [obj_explode1].xl    = 12;
		objinfo [obj_explode1].yl    = 12;
		objinfo [obj_explode1].msg   = msg_explode1;
		objinfo [obj_explode1].flags = 1;
		objinfo [obj_explode1].class_ = 0;

		objinfo [obj_explode2].na    = "EXPLODE2";
		objinfo [obj_explode2].xl    = 14;
		objinfo [obj_explode2].yl    = 14;
		objinfo [obj_explode2].msg   = msg_explode2;
		objinfo [obj_explode2].flags = 1;
		objinfo [obj_explode2].class_ = 0;

		objinfo [obj_explode3].na    = "EXPLODE3";
		objinfo [obj_explode3].xl    = 30;
		objinfo [obj_explode3].yl    = 26;
		objinfo [obj_explode3].msg   = msg_explode3;
		objinfo [obj_explode3].flags = 1;
		objinfo [obj_explode3].class_ = 0;

		objinfo [obj_explode4].na    = "EXPLODE4";
		objinfo [obj_explode4].xl    = 6;
		objinfo [obj_explode4].yl    = 4;
		objinfo [obj_explode4].msg   = msg_explode4;
		objinfo [obj_explode4].flags = 1;
		objinfo [obj_explode4].class_ = 0;

		objinfo [obj_explode5].na    = "EXPLODE5";
		objinfo [obj_explode5].xl    = 16;
		objinfo [obj_explode5].yl    = 16;
		objinfo [obj_explode5].msg   = msg_explode5;
		objinfo [obj_explode5].flags = 1;
		objinfo [obj_explode5].class_ = 0;

		objinfo [obj_explode6].na    = "EXPLODE6";
		objinfo [obj_explode6].xl    = 16;
		objinfo [obj_explode6].yl    = 16;
		objinfo [obj_explode6].msg   = msg_explode6;
		objinfo [obj_explode6].flags = 1;
		objinfo [obj_explode6].class_ = 0;

		objinfo [obj_triple].na      = "TRIPLE";
		objinfo [obj_triple].xl      = 8;
		objinfo [obj_triple].yl      = 8;
		objinfo [obj_triple].msg     = msg_triple;
		objinfo [obj_triple].flags   = 0;
		objinfo [obj_triple].class_   = 0;

		objinfo [obj_bonus1].na      = "BONUS1";
		objinfo [obj_bonus1].xl      = 14;
		objinfo [obj_bonus1].yl      = 14;
		objinfo [obj_bonus1].msg     = msg_bonus1;
		objinfo [obj_bonus1].flags   = 0;
		objinfo [obj_bonus1].class_   = 0;

		objinfo [obj_bonus2].na      = "BONUS2";
		objinfo [obj_bonus2].xl      = 8;
		objinfo [obj_bonus2].yl      = 10;
		objinfo [obj_bonus2].msg     = msg_bonus2;
		objinfo [obj_bonus2].flags   = 0;
		objinfo [obj_bonus2].class_   = 0;

		objinfo [obj_bonus3].na      = "BONUS3";
		objinfo [obj_bonus3].xl      = 8;
		objinfo [obj_bonus3].yl      = 10;
		objinfo [obj_bonus3].msg     = msg_bonus3;
		objinfo [obj_bonus3].flags   = 0;
		objinfo [obj_bonus3].class_   = 0;

		objinfo [obj_bonus4].na      = "BONUS4";
		objinfo [obj_bonus4].xl      = 8;
		objinfo [obj_bonus4].yl      = 10;
		objinfo [obj_bonus4].msg     = msg_bonus4;
		objinfo [obj_bonus4].flags   = 0;
		objinfo [obj_bonus4].class_   = 0;

		objinfo [obj_bonus5].na      = "BONUS5";
		objinfo [obj_bonus5].xl      = 8;
		objinfo [obj_bonus5].yl      = 10;
		objinfo [obj_bonus5].msg     = msg_bonus5;
		objinfo [obj_bonus5].flags   = 0;
		objinfo [obj_bonus5].class_   = 0;

		objinfo [obj_wing1].na       = "WING1";
		objinfo [obj_wing1].xl       = 8;
		objinfo [obj_wing1].yl       = 10;
		objinfo [obj_wing1].msg      = msg_wing1;
		objinfo [obj_wing1].flags    = 0;
		objinfo [obj_wing1].class_    = class_wing1;

		objinfo [obj_wing2].na       = "WING2";
		objinfo [obj_wing2].xl       = 8;
		objinfo [obj_wing2].yl       = 10;
		objinfo [obj_wing2].msg      = msg_wing2;
		objinfo [obj_wing2].flags    = 0;
		objinfo [obj_wing2].class_    = class_wing2;

		objinfo [obj_wing3].na       = "WING3";
		objinfo [obj_wing3].xl       = 8;
		objinfo [obj_wing3].yl       = 10;
		objinfo [obj_wing3].msg      = msg_wing3;
		objinfo [obj_wing3].flags    = 0;
		objinfo [obj_wing3].class_    = class_wing3;

		objinfo [obj_wing4].na       = "WING4";
		objinfo [obj_wing4].xl       = 8;
		objinfo [obj_wing4].yl       = 10;
		objinfo [obj_wing4].msg      = msg_wing4;
		objinfo [obj_wing4].flags    = 0;
		objinfo [obj_wing4].class_    = class_wing4;

		objinfo [obj_stars].na       = "STARS";
		objinfo [obj_stars].xl       = 4;
		objinfo [obj_stars].yl       = 4;
		objinfo [obj_stars].msg      = msg_stars;
		objinfo [obj_stars].flags    = 1;
		objinfo [obj_stars].class_    = 0;

		objinfo [obj_falling].na     = "FALLING";
		objinfo [obj_falling].xl     = 4;
		objinfo [obj_falling].yl     = 4;
		objinfo [obj_falling].msg    = msg_falling;
		objinfo [obj_falling].flags  = 1;
		objinfo [obj_falling].class_  = 0;

		objinfo [obj_score].na       = "SCORE";
		objinfo [obj_score].xl       = 0;
		objinfo [obj_score].yl       = 8;
		objinfo [obj_score].msg      = msg_score;
		objinfo [obj_score].flags    = 1;
		objinfo [obj_score].class_    = 0;

		objinfo [obj_level1].na      = "LEVEL1";
		objinfo [obj_level1].xl      = 64;
		objinfo [obj_level1].yl      = 8;
		objinfo [obj_level1].msg     = msg_level1;
		objinfo [obj_level1].flags   = 1;
		objinfo [obj_level1].class_   = 0;

		objinfo [obj_shield].na      = "SHIELD";
		objinfo [obj_shield].xl      = 16;
		objinfo [obj_shield].yl      = 22;
		objinfo [obj_shield].msg     = msg_shield;
		objinfo [obj_shield].flags   = 0;
		objinfo [obj_shield].class_   = class_shield;

		objinfo [obj_laser].na       = "LASER";
		objinfo [obj_laser].xl       = 2;
		objinfo [obj_laser].yl       = 30;
		objinfo [obj_laser].msg      = msg_laser;
		objinfo [obj_laser].flags    = 0;
		objinfo [obj_laser].class_    = 0;

		objinfo [obj_max].na         = "MAX";
		objinfo [obj_max].xl         = 12;
		objinfo [obj_max].yl         = 20;
		objinfo [obj_max].msg        = msg_max;
		objinfo [obj_max].flags      = 0;
		objinfo [obj_max].class_      = 0;

		objinfo [obj_mines].na       = "MINES";
		objinfo [obj_mines].xl       = 8;
		objinfo [obj_mines].yl       = 8;
		objinfo [obj_mines].msg      = msg_mines;
		objinfo [obj_mines].flags    = 0;
		objinfo [obj_mines].class_    = 0;

		objinfo [obj_demo].na        = "DEMO";
		objinfo [obj_demo].xl        = 30;
		objinfo [obj_demo].yl        = 8;
		objinfo [obj_demo].msg       = msg_demo;
		objinfo [obj_demo].flags     = 1;
		objinfo [obj_demo].class_     = 0;

		objinfo [obj_jump1].na       = "JUMP1";
		objinfo [obj_jump1].xl       = 18;
		objinfo [obj_jump1].yl       = 18;
		objinfo [obj_jump1].msg      = msg_jump1;
		objinfo [obj_jump1].flags    = 1;
		objinfo [obj_jump1].class_    = 0;

		objinfo [obj_jump2].na       = "JUMP2";
		objinfo [obj_jump2].xl       = 18;
		objinfo [obj_jump2].yl       = 18;
		objinfo [obj_jump2].msg      = msg_jump2;
		objinfo [obj_jump2].flags    = 1;
		objinfo [obj_jump2].class_    = 0;
		};

int16_t msg_enemy1 (int16_t n, int16_t msg, int16_t z) {		// Colonial Warrior
	int16_t sh, x, y;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&7;
			if (objs[n].count2++ == 8) {
				objs[n].xd = -objs[n].xd;
				objs[n].count2 = 0;
				};
			move_obj (n, objs[n].x+objs[n].xd, objs[n].y);
			if (xr_random(70)==0) objs[n].x = rand()%232;
			if (xr_random(50)==0) {
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_bullet2, x-2, y, 0, 4);
				}; break;

		case msg_draw:
			sh = 0x900 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemy2 (int16_t n, int16_t msg, int16_t z) {		// Small Orzot Fighter
	int16_t sh, x, y, ax, ay;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&7;
			if (objs[n].count2==0) {
				switch (xr_random(2)) {
					case 0: objs[n].count2 = 1; break;
					case 1: objs[n].count2 = 2; break;
					};
				};
			if (objs[n].count2==1) attract (n, &ax, &ay);
			else attract2 (n, &ax, &ay);
			move_obj (n, objs[n].x+(objs[n].xd+=ax),
				objs[n].y+(objs[n].yd+=ay));
			if (objs[n].y >= 144) objs[n].y = 144;
			if (xr_random(40)==0) {
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_bullet2, x-2, y, 0, 4);
				}; break;

		case msg_draw:
			sh = 0xa00 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemy3 (int16_t n, int16_t msg, int16_t z) {		// Planet Smasher
	int16_t sh, y, dx, dy;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&7;
			if (objs[n].count3==0) objs[n].count3 = 3;
			seekplayer (n, &dx, &dy);
			if (objs[n].count2++ == 10) {
				objs[n].yd = -objs[n].yd;
				objs[n].count2 = 0;
				};
			move_obj (n, objs[n].x+(objs[n].xd+=dx), objs[n].y+objs[n].yd);
			if (xr_random(40)==0) {
				y = objs[n].y + objs[n].yl;
				addobj (obj_spinner, objs[n].x, y-1, 0, 6);
				addobj (obj_spinner, objs[n].x+9, y-1, 0, 6);
				}; break;

		case msg_draw:
			sh = 0x1600 + 85 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemy4 (int16_t n, int16_t msg, int16_t z) {		// M-15 Abrams
	int16_t sh, x, y, l_y;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&7;
			if (objs[n].count3==0) objs[n].count3 = 2;
			if (objs[n].y > 96) objs[n].yd = -10;
			if (objs[n].y <= -25) {
				objs[n].x = rand()%240; objs[n].y = -24;
				objs[n].yd = 0;
				objs[n].count2 = 0;
				};
			if ((objs[n].count2++ == 30)&&(objs[n].yd==0)) {
				objs[n].yd = 8;
				};
			objs[n].y += objs[n].yd;
			if (xr_random(60)==0) {
				snd_play (1,21);
				x=objs[n].x+objs[n].xl/2; y=objs[n].y+objs[n].yl-6;
				for (l_y = y; l_y < scrn_y + 1; l_y += 30) {
					addobj (obj_laser, x, l_y, 0, 0);
					};
				}; break;

		case msg_draw:
			sh = 0x1600 + 45 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemy5 (int16_t n, int16_t msg, int16_t z) {		// Ionian Assault Ship
	int16_t sh, x, y;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&7;
			if (objs[n].y > 96) {
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_spinner, x-3, y, 0, 4);
				objs[n].yd = -3;
				switch (xr_random(2)) {
					case 0: objs[n].xd = 8; break;
					case 1: objs[n].xd = -8;
					};
				};
			if ((objs[n].x >= scrn_x) || (objs[n].x+objs[n].xl <= 0)) {
				enemy_cnt--; killobj (n);
				};
			objs[n].x += objs[n].xd; objs[n].y += objs[n].yd;
			break;

		case msg_draw:
			sh = 0x1600 + 50 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemy6 (int16_t n, int16_t msg, int16_t z) {		// Widow Maker
	int16_t sh, y, l_y;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&7;
			if (objs[n].count3==0) objs[n].count3 = 4;
			if (objs[n].count2++ == 10) {
				objs[n].yd = -objs[n].yd;
				objs[n].count2 = 0;
				};
			move_obj (n, objs[n].x+objs[n].xd, objs[n].y+objs[n].yd);
			if (objs[n].x==0) objs[n].xd = 6;
			if (objs[n].x+objs[n].xl==scrn_x) objs[n].xd = -6;
			if (xr_random(60)==0) {
				snd_play (1,21);
				y = objs[n].y + objs[n].yl;
				for (l_y = y; l_y < scrn_y + 1; l_y += 30) {
					addobj (obj_laser, objs[n].x+3, l_y, 0, 0);
					addobj (obj_laser, objs[n].x+11, l_y, 0, 0);
					};
				}; break;

		case msg_draw:
			sh = 0x1600 + 64 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemy7 (int16_t n, int16_t msg, int16_t z) {		// Thunderbolt
	int16_t sh, x, y;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&3;
			if (objs[n].count3==0) objs[n].count3 = 2;
			if (xr_random(20)==0) {
				objs[n].xd = xr_random(3) * 4 - 4;
				if (objs[n].xd==0) objs[n].yd = xr_random(2) * 4 - 2;
				else objs[n].yd = xr_random(3) * 2 - 2;
				};
			move_obj (n, objs[n].x+objs[n].xd*2, objs[n].y+objs[n].yd*2);
			if (objs[n].y >= 96) objs[n].yd = -objs[n].yd;
			if (xr_random(40)==0) {
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_spinner, x-3, y, 0, 4);
				}; break;

		case msg_draw:
			sh = 0x1600 + 69 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemy8 (int16_t n, int16_t msg, int16_t z) {		// Death Sting 2
	int16_t sh, x, y;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&7;
			if (objs[n].y > 96) {
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_spinner, x-3, y, 0, 4);
				objs[n].yd = -3;
				switch (xr_random(2)) {
					case 0: objs[n].xd = 8; break;
					case 1: objs[n].xd = -8;
					};
				};
			if ((objs[n].x >= scrn_x) || (objs[n].x+objs[n].xl <= 0)) {
				enemy_cnt--; killobj (n);
				};
			objs[n].x += objs[n].xd; objs[n].y += objs[n].yd;
			break;

		case msg_draw:
			sh = 0x1600 + 75 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemy9 (int16_t n, int16_t msg, int16_t z) {		// Bird of Prey
	int16_t sh, x, y, ax, ay;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&3;
			if (objs[n].count2==0) {
				switch (xr_random(3)) {
					case 0: objs[n].count2 = 1; break;
					case 1: objs[n].count2 = 2; break;
					case 2: objs[n].count2 = 3;
					};
				};
			if (objs[n].count2==1) attract (n, &ax, &ay);
			else if (objs[n].count2==2) attract2 (n, &ax, &ay);
			else attract3 (n, &ax, &ay);
			move_obj (n, objs[n].x+(objs[n].xd+=ax),
				objs[n].y+(objs[n].yd+=ay));
			if (objs[n].y >= 144) objs[n].y = 144;
			if (xr_random(40)==0) {
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_bullet2, x-2, y, 0, 4);
				}; break;

		case msg_draw:
			sh = 0x1600 + 20 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemya (int16_t n, int16_t msg, int16_t z) {		// SS Blizzard
	int16_t sh, x, y, l_y;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&7;
			if (objs[n].count3==0) objs[n].count3 = 2;
			if (objs[n].y > 96) objs[n].yd = -10;
			if (objs[n].y <= -21) {
				objs[n].x = rand()%240; objs[n].y = -20;
				objs[n].yd = 0;
				objs[n].count2 = 0;
				};
			if ((objs[n].count2++ == 30)&&(objs[n].yd==0)) {
				objs[n].yd = 8;
				};
			objs[n].y += objs[n].yd;
			if (xr_random(60)==0) {
				snd_play (1,21);
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				for (l_y = y; l_y < scrn_y + 1; l_y += 30) {
					addobj (obj_laser, x, l_y, 0, 0);
					};
				}; break;

		case msg_draw:
			sh = 0x1600 + 55 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemyb (int16_t n, int16_t msg, int16_t z) {		// Antagon's Revenge
	int16_t sh, x, y, dx, dy;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&7;
			if (objs[n].count3==0) objs[n].count3 = 4;
			seekplayer (n, &dx, &dy);
			move_obj (n, objs[n].x+(objs[n].xd+=dx), objs[n].y+objs[n].yd);
			if (xr_random(40)==0) {
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_bullet2, x-2, y, 0, 4);
				}; break;

		case msg_draw:
			sh = 0x1600 + 11 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemyc (int16_t n, int16_t msg, int16_t z) {		// Alpha Xyntari
	int16_t sh, x, y;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&3;
			if ((objs[n].x>=scrn_x)||(objs[n].x+objs[n].xl<=0)) {
				objs[n].x = rand()%240; objs[n].y = -24;
				objs[n].xd = 0; objs[n].yd = 8;
				};
			if (objs[n].y > 64) {
				objs[n].yd = 0;
				if (objs[n].x < 122) objs[n].xd = -12;
				if (objs[n].x >= 122) objs[n].xd = 12;
				};
			objs[n].x += objs[n].xd; objs[n].y += objs[n].yd;
			if (xr_random(40)==0) {
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_spinner, x-3, y, 0, 4);
				}; break;

		case msg_draw:
			sh = 0x1400 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemyd (int16_t n, int16_t msg, int16_t z) {		// Electra Flame
	int16_t sh, x, y, ax, ay, dx, dy;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&3;
			if (objs[n].count2==0) {
				switch (xr_random(2)) {
					case 0: objs[n].count2 = 1; break;
					case 1: objs[n].count2 = 2; break;
					};
				};
			if (objs[n].count2==1) attract (n, &ax, &ay);
			else attract2 (n, &ax, &ay);
			seekplayer (n, &dx, &dy);
			move_obj (n, objs[n].x+(objs[n].xd+=dx),
				objs[n].y+(objs[n].yd+=ay));
			if (objs[n].y >= 144) objs[n].y = 144;
			if (xr_random(50)==0) {
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_bullet2, x-2, y, 0, 4);
				}; break;

		case msg_draw:
			sh = 0x1406 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemye (int16_t n, int16_t msg, int16_t z) {		// Sentinel
	int16_t sh, x, y;
	switch (msg) {
		case msg_update:
			if (objs[n].count2==0) {
				addobj (obj_jump2, objs[n].x, objs[n].y, 0, 0);
				objs[n].count2 = 1;
				};
			objs[n].count = (objs[n].count + 1)&7;
			objs[n].xd += (xr_random(3) - 1) * (1 + (abs(objs[n].xd) < 3));
			objs[n].xd = max(-8, min(8, objs[n].xd));
			objs[n].yd += (xr_random(3) - 1) * (1 + (abs(objs[n].yd < 2)));
			objs[n].yd = max(-8, min(8, objs[n].yd));
			objs[n].x += objs[n].xd; objs[n].y += objs[n].yd;
			if (objs[n].x <= 0) {
				objs[n].x = 0; objs[n].xd = -objs[n].xd;
				};
			if (objs[n].x >= scrn_x - objs[n].xl) {
				objs[n].x = scrn_x-objs[n].xl; objs[n].xd = -objs[n].xd;
				};
			if (objs[n].y <= 0) {
				objs[n].y = 0; objs[n].yd = -objs[n].yd;
				};
			if (objs[n].y >= 64) {
				objs[n].y = 64; objs[n].yd = -objs[n].yd;
				};
			if (xr_random(40)==0) {
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_bullet2, x-2, y, 0, 4);
				}; break;

		case msg_draw:
			sh = 0x1600 + 28 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemyf (int16_t n, int16_t msg, int16_t z) {		// Apocalypse
	int16_t sh, x, y, ax, ay;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&3;
			if (objs[n].count2==0) {
				switch (xr_random(3)) {
					case 0: objs[n].count2 = 1; break;
					case 1: objs[n].count2 = 2; break;
					case 2: objs[n].count2 = 3;
					};
				};
			if (objs[n].count2==1) attract (n, &ax, &ay);
			else if (objs[n].count2==2) attract2 (n, &ax, &ay);
			else attract3 (n, &ax, &ay);
			move_obj (n, objs[n].x+(objs[n].xd+=ax),
				objs[n].y+(objs[n].yd+=ay));
			if (objs[n].y >= 144) objs[n].y = 144;
			if (xr_random(40)==0) {
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_bullet2, x-2, y, 0, 4);
				}; break;

		case msg_draw:
			sh = 0x140c + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemyg (int16_t n, int16_t msg, int16_t z) {		// Blue Spearhead
	int16_t sh, x, y;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&3;
			if (objs[n].y > 96) {
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_spinner, x-3, y, 0, 4);
				objs[n].yd = -3;
				switch (xr_random(2)) {
					case 0: objs[n].xd = 8; break;
					case 1: objs[n].xd = -8;
					};
				};
			if ((objs[n].x >= scrn_x) || (objs[n].x+objs[n].xl <= 0)) {
				objs[n].x = rand()%240; objs[n].y = -16;
				objs[n].xd = 0; objs[n].yd = 6;
				};
			objs[n].x += objs[n].xd; objs[n].y += objs[n].yd;
			break;

		case msg_draw:
			sh = 0x140f + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemyh (int16_t n, int16_t msg, int16_t z) {		// Javelin
	int16_t sh, x, y, ax, ay;
	int16_t a_tab[8]={0,1,2,3,3,2,1,0};
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&7;
			if (objs[n].count2==0) {
				switch (xr_random(2)) {
					case 0: objs[n].count2 = 1; break;
					case 1: objs[n].count2 = 2; break;
					};
				};
			if (objs[n].count2==1) attract (n, &ax, &ay);
			else attract2 (n, &ax, &ay);
			move_obj (n, objs[n].x+(objs[n].xd+=ax),
				objs[n].y+(objs[n].yd+=ay));
			if (objs[n].y >= 144) objs[n].y = 144;
			if (xr_random(40)==0) {
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_bullet2, x-2, y, 0, 4);
				}; break;

		case msg_draw:
			sh = 0x1600 + 23 + a_tab[objs[n].count];
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemyi (int16_t n, int16_t msg, int16_t z) {		// X-15 Destroyer
	int16_t sh, x, y, ax, ay, l_y;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&3;
			if (objs[n].count3==0) objs[n].count3 = 10;
			attract (n, &ax, &ay);
			if (objs[n].y==0) objs[n].yd = 6;
			if (objs[n].y > 96) objs[n].yd = -10;
			move_obj (n, objs[n].x+(objs[n].xd+=ax), objs[n].y+objs[n].yd);
			if (xr_random(60)==0) {
				snd_play (1,21);
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_spinner, x-3, y, 0, 4);
				for (l_y = y; l_y < scrn_y + 1; l_y += 30) {
					addobj (obj_laser, x-9, l_y, 0, 0);
					addobj (obj_laser, x+9, l_y, 0, 0);
					};
				}; break;

		case msg_draw:
			sh = 0x1400 + 21 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemyj (int16_t n, int16_t msg, int16_t z) {		// Strike Fighter
	int16_t sh, x, y;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&3;
			if ((objs[n].x>=scrn_x)||(objs[n].x+objs[n].xl<=0)) {
				objs[n].x = rand()%240; objs[n].y = -18;
				objs[n].xd = 0; objs[n].yd = 8;
				objs[n].count2 = 0;
				};
			if ((objs[n].y > 64) && (objs[n].yd==8)) {
				objs[n].yd = 9;
				if (objs[n].x < 122) {
					objs[n].xd = -6;
					objs[n].count2 = 4;
					};
				if (objs[n].x >= 122) {
					objs[n].xd = 6;
					objs[n].count2 = 1;
					};
				};
			objs[n].x += objs[n].xd; objs[n].y += objs[n].yd;
			if (xr_random(40)==0) {
				x = objs[n].x + objs[n].xl/2; y = objs[n].y + objs[n].yl;
				addobj (obj_spinner, x-3, y-1, 0, 4);
				}; break;

		case msg_draw:
			if (objs[n].count2==0) sh = 0x1600 + objs[n].count/2;
			if (objs[n].count2==3) sh = 0x1604 + objs[n].count/2;
			if (objs[n].count2==2) {sh = 0x1603; objs[n].count2 = 3;};
			if (objs[n].count2==1) {sh = 0x1603; objs[n].count2 = 2;};
			if (objs[n].count2==6) sh = 0x1608 + objs[n].count/2;
			if (objs[n].count2==5) {sh = 0x1607; objs[n].count2 = 6;};
			if (objs[n].count2==4) {sh = 0x1607; objs[n].count2 = 5;};
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

int16_t msg_enemyk (int16_t n, int16_t msg, int16_t z) {		// Kilofire Interceptor
	int16_t sh, ax, ay;
	switch (msg) {
		case msg_update:
			if (objs[n].count++ >= 11) objs[n].count = 0;
			if (objs[n].count2==0) {
				addobj (obj_jump2, objs[n].x, objs[n].y, 0, 0);
				switch (xr_random(2)) {
					case 0: objs[n].count2 = 1; break;
					case 1: objs[n].count2 = 2; break;
					};
				};
			if (objs[n].count2==1) attract (n, &ax, &ay);
			else attract2 (n, &ax, &ay);
			move_obj (n, objs[n].x+(objs[n].xd+=ax),
				objs[n].y+(objs[n].yd+=ay));
			if (objs[n].y >= 144) objs[n].y = 144; break;

		case msg_draw:
			sh = 0x1300 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y);
		}; return (0);
	};

void hit_bullet (int16_t n, int16_t z) {
	switch (objs[z].kind) {
		case obj_enemy1:
			fire_cnt--; killobj (n); points = 450;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode1, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,3); break;
		case obj_enemy2:
			fire_cnt--; killobj (n); points = 800;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode2, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,3); break;
		case obj_enemy3:
			fire_cnt--; killobj (n);
			if (--objs[z].count3 < 1) {
				points = 1400;
				addobj (obj_score, objs[z].x, objs[z].y,0,-2);
				addobj (obj_explode6, objs[z].x,objs[z].y,0,0);
				killobj (z); snd_play (1,8);
				}
			else {
				eject (objs[z].x, objs[z].y, 5);
				snd_play (1,6);
				if (first_e3) {
					first_e3 = 0;
					text (obj_msg[1],0);
					};
				}; break;
		case obj_enemy4:
			fire_cnt--; killobj (n);
			if (--objs[z].count3 < 1) {
				points = 1200;
				addobj (obj_score, objs[z].x, objs[z].y,0,-2);
				addobj (obj_explode2, objs[z].x,objs[z].y,0,0);
				killobj (z); snd_play (1,5);
				}
			else {
				eject (objs[z].x, objs[z].y, 5);
				snd_play (1,4);
				if (first_e4) {
					first_e4 = 0;
					text (obj_msg[0],0);
					};
				}; break;
		case obj_enemy5:
			fire_cnt--; killobj (n);
			points = 1000;
			addobj (obj_score, objs[z].x, objs[z].y,0,-2);
			addobj (obj_explode2, objs[z].x, objs[z].y,0,0);
			killobj (z); snd_play (1,20); break;
		case obj_enemy6:
			fire_cnt--; killobj (n);
			if (--objs[z].count3 < 1) {
				points = 6400;
				addobj (obj_score, objs[z].x, objs[z].y, 0,-2);
				addobj (obj_explode2, objs[z].x,objs[z].y,0,0);
				killobj (z); snd_play (1,8);
				}
			else {
				eject (objs[z].x, objs[z].y, 5);
				snd_play (1,4);
				if (first_e6) {
					first_e6 = 0;
					text (obj_msg[2],0);
					};
				}; break;
		case obj_enemy7:
			fire_cnt--; killobj (n);
			if (--objs[z].count3 < 1) {
				points = 840;
				addobj (obj_score, objs[z].x, objs[z].y, 0,-2);
				addobj (obj_explode6, objs[z].x,objs[z].y,0,0);
				killobj (z); snd_play (1,5);
				}
			else {
				eject (objs[z].x, objs[z].y, 5);
				snd_play (1,6);
				if (first_e7) {
					first_e7 = 0;
					text (obj_msg[0],0);
					};
				}; break;
		case obj_enemy8:
			fire_cnt--; killobj (n);	points = 1000;
			addobj (obj_score, objs[z].x, objs[z].y,0,-2);
			addobj (obj_explode2, objs[z].x, objs[z].y,0,0);
			killobj (z); snd_play (1,5); break;
		case obj_enemy9:
			fire_cnt--; killobj (n); points = 860;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode1, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,8); break;
		case obj_enemya:
			fire_cnt--; killobj (n);
			if (--objs[z].count3 < 1) {
				points = 1600;
				addobj (obj_score, objs[z].x, objs[z].y,0,-2);
				addobj (obj_explode2, objs[z].x, objs[z].y,0,0);
				killobj (z); snd_play (1,5);
				}
			else {
				eject (objs[z].x, objs[z].y, 5);
				snd_play (1,6);
				if (first_ea) {
					first_ea = 0;
					text (obj_msg[0],0);
					};
				}; break;
		case obj_enemyb:
			fire_cnt--; killobj (n);
			if (--objs[z].count3 < 1) {
				points = 2000;
				addobj (obj_score, objs[z].x, objs[z].y,0,-2);
				addobj (obj_explode6, objs[z].x, objs[z].y,0,0);
				killobj (z); snd_play (1,8);
				}
			else {
				eject (objs[z].x, objs[z].y, 3);
				snd_play (1,16);
				if (first_eb) {
					first_eb = 0;
					text (obj_msg[2],0);
					};
				}; break;
		case obj_enemyc:
			fire_cnt--; killobj (n); points = 1500;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode2, objs[z].x, objs[z].y, 0, 0);
			eject (objs[z].x, objs[z].y, 5);
			killobj (z); snd_play (1,8); break;
		case obj_enemyd:
			fire_cnt--; killobj (n); points = 460;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode1, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,6); break;
		case obj_enemye:
			fire_cnt--; killobj (n); points = 700;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode6, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,16); break;
		case obj_enemyf:
			fire_cnt--; killobj (n); points = 800;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode6, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,16); break;
		case obj_enemyg:
			fire_cnt--; killobj (n); points = 1200;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode2, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,20); break;
		case obj_enemyh:
			fire_cnt--; killobj (n); points = 1000;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode2, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,8); break;
		case obj_enemyi:
			fire_cnt--; killobj (n);
			if (--objs[z].count3 < 1) {
				points = 12000;
				addobj (obj_score, objs[z].x, objs[z].y,0,-2);
				addobj (obj_explode3, objs[z].x, objs[z].y,0,0);
				killobj (z); snd_play (1,5);
				}
			else {
				eject (objs[z].x, objs[z].y, 5);
				snd_play (1,4);
				if (first_ei) {
					first_ei = 0;
					text (obj_msg[3],0);
					};
				}; break;
		case obj_enemyj:
			fire_cnt--; killobj (n); points = 1200;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode6, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,16); break;
		case obj_enemyk:
			fire_cnt--; killobj (n); points = 640;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode2, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,16); break;
		case obj_bullet2:
		case obj_spinner:
			fire_cnt--; killobj (n);
			snd_play (1,9); killobj (z);
			};
		};

void hit_bomb (int16_t n, int16_t z) {
	switch (objs[z].kind) {
		case obj_enemy1:
			weapon_cnt--; killobj (n); points = 450;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode1, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,3); break;
		case obj_enemy2:
			weapon_cnt--; killobj (n); points = 800;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode2, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,3); break;
		case obj_enemy3:
			weapon_cnt--; killobj (n); points = 700;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode6, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,8); break;
		case obj_enemy4:
			weapon_cnt--; killobj (n); points = 600;
			addobj (obj_score, objs[z].x, objs[z].y,0,-2);
			addobj (obj_explode2, objs[z].x, objs[z].y,0,0);
			killobj (z); snd_play (1,5); break;
		case obj_enemy5:
			weapon_cnt--; killobj (n); points = 1000;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode2, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,20); break;
		case obj_enemy6:
			weapon_cnt--; killobj (n); points = 3200;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode2, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,8); break;
		case obj_enemy7:
			weapon_cnt--; killobj (n); points = 420;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode6, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,5); break;
		case obj_enemy8:
			weapon_cnt--; killobj (n); points = 1000;
			addobj (obj_score, objs[z].x, objs[z].y,0,-2);
			addobj (obj_explode2, objs[z].x, objs[z].y,0,0);
			killobj (z); snd_play (1,5); break;
		case obj_enemy9:
			weapon_cnt--; killobj (n); points = 860;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode1, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,8); break;
		case obj_enemya:
			weapon_cnt--; killobj (n); points = 800;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode2, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,5); break;
		case obj_enemyb:
			weapon_cnt--; killobj (n); points = 1000;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode6, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,8); break;
		case obj_enemyc:
			weapon_cnt--; killobj (n); points = 1500;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode2, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,8); break;
		case obj_enemyd:
			weapon_cnt--; killobj (n); points = 460;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode1, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,16); break;
		case obj_enemye:
			weapon_cnt--; killobj (n); points = 700;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode6, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,16); break;
		case obj_enemyf:
			weapon_cnt--; killobj (n); points = 800;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode6, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,16); break;
		case obj_enemyg:
			weapon_cnt--; killobj (n); points = 1200;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode2, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,20); break;
		case obj_enemyh:
			weapon_cnt--; killobj (n); points = 1000;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode2, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,8); break;
		case obj_enemyi:
			weapon_cnt--; killobj (n); objs[z].count3 -= 2;
			if (objs[z].count3 < 1) {
				points = 6000;
				addobj (obj_score, objs[z].x, objs[z].y,0,-2);
				addobj (obj_explode3, objs[z].x, objs[z].y,0,0);
				killobj (z); snd_play (1,5);
				}
			else {
				eject (objs[z].x, objs[z].y, 5);
				snd_play (1,4);
				if (first_ei) {
					first_ei = 0;
					text (obj_msg[3],0);
					};
				}; break;
		case obj_enemyj:
			weapon_cnt--; killobj (n); points = 1200;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode6, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,16); break;
		case obj_enemyk:
			weapon_cnt--; killobj (n); points = 640;
			addobj (obj_score, objs[z].x, objs[z].y, 0, -2);
			addobj (obj_explode2, objs[z].x, objs[z].y, 0, 0);
			killobj (z); snd_play (1,16);
			};
		};

int16_t msg_bullet (int16_t n, int16_t msg, int16_t z) {		// player bullet
	switch (msg) {
		case msg_update:
			objs[n].x += objs[n].xd; objs[n].y += objs[n].yd;
			objs[n].yd--;
			if ((objs[n].x + objs[n].xl <= 0)||(objs[n].x >= scrn_x)||
				(objs[n].y + objs[n].yl <= 0)) {
				fire_cnt--; killobj (n);
				}; break;

		case msg_draw:
			drawshape (&gamevp, 0x500, objs[n].x, objs[n].y); break;

		case msg_touch:
			hit_bullet (n, z);
		}; return (0);
	};

int16_t msg_platinum (int16_t n, int16_t msg, int16_t z) {		// player bullet
	switch (msg) {
		case msg_update:
			objs[n].x += objs[n].xd; objs[n].y += objs[n].yd;
			objs[n].yd--;
			if ((objs[n].x + objs[n].xl <= 0)||(objs[n].x >= scrn_x)||
				(objs[n].y + objs[n].yl <= 0)) {
				fire_cnt--; killobj (n);
				}; break;

		case msg_draw:
			drawshape (&gamevp, 0x605, objs[n].x, objs[n].y); break;

		case msg_touch:
			hit_bullet (n, z);
		}; return (0);
	};

int16_t msg_bomb (int16_t n, int16_t msg, int16_t z) {
	int16_t sh;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&7;
			objs[n].x += objs[n].xd; objs[n].y += objs[n].yd;
			objs[n].yd--;
			if (objs[n].y + objs[n].yl <= 0) {
				weapon_cnt--; killobj(n);
				}; break;

		case msg_draw:
			sh = 0x507 + objs[n].count/2;
			drawshape (&gamevp, sh, objs[n].x, objs[n].y); break;

		case msg_touch:
			hit_bomb (n, z);
		}; return (0);
	};

int16_t msg_missile (int16_t n, int16_t msg, int16_t z) {
	int16_t sh;
	switch (msg) {
		case msg_update:
			objs[n].count = (objs[n].count + 1)&3;
			if (objs[n].count2==0) {
				if (take_inv (inv_split)) objs[n].count3 = 1;
				objs[n].count2 = 1;
				};
			objs[n].x += objs[n].xd; objs[n].y += objs[n].yd;
			objs[n].yd--;
			objs[n].x = objs[0].x;
			if (objs[n].yd <= -10) {
				if (objs[n].count3==1) {
					addobj (obj_explode2, objs[n].x, objs[n].y, 0, 0);
					addobj (obj_bomb, objs[n].x, objs[n].y, -2, -4);
					addobj (obj_bomb, objs[n].x, objs[n].y, 2, -4);
					addobj (obj_bomb, objs[n].x, objs[n].y, 0, -4);
					weapon_cnt--; killobj(n);
					};
				};
			if (objs[n].y + objs[n].yl < 0) {
				weapon_cnt--; killobj(n);
				}; break;

		case msg_draw:
			if (objs[n].count3==1) {
				sh = 0x60d + objs[n].count/2;	// Pink Panther
				}
			else {
				sh = 0x60b + objs[n].count/2;	// Big Green
				};
			drawshape (&gamevp, sh, objs[n].x, objs[n].y); break;

		case msg_touch:
			hit_bomb (n, z);
		}; return (0);
	};

void level (int16_t d_flg) {
	int16_t n;
	init_objs(); init_brd (); p_new ();
	for (n = 32; n < 43; n++) {
		shm_want [n]=0;
		};

	switch (pl.level) {
		case 1:						// sky2
			shm_want [32]=1; pcx_sh = 0x6001;
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			enemy_max = 10; star_flag = 1; break;
		case 2:						// sky2
			shm_want [32]=1; pcx_sh = 0x6001;
			addobj (obj_enemy1, rand()%240, 12, 2, 0);
			addobj (obj_enemy1, rand()%240, 12, 2, 0);
			addobj (obj_enemy1, rand()%240, 12, 2, 0);
			addobj (obj_enemy1, rand()%240, 24, 2, 0);
			addobj (obj_enemy1, rand()%240, 24, 2, 0);
			addobj (obj_enemy1, rand()%240, 24, 2, 0);
			enemy_max = 10; star_flag=1; break;
		case 3:						// sky2
			shm_want [32]=1; pcx_sh = 0x6001;
			addobj (obj_enemyb, 0, 4, 3, 0);
			addobj (obj_enemyb, 64, 34, 2, 0);
			addobj (obj_enemyb, 128, 64, 2, 0);
			addobj (obj_enemyb, 240, 94, 3, 0);
			enemy_max = 15; star_flag = 1; break;
		case 4:						// comet
			shm_want [33]=1; pcx_sh = 0x6101; cosmos ();
			addobj (obj_mines, rand()%240, rand()%96+48, 0, 0);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			enemy_max = 15; level_flg = 3; break;
		case 5:						// comet
			shm_want [33]=1; pcx_sh = 0x6101; cosmos ();
			addobj (obj_enemyh, rand()%240, -18, 2, 2);
			addobj (obj_enemyh, rand()%240, -18, 2, 2);
			addobj (obj_enemyh, rand()%240, -18, 2, 2);
			addobj (obj_enemyh, rand()%240, -18, 2, 2);
			enemy_max = 15; break;
		case 6:						// comet
			shm_want [33]=1; pcx_sh = 0x6101; cosmos ();
			addobj (obj_mines, rand()%240, rand()%96+48, 0, 0);
			addobj (obj_mines, rand()%240, rand()%96+48, 0, 0);
			addobj (obj_enemy3, 0, 4, 2, 1);
			addobj (obj_enemy3, 112, 8, 3, 1);
			addobj (obj_enemy3, 170, 4, 2, 1);
			enemy_max = 15; break;
		case 7:						// c_planet
			shm_want [34]=1; pcx_sh = 0x6201; cosmos ();
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			enemy_max = 15; level_flg = 3; break;
		case 8:						// c_planet
			shm_want [34]=1; pcx_sh = 0x6201; cosmos ();
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy6, 0, 3, 2, 4);
			addobj (obj_enemy6, 80, 30, 2, 4);
			enemy_max = 15; level_flg = 3; break;
		case 9:						// c_planet
			shm_want [34]=1; pcx_sh = 0x6201; cosmos ();
			addobj (obj_enemyb, 0, 4, 2, 0);
			addobj (obj_enemyb, 64, 34, 3, 0);
			addobj (obj_enemy9, rand()%240, -16, 2, 2);
			addobj (obj_enemy9, rand()%240, -16, 2, 2);
			enemy_max = 15; break;
		case 10:						// pollen4b
			shm_want [35]=1; pcx_sh = 0x6301;
			addobj (obj_enemya, rand()%240, -20, 0, 8);
			addobj (obj_enemya, rand()%240, -20, 0, 8);
			addobj (obj_enemya, rand()%240, -20, 0, 8);
			enemy_max = 15; star_flag = 1; level_flg = 3; break;
		case 11:						// pollen4b
			shm_want [35]=1; pcx_sh = 0x6301;
			addobj (obj_enemy3, 0, 4, 2, 1);
			addobj (obj_enemy3, 112, 8, 3, 1);
			addobj (obj_enemy3, 170, 4, 2, 1);
			enemy_max = 15; star_flag = 1; break;
		case 12:						// pollen4b
			shm_want [35]=1; pcx_sh = 0x6301;
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			enemy_max = 15; star_flag = 1; break;
		case 13:						// space4
			shm_want [36]=1; pcx_sh = 0x6401;
			addobj (obj_enemyk, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemyk, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemyk, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemyk, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemyi, 0, 6, 2, 6);
			enemy_max = 15; star_flag = 1; level_flg = 3; break;
		case 14:						// space4
			shm_want [36]=1; pcx_sh = 0x6401;
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			enemy_max = 15; star_flag = 1; break;
		case 15:						// space4
			shm_want [36]=1; pcx_sh = 0x6401;
			addobj (obj_enemyf, rand()%240, -12, 2, 2);
			addobj (obj_enemyf, rand()%240, -12, 2, 2);
			addobj (obj_enemyf, rand()%240, -12, 2, 2);
			addobj (obj_enemyf, rand()%240, -12, 2, 2);
			enemy_max = 15; star_flag = 1; break;
		case 16:						// space6b
			shm_want [37]=1; pcx_sh = 0x6501; cosmos ();
			addobj (obj_mines, rand()%240, rand()%96+48, 0, 0);
			addobj (obj_mines, rand()%240, rand()%96+48, 0, 0);
			addobj (obj_enemy6, 0, 3, 2, 4);
			addobj (obj_enemy6, 80, 30, 2, 4);
			enemy_max = 20; level_flg = 3; break;
		case 17:						// space6b
			shm_want [37]=1; pcx_sh = 0x6501; cosmos ();
			addobj (obj_enemy1, rand()%240, 12, 2, 0);
			addobj (obj_enemy1, rand()%240, 12, 2, 0);
			addobj (obj_enemy1, rand()%240, 12, 2, 0);
			addobj (obj_enemy1, rand()%240, 24, 2, 0);
			addobj (obj_enemy1, rand()%240, 24, 2, 0);
			addobj (obj_enemy1, rand()%240, 24, 2, 0);
			addobj (obj_enemy1, rand()%240, 36, 2, 0);
			addobj (obj_enemy1, rand()%240, 36, 2, 0);
			addobj (obj_enemy1, rand()%240, 36, 2, 0);
			enemy_max = 20; level_flg = 3; break;
		case 18:						// space6b
			shm_want [37]=1; pcx_sh = 0x6501; cosmos ();
			addobj (obj_mines, rand()%240, rand()%96+48, 0, 0);
			addobj (obj_mines, rand()%240, rand()%96+48, 0, 0);
			addobj (obj_mines, rand()%240, rand()%96+48, 0, 0);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			enemy_max = 20; break;
		case 19:						// palace
			shm_want [38]=1; pcx_sh = 0x6601;
			addobj (obj_enemyb, 0, 8, 3, 0);
			addobj (obj_enemyb, 16, 35, 2, 0);
			addobj (obj_enemyb, 32, 62, 3, 0);
			addobj (obj_enemyb, 48, 89, 2, 0);
			addobj (obj_enemyb, 64, 116, 3, 0);
			enemy_max = 20; star_flag = 1; break;
		case 20:						// palace
			shm_want [38]=1; pcx_sh = 0x6601;
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			enemy_max = 20; star_flag = 1; break;
		case 21:						// palace
			shm_want [38]=1; pcx_sh = 0x6601;
			addobj (obj_enemyh, rand()%240, -18, 2, 2);
			addobj (obj_enemyh, rand()%240, -18, 2, 2);
			addobj (obj_enemyh, rand()%240, -18, 2, 2);
			addobj (obj_enemyi, 0, 6, 2, 6);
			enemy_max = 20; star_flag = 1; level_flg = 3; break;
		case 22:						// object
			shm_want [39]=1; pcx_sh = 0x6701;
			addobj (obj_enemy3, 0, 4, 2, 1);
			addobj (obj_enemy3, 112, 8, 3, 1);
			addobj (obj_enemy3, 170, 4, 2, 1);
			addobj (obj_enemyk, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemyk, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemyk, rand()%240, rand()%32, 2, 2);
			enemy_max = 15; star_flag = 1; break;
		case 23:						// object
			shm_want [39]=1; pcx_sh = 0x6701;
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			enemy_max = 20; star_flag = 1; break;
		case 24:						// object
			shm_want [39]=1; pcx_sh = 0x6701;
			addobj (obj_enemy9, rand()%240, -16, 2, 2);
			addobj (obj_enemy9, rand()%240, -16, 2, 2);
			addobj (obj_enemy9, rand()%240, -16, 2, 2);
			addobj (obj_enemy9, rand()%240, -16, 2, 2);
			enemy_max = 20; star_flag = 1; break;
		case 25:						// orion
			shm_want [40]=1; pcx_sh = 0x6801; cosmos ();
			addobj (obj_mines, rand()%240, rand()%96+48, 0, 0);
			addobj (obj_enemy6, 0, 3, 2, 4);
			addobj (obj_enemy6, 80, 30, 2, 4);
			addobj (obj_enemyh, rand()%240, -18, 2, 2);
			addobj (obj_enemyh, rand()%240, -18, 2, 2);
			addobj (obj_enemyh, rand()%240, -18, 2, 2);
			enemy_max = 20; level_flg = 3; break;
		case 26:						// orion
			shm_want [40]=1; pcx_sh = 0x6801; cosmos ();
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemye, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemyk, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemyk, rand()%240, rand()%32, 2, 2);
			enemy_max = 25; break;
		case 27:						// orion
			shm_want [40]=1; pcx_sh = 0x6801; cosmos ();
			addobj (obj_mines, rand()%240, rand()%96+48, 0, 0);
			addobj (obj_mines, rand()%240, rand()%96+48, 0, 0);
			addobj (obj_mines, rand()%240, rand()%96+48, 0, 0);
			addobj (obj_enemya, rand()%240, -20, 0, 8);
			addobj (obj_enemya, rand()%240, -20, 0, 8);
			addobj (obj_enemya, rand()%240, -20, 0, 8);
			addobj (obj_enemy3, 112, 8, 3, 1);
			enemy_max = 25; level_flg = 3; break;
		case 28:						// design2
			shm_want [41]=1; pcx_sh = 0x6901;
			addobj (obj_enemyb, 0, 8, 3, 0);
			addobj (obj_enemyb, 16, 35, 2, 0);
			addobj (obj_enemyb, 32, 62, 3, 0);
			addobj (obj_enemyb, 48, 89, 2, 0);
			addobj (obj_enemyb, 64, 116, 3, 0);
			enemy_max = 25; star_flag = 1; break;
		case 29:						// design2
			shm_want [41]=1; pcx_sh = 0x6901;
			addobj (obj_enemy3, 0, 4, 2, 1);
			addobj (obj_enemy3, 112, 8, 3, 1);
			addobj (obj_enemy3, 170, 4, 2, 1);
			addobj (obj_enemy4, rand()%240, -24, 0, 8);
			addobj (obj_enemy4, rand()%240, -24, 0, 8);
			enemy_max = 25; star_flag = 1; level_flg = 3; break;
		case 30:						// design2
			shm_want [41]=1; pcx_sh = 0x6901;
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemy7, rand()%240, rand()%32, 2, 2);
			addobj (obj_enemyi, 0, 6, 2, 6);
			addobj (obj_enemyi, 32, 24, 4, 6);
			enemy_max = 30; star_flag = 1; level_flg = 3; break;
		};
	if (d_flg) addobj (obj_demo, 8, 8, 0, 0);
	addobj (obj_level1, 96, 88, 0, 0); shm_do ();
	};

void upd_objs2 (void) {
	if ((xr_random(2000)==0) && (pl.level > 3)) {
		text (rnd_msg[xr_random(10)],0);
		snd_play (2,19);
		};
	if (star_flag==0) {
		if (xr_random(300)==0) addobj (obj_falling,rand()%240,rand()%176,6,1);
		};
	if (xr_random(600)==0) addobj (obj_triple, 8, 54, 2, 2);
	if (xr_random(425)==0) addobj (obj_bonus1, rand()%240, -14, 0, 2);
	if ((xr_random(900)==0)&&(bonus_flg1==0)) {
		addobj (obj_bonus2, rand()%240, -10, 0, 2);
		bonus_flg1 = 1;
		};
	if (xr_random(450)==0) addobj (obj_bonus3, rand()%240, -10, 0, 2);
	if (xr_random(400)==0) addobj (obj_bonus4, rand()%240, -10, 0, 2);
	if ((xr_random(800)==0)&&(bonus_flg2==0)) {
		addobj (obj_bonus5, rand()%240, -10, 0, 2);
		if (pl.level < 21) bonus_flg2 = 1;
		};
	if (((pl.level==6)||(pl.level==12)||(pl.level==18)||
		(pl.level==24)||(pl.level==30))&&(bonus_flg3==0)) {
		addobj (obj_max, rand()%240, -20, 0, 3);
		bonus_flg3 = 1;
		};
	if (xr_random(80)==0) {x_pnt = rand()%224; y_pnt = rand()%176;};
	if (xr_random(100)==0) {x_pnt2 = rand()%224; y_pnt2 = rand()%156;};
	if (xr_random(60)==0) {x_pnt3 = rand()%224; y_pnt3 = 36;};

	if (((xr_random(200)==0)||(class_cnt(class_enemy) < 2))&&
		(class_cnt(class_enemy) < level_flg)) {
		if (enemy_cnt < (enemy_max + 1)) {
			switch (pl.level) {
				case 1:; case 11:; case 16:; case 19:; case 24:; case 29:
					addobj (obj_enemyj, rand()%240, -18, 0, 8);
					addobj (obj_enemyj, rand()%240, -18, 0, 8); break;
				case 2:; case 10:
					addobj (obj_enemyh, rand()%240, -18, 2, 2); break;
				case 3:; case 20:; case 27:
					addobj (obj_enemyf, rand()%240, -12, 2, 2);
					addobj (obj_enemyf, rand()%240, -12, 2, 2); break;
				case 4:
					addobj (obj_enemy8, 56, -18, 0, 8);
					addobj (obj_enemy8, 88, -78, 0, 8);
					addobj (obj_enemy8, 120, -48, 0, 8);
					addobj (obj_enemy8, 152, -78, 0, 8);
					addobj (obj_enemy8, 184, -18, 0, 8); break;
				case 5:; case 13:; case 23:
					addobj (obj_enemyc, rand()%240, -24, 0, 8);
					addobj (obj_enemyc, rand()%240, -24, 0, 8); break;
				case 6:; case 12:
					addobj (obj_enemy4, rand()%240, -24, 0, 8); break;
				case 7:
					addobj (obj_enemy5, 56, -20, 0, 8);
					addobj (obj_enemy5, 120, -50, 0, 8);
					addobj (obj_enemy5, 184, -20, 0, 8); break;
				case 8:; case 25:
					addobj (obj_enemy2, rand()%240, -12, 2, 2); break;
				case 9:
					addobj (obj_enemyd, rand()%240, -16, 2, 2); break;
				case 14:; case 22:
					addobj (obj_enemyg, rand()%240, -16, 0, 6);
					addobj (obj_enemyg, rand()%240, -16, 0, 6); break;
				case 15:; case 21:
					addobj (obj_enemy5, rand()%240, -20, 0, 8);
					addobj (obj_enemy5, rand()%240, -50, 0, 8); break;
				case 17:
					addobj (obj_enemy5, 56, -20, 0, 8);
					addobj (obj_enemy8, 88, -68, 0, 8);
					addobj (obj_enemy8, 120, -44, 0, 8);
					addobj (obj_enemy8, 152, -68, 0, 8);
					addobj (obj_enemy5, 184, -20, 0, 8); break;
				case 18:
					switch (xr_random(3)) {
						case 0:
							addobj (obj_enemyg, rand()%240, -16, 0,6);
							addobj (obj_enemyg, rand()%240, -16, 0,6);
							break;
						case 1:
							addobj (obj_enemy2, rand()%240, -12, 2,2);
							addobj (obj_enemy2, rand()%240, -12, 2,2);
							break;
						}; break;
				case 26:
					addobj (obj_enemy8, rand()%240, -18, 0, 8);
					addobj (obj_enemy8, rand()%240, -48, 0, 8); break;
				case 28:
					switch (xr_random(3)) {
						case 0:
							addobj (obj_enemyg, rand()%240, -16, 0,6);
							addobj (obj_enemyg, rand()%240, -16, 0,6);
							break;
						case 1:
							addobj (obj_enemyh, rand()%240, -18, 2,2);
							addobj (obj_enemyh, rand()%240, -18, 2,2);
							break;
						}; break;
				case 30:
					switch (xr_random(3)) {
						case 0:
							addobj (obj_enemyc, rand()%240, -24, 0,8);
							addobj (obj_enemyc, rand()%240, -24, 0,8);
							break;
						case 1:
							addobj (obj_enemyj, rand()%240, -18, 0,8);
							addobj (obj_enemyj, rand()%240, -18, 0,8);
						};
				};
			}
		else enemy_flag = 0;
		};
	};

void wait (void) {
	clrpal (); setpagemode (1);
	defwin (&waitwin, 8, 64, 11, 3, 0, 0, textbox);
	drawwin (&waitwin);
	fontcolor (&waitwin.inside, 12, -1);
	wprint (&waitwin.inside, 16, 2, 1, "A NEW RELEASE FROM");
	wprint (&waitwin.inside, 32, 14, 1, "EPIC MEGAGAMES");
	fontcolor (&waitwin.inside, 13, -1);
	wprint (&waitwin.inside, 48, 26, 1, "WRITTEN BY");
	wprint (&waitwin.inside, 36, 38, 1, "ALLEN PILGRIM");
	fontcolor (&mainvp, 15, 0);
	wprint (&mainvp, 79, 144, 2, "Now Loading, Please Wait...");
	pageflip (); setpagemode (0);	fadein ();
	};

void wait2 (void) {
	int16_t x, y;
	clrpal (); setpagemode (1);
	for (x = 0; x < 20; x++) {
		for (y = 0; y < end_y; y++) {
			drawshape (&mainvp, 0x5e01 + x + y * 20, x * 16, y * 16);
			};
		};
	fontcolor (&mainvp, 10, -1);
	wprint (&mainvp, 4, 10, 1, "KILOBLASTER");
	wprint (&mainvp, 8, 22, 1, " VOLUME 2 ");
	fontcolor (&mainvp, 14, -1);
	wprint (&mainvp, 8, 34, 1, "NO WAY OUT");
	pageflip (); setpagemode (0);	fadein ();
	};

void wait4 (void) {
	int16_t x, y;
	clrpal (); setpagemode (1);
	for (x = 0; x < 20; x++) {
		for (y = 0; y < end_y; y++) {
			drawshape (&mainvp, 0x5f01 + x + y * 20, x * 16, y * 16);
			};
		};
	fontcolor (&mainvp, 10, -1);
	wprint (&mainvp, 4, 10, 2, "THE ADVENTURE");
	wprint (&mainvp, 4, 22, 2, "OF A LIFETIME");
	wprint (&mainvp, 4, 34, 2, " AWAITS YOU! ");
	pageflip (); setpagemode (0);	fadein ();
	};

void story (int16_t page) {
	int16_t x, y;
	char tempstr[2];
	wprint (&statvp, 20, 22, 1, _itoa (page, tempstr, 10));
	switch (page) {
		case 1:
			clrvp (&gamevp, 0); setpagemode (1);
			for (x = 0; x < end_x; x++) {
				for (y = 0; y < end_y; y++) {
					drawshape (&gamevp, pcx_sh+x+y*16, x*16, y*16);
					};
				};
			fontcolor (&gamevp, 15, -1);
		wprint (&gamevp, 26, 24,  2, "     THE STORY CONTINUES. . .     ");
			fontcolor (&gamevp, 10, -1);
		wprint (&gamevp, 26, 34,  2, "   You find yourself at the mercy ");
		wprint (&gamevp, 26, 44,  2, "of the powerful alien fighters. It");
		wprint (&gamevp, 26, 54,  2, "seems that the end is near and you");
		wprint (&gamevp, 26, 64,  2, "don't know if you will make it out");
		wprint (&gamevp, 26, 74,  2, "this time.                        ");
			fontcolor (&gamevp, 14, -1);
		wprint (&gamevp, 26, 84,  2, "   As you scan the sector of space");
		wprint (&gamevp, 26, 94,  2, "you are in you detect two blips on");
		wprint (&gamevp, 26, 104, 2, "the scanner. You raise the shields");
		wprint (&gamevp, 26, 114, 2, "and prepare for another battle in:");
			fontcolor (&gamevp, 11, -1);
		wprint (&gamevp, 26, 124, 2, "      < < < NO WAY OUT > > >      ");
			fontcolor (&gamevp, 14, -1);
		wprint (&gamevp, 26, 134, 2, "   This volume features new songs,");
		wprint (&gamevp, 26, 144, 2, "all new digitized backgrounds, all");
		wprint (&gamevp, 26, 154, 2, "kinds of new and interesting space");
		wprint (&gamevp, 26, 164, 2, "vessels and 30 more levels of fun!");
			pageflip (); setpagemode (0); break;
		case 2:
			clrvp (&gamevp, 0); setpagemode (1);
			for (x = 0; x < end_x; x++) {
				for (y = 0; y < end_y; y++) {
					drawshape (&gamevp, pcx_sh+x+y*16, x*16, y*16);
					};
				};
			fontcolor (&gamevp, 11, -1);
		wprint (&gamevp, 28, 24, 1, " - - - BONUS ITEMS - - - ");
			fontcolor (&gamevp, 10, -1);
		wprint (&gamevp, 26, 44,  2, "Strawberries give you wingmen");
			drawshape (&gamevp, 0x1203, 214, 42);
		wprint (&gamevp, 26, 64, 2, "Bananas give you 1,000 points");
			drawshape (&gamevp, 0x1202, 214, 62);
			fontcolor (&gamevp, 13, -1);
		wprint (&gamevp, 26, 84,  2, "Apples increase your shields!");
			drawshape (&gamevp, 0x1201, 214, 82);
		wprint (&gamevp, 26, 104, 2, "Spinach makes you invincible!");
			drawshape (&gamevp, 0x1200, 214, 102);
			fontcolor (&gamevp, 14, -1);
		wprint (&gamevp, 26, 124, 2, "Get this for triple firepower");
			drawshape (&gamevp, 0xd00, 214, 122);
		wprint (&gamevp, 26, 144,  2, "Get this for maximum shields!");
			drawshape (&gamevp, 0x160f, 212, 137);
			pageflip (); setpagemode (0); break;
		case 3:
			clrvp (&gamevp, 0); setpagemode (1);
			for (x = 0; x < end_x; x++) {
				for (y = 0; y < end_y; y++) {
					drawshape (&gamevp, pcx_sh+x+y*16, x*16, y*16);
					};
				};
			fontcolor (&gamevp, 10, -1);
		wprint (&gamevp, 26, 24, 2, "This pulsating orb gives you");
		wprint (&gamevp, 26, 34, 2, "either a guided missile or a");
		wprint (&gamevp, 26, 44, 2, "multiple warhead missile!");
			drawshape (&gamevp, 0x1600+80, 213, 30);
			fontcolor (&gamevp, 11, -1);
		wprint (&gamevp, 26, 60, 2, "This bomb is used by wingmen");
			drawshape (&gamevp, 0x507, 214, 59);	// bomb (6x8)
		wprint (&gamevp, 26, 108, 2, "The patriot is a guided missile");
		wprint (&gamevp, 26, 124, 2, "system with a 98.7% kill ratio!");
			drawshape (&gamevp, 0x60b, 218, 109);	// Patriot (10x20)
			fontcolor (&gamevp, 14, -1);
		wprint (&gamevp, 26, 76,  2, "The pink panther has a multiple");
		wprint (&gamevp, 26, 92,  2, "warhead and is very powerful. .");
			drawshape (&gamevp, 0x60d, 218, 77);	// Pink Panther (10x20)
		wprint (&gamevp, 32, 146, 1, "-- HANG TOUGH CAPTAIN --");
			pageflip (); setpagemode (0); break;
		case 4:
			loadboard ("story2"); drawboard ();
			fontcolor (&gamevp, 10, 8);
			wprint (&gamevp, 28, 4, 1, "* * * Enemy Objects * * *");
			wprint (&gamevp, 83, 178, 2, "Beware of death lasers!");
			fontcolor (&gamevp, 14, -1);
			drawshape (&gamevp, 0x140f, 27, 24);		// (10x16)
			wprint (&gamevp, 72, 28, 1,  "   Blue Spearhead   ");
			drawshape (&gamevp, 0x1600+75, 25, 71);		// (14x18)
			wprint (&gamevp, 76, 76, 1,  "   Death Sting 2   ");
			drawshape (&gamevp, 0x1600+20, 26,120);		// (12x16)
			wprint (&gamevp, 72, 124, 1, "    Bird of Prey    ");
			drawshape (&gamevp, 0x1600+55, 26,166);		// (12x20)
			wprint (&gamevp, 76, 166, 1, "    SS Blizzard    "); break;
		case 5:
			loadboard ("story2"); drawboard ();
			fontcolor (&gamevp, 10, 8);
			wprint (&gamevp, 28, 4, 1, "* * * Enemy Objects * * *");
			fontcolor (&gamevp, 14, -1);
			drawshape (&gamevp, 0x1600+50, 26, 22);		// (12x20)
			wprint (&gamevp, 76, 28, 1,  "Ionian Assault Ship");
			drawshape (&gamevp, 0x1600+28, 26, 72);		// (12x16)
			wprint (&gamevp, 72, 76, 1,  "      Sentinel      ");
			drawshape (&gamevp, 0x1400+21, 22,117);		// (20x22)
			wprint (&gamevp, 72, 124, 1, "   X-15 Destroyer   ");
			drawshape (&gamevp, 0x1600+69, 26,166);		// (12x20)
			wprint (&gamevp, 76, 172, 1, "    Thunderbolt    "); break;
		case 6:
			loadboard ("story2"); drawboard ();
			fontcolor (&gamevp, 10, 8);
			wprint (&gamevp, 28, 4, 1, "* * * Enemy Objects * * *");
			fontcolor (&gamevp, 14, -1);
			drawshape (&gamevp, 0x1600+64, 25, 22);		// (14x20)
			wprint (&gamevp, 76, 28, 1,  "    Widow Maker    ");
			drawshape (&gamevp, 0xa00, 25, 74);		// (14x12)
			wprint (&gamevp, 76, 76, 1,  "Small Orzot Fighter");
			drawshape (&gamevp, 0x1301, 25, 121);		// (14x14)
			wprint (&gamevp, 72, 124, 1, "Kilofire Interceptor");
			drawshape (&gamevp, 0x1600+85, 25,166);		// (14x20)
			wprint (&gamevp, 76, 172, 1, "  Planet Smasher!  "); break;
		case 7:
			loadboard ("story2"); drawboard ();
			fontcolor (&gamevp, 10, 8);
			wprint (&gamevp, 28, 4, 1, "* * * Enemy Objects * * *");
			fontcolor (&gamevp, 14, -1);
			drawshape (&gamevp, 0x1400, 26, 20);		// (12x24)
			wprint (&gamevp, 76, 28, 1,  "   Alpha Xyntari   ");
			drawshape (&gamevp, 0x1600+23, 26, 71);		// (12x18)
			wprint (&gamevp, 76, 76, 1,  "      Javelin      ");
			drawshape (&gamevp, 0x1600+11, 24, 119);	// (16x18)
			wprint (&gamevp, 76, 124, 1, " Antagon's Revenge ");
			drawshape (&gamevp, 0x140c, 27, 168);		// (10x16)
			wprint (&gamevp, 72, 172, 1, "     Apocalypse     "); break;
		case 8:
			loadboard ("story3"); drawboard ();
			fontcolor (&gamevp, 11, 1);
			wprint (&gamevp, 28, 4, 1, "* * * Enemy Objects * * *");
			fontcolor (&gamevp, 14, -1);
			drawshape (&gamevp, 0x900, 26, 28);		// (12x8)
			wprint (&gamevp, 72, 28, 1,  "  Colonial Warrior  ");
			drawshape (&gamevp, 0x1406, 27, 72);		// (10x16)
			wprint (&gamevp, 76, 76, 1,  "   Electra Flame   ");
			drawshape (&gamevp, 0x1600, 26, 119);		// (12x18)
			wprint (&gamevp, 72, 124, 1, "   Strike Fighter   ");
			drawshape (&gamevp, 0x1600+45, 24, 164);	// (16x24)
			wprint (&gamevp, 76, 172, 1, "    M-15 Abrams    ");
		};
	};

void menu (void) {
	int16_t cur = 0;
	int16_t oldcur = 1;
	int16_t count = 0;
	int16_t oldclock = 0;
	int16_t moveclock = 0;
	int16_t democlock = 0;
	int16_t done = 0;
	int16_t gotkey, c = 0;
	int16_t n, x, y;
	int16_t page;
	char s[2];
	char keytab[]="PLWSIHDACE";
	text (k_msg,1); refresh (pagemode);

	do {
		loadboard ("menu");				// background for menu screen
		setpagemode (1);
		init_colors();
		drawboard ();
		pageflip (); setpagemode (0);

		setpagemode (1);
		defwin (&menuwin,7,40,8,6,0,0,textbox);
		drawwin (&menuwin);
		democlock = getclock();
		snd_play (1,22);
		fontcolor (&menuwin.inside, 15, -1);
		wprint (&menuwin.inside, 10, 4, 2, "--- MAIN MENU ---");

		fontcolor (&menuwin.inside, 14, -1);
		wprint (&menuwin.inside, 25, 16, 2, "P");
		wprint (&menuwin.inside, 25, 24, 2, "L");
		wprint (&menuwin.inside, 25, 32, 2, "W");
		wprint (&menuwin.inside, 25, 40, 2, "S");
		wprint (&menuwin.inside, 25, 48, 2, "I");
		wprint (&menuwin.inside, 25, 56, 2, "H");
		wprint (&menuwin.inside, 25, 64, 2, "D");
		wprint (&menuwin.inside, 25, 72, 2, "A");
		wprint (&menuwin.inside, 25, 80, 2, "C");
		wprint (&menuwin.inside, 25, 88, 2, "E");

		fontcolor (&menuwin.inside, 11, -1);
		wprint (&menuwin.inside, 31, 16, 2, "LAY NEW GAME");
		wprint (&menuwin.inside, 31, 24, 2, "OAD GAME");
		wprint (&menuwin.inside, 31, 32, 2, "ARP TO LEVEL");
		wprint (&menuwin.inside, 31, 40, 2, "TORY");
		wprint (&menuwin.inside, 31, 48, 2, "NSTRUCTIONS");
		wprint (&menuwin.inside, 31, 56, 2, "IGH SCORES");
		wprint (&menuwin.inside, 31, 64, 2, "EMO");
		wprint (&menuwin.inside, 31, 72, 2, "BOUT EPIC");
		wprint (&menuwin.inside, 31, 80, 2, "REDITS");
		fontcolor (&menuwin.inside, 15, -1);
		wprint (&menuwin.inside, 31, 88, 2, "XIT");
		pageflip (); setpagemode (0);

		do {
			if ((oldcur != cur) || (getclock() != oldclock)) {
				if (++count >= 12) count = 0;
				if ((count & 1) || (oldcur != cur)) {
					drawshape (&menuwin.inside, 0x4709, 8, 16+oldcur*8);
					drawshape (&menuwin.inside,
						0x0201 + (count >> 1), 8, 16 + cur * 8);
					};
				oldclock = getclock(); oldcur = cur;
				};
			checkctrl0 (0);
			key = toupper(key);
			if (((dx1 + dy1) != 0) && (abs(getclock() - moveclock) > 1)) {
				moveclock = getclock();
				cur += dx1 + dy1;
				if ((cur>=0)&&(cur<10)) snd_play (1,9);
				cur = min (9, max(0, cur));
				democlock = getclock();
				};

			if ((getclock() - democlock) > 300) key = 'D';

			gotkey = 0;
			if (key==escape) key = 'E';
			if (key==187) key='I';
			if ((key==enter)||(key==' ')||(fire1)) {key=keytab[cur]; gotkey=1;}
			else
				for (c = 0; c < strlen(keytab); c++)
					if (key==keytab[c]) gotkey = 1;
			} while (!gotkey);

		setpagemode (0);
		if ((key==escape) || (key=='E')) {done = 1;}
		else if (key=='P') {
			pageflip (); setpagemode (0); play (0, 0);
			sb_playtune (song1[0]);
			}
		else if (key=='L') {
			if (loadgame()==1) {
				refresh (pagemode);	pageflip (); setpagemode (0);
				pageflip (); setpagemode (0); play (0, 1);
				sb_playtune (song1[0]);
				};
			}
		else if (key=='W') {
			fontcolor (&statvp, 12, 0);
			clearvp (&statvp);
			wprint (&statvp, 4, 4, 1,"Which");
			wprint (&statvp, 0, 16, 1,"Level?");
			fontcolor (&statvp, 10, 0);
			winput (&statvp, 16, 28, 1, _itoa (pl.level, s, 10), 2);
			pl.level = atoi(s);
			if (pl.level <= 0) pl.level = 1;
			if (pl.level==71) pl.level = 30;
			else if (pl.level >= 21) pl.level = 20;
			pageflip (); setpagemode (0); play (0, 0);
			sb_playtune (song1[0]);
			}
		else if (key=='S') {
			for (n = 32; n < 43; n++) shm_want [n]=0;
			shm_want [35]=1; pcx_sh = 0x6301; shm_do ();
			page = 1; cmds ();
			do {
				if (key==200) page--; if (key==208) page++;
				if (page < 1) page = 8; if (page > 8) page = 1;
				story (page); rest (3);
				} while (key != escape);
			}
		else if (key=='I') {
			for (n = 32; n < 43; n++) shm_want [n]=0;
			shm_want [38]=1; pcx_sh = 0x6601; shm_do ();
			page = 1; cmds ();
			do {
				if (key==200) page--; if (key==208) page++;
				if (page < 1) page = 2; if (page > 2) page = 1;
				instructions (page); rest (3);
				} while (key != escape);
			}
		else if (key=='H') {
			printhi (0);
			}
		else if (key=='D') {			// demo
			demonum = 0; macabort = 0;
			do {
				if (demonum != 0) setpagemode (1);
				pl.level = demolvl[demonum];
				pageflip (); setpagemode (0);

				playmac (demoname[demonum]);
				if (demonum < 3) {
					do {checkctrl (0);} while (!fire1);
					};
				if (macplay) {
					play (1, 0);
					stopmac ();
					demonum++;
					}
				else macaborted = 1;
				} while (!macaborted);
			sb_playtune (song1[0]);
			}
		else if (key=='A') {
			for (n = 32; n < 43; n++) shm_want [n]=0;
			shm_want [37]=1; pcx_sh = 0x6501; shm_do ();
			page = 1; cmds ();
			do {
				if (key==200) page--; if (key==208) page++;
				if (page < 1) page = 4; if (page > 4) page = 1;
				about (page); rest (3);
				} while (key != escape);
			}
		else if (key=='C') {
			for (n = 32; n < 43; n++) shm_want [n]=0;
			shm_want [36]=1; pcx_sh = 0x6401; shm_do ();
			page = 1; cmds ();
			do {
				if (key==200) page--; if (key==208) page++;
				if (page < 1) page = 2; if (page > 2) page = 1;
				credit (page); rest (3);
				} while (key != escape);
			}
		init_inv (); cmd_win (); text (k_msg,1); refresh (pagemode);
		} while (!done);
	};

void intro (void) {
	shm_want [30]=1; shm_do ();

	wait2 (); Sleep (5);
	shm_want [30]=0;
	init_colors(); fadeout (); clrvp (&mainvp, 0);
	shm_want [31]=1; shm_do ();
	wait4 (); Sleep (5);
	shm_want [31]=0;
	init_colors(); fadeout (); clrvp (&mainvp, 0); fadein ();
	};

void game_end (void) {
	text ("Press the escape key to exit",0); refresh (pagemode);
	snd_play (2,19); setpagemode (0);

	defwin (&msg_win, 4, 64, 11, 3, 0, 0, textbox); drawwin (&msg_win);
	fontcolor (&msg_win.inside, 10, -1);
	wprint (&msg_win.inside, 13, 5, 1, "- CONGRATULATIONS -");
	fontcolor (&msg_win.inside, 11, -1);
	wprint (&msg_win.inside, 3, 16, 2, "  Once again you've defeated");
	wprint (&msg_win.inside, 3, 26, 2, "the enemy. The once powerful");
	wprint (&msg_win.inside, 3, 36, 2, "fleet now retreats to safety");
	rest (1);	setpagemode (0); printhi (1);
	num_objs = 0; purgeobjs (); snd_play (8,23);

	pageflip (); setpagemode (0); clrvp (&gamevp, 0);
	shm_want [41]=0; shm_want [35]=1; pcx_sh = 0x6301; shm_do ();
	drawboard ();
	fontcolor (&gamevp, 10, -1);
	wprint (&gamevp, 40, 71,  1, "Another great job that");
	wprint (&gamevp, 40, 85,  1, "deserves  recognition.");
	wprint (&gamevp, 40, 99,  1, "Your actions bring you");
	wprint (&gamevp, 40, 113, 1, "honor in the galaxy.  ");
	setpagemode(1);
	text ("Press the escape key to exit",0); refresh (pagemode); rest (1);

	pageflip (); setpagemode (0); clrvp (&gamevp, 0);
	drawboard ();
	fontcolor (&gamevp, 11, -1);
	wprint (&gamevp, 40, 71,  1, "Your next task is more");
	wprint (&gamevp, 40, 85,  1, "difficult. You need to");
	wprint (&gamevp, 40, 99,  1, "confront the enemy in:");
	fontcolor (&gamevp, 14, -1);
	wprint (&gamevp, 40, 113, 1, "<< THE FINAL BATTLE >>");
	setpagemode(1);
	text ("Press the escape key to exit",0); refresh (pagemode); rest (1);

	pageflip (); setpagemode (0); clrvp (&gamevp, 0);
	shm_want [35]=0; shm_want [42]=1; pcx_sh = 0x6a01; shm_do ();
	drawboard (); setpagemode(1); cosmos ();
	text ("Press the escape key to exit",0); refresh (pagemode); rest (4);

	gameover = 1; winflg = 1; shm_want [42]=0;
	};

void scrn_1 (void) {
	uncrunch ((char*)K_BEGIN, scrnaddr, b_len);
	window (11, 7, 70, 16);
	gotoxy (1,1);
	textcolor (15); textbackground (1);
	clrscr ();
	};
