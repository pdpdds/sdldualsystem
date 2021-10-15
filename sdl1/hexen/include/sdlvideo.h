#ifndef __SDLVIDEO_H
#define __SDLVIDEO_H

#include "h2def.h"

#define WIDTH	SCREENWIDTH
#define HEIGHT	SCREENHEIGHT

extern int SDLWidth;
extern int SDLHeight;
extern int SDLBPP;
extern int SDLVFlags;
extern SDL_Surface *Surface;
extern SDL_Overlay *Overlay;
extern SDL_Rect OvRect;

extern Uint32 Rmask, Gmask, Bmask, Amask;
extern Uint8 Rshift, Gshift, Bshift, Ashift;
extern Uint8 Rloss, Gloss, Bloss, Aloss;
extern Uint32 colorkey;
extern Uint8 alpha;

void SetSDLColorsDirect(byte *);
void SetSDLColorsFor8bit(byte *);
void SetSDLColorsYUV(byte *);

void PaintOnSDLSurface1(void);
void PaintOnSDLSurface2(void);
void PaintOnSDLSurface3(void);
void PaintOnSDLSurface4(void);
void PaintOnSDLSurfaceYV12(void);
void PaintOnSDLSurfaceIYUV(void);

#endif
