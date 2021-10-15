
#ifndef GFX_H
#define GFX_H

#define KEYMAX 128


extern int usedcolors;
extern uchar mymap[];
extern int screen;
extern int fontbase,fontysize;
extern char *imageloc;

extern long map2[];
extern uchar fmap[128];
extern int buttonstate,buttondown;
extern int mxpos,mypos;

extern int pressedcodes[KEYMAX],downcodes[KEYMAX],numpressed,numdown;


void opengfx(int argc, char **argv);
void gfxlock(void);
void gfxunlock(void);
void pollinput(void);
int takedown(void);
void closegfx(void);
void greyrect(int x,int y,int xsize,int ysize);
void clearrect(int x,int y,int xsize,int ysize);
void solidfetch(gfxset *gs,solid *dest);
extern void dumpgfx();
extern void createinout(struct gfxset *);
extern void getcolors();
extern void gfxfetch(struct gfxset *,struct figure *,int);
extern void puttile(int destx,int desty,int source);
extern void store(int x,int y,int which);
extern void restore(int x,int y,int which);
extern void copyup();
extern void copyupxy(int x,int y);
extern void copyupxysize(int x,int y,int xsize,int ysize);
extern void getfigures();
extern unsigned long getcolor(char *name);  /* unsigned long */
extern int checkpressed(int code);
extern int checkdown(int code);
extern int checkbutton(int button);
extern int checkbuttondown(int button);
extern int anydown();
extern int firstdown();
extern void scaninput();
extern void fontinit();
extern void writechar(int x,int y,uchar ch);
extern void clear();
extern void drawbox(int x,int y,int size,int color);
extern void drawbox2(int x,int y,int sizex,int sizey,int color);
extern void drawfillrect(int x,int y,int size,int color);
extern void bigpixel(int x,int y,int color);
extern void invert(int x,int y);
extern int getmousex();
extern int getmousey();
extern void drawsquare(int x,int y,uchar *source);
extern void colormapon();
extern void colormapoff();
extern void palette(uchar *pal);
extern void drawfigure(int x,int y,figure *fig);
extern void drawfigureany(int x,int y,figure *fig,solid *dest);
void solidcopy(solid *src,int destx,int desty,int sizex,int sizey);
void solidcopyany(solid *src,solid *dest,int destx,int desty,int sizex,int sizey);

#endif // GFXX_H
