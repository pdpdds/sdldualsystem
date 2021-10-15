/* FreeBSD syscons video  */

#ifdef FBSD_CONSOLE

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/fbio.h>
#include <sys/kbio.h>
#include <sys/consio.h>
#include <sys/termios.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include "h2def.h"
#include "soundst.h"

#define SavePalette()  SavRstPalette(FBIO_GETPALETTE)
#define RestorePalette()  SavRstPalette(FBIO_SETPALETTE)

extern int usemouse;
int mouse_scale_factor;

static event_t	event;

/* VGA state flags */

#define FB_MAPPED   1
#define CONS_ACTIVE 2
#define TTY_STARTED 4
#define KBD_RAWMODE 8
#define GRAPH_MODE  16
#define PALETTE_SAVED 32

struct vgainfo {
 int fd;    		/* console file descriptor */
 int oldmode;		/* old video mode */
 int oldkmode;		/* old keyboard mode */
 int consnum;		/* current active console */
 int ourcons;		/* our console */
 vtmode_t curmod;	/* current console mode */
 struct termios oti;	/* old termios settings */
 struct termios ti;	/* our termios settings */
 void *lfb;		/* linear frame buffer */
 int bufsize;		/* frame buffer size */
 int flags;
 unsigned char red[256];
 unsigned char green[256];
 unsigned char blue[256];
};

void *VideoBuffer;

struct vgainfo VGAInfo;

/* Signals array */

int signals[]= {
  SIGHUP,SIGINT,SIGQUIT,SIGILL,SIGTRAP,SIGEMT,SIGFPE,SIGKILL,
  SIGBUS,SIGSEGV,SIGSYS,SIGPIPE,SIGALRM,SIGTERM,SIGURG,SIGSTOP,SIGTSTP,
  SIGCONT,SIGCHLD,SIGTTIN,SIGTTOU,SIGIO,SIGXCPU,SIGXFSZ,SIGVTALRM,SIGPROF,
  SIGWINCH,SIGINFO,SIGUSR1,SIGUSR2 
};

unsigned char ReadKey(void) {
 unsigned char c;
 if (read(VGAInfo.fd,&c,1)<=0) return 0;
 return c;
}

unsigned char keys0[]="1234567890-=\x08";
unsigned char keys1[]="\x09qwertyuiop[]";
unsigned char keys2[]="asdfghjkl;'";
unsigned char keys3[]="zxcvbnm,./";

unsigned int GetKey(void) {
 unsigned int c;
 unsigned int r;
 c=(unsigned int)ReadKey();
 if (!c) return 0;
 r=(c&0x80)?0x80000000:0;
 c&=~0x80;
 if (c>=2 && c<=11)
   c=keys0[c-2];
 else if (c>=15 && c<=27) 
   c=keys1[c-15];
 else if (c>=30 && c<=40) 
   c=keys2[c-30];
 else if (c>=44 && c<=53) 
   c=keys3[c-44];
 else 
  switch (c) {
   case 57: c=' '; break;
   case 77: c=KEY_RIGHTARROW; break;
   case 75: c=KEY_LEFTARROW; break;
   case 72: c=KEY_UPARROW; break;
   case 80: c=KEY_DOWNARROW; break;
   case 1: c=KEY_ESCAPE; break;
   case 28: c=KEY_ENTER; break;
   case 9: c=KEY_TAB; break;
   case 58: c=KEY_CAPSLOCK; break;
   case 59: c=KEY_F1; break;
   case 60: c=KEY_F2; break;
   case 61: c=KEY_F3; break;
   case 62: c=KEY_F4; break;
   case 63: c=KEY_F5; break;
   case 64: c=KEY_F6; break;
   case 65: c=KEY_F7; break;
   case 66: c=KEY_F8; break;
   case 67: c=KEY_F9; break;
   case 68: c=KEY_F10; break;
   case 87: c=KEY_F11; break;
   case 88: c=KEY_F12; break;
   case 14: c=KEY_BACKSPACE; break;
   case 69: c=KEY_PAUSE; break;
   case 13: c=KEY_EQUALS; break;
   case 12: c=KEY_MINUS; break;
   case 42: c=KEY_RSHIFT; break;
   case 29: c=KEY_RCTRL; break;
   case 56: c=KEY_RALT; break;
   case 0x35: c=KEY_FIVE; break;
   case 0x36: c=KEY_SIX; break;
   case 0x37: c=KEY_SEVEN; break;
//   case 0x38: c=KEY_EIGHT; break;
//   case 0x39: c=KEY_NINE; break;
   case 0x30: c=KEY_ZERO; break;
   case 0x5C: c=KEY_BACKSLASH; break;
   case 71: c=KEY_HOME; break;
   case 79: c=KEY_END; break;
   case 73: c=KEY_PGUP; break;
   case 81: c=KEY_PGDN; break;
  }
 return (c|r);   
}

