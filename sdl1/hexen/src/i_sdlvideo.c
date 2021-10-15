#ifdef USE_SDL

#include <stdio.h>
#include <SDL.h>
#include <SDL_video.h>
#include <SDL_events.h>
#include "h2def.h"
#include "soundst.h"
#include "sdlvideo.h"

int SDLWidth;
int SDLHeight;
int SDLMaxWidth;
int SDLMaxHeight;
int SDLBPP;
int SDLVFlags = SDL_HWSURFACE | SDL_FULLSCREEN;
int WMAvailable;

Uint32 Rmask, Gmask, Bmask, Amask;
Uint8 Rshift, Gshift, Bshift, Ashift;
Uint8 Rloss, Gloss, Bloss, Aloss;
Uint32 colorkey;
Uint8 alpha;


SDL_Surface* Surface;
int unknown_surface;

static event_t	event;
SDL_Overlay* Overlay;
SDL_Rect OvRect;

int mouse_scale_factor;
int save_buttons;

extern int usemouse;

void (*SetSDLColors)(byte*);
void (*PaintOnSDLSurface)(void);

/* RGB palette for RGB rendering with direct color coding */

Uint32 Palette[256];

/* YUV palettes for YUV rendering */

int Ypal[256];
int Upal[256];
int Vpal[256];
int YUVPaletteChanged;


void SetSDLColorsDirect(byte* palette) {
	Uint32 red, green, blue;
	int	i, j;
	for (i = 0, j = 0; i < 768; i += 3, j++)
	{
		red = gammatable[usegamma][palette[i]];
		green = gammatable[usegamma][palette[i + 1]];
		blue = gammatable[usegamma][palette[i + 2]];
		red >>= Rloss;
		green >>= Gloss;
		blue >>= Bloss;
		Palette[j] = ((red << Rshift) & Rmask) | ((green << Gshift) & Gmask) | ((blue << Bshift) & Bmask);
	}
}

void SetSDLColorsFor8bit(byte* palette) {
	SDL_Color c[256];
	Uint32 red, green, blue;
	int	i, j;
	for (i = 0, j = 0; i < 768; i += 3, j++)
	{
		red = gammatable[usegamma][palette[i]];
		green = gammatable[usegamma][palette[i + 1]];
		blue = gammatable[usegamma][palette[i + 2]];
		c[j].r = red;
		c[j].g = green;
		c[j].b = blue;
	}
	SDL_SetColors(Surface, c, 0, 256);
}

void SetSDLColorsYUV(byte* palette) {
	Uint32 red, green, blue;
	int	i, j;
	for (i = 0, j = 0; i < 768; i += 3, j++)
	{
		red = gammatable[usegamma][palette[i]];
		green = gammatable[usegamma][palette[i + 1]];
		blue = gammatable[usegamma][palette[i + 2]];
		Ypal[j] = 0.257 * red + 0.504 * green + 0.098 * blue + 16;
		Upal[j] = -0.148 * red - 0.291 * green + 0.439 * blue + 128;
		Vpal[j] = 0.439 * red - 0.368 * green - 0.071 * blue + 128;
	}
	YUVPaletteChanged = 1;
}

void PaintOnSDLSurface1(void) {
	int i, j, dx, dy;
	Uint8* s, * os;
	Uint8* d;

	SDL_LockSurface(Surface);
	s = screen;
	d = (Uint8*)Surface->pixels;
	dy = 0;
	for (i = 0; i < SDLHeight; i++) {
		dx = 0;
		os = s;
		for (j = 0; j < SDLWidth; j++) {
			*d++ = *s;
			if ((dx += WIDTH) >= SDLWidth) { dx -= SDLWidth; s++; }
		}
		s = os;
		if ((dy += HEIGHT) >= SDLHeight) { dy -= SDLHeight; s += WIDTH; }
	}
	SDL_UnlockSurface(Surface);
	SDL_UpdateRect(Surface, 0, 0, 0, 0);
}


