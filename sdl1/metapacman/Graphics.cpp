//==============================================================================
#include "graphics.h"

//==============================================================================
void Blit(SDL_Surface* Display, SDL_Surface* Image, int X, int Y) {
	if(Display  == NULL) return;
	if(Image 	== NULL) return;

	SDL_Rect DestR;
	DestR.x = X; DestR.y = Y;

	SDL_BlitSurface(Image, NULL, Display, &DestR);
};

//------------------------------------------------------------------------------
void Blit(SDL_Surface* Display, SDL_Surface* Image, int X, int Y, int X2, int Y2, int Width, int Height) {
	if(Display 	== NULL) return;
	if(Image 	== NULL) return;

	SDL_Rect DestR;  	SDL_Rect SrcR;
	DestR.x = X; 		DestR.y = Y;

	SrcR.x = X2;   		SrcR.y = Y2;
	SrcR.w = Width;   	SrcR.h = Height;

	SDL_BlitSurface(Image, &SrcR, Display, &DestR);
};

//------------------------------------------------------------------------------
void Transparency(SDL_Surface* Surface, SDL_Color Color) {
	SDL_SetColorKey(Surface, 
		SDL_SRCCOLORKEY | SDL_RLEACCEL, 
		SDL_MapRGB(Surface->format, Color.r, Color.g, Color.b)
	);
};

//------------------------------------------------------------------------------
SDL_Surface* ImageLoad(char* File) {
	SDL_Surface* Surf_Temp = IMG_Load(File);
	
	SDL_Surface* Surf_Temp2 = SDL_DisplayFormat(Surf_Temp);
	
	SDL_FreeSurface(Surf_Temp);
	
	return Surf_Temp2;
};

//------------------------------------------------------------------------------
SDL_Color SetColor(int R, int G, int B) {
    SDL_Color Color;

	Color.r = R;
	Color.g = G;
	Color.b = B;

	return Color;
};

//==============================================================================
SDL_Surface *ctoc(SDL_Surface* surf, SDL_Color from, SDL_Color to)
{    
    Uint32 pixel;
    SDL_Color Color;
    int pixelCount = surf->w * surf->h; 

    for(int loop = 0; loop < pixelCount; loop++)
    {
	    pixel = *((unsigned int *)((unsigned char *)surf->pixels + loop * surf->format->BytesPerPixel));
        SDL_GetRGB(pixel, surf->format, &Color.r, &Color.g, &Color.b);
        if(Color.r == from.r && Color.g == from.g && Color.b == from.b)
        {
            pixel = SDL_MapRGB(surf->format, to.r, to.g, to.b);
            *((unsigned int *)((unsigned char *)surf->pixels + loop * surf->format->BytesPerPixel)) = pixel;
        }
    }
    return surf;
}

//------------------------------------------------------------------------------
void ClearImage(SDL_Surface* Surface, Uint32 Color) {
	SDL_Rect tRect;
	tRect.x = 0; 			tRect.y = 0;
	tRect.w = Surface->w; 	tRect.h = Surface->h;

	SDL_FillRect(Surface, &tRect, Color);
}

//------------------------------------------------------------------------------
void SetAlpha(SDL_Surface* Surface, int Alpha) {
	SDL_SetAlpha(Surface, SDL_SRCALPHA | SDL_RLEACCEL, Alpha);
};


//==============================================================================
