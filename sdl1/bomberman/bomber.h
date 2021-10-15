#ifndef BOMBER_H
#define BOMBER_H

#include <SDL.h>

typedef unsigned char uchar;
extern int xcolors[256];
#define NUMGFX 3
#define IXSIZE 640
#define IYSIZE 480


#define MYF1 0x180
#define MYF2 0x181
#define MYF3 0x182
#define MYF4 0x183
#define MYF5 0x184
#define MYF6 0x185
#define MYF7 0x186
#define MYF8 0x187
#define MYF9 0x188
#define MYF10 0x189
#define MYLEFT 0x190
#define MYRIGHT 0x191
#define MYUP 0x192
#define MYDOWN 0x193
#define MYPAGEUP 0x194
#define MYPAGEDOWN 0x195
#define MYHOME 0x196
#define MYEND 0x197
#define MYALTL 0x198
#define MYALTR 0x199

#define MYDELETE 0x7f

#define MYSHIFTED 0x40
#define MYALTED 0x200


typedef struct gfxset
{
	uchar gs_colormap[768];
	uchar gs_inout[256];
	uchar *gs_pic;
	int gs_xsize;
	int gs_ysize;
} gfxset;

typedef struct figure
{
	int	xsize,ysize;
	int	xdelta,ydelta;
	uchar *graphics;
} figure;

typedef struct solid
{
	int	xsize,ysize;
	uchar	*graphics;
} solid;

typedef struct player
{
	struct player *next;
	int xpos,ypos;
	int flags;
	int abilities;
	int speed;
	int bombsused;
	int bombsavailable;
	int flamelength;
	int *at;
	int figcount;
	int doing;
	int action;
	int color;
	int controller;
	int fixx,fixy;
	figure *figure;
} player;

#define FLG_CONTROL 1
#define FLG_DEAD 2



typedef struct sprite
{
	int	flags;
	int	xpos,ypos;
	figure	*fig;
} sprite;

typedef struct damage
{
	int xpos,ypos;
	int xsize,ysize;
} damage;

typedef struct list
{
	void *next;
} list;

typedef struct bomb
{
	struct bomb *next;
	int type;
	int xpos,ypos;
	int px,py;
	int power;
	int timer;
	int figcount;
	player *owner;
} bomb;


#define BOMB_OFF 0
#define BOMB_NORMAL 1
#define BOMB_CONTROLLED 2
#define BOMBLIFE 60
#define FLAMELIFE 15
#define DECAYLIFE 15

typedef struct flame
{
	struct flame *next;
	int xpos,ypos;
	int px,py;
	int timer;
	int lurd;
	player *owner;
} flame;

#define FL_UP 2
#define FL_DOWN 8
#define FL_LEFT 1
#define FL_RIGHT 4

typedef struct brickdecay
{
	struct brickdecay *next;
	int xpos,ypos;
	int px,py;
	int timer;
} brickdecay;

typedef struct generic
{
	struct generic *next;
	int xpos,ypos;
	int px,py;
	int timer;
	void (*process)();
	void (*draw)();
	void *ptr1,*ptr2;
	int data1,data2;
} generic;

typedef struct bonustile
{
	struct bonustile *next;
	int xpos,ypos;
	int px,py;
	int type;
}bonustile;
#define TILE_NONE -1
#define TILE_BOMB 5
#define TILE_FLAME 2
#define TILE_GOLDFLAME 7
#define TILE_CONTROL 9
#define TILE_SKATES 4
#define TILE_TURTLE 14


#define ACT_INVALID 0x88
#define ACT_NONE  0
#define ACT_UP    1
#define ACT_DOWN  2
#define ACT_LEFT  3
#define ACT_RIGHT 4
#define ACT_ENTER 5
#define ACT_QUIT 6
#define ACT_MASK 0x1f
#define ACT_PRIMARY 0x40
#define ACT_SECONDARY 0x80


#define FIELD_EMPTY 0
#define FIELD_BORDER 1
#define FIELD_BOMB 2
#define FIELD_BRICK 3
#define FIELD_FLAME 4
#define FIELD_EXPLODING 5
#define FIELD_BONUS 6

#define CODE_CONT 0
#define CODE_QUIT 1
#define CODE_ALLDEAD 2



#define MAXTHINGS 500
#define MAXSETS 8
#define MAXSPRITES 128
#define MAXDAMAGES 512
#define MAXBOMBSDETONATED 32

extern char exitflag;

extern player players[];
extern sprite sprites[];
extern gfxset gfxsets[NUMGFX];
extern uchar needwhole;
extern figure walking[MAXSETS][60];
extern damage damages[];
extern bomb bombs[];