void PaintOnSDLSurface2(void) {
	int i, j, dx, dy;
	Uint8* s, * os;
	Uint16* d;

	SDL_LockSurface(Surface);
	s = screen;
	d = (Uint16*)Surface->pixels;
	dy = 0;
	for (i = 0; i < SDLHeight; i++) {
		dx = 0;
		os = s;
		for (j = 0; j < SDLWidth; j++) {
			*d++ = (Uint16)Palette[*s];
			if ((dx += WIDTH) >= SDLWidth) { dx -= SDLWidth; s++; }
		}
		s = os;
		if ((dy += HEIGHT) >= SDLHeight) { dy -= SDLHeight; s += WIDTH; }
	}
	SDL_UnlockSurface(Surface);
	SDL_UpdateRect(Surface, 0, 0, 0, 0);
}

void PaintOnSDLSurface3(void) {
	int i, j, dx, dy;
	Uint8* s, * os;
	Uint8* d;
	union palent {
		Uint32 i;
		Uint8 b[4];
	} pe;

	SDL_LockSurface(Surface);
	s = screen;
	d = (Uint8*)Surface->pixels;
	dy = 0;
	for (i = 0; i < SDLHeight; i++) {
		dx = 0;
		os = s;
		for (j = 0; j < SDLWidth; j++) {
			pe.i = Palette[*s];
			*d++ = pe.b[0];
			*d++ = pe.b[1];
			*d++ = pe.b[2];
			if ((dx += WIDTH) >= SDLWidth) { dx -= SDLWidth; s++; }
		}
		s = os;
		if ((dy += HEIGHT) >= SDLHeight) { dy -= SDLHeight; s += WIDTH; }
	}
	SDL_UnlockSurface(Surface);
	SDL_UpdateRect(Surface, 0, 0, 0, 0);
}

void PaintOnSDLSurface4(void) {
	int i, j, dx, dy;
	Uint8* s, * os;
	Uint32* d;

	SDL_LockSurface(Surface);
	s = screen;
	d = (Uint32*)Surface->pixels;
	dy = 0;
	for (i = 0; i < SDLHeight; i++) {
		dx = 0;
		os = s;
		for (j = 0; j < SDLWidth; j++) {
			*d++ = Palette[*s];
			if ((dx += WIDTH) >= SDLWidth) { dx -= SDLWidth; s++; }
		}
		s = os;
		if ((dy += HEIGHT) >= SDLHeight) { dy -= SDLHeight; s += WIDTH; }
	}
	SDL_UnlockSurface(Surface);
	SDL_UpdateRect(Surface, 0, 0, 0, 0);
}



void AdjustYUVPalette(void) {
	/* int maxbits;
	 int i,j;
	 int minU,minV;

	 maxbits=0;
	 minU=0; minV=0;

	 for (i=0;i<256;i++)  {
	  if (Upal[i]<minU) minU=Upal[i];
	  if (Vpal[i]<minU) minU=Vpal[i];
	 }


	 for (i=0;i<256;i++)  {
	  Upal[i]-=minU;
	  Vpal[i]-=minU;
	 }

	 for(i=0;i<256;i++) {
	  for(j=31;j>0;j--)
	   if (Ypal[i]&(1<<j)) break;
	  if (j>maxbits) maxbits=j;
	 }

	 for(i=0;i<256;i++) {
	  for(j=31;j>0;j--)
	   if (Upal[i]&(1<<j)) break;
	  if (j>maxbits) maxbits=j;
	 }

	 for(i=0;i<256;i++) {
	  for(j=31;j>0;j--)
	   if (Vpal[i]&(1<<j)) break;
	  if (j>maxbits) maxbits=j;
	 }
	 printf("maxbits=%d\n",maxbits);*/
	YUVPaletteChanged = 0;
}


void PaintYUVPlanes(Uint8* Y, Uint8* U, Uint8* V) {
	int i, j, k, l;
	Uint8* s;
	s = (Uint8*)screen;
	k = 0; l = 0;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			Y[k] = Y[k + 1] = Y[k + WIDTH * 2] = Y[k + WIDTH * 2 + 1] = Ypal[*s];
			U[l] = Upal[*s];
			V[l] = Vpal[*s];
			l++; k += 2;
			s++;
		}
		k += WIDTH * 2;
	}
}

void PaintOnSDLSurfaceIYUV(void) {
	if (YUVPaletteChanged) AdjustYUVPalette();
	SDL_LockYUVOverlay(Overlay);
	PaintYUVPlanes(Overlay->pixels[0], Overlay->pixels[1], Overlay->pixels[2]);
	SDL_UnlockYUVOverlay(Overlay);
	SDL_DisplayYUVOverlay(Overlay, &OvRect);
}

