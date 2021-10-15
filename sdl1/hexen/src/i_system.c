#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdarg.h>
//#include <sys/time.h>
//#include <unistd.h>
#include <SDL.h>

#include "h2def.h"

int	mb_used = 8;

void
I_Tactile(int on, int off,  int total)
{
//  on = off = total = 0;
}

ticcmd_t	emptycmd;
ticcmd_t*	I_BaseTiccmd(void)
{
    return &emptycmd;
}

int  I_GetHeapSize (void)
{
    return mb_used*1024*1024;
}

byte* I_ZoneBase (int*	size)
{
    *size = mb_used*1024*1024;
    return (byte *) malloc (*size);
}

int  I_GetTime (void)
{
  return (TICRATE*SDL_GetTicks())/1000;
}

void I_Init (void)
{
    S_Init();
}

void I_Quit (void)
{
    D_QuitNetGame ();
    I_ShutdownSound();
    M_SaveDefaults ();
    I_ShutdownGraphics();
    exit(0);
}

void I_WaitVBL(int count)
{
  SDL_Delay(count * (10000/70));
}

void I_BeginRead(void)
{
}

void I_EndRead(void)
{
}

byte*	I_AllocLow(int length)
{
    byte* mem;
    mem=(byte *)malloc(length);
    memset(mem,0,length);
    return mem;
}

extern boolean demorecording;

void I_Error (char *error, ...)
{
    va_list	argptr;

    // Message first.
    va_start (argptr,error);
    fprintf (stderr, "Error: ");
    vfprintf (stderr,error,argptr);
    fprintf (stderr, "\n");
    va_end (argptr);

    fflush( stderr );

    // Shutdown. Here might be other errors.
    if (demorecording)
	G_CheckDemoStatus();

    D_QuitNetGame ();
    I_ShutdownGraphics();
    
    exit(-1);
}
