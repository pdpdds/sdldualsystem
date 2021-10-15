#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "bomber.h"
#include "gfx.h"

#define MAXCOLORS 256

int usedcolors=0;
SDL_Surface *thescreen;
SDL_Color themap[256];
uchar *videomem;
int stride;

int mousex,mousey,mouseb;
uchar mustlock=0,locked=0;
uchar *block64;


int buttonstate=0,buttondown=0;
int mxpos,mypos;

int pressedcodes[KEYMAX],downcodes[KEYMAX],numpressed,numdown;


void dumpgfx()
{
	usedcolors = 0;
}
int bestmatch(int red,int green,int blue)
{
int i;
int bestcolor,bestdelta=0;
int rdelta,gdelta,bdelta;
int delta;


	i=0;
	bestcolor=-1;
	while(i<usedcolors)
	{
		rdelta=themap[i].r;
		rdelta-=red;
		rdelta*=rdelta;
		gdelta=themap[i].g;
		gdelta-=green;
		gdelta*=gdelta;
		bdelta=themap[i].b;
		bdelta-=blue;
		bdelta*=bdelta;
		delta=rdelta+gdelta+bdelta;
		if(bestcolor<0 || delta<bestdelta)
		{
			bestdelta=delta;
			bestcolor=i;
		}
		i++;
	}
	return bestcolor;
}

void updatemap(void)
{
	SDL_SetColors(thescreen, themap, 0, 256);
}


void createinout(gfxset *gs)
{
uchar *p;
int i,j,counts[256];
uchar red,green,blue;
int cnt;


	p=gs->gs_pic;
	for(i=0;i<256;i++) counts[i]=0;
	i=gs->gs_xsize*gs->gs_ysize;
	while(i--)
		counts[*p++]++;
	cnt=0;
	gs->gs_inout[0]=0;
	for(i=1;i<256;i++)
	{
		if(counts[i])
		{
			cnt++;
			p=gs->gs_colormap+i+i+i;
			red=*p++;
			green=*p++;
			blue=*p++;
			for(j=0;j<usedcolors;j++)
			{
				if(red==themap[j].r && 
					green==themap[j].g && blue==themap[j].b)
				{
					gs->gs_inout[i]=j;
					break;
				}
			}
			if(j==usedcolors)
			{
				if(usedcolors<MAXCOLORS)
				{
					themap[j].r=red;
					themap[j].g=green;
					themap[j].b=blue;
					gs->gs_inout[i]=usedcolors;
					++usedcolors;
				} else
					gs->gs_inout[i]=bestmatch(red,green,blue);
			}
		}
	}
	updatemap();
}

uchar *compressfig(uchar *put,gfxset *gs,
				int sourcex,int sourcey,int sizex,int sizey)
{
int j,gswidth;
uchar *p, *p2,pixel,*map1;
int dx,dy;

	gswidth=gs->gs_xsize;
	map1=gs->gs_inout;
	p=gs->gs_pic+sourcex+gswidth*sourcey;
	for(dy=0;dy<sizey;dy++)
	{
		p2=p;
		j=0;
		for(dx=0;dx<=sizex;dx++)
		{
			if(dx<sizex) pixel=*p2;
			else pixel=0;
			++p2;
			if(pixel)
				++j;
			else if(j)
			{
				*put++=j;
				*put++=dx-j;
				*put++=dy;
				p2-=j+1;
				while(j)
					*put++=map1[*p2++],--j;
				++p2;
			}
		}
		p+=gswidth;
	}
	*put++=0;
	return put;
}
void gfxfetchsingle(figure *fig,gfxset *gs,int sourcex,int sourcey,int sizex,int sizey)
{
uchar *p,*p2;
int dx,dy;
uchar *map1;
int gswidth;
int minx,miny,maxx,maxy;
int tx,ty;

	map1=gs->gs_inout;
	gswidth=gs->gs_xsize;
	p=gs->gs_pic+sourcex+gswidth*sourcey;
	minx=miny=maxx=maxy=-1;
	for(dy=0;dy<sizey;dy++)
	{
		p2=p;
		ty=sourcey+dy;
		for(dx=0;dx<sizex;dx++)
		{
			if(!*p2++) continue;
			if(miny==-1 || ty<miny) miny=ty;
			if(maxy==-1 || ty>maxy) maxy=ty;
			tx=sourcex+dx;
			if(minx==-1 || tx<minx) minx=tx;
			if(maxx==-1 || tx>maxx) maxx=tx;
		}
		p+=gswidth;
	}

	if(minx==-1)
	{
		minx=maxx=sourcex;
		miny=maxy=sourcey;
	}

	fig->xdelta=minx-sourcex;
	fig->ydelta=miny-sourcey;

	sourcex=minx;
	sourcey=miny;
	fig->xsize=sizex=maxx-minx+1;
	fig->ysize=sizey=maxy-miny+1;

	p=compressfig(block64,gs,sourcex,sourcey,sizex,sizey);
	fig->graphics=malloc(p-block64);
	if(fig->graphics)
		memcpy(fig->graphics,block64,p-block64);
	

}
//(gfxset *gs,figure *fig,int sourcex,int sourcey,int sizex,int sizey)
void gfxfetch(gfxset *gs,figure *fig,int num)
{
int x,y;
int xsize,ysize;
int fxsize,fysize;
unsigned char *p,*p2;

	xsize=gs->gs_xsize;
	ysize=gs->gs_ysize;
	p2=p=gs->gs_pic+xsize+1;
	fxsize=2;
	while(*p++==0) ++fxsize;
	fysize=2;
	while(*p2==0) ++fysize,p2+=xsize;
	x=fxsize;
	y=0;
	while(num--)
	{
		gfxfetchsingle(fig,gs,x,y,fxsize,fysize);
		x+=fxsize;
		if(x>xsize-fxsize)
		{
			x=0;
			y+=fysize;
			if(y>ysize-fysize)
				y=0;
		}
		fig++;
	}
}