void PaintOnSDLSurfaceYV12(void) {
	if (YUVPaletteChanged) AdjustYUVPalette();
	SDL_LockYUVOverlay(Overlay);
	PaintYUVPlanes(Overlay->pixels[0], Overlay->pixels[2], Overlay->pixels[1]);
	SDL_UnlockYUVOverlay(Overlay);
	SDL_DisplayYUVOverlay(Overlay, &OvRect);
}


int I_TrySDLOverlay(Uint32 format) {
	Overlay = SDL_CreateYUVOverlay(WIDTH * 2, HEIGHT * 2, format, Surface);
	if (!Overlay->hw_overlay) {
		SDL_FreeYUVOverlay(Overlay);
		Overlay = NULL;
		return 0;
	}
	return 1;
}

void I_InitSDLOverlay(void) {
	if (I_TrySDLOverlay(SDL_IYUV_OVERLAY)) {
		PaintOnSDLSurface = PaintOnSDLSurfaceIYUV;
		printf("Using IYUV accelerated blits.\n");
		return;
	}
	if (I_TrySDLOverlay(SDL_YV12_OVERLAY)) {
		PaintOnSDLSurface = PaintOnSDLSurfaceYV12;
		printf("Using YV12 accelerated blits.\n");
		return;
	}
	printf("No YUV acceleration supported.\n");
}

