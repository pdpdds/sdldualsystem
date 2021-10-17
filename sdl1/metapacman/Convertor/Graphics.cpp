//==============================================================================
#include "Graphics.h"

//==============================================================================
void SurfaceLock(SDL_Surface *Dest) {
  if (SDL_MUSTLOCK(Dest)) {
		if (SDL_LockSurface(Dest) < 0) return;
	}
}

//----------------------------------------------------------------------------
void SurfaceUnlock(SDL_Surface *Dest) {
	if (SDL_MUSTLOCK(Dest)) {
		SDL_UnlockSurface(Dest);
	}
}

//----------------------------------------------------------------------------
void Blit(SDL_Surface *Dest, int DestX, int DestY, SDL_Surface *Src) {
	if(Dest == NULL) {
		printf("Blit > *Dest = NULL");
		return;
	}

	if(Src == NULL) {
		printf("Blit > *Src = NULL");
		return;
	}

	SDL_Rect DestR;
	DestR.x = DestX; DestR.y = DestY;
	SDL_BlitSurface(Src, NULL, Dest, &DestR);
}

//----------------------------------------------------------------------------
void Blit(SDL_Surface *Dest, int DestX, int DestY,
					SDL_Surface *Src, int SrcX, int SrcY, int SrcW, int SrcH) {
	if(Dest == NULL) {
		printf("Blit > *Dest = NULL");
		return;
	}

	if(Src == NULL) {
		printf("Blit > *Src = NULL");
		return;
	}

	SDL_Rect DestR;  SDL_Rect SrcR;
	DestR.x = DestX; DestR.y = DestY;
	SrcR.x = SrcX;   SrcR.y = SrcY;
	SrcR.w = SrcW;   SrcR.h = SrcH;
	SDL_BlitSurface(Src, &SrcR, Dest, &DestR);
}

//----------------------------------------------------------------------------
void Transparent(SDL_Surface *Dest, int R, int G, int B) {
	if(Dest == NULL) {
		printf("Transparent > *Dest = NULL");
		return;
	}
	SDL_SetColorKey(Dest, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(Dest->format, R, G, B));
}

//----------------------------------------------------------------------------
void Transparent(SDL_Surface *Dest, SDL_Color Color) {
	if(Dest == NULL) {
		printf("Transparent > *Dest = NULL");
		return;
	} 

	Transparent(Dest, Color.r, Color.g, Color.b);
};

//----------------------------------------------------------------------------
void DrawLine(SDL_Surface *Dest, double OriginX, double OriginY, double TargetX, double TargetY, Uint8 R, Uint8 G, Uint8 B) {
	float m = 0;

	if(OriginX < -250 && TargetX < -250) return;
	if(OriginY < -250 && TargetY < -250) return;

	if(OriginX >  250 && TargetX >  250) return;
	if(OriginY >  250 && TargetY >  250) return;

	if(OriginX == TargetX && OriginY == TargetY) {
		return;
	}

	if((TargetX - OriginX) == 0) {
		m = (TargetY - OriginY) / 1;
	}else{
		m = (TargetY - OriginY) / (TargetX - OriginX);
	}

	float b = (m * -OriginX) + -OriginY;

	float StartX	= 0;
	float EndX	= 0;

	if(OriginX > TargetX) {
		StartX	= TargetX;
		EndX	= OriginX;
	}else{
		StartX	= OriginX;
		EndX	= TargetX;
	}

	for(float X = StartX;X < EndX;X += .01) {
		float Y = (m*X) + b;

		DrawPixel(Dest, X, Y, R, G, B); 
	}
};

//----------------------------------------------------------------------------
void DrawPixel(SDL_Surface *Dest, int DestX, int DestY, SDL_Color Color) {
	if(Dest == NULL) {
		printf("DrawPixel > *Dest = NULL");
		return;
	}

	DrawPixel(Dest, DestX, DestY, Color.r, Color.g, Color.b);
};