void solidfetch(gfxset *gs,solid *dest)
{
int xsize,ysize;
int i;
unsigned char *p,*map;
uchar *gfx;

	memset(dest,0,sizeof(solid));
	xsize=gs->gs_xsize;
	ysize=gs->gs_ysize;

	i=xsize*ysize;
	gfx=dest->graphics=malloc(i);
	if(!gfx) return;
	dest->xsize=xsize;
	dest->ysize=ysize;
	map=gs->gs_inout;
	memcpy(gfx,gs->gs_pic,i);
	p=gfx;
	while(i--)
	{
		if(*p) *p=map[*p];
		++p;
	}
}
void solidcopyany(solid *src,solid *dest,int destx,int desty,int sizex,int sizey)
{
int xmax,ymax;
int j;
uchar *p1,*p2;
int w;

	xmax=src->xsize;
	ymax=src->ysize;
	if(destx>=xmax || desty>=ymax || destx+sizex<=0 || desty+sizey<=0)
		return;
	if(destx<0)
	{
		sizex+=destx;
		destx=0;
	}
	if(desty<0)
	{
		sizey+=desty;
		desty=0;
	}
	if(destx+sizex>xmax)
		sizex=xmax-destx;
	if(desty+sizey>ymax)
		sizey=ymax-desty;

	if(dest)
	{
		w=dest->xsize;
		p1=dest->graphics+desty*w+destx;
	} else
	{
		gfxlock();
		w=stride;
		p1=videomem+desty*stride+destx;
	}
	p2=src->graphics+desty*xmax+destx;
	for(j=0;j<sizey;++j)
	{
		memcpy(p1,p2,sizex);
		p1+=w;
		p2+=xmax;
	}

}
void solidcopy(solid *solid,int destx,int desty,int sizex,int sizey)
{
	solidcopyany(solid,0,destx,desty,sizex,sizey);
}


void drawfigureany(int x,int y,figure *fig,solid *dest)
{
int run;
int dx,dy;
int xsize,ysize;
int clipx,clipy,w;
unsigned char *pc,*p,*p2,*take;

	take=fig->graphics;
	if(dest)
	{
		w=clipx=dest->xsize;
		clipy=dest->ysize;
		pc=dest->graphics;
	} else
	{
		gfxlock();
		w=stride;
		clipx=IXSIZE;
		clipy=IYSIZE;
		pc=videomem;
	}

	dx=fig->xdelta;
	dy=fig->ydelta;
	xsize=fig->xsize;
	ysize=fig->ysize;
	x+=dx;
	y+=dy;
	if(x>=0 && y>=0 && x<=clipx-xsize && y<=clipy-ysize)
	{
		while((run=*take++))
		{
			dx=*((signed char *)take); ++take;
			dy=*((signed char *)take); ++take;
			p=pc+w*(y+dy)+x+dx;
			while(run--)
				*p++=*take++;
		}
	} else
	{
		while((run=*take++))
		{
			dx=*((signed char *)take); ++take;
			dy=*((signed char *)take); ++take;
			dx+=x;
			dy+=y;
			p2=take;
			take+=run;
			if(dy<0 || dy>=clipy) continue;
			if(dx>=clipx) continue;
			if(dx<0)
			{
				p2-=dx;
				run+=dx;
				dx=0;
			} else if(dx+run>clipx)
				run=clipx-dx;
			p=pc+w*dy+dx;
			if(run)
				memcpy(p,p2,run);
		}
	}
}
void drawfigure(int destx,int desty,figure *fig)
{
	drawfigureany(destx,desty,fig,0);
}