int VSDL_InitGraphics(void) {
	char drvname[256];
	SDL_Rect** modes;
	int i;
	int wid, hei;
	SDL_VideoInfo* vinfo;

	unknown_surface = 0;

	if (!M_CheckParm("-fullscreen"))
		SDLVFlags &= ~SDL_FULLSCREEN;

	vinfo = SDL_GetVideoInfo();
	if (!vinfo)
	{
		printf("SDL: Cannot obtain video info!\n");
		return 0;
	}
	SDLBPP = vinfo->vfmt->BitsPerPixel;
	WMAvailable = (int)vinfo->wm_available;

	switch (vinfo->vfmt->BytesPerPixel) {
	case 2:
		SetSDLColors = SetSDLColorsDirect;
		PaintOnSDLSurface = PaintOnSDLSurface2;
		break;
	case 4:
		SetSDLColors = SetSDLColorsDirect;
		PaintOnSDLSurface = PaintOnSDLSurface4;
		break;
	case 3:
		SetSDLColors = SetSDLColorsDirect;
		PaintOnSDLSurface = PaintOnSDLSurface3;
		break;
	case 1:
		SDLVFlags |= SDL_HWPALETTE;
		SetSDLColors = SetSDLColorsFor8bit;
		PaintOnSDLSurface = PaintOnSDLSurface1;
		break;
	default:
		unknown_surface = 1;
		SDLBPP = 8;
		SetSDLColors = SetSDLColorsFor8bit;
		PaintOnSDLSurface = PaintOnSDLSurface1;
		printf("SDL video: This surface is unknown to me, trying 8 bpp.\n");
	}

	Rmask = vinfo->vfmt->Rmask;
	Gmask = vinfo->vfmt->Gmask;
	Bmask = vinfo->vfmt->Bmask;
	Amask = vinfo->vfmt->Amask;

	Rshift = vinfo->vfmt->Rshift;
	Gshift = vinfo->vfmt->Gshift;
	Bshift = vinfo->vfmt->Bshift;
	Ashift = vinfo->vfmt->Ashift;

	Rloss = vinfo->vfmt->Rloss;
	Gloss = vinfo->vfmt->Gloss;
	Bloss = vinfo->vfmt->Bloss;
	Aloss = vinfo->vfmt->Aloss;

	colorkey = vinfo->vfmt->colorkey;
	alpha = vinfo->vfmt->alpha;

	if (!SDL_VideoDriverName(drvname, 256)) {
		printf("SDL: No video driver attached\n");
		return 0;
	}

	printf("SDL: Using video driver:%s\n", drvname);
	modes = SDL_ListModes(NULL, SDLVFlags);

	if (modes == (SDL_Rect**)0) {
		printf("SDL: No video modes available!\n");
		return 0;
	}

	/* Find the best resolution to render 320x200 picture.
	 Use 320x240 for realistic picture in windowed modes */

	if (modes == (SDL_Rect**)-1) {
		printf("SDL: All modes available\n");
		SDLWidth = SDLMaxWidth = 320;
		SDLHeight = SDLMaxHeight = 240;

		if (M_CheckParm("-2")) {
			SDLWidth *= 2; SDLMaxWidth *= 2;
			SDLHeight *= 2; SDLMaxHeight *= 2;
		}

		if (M_CheckParm("-3")) {
			SDLWidth *= 3; SDLMaxWidth *= 3;
			SDLHeight *= 3; SDLMaxHeight *= 3;
		}

		if (M_CheckParm("-4")) {
			SDLWidth *= 4; SDLMaxWidth *= 4;
			SDLHeight *= 4; SDLMaxHeight *= 4;
		}

	}
	else
	{
		SDLMaxWidth = SDLMaxHeight = wid = hei = 0;
		for (i = 0; modes[i]; i++) {
			if (!wid) wid = modes[i]->w;
			if (!hei) hei = modes[i]->h;
			if ((wid > modes[i]->w && modes[i]->w >= 320) ||
				(hei > modes[i]->h && modes[i]->h >= 240)) {
				wid = modes[i]->w;
				hei = modes[i]->h;
			}
			if ((SDLMaxWidth < modes[i]->w) || (SDLMaxHeight < modes[i]->h)) {
				SDLMaxHeight = modes[i]->h;
				SDLMaxWidth = modes[i]->w;
			}
		}
		SDLWidth = wid;
		SDLHeight = hei;
		if (!SDLWidth || !SDLHeight) {
			printf("SDL: Can't set video mode");
			return 0;
		}
	}
	printf("Found best mode: %dx%dx%d\n", SDLWidth, SDLHeight, SDLBPP);
	Surface = SDL_SetVideoMode(SDLWidth, SDLHeight, SDLBPP, SDLVFlags);

	if (!Surface) {
		/* We cannot set video mode with desired parameters. Try to rely on SDL's
		knowledge about it */
		unknown_surface = 1;
		SDLBPP = 8;
		SDLVFlags |= SDL_HWPALETTE;
		Surface = SDL_SetVideoMode(SDLWidth, SDLHeight, SDLBPP, SDLVFlags);
		if (Surface) goto success;
		SDLVFlags &= ~SDL_HWPALETTE;
		Surface = SDL_SetVideoMode(SDLWidth, SDLHeight, SDLBPP, SDLVFlags);
		if (Surface) goto success;
		SDLVFlags &= ~SDL_HWSURFACE;
		Surface = SDL_SetVideoMode(SDLWidth, SDLHeight, SDLBPP, SDLVFlags);
		if (Surface) goto success;
		/* Give up */
		printf("SDL: Can't set video mode!\n");
		return 0;
	}

	if ((SDLMaxWidth >= WIDTH * 2 || SDLMaxHeight >= HEIGHT * 2) && !unknown_surface) {
		I_InitSDLOverlay();
		if (Overlay) {
			SetSDLColors = SetSDLColorsYUV;
			SDLWidth = SDLMaxWidth;
			SDLHeight = SDLMaxHeight;
			OvRect.x = OvRect.y = 0;
			OvRect.w = SDLWidth;
			OvRect.h = SDLHeight;
			Surface = SDL_SetVideoMode(SDLWidth, SDLHeight, SDLBPP, SDLVFlags);
		}
	}

success:
	if (WMAvailable) {
		SDL_WM_SetCaption("U-Hexen 0.6", NULL);
		SDL_WM_GrabInput(SDL_GRAB_ON);
	}
	SDL_WarpMouse(SDLWidth / 2, SDLHeight / 2);
	SDL_ShowCursor(SDL_DISABLE);
	return 1;
}

void VSDL_ShutdownGraphics(void)
{
	if (WMAvailable) {
		SDL_WM_GrabInput(SDL_GRAB_OFF);
	}
}

