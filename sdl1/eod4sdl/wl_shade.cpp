#include "version.h"

#ifdef USE_SHADING
#include "wl_def.h"
#include "wl_shade.h"

void InitLevelShadeTable(void);

uint8_t shadetable[SHADE_COUNT][256];
int LSHADE_flag;

// Returns the palette index of the nearest matching color of the
// given RGB color in given palette
byte GetColor(byte red, byte green, byte blue, SDL_Color *palette)
{
    byte mincol = 0;
    double mindist = 200000.F, curdist, DRed, DGreen, DBlue;

    SDL_Color *palPtr = palette;

    for(int col = 0; col < 256; col++, palPtr++)
    {
        DRed   = (double) (red   - palPtr->r);
        DGreen = (double) (green - palPtr->g);
        DBlue  = (double) (blue  - palPtr->b);
        curdist = DRed * DRed + DGreen * DGreen + DBlue * DBlue;
        if(curdist < mindist)
        {
            mindist = curdist;
            mincol = (byte) col;
        }
    }
    return mincol;
}

// Fade all colors in 32 steps down to the destination-RGB
// (use gray for fogging, black for standard shading)
void GenerateShadeTable(byte destRed, byte destGreen, byte destBlue,
                        SDL_Color *palette, int fog)
{
    double curRed, curGreen, curBlue, redStep, greenStep, blueStep;
    SDL_Color *palPtr = palette;

    // Set the fog-flag
    LSHADE_flag=fog;

    // Color loop
    for(int i = 0; i < 256; i++, palPtr++)
    {
        // Get original palette color
        curRed   = palPtr->r;
        curGreen = palPtr->g;
        curBlue  = palPtr->b;

        // Calculate increment per step
        redStep   = ((double) destRed   - curRed)   / (SHADE_COUNT + 8);
        greenStep = ((double) destGreen - curGreen) / (SHADE_COUNT + 8);
        blueStep  = ((double) destBlue  - curBlue)  / (SHADE_COUNT + 8);

        // Calc color for each shade of the current color
        for (int shade = 0; shade < SHADE_COUNT; shade++)
        {
            shadetable[shade][i] = GetColor((byte) curRed, (byte) curGreen, (byte) curBlue, palette);

            // Inc to next shade
            curRed   += redStep;
            curGreen += greenStep;
            curBlue  += blueStep;
        }
    }
}

void NoShading()
{
    for(int shade = 0; shade < SHADE_COUNT; shade++)
        for(int i = 0; i < 256; i++)
            shadetable[shade][i] = i;
}

void InitLevelShadeTable()
{
    int tmp = 0xFF;

    switch (levelinfo.shadestr) {
        case 1:
            tmp = 0;
            break;
        case 2:
            tmp = 5;
            break;
    }

    if(!switches.shading || levelinfo.shadestr == 0xFF || gamestate.lightson)
        NoShading();
    else
        GenerateShadeTable(levelinfo.shadeR, levelinfo.shadeG, levelinfo.shadeB, gamepal, tmp);
}

int GetShade(int scale)
{
    int shade = (scale >> 1) / (((viewwidth * 3) >> 8) + 1 + LSHADE_flag);  // TODO: reconsider this...
    if(shade > 32) shade = 32;
    else if(shade < 1) shade = 1;
    shade = 32 - shade;

    return shade;
}

#endif