void copyup()
{
	gfxunlock();
	SDL_UpdateRect(thescreen, 0, 0, 0, 0);
	needwhole=0;
}

void copyupxy(int x,int y)
{
	gfxunlock();
	SDL_UpdateRect(thescreen,x,y,24,24);
}

void copyupxysize(int x,int y,int xsize,int ysize)
{
	gfxunlock();
	SDL_UpdateRect(thescreen,x,y,xsize,ysize);
}


void set_color(int color, int red, int green, int blue)
{
	themap[color].r=red;
	themap[color].g=green;
	themap[color].b=blue;

}
void opengfx(int argc, char **argv)
{
unsigned long videoflags;

	themap[0].r=0;
	themap[0].g=0;
	themap[0].b=0;
	usedcolors=1;

	block64=malloc(65536);
	if(!block64)
	{
		printf("Couldn't allocate block64\n");
		exit(50);
	}

	if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_AUDIO) < 0 )
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}
	videoflags = SDL_SWSURFACE|SDL_HWPALETTE; //|SDL_FULLSCREEN;
	
	thescreen = SDL_SetVideoMode(IXSIZE, IYSIZE, 8, videoflags);
	if ( thescreen == NULL )
	{
		fprintf(stderr, "Couldn't set display mode: %s\n",
							SDL_GetError());
		SDL_Quit();
		exit(5);
	}
	stride=thescreen->pitch;
	videomem=thescreen->pixels;
	mustlock=SDL_MUSTLOCK(thescreen);
	locked=0;
	SDL_ShowCursor(0);
}


void closegfx(void)
{
	SDL_Quit();
}

int checkpressed(int code)
{
int *p,i;
	i=numpressed;
	p=pressedcodes;
	while(i--)
		if(*p++==code) return 1;
	return 0;
}

int checkdown(int code)
{
int *p,i;
	i=numdown;
	p=downcodes;
	while(i--)
		if(*p++==code) return 1;
	return 0;
}

int checkbutton(int button)
{
	return buttonstate & (1<<button);
}

int checkbuttondown(int button)
{
	return buttondown & (1<<button);
}

int anydown()
{
	return numdown;
}

int takedown(void)
{
int res=0;

	if(numdown)
	{
		res=*downcodes;
		--numdown;
		if(numdown)
			memmove(downcodes,downcodes+1,numdown*sizeof(int));
	}
	return res;
}
int firstdown(vopid)
{
	return *downcodes;
}

#define ENDMARK 0xaabacada