//----------------------------------------------------------------------------
void DrawPixel(SDL_Surface *Dest, int DestX, int DestY, Uint8 R, Uint8 G, Uint8 B) {
	if(Dest == NULL) {
		printf("DrawPixel > *Dest = NULL");
		return;
	}

	DestX = DestX;
	DestY = DestY;

	DestX += 250;
	DestY += 250;

	if(DestX < 0) return;
	if(DestY < 0) return; 

	if(DestX > 500) return;
	if(DestY > 500) return;

	Uint32 Color = SDL_MapRGB(Dest->format, R, G, B);
	switch (Dest->format->BytesPerPixel) {
		case 1: { // 8-bpp Color Resolution
			Uint8 *Pixel;
			Pixel = (Uint8 *)Dest->pixels + DestY * Dest->pitch + DestX;
			*Pixel = Color;
			break;
		}
		case 2: {// 15-bpp or 16-bpp Color Resolution
			Uint16 *Pixel;
			Pixel = (Uint16 *)Dest->pixels + ((DestY * Dest->pitch) / 2) + DestX;
			*Pixel = Color;
			break;
		}
		case 3: { // 24-bpp Color Resolution
			Uint8 *Pixel;
			Pixel = (Uint8 *)Dest->pixels + (DestY * Dest->pitch) + (DestX * 3);
			if (SDL_BYTEORDER == SDL_LIL_ENDIAN) {
				Pixel[0] = Color;
				Pixel[1] = Color >> 8;
				Pixel[2] = Color >> 16;
			} else {
				Pixel[2] = Color;
				Pixel[1] = Color >> 8;
				Pixel[0] = Color >> 16;
			}
			break;
		}
		case 4: {// 32-bpp Color Resolution
			Uint32 *Pixel;
			Pixel = (Uint32 *)Dest->pixels + DestY*Dest->pitch/4 + DestX;
			*Pixel = Color;
			break;
		}
	}
}

//----------------------------------------------------------------------------
Uint32 GetPixel(SDL_Surface *Dest, int DestX, int DestY) {
	if(Dest == NULL) {
		printf("DrawPixel > *Dest = NULL");
		return 0;
	}

	switch (Dest->format->BytesPerPixel) {
		case 1: { // 8-bpp Color Resolution
			Uint8 *Pixel;
			Pixel = (Uint8 *)Dest->pixels + DestY * Dest->pitch + DestX;
			
			return (Uint32)*Pixel;
			
			break;
		}
		case 2: {// 15-bpp or 16-bpp Color Resolution
			Uint16 *Pixel;
			Pixel = (Uint16 *)Dest->pixels + ((DestY * Dest->pitch) / 2) + DestX;
			
			return (Uint32)*Pixel;
			
			break;
		}
		case 3: { // 24-bpp Color Resolution
			Uint8 *Pixel;
			Pixel = (Uint8 *)Dest->pixels + (DestY * Dest->pitch) + (DestX * 3);
			
			return (Uint32)*Pixel;
			
			break;
		}
		case 4: {// 32-bpp Color Resolution
			Uint32 *Pixel;
			Pixel = (Uint32 *)Dest->pixels + DestY*Dest->pitch/4 + DestX;
			
			return (Uint32)*Pixel;

			break;
		}
	}
}

//----------------------------------------------------------------------------
SDL_Surface* ImageLoad(char *cFile) {
	SDL_Surface *sOrig, *sConv;
	sOrig = SDL_LoadBMP(cFile);

	if(sOrig == NULL) {
		printf("ImageLoad > %s\n", SDL_GetError());
		return NULL;
	}

	sConv = SDL_DisplayFormat(sOrig);
	SDL_FreeSurface(sOrig);
	return sConv;
}

//----------------------------------------------------------------------------
SDL_Color SetColor(int R, int G, int B) {
    SDL_Color Color;

	Color.r = R;
	Color.g = G;
	Color.b = B;

	return Color;
}

//----------------------------------------------------------------------------
SDL_Surface* CreateRGBSurface(int Flags, int Width, int Height, int Bits) {
	SDL_Surface* Surface;

	Surface = SDL_CreateRGBSurface(Flags, Width, Height, Bits, RMASK, GMASK, BMASK, AMASK);
	Transparent(Surface, 255, 0, 255);
	Surface = SDL_DisplayFormatAlpha(Surface);

	return Surface;
}

//==============================================================================

