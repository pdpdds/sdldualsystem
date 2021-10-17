//==============================================================================
#include "XFontMap.h"

//==============================================================================
XFontMap::XFontMap() {
	FontControl 	= NULL;

	Crop			= false;

	Surf_FontMap	= NULL;

	CharWidth		= 0;

	CharHeight		= 0;

	FontColor 		= SetColor(255, 255, 255);

	FontBGColor		= SetColor(255, 0, 255);
};

//------------------------------------------------------------------------------
XFontMap::~XFontMap() {
};

//------------------------------------------------------------------------------
void XFontMap::OnCleanup() {
	if(FontControl) 	TTF_CloseFont(FontControl);
	if(Surf_FontMap) 	SDL_FreeSurface(Surf_FontMap);
};

//------------------------------------------------------------------------------
bool XFontMap::Initialize(char* File, int Pointsize, int Bit) {
	FontControl = TTF_OpenFont(File, Pointsize);

	if(FontControl == NULL) {
		cerr << "XFontMap::Initialize > File not found '" << File << "'\n";
		return false;
	}

	char 	tempChar[2];
	int 	tempWidth = 0;

	//--- Get the largest width
	for(int i = 0;i < 255;i++) {
		sprintf(tempChar, "%c", (char)i);
		TTF_SizeText(FontControl, tempChar, &tempWidth, NULL);	

		if(tempWidth > CharWidth) CharWidth = tempWidth;
	}

	//--- Get the height
	CharHeight = TTF_FontHeight(FontControl);

	//--- Create the font map surface
	Surf_FontMap = SDL_CreateRGBSurface(SDL_HWSURFACE, CharWidth, CharHeight * 255, Bit, RMASK, GMASK, BMASK, AMASK);
	Surf_FontMap = SDL_DisplayFormat(Surf_FontMap);

	ClearImage(Surf_FontMap, SDL_MapRGB(Surf_FontMap->format, FontBGColor.r, FontBGColor.g, FontBGColor.b));
	
	return true;
};

//------------------------------------------------------------------------------
void XFontMap::Print(SDL_Surface* Display, int X, int Y, char* Text, ...) {
	int CurrentCharW = 0;

	int CurrentCharX = X;

	char tempChar[2];

	for(int i = 0;i < strlen(Text);i++) {
		int ID = (int)Text[i];

		sprintf(tempChar, "%c", Text[i]);

		TTF_SizeText(FontControl, tempChar, &CurrentCharW, NULL);

		Blit(Display, Surf_FontMap, CurrentCharX, Y, 0, ID * CharHeight, CurrentCharW, CharHeight);

		//Move over so we're ready for the next character
		CurrentCharX += CurrentCharW;
	}
};

//------------------------------------------------------------------------------
void XFontMap::Print(SDL_Surface* Display, int X, int Y, int Pos, int MaxWidth, const char* Text, ...) {
	int CurrentCharW = 0;

	int CurrentCharX = X;
	int CurrentCharY = Y;

	int OffsetW = 0;
	int OffsetH = 0;

	char tempChar[2];

	for(int i = Pos;i < strlen(Text);i++) {
		int ID = (int)Text[i];		//Character ID on the FontMap

		sprintf(tempChar, "%c", Text[i]);

		//--- Grab the Size of the Current Character
		TTF_SizeText(FontControl, tempChar, &CurrentCharW, NULL);

		//If outside the bounderies, stop
		if(CurrentCharX > X + MaxWidth) {
			break;
		}

		//If outside the bounderies, crop it
		if(CurrentCharW + CurrentCharX > X + MaxWidth) {
			OffsetW = (CurrentCharW + CurrentCharX) - (X + MaxWidth);
		}

		//Draw our character, include cropping
		Blit(
				Display, 					Surf_FontMap, 			//Display, FontMap
				CurrentCharX, 				CurrentCharY, 			//X, Y (on Display)
				0, 							ID * CharHeight, 		//X, Y (on FontMap)
				CurrentCharW - OffsetW, 	CharHeight - OffsetH	//W, H (on FontMap)
		);

		//Move over so we're ready for the next character
		CurrentCharX += CurrentCharW;
	}
};