int sdlinoutnormal[]={
SDLK_0,'0',
SDLK_1,'1',
SDLK_2,'2',
SDLK_3,'3',
SDLK_4,'4',
SDLK_5,'5',
SDLK_6,'6',
SDLK_7,'7',
SDLK_8,'8',
SDLK_9,'9',
SDLK_a,'a',
SDLK_b,'b',
SDLK_c,'c',
SDLK_d,'d',
SDLK_e,'e',
SDLK_f,'f',
SDLK_g,'g',
SDLK_h,'h',
SDLK_i,'i',
SDLK_j,'j',
SDLK_k,'k',
SDLK_l,'l',
SDLK_m,'m',
SDLK_n,'n',
SDLK_o,'o',
SDLK_p,'p',
SDLK_q,'q',
SDLK_r,'r',
SDLK_s,'s',
SDLK_t,'t',
SDLK_u,'u',
SDLK_v,'v',
SDLK_w,'w',
SDLK_x,'x',
SDLK_y,'y',
SDLK_z,'z',
SDLK_MINUS,'-',
SDLK_EQUALS,'=',
SDLK_LEFTBRACKET,'[',
SDLK_RIGHTBRACKET,']',
SDLK_SEMICOLON,';',
SDLK_QUOTE,'\'',
SDLK_SLASH,'/',
SDLK_PERIOD,'.',
SDLK_COMMA,',',
SDLK_BACKQUOTE,'`',
SDLK_BACKSPACE,8,
SDLK_TAB,9,
SDLK_DELETE,MYDELETE,
SDLK_RETURN,13,
SDLK_F1,MYF1,
SDLK_F2,MYF2,
SDLK_F3,MYF3,
SDLK_F4,MYF4,
SDLK_F5,MYF5,
SDLK_F6,MYF6,
SDLK_F7,MYF7,
SDLK_F8,MYF8,
SDLK_F9,MYF9,
SDLK_F10,MYF10,
SDLK_ESCAPE,0x1b,
SDLK_LEFT,MYLEFT,
SDLK_RIGHT,MYRIGHT,
SDLK_UP,MYUP,
SDLK_DOWN,MYDOWN,
SDLK_PAGEUP,MYPAGEUP,
SDLK_PAGEDOWN,MYPAGEDOWN,
SDLK_SPACE,' ',
SDLK_HOME,MYHOME,
SDLK_END,MYEND,
SDLK_LALT,MYALTL,
SDLK_RALT,MYALTR,
ENDMARK
};
int sdlinoutshifted[]={
SDLK_0,')',
SDLK_1,'!',
SDLK_2,'@',
SDLK_3,'#',
SDLK_4,'$',
SDLK_5,'%',
SDLK_6,'^',
SDLK_7,'&',
SDLK_8,'*',
SDLK_9,'(',
SDLK_a,'A',
SDLK_b,'B',
SDLK_c,'C',
SDLK_d,'D',
SDLK_e,'E',
SDLK_f,'F',
SDLK_g,'G',
SDLK_h,'H',
SDLK_i,'I',
SDLK_j,'J',
SDLK_k,'K',
SDLK_l,'L',
SDLK_m,'M',
SDLK_n,'N',
SDLK_o,'O',
SDLK_p,'P',
SDLK_q,'Q',
SDLK_r,'R',
SDLK_s,'S',
SDLK_t,'T',
SDLK_u,'U',
SDLK_v,'V',
SDLK_w,'W',
SDLK_x,'X',
SDLK_y,'Y',
SDLK_z,'Z',
SDLK_MINUS,'_',
SDLK_EQUALS,'+',
SDLK_LEFTBRACKET,'{',
SDLK_RIGHTBRACKET,'}',
SDLK_SEMICOLON,':',
SDLK_QUOTE,'"',
SDLK_SLASH,'?',
SDLK_PERIOD,'>',
SDLK_COMMA,'<',
SDLK_BACKQUOTE,'~',
SDLK_BACKSPACE,8,
SDLK_TAB,9,
SDLK_DELETE,MYDELETE,
SDLK_RETURN,13,
SDLK_F1,MYF1+MYSHIFTED,
SDLK_F2,MYF2+MYSHIFTED,
SDLK_F3,MYF3+MYSHIFTED,
SDLK_F4,MYF4+MYSHIFTED,
SDLK_F5,MYF5+MYSHIFTED,
SDLK_F6,MYF6+MYSHIFTED,
SDLK_F7,MYF7+MYSHIFTED,
SDLK_F8,MYF8+MYSHIFTED,
SDLK_F9,MYF9+MYSHIFTED,
SDLK_F10,MYF10+MYSHIFTED,
SDLK_ESCAPE,0x1b,
SDLK_LEFT,MYLEFT+MYSHIFTED,
SDLK_RIGHT,MYRIGHT+MYSHIFTED,
SDLK_UP,MYUP+MYSHIFTED,
SDLK_DOWN,MYDOWN+MYSHIFTED,
SDLK_PAGEUP,MYPAGEUP,
SDLK_PAGEDOWN,MYPAGEDOWN,
SDLK_SPACE,' ',
SDLK_HOME,MYHOME,
SDLK_END,MYEND,
SDLK_LALT,MYALTL,
SDLK_RALT,MYALTR,
ENDMARK
};
int looklist(int code,int *list)
{
	while(*list!=ENDMARK)
	{
		if(*list==code)
			return list[1];
		list+=2;
	}
	return -1;
}
int mapkey(int code,int qual)
{

	if(qual & KMOD_SHIFT)
		code=looklist(code,sdlinoutshifted);
	else
		code=looklist(code,sdlinoutnormal);
	if(code<0) return -1;

	if(qual & KMOD_ALT)
		code|=MYALTED;

	return code;
}
void markkey(int code,int status)
{
int i;
int *ip;

	if(status)
	{
		if(numdown<KEYMAX)
			downcodes[numdown++]=code;
		ip=pressedcodes;
		i=numpressed;
		while(i)
			if(*ip++==code) break;
			else i--;
		if(!i && numpressed<KEYMAX)
			pressedcodes[numpressed++]=code;
	} else
	{
		i=numpressed;
		ip=pressedcodes;
		while(i)
			if(*ip++==code)
			{
				*--ip=pressedcodes[--numpressed];
				break;
			} else i--;
	}

}