void VSDL_GetEvent(void)
{
	SDL_Event Event;
	while (SDL_PollEvent(&Event)) {

		switch (Event.type) {
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			switch (Event.key.keysym.sym) {
			case SDLK_UP:
			case SDLK_KP8:
				event.data1 = KEY_UPARROW; break;

			case SDLK_DOWN:
			case SDLK_KP2:
				event.data1 = KEY_DOWNARROW; break;

			case SDLK_RIGHT:
			case SDLK_KP6:
				event.data1 = KEY_RIGHTARROW; break;

			case SDLK_LEFT:
			case SDLK_KP4:
				event.data1 = KEY_LEFTARROW; break;

			case SDLK_HOME:
			case SDLK_KP7:
				event.data1 = KEY_HOME; break;

			case SDLK_PAGEUP:
			case SDLK_KP9:
				event.data1 = KEY_PGUP; break;

			case SDLK_PAGEDOWN:
			case SDLK_KP3:
				event.data1 = KEY_PGDN; break;

			case SDLK_END:
			case SDLK_KP1:
				event.data1 = KEY_END; break;

			case SDLK_RSHIFT:
			case SDLK_LSHIFT:
				event.data1 = KEY_RSHIFT; break;

			case SDLK_RCTRL:
			case SDLK_LCTRL:
				event.data1 = KEY_RCTRL; break;

			case SDLK_RALT:
			case SDLK_LALT:
				event.data1 = KEY_RALT; break;

			case SDLK_F1:
				event.data1 = KEY_F1; break;
			case SDLK_F2:
				event.data1 = KEY_F2; break;
			case SDLK_F3:
				event.data1 = KEY_F3; break;
			case SDLK_F4:
				event.data1 = KEY_F4; break;
			case SDLK_F5:
				event.data1 = KEY_F5; break;
			case SDLK_F6:
				event.data1 = KEY_F6; break;
			case SDLK_F7:
				event.data1 = KEY_F7; break;
			case SDLK_F8:
				event.data1 = KEY_F8; break;
			case SDLK_F9:
				event.data1 = KEY_F9; break;
			case SDLK_F10:
				event.data1 = KEY_F10; break;
			case SDLK_F11:
				event.data1 = KEY_F11; break;
			case SDLK_F12:
				event.data1 = KEY_F12; break;

			default:
				event.data1 = Event.key.keysym.sym;

			}
			event.type = Event.type == SDL_KEYDOWN ? ev_keydown : ev_keyup;
			H2_PostEvent(&event);
			break;
		case SDL_MOUSEBUTTONUP:
			if (!usemouse) break;
			event.data1 = 0;
			switch (Event.button.button) {
			case SDL_BUTTON_LEFT:
				event.data1 &= 1; break;
			case SDL_BUTTON_RIGHT:
				event.data1 &= ~4; break;
			case SDL_BUTTON_MIDDLE:
				event.data1 &= ~2; break;
			}
			save_buttons = event.data1;
			event.data2 = 0;
			event.data3 = 0;
			event.type = ev_mouse;
			H2_PostEvent(&event);
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (!usemouse) break;
			event.data1 = 0;
			switch (Event.button.button) {
			case SDL_BUTTON_LEFT:
				event.data1 |= 1; break;
			case SDL_BUTTON_RIGHT:
				event.data1 |= 4; break;
			case SDL_BUTTON_MIDDLE:
				event.data1 |= 2; break;
			}
			save_buttons = event.data1;
			event.data2 = 0;
			event.data3 = 0;
			event.type = ev_mouse;
			H2_PostEvent(&event);
			break;
		case SDL_MOUSEMOTION:
			if (!usemouse) break;
			event.data1 = save_buttons;
			if (Event.motion.x == SDLWidth / 2 && Event.motion.y == SDLHeight / 2) break;
			if ((event.data2 = (Event.motion.xrel) * mouse_scale_factor) ||
				(event.data3 = (-Event.motion.yrel) * mouse_scale_factor)) {
				SDL_WarpMouse(SDLWidth / 2, SDLHeight / 2);
				event.type = ev_mouse;
				H2_PostEvent(&event);
			}
			break;
		}
	}
}


void VSDL_FinishUpdate(void)
{
	PaintOnSDLSurface();
	SDL_Delay(10); /* give time to another threads */
}

void VSDL_SetPalette(byte* palette)
{
	SetSDLColors(palette);
}

#endif
void VSDL_Stub(void)
{

}