static int mousex, mousey, mousebutt;

struct mouse_info minfo;

void FBSD_CheckMouse(void) {
 
  minfo.operation=MOUSE_GETINFO;

  if (ioctl(VGAInfo.fd,CONS_MOUSECTL,&minfo)<0) 
    return;

  if (mousebutt!=minfo.u.data.buttons || 
      mousex!=minfo.u.data.x ||
      mousey!=minfo.u.data.y) {
    event.type=ev_mouse;
    event.data2=((minfo.u.data.x-mousex)*mouse_scale_factor);
    event.data3=((mousey-minfo.u.data.y)*mouse_scale_factor);
    event.data1=mousebutt=minfo.u.data.buttons;
    minfo.u.data.x=mousex;
    minfo.u.data.y=mousey;
    minfo.operation=MOUSE_MOVEABS;
    if (ioctl(VGAInfo.fd,CONS_MOUSECTL,&minfo)<0) 
      return;
    H2_PostEvent(&event);
  }
}

int SavRstPalette(int ioctlnum) {
 video_color_palette_t pal;
 pal.index=0;
 pal.count=256;
 pal.red=VGAInfo.red;
 pal.green=VGAInfo.green;
 pal.blue=VGAInfo.blue;
 pal.transparent=NULL;
 return ioctl(VGAInfo.fd,ioctlnum,&pal);
}

void sighandler(int sig);

void catchallsigs(void) {
 int i;
 for (i=0;i<sizeof(signals)/sizeof(int);i++) 
  signal(signals[i],sighandler);
}

void freeallsigs(void) {
 int i;
 for (i=0;i<sizeof(signals)/sizeof(int);i++) 
  signal(signals[i],SIG_DFL);
}

int FBSD_ShutdownGraphics(void);

void sighandler(int sig) {
 if (sig==SIGWINCH) return;
 printf("Fatal signal caught %d!! Dying!\n",sig);
 freeallsigs();
 FBSD_ShutdownGraphics();
 abort();
}

int FBSD_ShutdownGraphics(void) {
  
  /* If StartupGraphics() fails, it leaves a big piece of shit */
  /* Trying to take it away */
  
   if (VGAInfo.flags & PALETTE_SAVED) 
   RestorePalette();

  if (VGAInfo.flags & GRAPH_MODE) 
   ioctl(VGAInfo.fd,FBIO_SETMODE,&VGAInfo.oldmode);

  if (VGAInfo.flags & KBD_RAWMODE) {
    ioctl(VGAInfo.fd,KDSKBMODE,VGAInfo.oldkmode);
  }

  if (VGAInfo.flags & TTY_STARTED) 
    tcsetattr(VGAInfo.fd,TCSANOW,&VGAInfo.oti);

  if (VGAInfo.flags & CONS_ACTIVE) 
    ioctl(VGAInfo.fd,VT_ACTIVATE,VGAInfo.consnum);

  if (VGAInfo.flags & FB_MAPPED) 
    munmap(VGAInfo.lfb,0x10000);

  close(VGAInfo.fd);
  
  freeallsigs();
  
  VGAInfo.flags=0;

  return 0; 
}