void pollinput(void)
{
	SDL_PollEvent(0);
}


void scaninput(void)
{
SDL_Event event;
int key,mod;
static int bs=0;

	numdown=0;
	buttondown=0;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_KEYDOWN:
			key=event.key.keysym.sym;
			mod=event.key.keysym.mod;
			markkey(mapkey(key,mod),1);
			break;
		case SDL_KEYUP:
			key=event.key.keysym.sym;
			mod=event.key.keysym.mod;
			markkey(mapkey(key,mod),0);
			break;
		case SDL_MOUSEBUTTONUP:
			bs&=~(1<<(event.button.button-1));
			mousex=event.button.x>>1;
			mousey=event.button.y>>1;
			mouseb=bs;
			break;
		case SDL_MOUSEBUTTONDOWN:
			bs|=1<<(event.button.button-1);
			mousex=event.button.x>>1;
			mousey=event.button.y>>1;
			mouseb=bs;
			break;
		case SDL_MOUSEMOTION:
			mousex=event.motion.x>>1;
			mousey=event.motion.y>>1;
			break;
		case SDL_QUIT:
			exitflag = 1;
			break;
		}
	}
}

/*
void scaninput()
{

int i,*ip,code;
	numdown=0;
	buttondown=0;
	while(XCheckMaskEvent(dp,~0,&event))
		switch(event.type)
		{
		case KeyPress:
			code=XLookupKeysym(keyevent,0);
			if(numdown<KEYMAX)
				downcodes[numdown++]=code;
			ip=pressedcodes;
			i=numpressed;
			while(i)
				if(*ip++==code) break;
				else i--;
			if(!i && numpressed<KEYMAX)
				pressedcodes[numpressed++]=code;
			break;
		case KeyRelease:
			code=XLookupKeysym(keyevent,0);
			i=numpressed;
			ip=pressedcodes;
			while(i)
				if(*ip++==code)
				{
					*--ip=pressedcodes[--numpressed];
					break;
				} else i--;
			break;
		case ButtonPress:
			i=1<<buttonevent->button;
			buttonstate|=i;
			buttondown|=i;
			break;
		case ButtonRelease:
			buttonstate&=~(1<<buttonevent->button);
			break;
		case MotionNotify:
			mxpos=motionevent->x;
			mypos=motionevent->y;
			break;
		case Expose:
			copyup();
			needwhole=0;
			break;
		case FocusOut:
			numpressed=0;
			break;
		}
}
*/

void drawrect(int x,int y,int xs,int ys,int c)
{
uchar *p;

	gfxlock();
	p=videomem+y*stride+x;
	while(ys--)
	{
		memset(p,c,xs);
		p+=stride;
	}
}

void greyrect(int x,int y,int xsize,int ysize)
{
static int greycolor=-1;
	if(greycolor==-1)
		greycolor=bestmatch(0,0,0x70);
	drawrect(x,y,xsize,ysize,greycolor);
}

void clear()
{
int j;
uchar *p;

	gfxlock();
	p=videomem;
	for(j=0;j<IYSIZE;++j)
	{
		memset(p,0,IXSIZE);
		p+=stride;
	}
}
void clearrect(int x,int y,int xsize,int ysize)
{
	drawrect(x,y,xsize,ysize,0);
}

void gfxlock(void)
{
	if(locked || !mustlock) return;
	if ( SDL_LockSurface(thescreen) < 0 )
	{
		fprintf(stderr, "Couldn't lock display surface: %s\n",
							SDL_GetError());
	}
	locked=1;
}
void gfxunlock(void)
{
	if(!locked || !mustlock) return;
	SDL_UnlockSurface(thescreen);
	locked=0;
}