//------------------------------------------------------------------------------
void XFontMap::PrintClip(SDL_Surface* Display, int X, int Y, int MaxWidth, int MaxHeight, const char* Text, ...) {
	int CurrentCharW = 0;

	int CurrentCharX = X;
	int CurrentCharY = Y;

	int OffsetW = 0;
	int OffsetH = 0;

	char tempChar[2];

	for(int i = 0;i < strlen(Text);i++) {
		int ID = (int)Text[i];		//Character ID on the FontMap

		sprintf(tempChar, "%c", Text[i]);

		//--- Grab the Size of the Current Character
		TTF_SizeText(FontControl, tempChar, &CurrentCharW, NULL);

		//If outside the bounderies, skip down to the next line
		//(checking the left side of the character)
		if(CurrentCharX > X + MaxWidth) {
			CurrentCharX 	= 	X;
			CurrentCharY 	+= 	CharHeight;

			//When skipping down the to the next line, we don't want blank spaces
			if(Text[i] == ' ') continue;
		}

		//If outside the bounderies, crop it or skip to next line
		//(checking the right side of the character)
		if(CurrentCharW + CurrentCharX > X + MaxWidth) {
			if(Crop) {
				OffsetW = (CurrentCharW + CurrentCharX) - (X + MaxWidth);
			}else{
				CurrentCharX 	= 	X;
				CurrentCharY 	+= CharHeight;

				//When skipping down the to the next line, we don't want blank spaces
				if(Text[i] == ' ') continue;
			}
		}

		//If outside the bounderies, stop
		//(checking the top side of the character)
		if(CurrentCharY > Y + MaxHeight) {
			break;
		}

		//If outside the bounderies, crop it or stop
		//(checking the bottom side of the character)
		if(CharHeight + CurrentCharY > Y + MaxHeight) {
			if(Crop) {
				OffsetH = (CharHeight + CurrentCharY) - (Y + MaxHeight);
			}else{
				break;
			}
		}

		//Draw our character, include cropping
		Blit(
				Display, 					Surf_FontMap, 			//Display, FontMap
				CurrentCharX, 				CurrentCharY, 			//X, Y (on Display)
				0, 							ID * CharHeight, 		//X, Y (on FontMap)
				CurrentCharW - OffsetW, 	CharHeight - OffsetH	//W, H (on FontMap)
		);

		//Move over so we're ready for the next character
		CurrentCharX += CurrentCharW;

		OffsetW = 0;
		OffsetH = 0;
	}
};

//------------------------------------------------------------------------------
void XFontMap::RenderFontMap(int Type) {
	char tempChar[2];

	//--- Create a vertical font map
	for(int i = 0;i < 255;i++) {
		sprintf(tempChar, "%c", (char)i);

		switch(Type) {
			case TTF_TYPE_SOLID: {
				Blit(Surf_FontMap, TTF_RenderText_Solid(FontControl, tempChar, FontColor), 0, i * CharHeight);
				break;
			}
	
			case TTF_TYPE_SHADED: {
				Blit(Surf_FontMap, TTF_RenderText_Shaded(FontControl, tempChar, FontColor, FontBGColor), 0, i * CharHeight);
				break;
			}
	
			case TTF_TYPE_BLENDED: {
				Blit(Surf_FontMap, TTF_RenderText_Blended(FontControl, tempChar, FontColor), 0, i * CharHeight);
				break;
			}
		}
	}

	if(Type != TTF_TYPE_SHADED) {
//		XTransparency(Surf_FontMap, XSetColor(0, 0, 0));
	}
};

//------------------------------------------------------------------------------
int XFontMap::GetWidth(char* Text) {
	char tempChar[2];
	
	int CurrentCharW 	= 0;
	int TotalWidth 		= 0;

	for(int i = 0;i < strlen(Text);i++) {
		sprintf(tempChar, "%c", Text[i]);

		//--- Grab the Size of the Current Character
		TTF_SizeText(FontControl, tempChar, &CurrentCharW, NULL);
		
		TotalWidth += CurrentCharW;
	}
	
	return TotalWidth;
};

//==============================================================================
