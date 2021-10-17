#pragma once
#define far

#include <stdio.h>
//#include <stdlib.h>

/*char *ultoa(unsigned long val, char *str, int base);
#ifndef __WIN32
char *itoa(int val, char *str, int base);
char *ltoa(long val, char *str, int base);
char *strupr(char *s);*/
//unsigned long filelength(int fd);
#define O_BINARY  0 // unused on POSIX

#define _creat creat
#define _close close
#define _open open
#define _read read
#define _write write

unsigned long coreleft(void);

#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>
void delay(int s);
int getclock();

typedef struct REGPACK
{
	unsigned int r_ax, r_bx, r_cx, r_dx;
}REGPACK;

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

void cputs(const char *s);
void textcolor(int c);
void textbackground(int c);
void enable(void);
void disable(void);
void window(int x, int y, int w, int h);
void gotoxy(int x, int y);
void clrscr(void);
int xr_random(int max);
void setmem(void *target, unsigned long len, char val);
unsigned char inportb(unsigned int port);
unsigned char outportb(unsigned int port, unsigned char value);
unsigned char outport(unsigned int port, unsigned int value);

#if defined(__GNUC__) || defined(__DMC__)
#define HAVE_STDINT_H	1
#elif defined(_MSC_VER)
typedef signed __int8		int8_t;
typedef unsigned __int8		uint8_t;
typedef signed __int16		int16_t;
typedef unsigned __int16	uint16_t;
typedef signed __int32		int32_t;
typedef unsigned __int32	uint32_t;
typedef signed __int64		int64_t;
typedef unsigned __int64	uint64_t;
#ifndef _UINTPTR_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    uintptr_t;
#else
typedef unsigned int   uintptr_t;
#endif
#define _UINTPTR_T_DEFINED
#endif
#endif

void intr(int interrupt, struct REGPACK *regs);

void lcopypage(void);

#define pal6to8(x) (((x) << 2) | ((x) >> 4))

int getch();

// 2blaster.c
void stat_win(void);
void text(const char *msg, int16_t flg);
void play(int16_t demoflg, int16_t loadflg);
int16_t loadgame(void);
void instructions(int16_t page);
void cmds(void);
void printhi(int16_t newhi);
void order(int16_t page);
void credit(int16_t page);
void cmd_win(void);
void about(int16_t page);

// 2b_vol[123].c
void level (int16_t d_flg);

// graphics.c
void dim(void);
void undim(void);

// 2b_objs.c
void explosion(int16_t x, int16_t y, int16_t num);
void eject(int16_t x, int16_t y, int16_t num);

// 2b_objmn.c
void seekplayer(int16_t n, int16_t *dx, int16_t *dy);
void move_obj(int16_t n, int16_t new_x, int16_t new_y);
int16_t class_cnt(int16_t c);
int16_t take_inv (int16_t inv_type);
void add_inv (int16_t inv_type);
int16_t inv_cnt (int16_t inv_type);
void attract(int16_t n, int16_t *ax, int16_t *ay);
void attract2(int16_t n, int16_t *ax, int16_t *ay);
void attract3(int16_t n, int16_t *ax, int16_t *ay);
void cosmos(void);

// music
#define interrupt
void nosound(void);

typedef void (*intfunc)(void);

void setvect(int interrupt, void (*handler)(void));
intfunc getvect(int interrupt);

// SDL stuff
#include <SDL/SDL.h>
extern SDL_Surface *screen;