int FBSD_StartupGraphics(void) {
 
 int gmode;

 VGAInfo.fd=0;  /* STDIN */

 VGAInfo.flags=0;

 catchallsigs();
 
 #define CHECKIT(xxx) if ((xxx)<0) goto fatal_error;

 /* set noncanonical input mode */
 
 if (tcgetattr(VGAInfo.fd,&VGAInfo.oti)) goto fatal_error;
 VGAInfo.ti.c_lflag&=~(ICANON | ECHOKE | ECHOE | ECHONL | ECHOPRT | ECHOCTL | ECHO | ISIG);
 VGAInfo.ti.c_cc[VMIN]=VGAInfo.ti.c_cc[VTIME]=0;
 if (tcsetattr(VGAInfo.fd,TCSANOW,&VGAInfo.ti)) goto fatal_error;

 VGAInfo.flags |= TTY_STARTED;

 /* set raw keyboard mode */
 
 CHECKIT(ioctl(VGAInfo.fd,KDGKBMODE,&VGAInfo.oldkmode));
 CHECKIT(ioctl(VGAInfo.fd,KDSKBMODE,K_RAW));
 CHECKIT(fcntl(VGAInfo.fd,F_SETFL,O_NONBLOCK));
 VGAInfo.flags |= KBD_RAWMODE;

 gmode=M_VGA13;
 CHECKIT(ioctl(VGAInfo.fd,FBIO_GETMODE,&VGAInfo.oldmode));
 CHECKIT(ioctl(VGAInfo.fd,FBIO_SETMODE,&gmode));
 VGAInfo.flags |= GRAPH_MODE;

 VGAInfo.bufsize=0x10000;
 VGAInfo.lfb=mmap(0,0x10000,PROT_READ|PROT_WRITE,MAP_SHARED,VGAInfo.fd,0);
 if (!VGAInfo.lfb) goto fatal_error;
 VGAInfo.flags |= FB_MAPPED;

 CHECKIT(SavePalette());
 VGAInfo.flags |= PALETTE_SAVED;

 VideoBuffer=VGAInfo.lfb;
 
 /* All ok */
 return 0;
 
fatal_error: 

 /* OOPS! Console can't set up graphics! */

  FBSD_ShutdownGraphics();
  return -1;
}


void FBSD_GetEvent(void)
{
 unsigned int k;

 /* Check the keyboard */

 k=GetKey();
 
 if (k) {
  if (k&0x80000000) {
   event.type=ev_keyup;
   event.data1=k&~0x80000000;
  }
  else {
   event.type=ev_keydown;
   event.data1=k;
  }
  H2_PostEvent(&event);
 }
 if (usemouse)
  FBSD_CheckMouse();
 
}


int FBSD_InitGraphics(void)
{

   if (FBSD_StartupGraphics()<0) {
     fprintf(stderr,"syscons: Unable to initialize console: flags=%04X\n",VGAInfo.flags);
     return 0;
   };

  minfo.operation=MOUSE_GETINFO;

  if (ioctl(VGAInfo.fd,CONS_MOUSECTL,&minfo)>=0) {
    mousex=minfo.u.data.x;
    mousey=minfo.u.data.y;
    mousebutt=minfo.u.data.buttons;
    printf("syscons: Init mouse x=%d, y=%d, butt=%d\n",mousex,mousey,mousebutt);
  } 
  else {
   usemouse=0;
   printf("syscons: Mouse init failed: %s\n",strerror(errno));
  }
 return 1;
}


void FBSD_FinishUpdate(void)
{
     memcpy(VGAInfo.lfb,screen,320*200);
}

void FBSD_SetPalette(byte *palette)
{
    int	i,j;
    byte red[256],green[256],blue[256];
    video_color_palette_t pal;

    // set the VGA colormap entries
    for (i=0,j=0 ; i<768 ; i+=3,j++)
    {
      red[j] = gammatable[usegamma][palette[i]];
      green[j] = gammatable[usegamma][palette[i+1]];
      blue[j] = gammatable[usegamma][palette[i+2]];
    }
    
    pal.index=0;
    pal.count=256;
    pal.red=red;
    pal.green=green;
    pal.blue=blue;
    pal.transparent=NULL;
    if (ioctl(VGAInfo.fd,FBIO_SETPALETTE,&pal)<0) {
     I_Error("syscons: cannot set palette!");
    }
 
}

#endif

void FBSD_Stub(void) {

}