extern void centerx(player *pl);
extern void centery(player *pl);
extern void dropbomb(player *pl,int px,int py,int type);
extern void adddetonate(bomb *bmb);
extern void flameshaft(player *owner,int px,int py,int dx,int dy,int power);
extern void detonatebomb(bomb *bmb);
extern void initlist(void *first,int size,int num);
extern void freeentry(void *entry);
extern void addtail(void *header,void *entry);
extern void delink(void *header,void *entry);
extern void *allocentry();
extern void adddecay(int px,int py);
extern void trybonus(int px,int py);
extern void addbonus(int px,int py,int type);
extern void deletebonus(bonustile *bonus);
extern void queuesequence(int xpos,int ypos,figure *fig,int count);
extern void playonce(generic *gen);
extern void processgenerics(void);
extern void drawgenerics(void);
extern void drawgeneric(generic *gen);
extern void killplayer(player *pl);
extern void adddeath(player *pl);
extern void drawbigstring(int xpos,int ypos,char *str);
extern void bigscrprintf(char *str, ...);
extern void drawstring(int xpos,int ypos,char *str);
extern void scrprintf(char *str, ...);
extern int textx,texty,fontxsize,fontysize;
extern void texthome(void);

extern unsigned char field[32][32];
extern void *info[32][32];
extern unsigned char singleoptions[10];
extern unsigned char gameoptions[10];
extern int getmsg(int);

#define GO_DENSITY 0
#define GO_FLAMES 1
#define GO_BOMBS 2
#define GO_GENEROSITY 3

// network packet types
// slave -> master packets
#define PKT_MYDATA 0 // 4 bytes unique #,4 bytes frame #, 1 byte data
#define PKT_JOIN 1 // 4 bytes unique #,16 bytes name
// master -> slave packets
#define PKT_INVITE 9 // 4 bytes unique #, any # of 1:slot,16:name sets (-1 end)
#define PKT_BEGIN 10 // clone of INVITE
#define PKT_STEP 11 // 4 bytes unique #, 4 bytes frame #, 8 bytes ACT_*
#define PKT_QUIT 12 // 4 bytes unique #
// master -> matcher packets
#define PKT_REGISTER 16 // 4:unique #,4:pword hash,4:version #,16:name, 1:status
// matcher -> master packets
#define PKT_ACK 24 // perfect copy of packet received
// slave -> matcher packets
#define PKT_QUERY 32 // 4 bytes password hash
// matcher -> slave packets
#define PKT_INFO 40 // 4: pword hash, 2: count,#(4:unique,4:IP,2:port,16:name)

// all bytes stored MSB first

/*
game startup:
<master and matcher>
Master: send REGISTER to matcher with optional password, wait for ack. If
  timout, resend.
matcher: Wait for REGISTER packet, when received maintain database. respond
  to sender with ACK. REGISTER packet can close a game also. The REGISTER
  packet sent by the master has a unique word to be used to avoid confusion.
  REGISTER packet also contains a game version #

After master registers game and receives ACK, just waits for slaves to contact.

<slave and matcher>
slave: send QUERY to matcher with optional password, wait for INFO, if timeout,
  resend.
matcher: respond to QUERY with INFO packet. matcher need not maintain any
  database for slave requests. INFO packet contains IP addr and port for each
  master machine that matches the QUERY spec (ALL or password). Only a
  certain MAX # of entries are sent if there are too many to choose from.

<slave and master>
slave: send JOIN to master, wait for INVITE. If timeout, resend. JOIN packet
  contains the unique word the master created. JOIN also contains username.
master: Respond to JOIN with INVITE. INVITE contains unique word from JOIN
  packet. INVITE either contains NO meaning game no longer exists or is closed
  or player is not invited. IF yes, INVITE contains info on other players
  already in the game (username and slot # for each). Master allocates the
  slots and avoids confusion based on IP addr and port #. INVITE also contains
  game options structure. Whenever a new player JOINS and is admitted, master
  sends updated INVITE packets to everyone already in the JOIN list. Whenever
  master changes game options, master sends out another set of INVITES

Duplicate JOINS are answered with updated INVITE but nothing changes as far
as allocation.

Master player launches game after he's satisfied everyone has joined.

Master sends BEGIN packet to everyone. BEGIN is identical to INVITE except
that the data is final. Slave must respond with its first MYDATA packet with
frame # of 0. If master times out waiting, master sends duplicate BEGIN to
wayward slaves. Once master has received MYDATA from everyone, game starts.

Within game slave sends MYDATA to master and waits for STEP packet. If
timeout, slave sends duplicate MYDATA.

If master times out waiting for a slave's MYDATA, slave gets dropped. MYDATAs
received will be answered with PKT_QUIT.

*/

int soundopen(void);
void soundclose(void);
void playsound(int n);




#endif // BOMBER_H


