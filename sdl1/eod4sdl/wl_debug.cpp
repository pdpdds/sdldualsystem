// WL_DEBUG.C

#ifdef _WIN32
	#include <io.h>
#else
	#include <unistd.h>
#endif

#include "wl_def.h"
#pragma hdrstop

#ifdef USE_CLOUDSKY
#include "wl_cloudsky.h"
#endif

/*
=============================================================================

                                                 LOCAL CONSTANTS

=============================================================================
*/
#ifndef CHRISRULES
#define VIEWTILEX       (viewwidth >> 4)
#define VIEWTILEY       (viewheight >> 4)
#endif
/*
=============================================================================

                                                 GLOBAL VARIABLES

=============================================================================
*/

#ifdef DEBUGKEYS

int DebugKeys (void);

// from WL_DRAW.C

void ScalePost();
void SimpleScaleShape (int xcenter, int shapenum, unsigned height);

/*
=============================================================================

                                                 LOCAL VARIABLES

=============================================================================
*/

int     maporgx;
int     maporgy;
enum {mapview,tilemapview,actoratview,visview}  viewtype;

void ViewMap (byte exposed);

//===========================================================================

/*
==================
=
= CountObjects
=
==================
*/

void CountObjects (void)
{
    int     i,total,count,active,inactive,doors;
    objtype *obj;

    CenterWindow (17,7);
    active = inactive = count = doors = 0;

    US_Print ("Total statics :");
    total = (int)(laststatobj-&statobjlist[0]);
    US_PrintUnsigned (total);

    char str[60];
    sprintf(str,"\nlaststatobj=%.8X",(int32_t)(uintptr_t)laststatobj);
    US_Print(str);

    US_Print ("\nIn use statics:");
    for (i=0;i<total;i++)
    {
        if (statobjlist[i].shapenum != -1)
            count++;
        else
            doors++;        //debug
    }
    US_PrintUnsigned (count);

    US_Print ("\nDoors         :");
    US_PrintUnsigned (doornum);

    for (obj=player->next;obj;obj=obj->next)
    {
        if (obj->active)
            active++;
        else
            inactive++;
    }

    US_Print ("\nTotal actors  :");
    US_PrintUnsigned (active+inactive);

    US_Print ("\nActive actors :");
    US_PrintUnsigned (active);

    VW_UpdateScreen();
    IN_Ack ();
}


//===========================================================================

/*
===================
=
= PictureGrabber
=
===================
*/
void PictureGrabber (void)
{
    static char fname[] = "WSHOT000.BMP";

    for(int i = 0; i < 1000; i++)
    {
        fname[7] = i % 10 + '0';
        fname[6] = (i / 10) % 10 + '0';
        fname[5] = i / 100 + '0';
        int file = open(fname, O_RDONLY | O_BINARY);
        if(file == -1) break;       // file does not exist, so use that filename
        close(file);
    }

    // overwrites WSHOT999.BMP if all wshot files exist

    SDL_SaveBMP(curSurface, fname);

    CenterWindow (18,2);
    US_PrintCentered ("Screenshot taken");
    VW_UpdateScreen();
    IN_Ack();
}


//===========================================================================

/*
===================
=
= BasicOverhead
=
===================
*/

void BasicOverhead (void)
{
    int x, y, z, offx, offy;

    z = 128/MAPSIZE; // zoom scale
    offx = 320/2;
    offy = (160-MAPSIZE*z)/2;

#ifdef MAPBORDER
    int temp = viewsize;
    NewViewSize(16);
    DrawPlayBorder();
#endif

    // right side (raw)

    for(x=0;x<MAPSIZE;x++)
        for(y=0;y<MAPSIZE;y++)
            VWB_Bar(x*z+offx, y*z+offy,z,z,(unsigned)(uintptr_t)actorat[x][y]);

    // left side (filtered)

    uintptr_t tile;
    int color;
    offx -= 128;

    for(x=0;x<MAPSIZE;x++)
    {
        for(y=0;y<MAPSIZE;y++)
        {
            tile = (uintptr_t)actorat[x][y];
            if (ISPOINTER(tile) && ((objtype *)tile)->flags&FL_SHOOTABLE) color = 72;  // enemy
            else if (!tile || ISPOINTER(tile))
            {
                if (spotvis[x][y]) color = 111;  // visable
                else color = 0;  // nothing
            }
            else if (MAPSPOT(x,y,1) == PUSHABLETILE) color = 171;  // pushwall
            else if (tile == BIT_WALL) color = 158; // solid obj
            else if (tile < BIT_DOOR) color = 154;  // walls
            else if (tile < BIT_ALLTILES) color = 146;  // doors

            VWB_Bar(x*z+offx, y*z+offy,z,z,color);
        }
    }

    VWB_Bar(player->tilex*z+offx,player->tiley*z+offy,z,z,15); // player

    // resize the border to match

    VW_UpdateScreen();
    IN_Ack();

#ifdef MAPBORDER
    NewViewSize(temp);
    DrawPlayBorder();
#endif
}


//===========================================================================

/*
================
=
= ShapeTest
=
================
*/

void ShapeTest (void)
{
    //TODO
#if NOTYET
    extern  word    NumDigi;
    extern  word    *DigiList;
    extern  int     postx;
    extern  int     postwidth;
    extern  byte    *postsource;
    static  char    buf[10];

    boolean         done;
    ScanCode        scan;
    int             i,j,k,x;
    longword        l;
    byte            *addr;
    soundnames      sound;
    //      PageListStruct  far *page;

    CenterWindow(20,16);
    VW_UpdateScreen();
    for (i = 0,done = false; !done;)
    {
        US_ClearWindow();
        sound = (soundnames) -1;

        //              page = &PMPages[i];
        US_Print(" Page #");
        US_PrintUnsigned(i);
        if (i < PMSpriteStart)
            US_Print(" (Wall)");
        else if (i < PMSoundStart)
            US_Print(" (Sprite)");
        else if (i == ChunksInFile - 1)
            US_Print(" (Sound Info)");
        else
            US_Print(" (Sound)");

        /*              US_Print("\n XMS: ");
        if (page->xmsPage != -1)
        US_PrintUnsigned(page->xmsPage);
        else
        US_Print("No");

        US_Print("\n Main: ");
        if (page->mainPage != -1)
        US_PrintUnsigned(page->mainPage);
        else if (page->emsPage != -1)
        {
        US_Print("EMS ");
        US_PrintUnsigned(page->emsPage);
        }
        else
        US_Print("No");

        US_Print("\n Last hit: ");
        US_PrintUnsigned(page->lastHit);*/

        US_Print("\n Address: ");
        addr = (byte *) PM_GetPage(i);
        sprintf(buf,"0x%08X",(int32_t) addr);
        US_Print(buf);

        if (addr)
        {
            if (i < PMSpriteStart)
            {
                //
                // draw the wall
                //
                vbuf += 32*SCREENWIDTH;
                postx = 128;
                postwidth = 1;
                postsource = addr;
                for (x=0;x<64;x++,postx++,postsource+=64)
                {
                    wallheight[postx] = 256;
                    ScalePost ();
                }
                vbuf -= 32*SCREENWIDTH;
            }
            else if (i < PMSoundStart)
            {
                //
                // draw the sprite
                //
                vbuf += 32*SCREENWIDTH;
                SimpleScaleShape (160, i-PMSpriteStart, 64);
                vbuf -= 32*SCREENWIDTH;
            }
            else if (i == ChunksInFile - 1)
            {
                US_Print("\n\n Number of sounds: ");
                US_PrintUnsigned(NumDigi);
                for (l = j = k = 0;j < NumDigi;j++)
                {
                    l += DigiList[(j * 2) + 1];
                    k += (DigiList[(j * 2) + 1] + (PMPageSize - 1)) / PMPageSize;
                }
                US_Print("\n Total bytes: ");
                US_PrintUnsigned(l);
                US_Print("\n Total pages: ");
                US_PrintUnsigned(k);
            }
            else
            {
                byte *dp = addr;
                for (j = 0;j < NumDigi;j++)
                {
                    k = (DigiList[(j * 2) + 1] + (PMPageSize - 1)) / PMPageSize;
                    if ((i >= PMSoundStart + DigiList[j * 2])
                            && (i < PMSoundStart + DigiList[j * 2] + k))
                        break;
                }
                if (j < NumDigi)
                {
                    sound = (soundnames) j;
                    US_Print("\n Sound #");
                    US_PrintUnsigned(j);
                    US_Print("\n Segment #");
                    US_PrintUnsigned(i - PMSoundStart - DigiList[j * 2]);
                }
                for (j = 0;j < PageLengths[i];j += 32)
                {
                    byte v = dp[j];
                    int v2 = (unsigned)v;
                    v2 -= 128;
                    v2 /= 4;
                    if (v2 < 0)
                        VWB_Vlin(WindowY + WindowH - 32 + v2,
                        WindowY + WindowH - 32,
                        WindowX + 8 + (j / 32),BLACK);
                    else
                        VWB_Vlin(WindowY + WindowH - 32,
                        WindowY + WindowH - 32 + v2,
                        WindowX + 8 + (j / 32),BLACK);
                }
            }
        }

        VW_UpdateScreen();

        IN_Ack();
        scan = LastScan;

        IN_ClearKey(scan);
        switch (scan)
        {
            case sc_LeftArrow:
                if (i)
                    i--;
                break;
            case sc_RightArrow:
                if (++i >= ChunksInFile)
                    i--;
                break;
            case sc_W:      // Walls
                i = 0;
                break;
            case sc_S:      // Sprites
                i = PMSpriteStart;
                break;
            case sc_D:      // Digitized
                i = PMSoundStart;
                break;
            case sc_I:      // Digitized info
                i = ChunksInFile - 1;
                break;
/*            case sc_L:      // Load all pages
                for (j = 0;j < ChunksInFile;j++)
                    PM_GetPage(j);
                break;*/
            case sc_P:
                if (sound != -1)
                    SD_PlayDigitized(sound,8,8);
                break;
            case sc_Escape:
                done = true;
                break;
/*            case sc_Enter:
                PM_GetPage(i);
                break;*/
        }
    }
    SD_StopDigitized();
#endif
}


//===========================================================================


/*
================
=
= DebugKeys
=
================
*/

int DebugKeys (void)
{
    boolean esc;
    int level;

    if (Keyboard[sc_A]) {
        CenterWindow (18,2);
        if (!ammocheat) 
            US_PrintCentered ("Infinite Ammo ON");

        else
            US_PrintCentered ("Infinite Ammo OFF");

        VW_UpdateScreen();
        IN_Ack();
        ammocheat ^= 1;
        return 1;
    }
    else if (Keyboard[sc_B])             // B = knife fury
    {
        CenterWindow (12,2);
        if (!gamestate.furycheat) 
            US_PrintCentered ("Knife fury ON");

        else
            US_PrintCentered ("Knife fury OFF");

        VW_UpdateScreen();
        IN_Ack();
        gamestate.furycheat ^= 1;
        return 1;
    } 
    else if (Keyboard[sc_C])             // C = count objects
    {
        CountObjects();
        return 1;
    }
    else if (Keyboard[sc_E])             // E = quit level
    { 
        if (gamestate.weapon == wp_spear)
            playstate = ex_victorious;
        else
            playstate = ex_completed;
		return 1;
    }
    else if (Keyboard[sc_F])             // F = facing spot
    {
        char dirbuf[32];
        switch (GetCardinalDirection(player)) {
            case east:
                sprintf(dirbuf,"East");
                break;
            case northeast:
                sprintf(dirbuf,"Northeast");
                break;
            case north:
                sprintf(dirbuf,"North");
                break;
            case northwest:
                sprintf(dirbuf,"Northwest");
                break;
            case west:
                sprintf(dirbuf,"West");
                break;
            case southwest:
                sprintf(dirbuf,"Southwest");
                break;
            case south:
                sprintf(dirbuf,"South");
                break;
            case southeast:
                sprintf(dirbuf,"Southeast");
                break;
        }

        CenterWindow (12,15);

        US_Print (" Player Info\n");
        US_Print (" X: ");    US_PrintUnsigned (player->tilex); US_Print("\n"); 
        US_Print (" Y: ");    US_PrintUnsigned (player->tiley); US_Print("\n"); 
        US_Print (" A: ");    US_PrintUnsigned (player->angle); US_Print("\n"); 
        US_Print (" D: ");    US_Print(dirbuf); US_Print("\n");
        US_Print (" ------------\n");
        US_Print (" Map Info\n");
        US_Print (" 1: "); US_PrintUnsigned (MAPSPOT(player->tilex,player->tiley,0)); US_Print("\n");
        US_Print (" 2: "); US_PrintUnsigned (MAPSPOT(player->tilex,player->tiley,1)); US_Print("\n");
        US_Print (" 3: "); US_PrintUnsigned (MAPSPOT(player->tilex,player->tiley,2)); US_Print("\n");
        US_Print (" A: "); sprintf(str,"%.8X",(unsigned)(uintptr_t)actorat[player->tilex][player->tiley]); US_Print(str); US_Print("\n");
        US_Print (" T: "); US_PrintUnsigned (tilemap[player->tilex][player->tiley]);

        VW_UpdateScreen();
        IN_Ack();
        return 1;
    }

    else if (Keyboard[sc_G])             // G = god mode
    {
        CenterWindow (12,2);
        if (!gamestate.godcheat) 
            US_PrintCentered ("God mode ON");
        else
            US_PrintCentered ("God mode OFF");

        VW_UpdateScreen();
        IN_Ack();
        gamestate.godcheat ^= 1;
        return 1;
    }
    else if (Keyboard[sc_H])             // H = hurt self
    {
        IN_ClearKeysDown ();
        TakeDamage (16,NULL);
		return 1;
    }
    else if (Keyboard[sc_I])        // I = item cheat
    {
        CenterWindow (12,3);
        US_PrintCentered ("Free items!");
        VW_UpdateScreen();
        HealSelf (99);
    
        GiveWeapon(wp_launcher);
        GiveWeapon(wp_rifle);
        GiveWeapon(wp_mk4);
        GiveWeapon(wp_mk3);
        GiveWeapon(wp_machinegun);
        GiveWeapon(wp_pistol);
        GiveWeapon(wp_knife);
        gamestate.backpack = true;
        if (gamestate.weapon < wp_launcher) {
            NEXTWEAPON((weapontype)(gamestate.weapon + 1));
        }

        gamestate.medkits = MAXMEDKITS_B;
        gamestate.primaryammo = MAXPRIMARYAMMO_B;
        gamestate.secondaryammo = MAXSECONDARYAMMO_B;
        gamestate.rifleammo = MAXRIFLEAMMO_B;
        gamestate.rocketammo = MAXROCKETAMMO_B;
        DrawAmmo();

        IN_Ack ();
        return 1;
    }
    else if (Keyboard[sc_K])        // K = give keys
    {
        SD_PlaySound(GETKEYSND);
        CenterWindow(12,3);
        US_PrintCentered("All keys!");
        VW_UpdateScreen();

        gamestate.keys = 0xF;
        DrawKeys();
        IN_Ack();
        return 1;
    }
    else if (Keyboard[sc_R])        // R = level ratios
    {
       CenterWindow(18,3);
       
       if (ratiocheat)
           US_PrintCentered ("Level Ratios OFF");
       else
           US_PrintCentered ("Level Ratios ON");

       VW_UpdateScreen();
       ratiocheat ^= 1;
       IN_Ack ();
       return 1;
    }
    else if (Keyboard[sc_N])        // N = no clip
    {
        noclip^=1;
        CenterWindow (18,3);
        if (noclip)
            US_PrintCentered ("No clipping ON");
        else
            US_PrintCentered ("No clipping OFF");
        VW_UpdateScreen();
        IN_Ack ();
        return 1;
    }
    else if (Keyboard[sc_M])        // O = basic overhead
    {
#ifdef CHRISRULES
        ViewMap(1); 
#else
        BasicOverhead();
#endif
        return 1;
    }
    else if(Keyboard[sc_P])         // P = Ripper's picture grabber
    {
        PictureGrabber();
        return 1;
    }
    else if (Keyboard[sc_Q])        // Q = fast quit
        Quit (NULL);
    else if (Keyboard[sc_D]) {
        CenterWindow (18,3);
        US_PrintCentered ("Secrets revealed!");
        VW_UpdateScreen();

        for(int x = 0; x < MAPSIZE; x++) {
            for (int y = 0; y < MAPSIZE; y++) {
                if (MAPSPOT(x,y,1) == PUSHABLETILE)
                    tilemap[x][y] = 87; // hell tile
            }
        }
        IN_Ack ();
        return 1;
    }
    else if (Keyboard[sc_S])        // S = slow motion
    {
        CenterWindow(30,3);
        PrintY+=6;
        US_Print(" Slow Motion steps (default 14): ");
        VW_UpdateScreen();
        esc = !US_LineInput (px,py,str,NULL,true,2,0);
        if (!esc)
        {
            level = atoi (str);
            if (level>=0 && level<=50)
                singlestep = level;
        }
        return 1;
    }
    else if (Keyboard[sc_T])        // T = shape test
    {
        ShapeTest ();
        return 1;
    }
    else if (Keyboard[sc_V])        // V = extra VBLs
    {
        CenterWindow(30,3);
        PrintY+=6;
        US_Print("  Add how many extra VBLs(0-8): ");
        VW_UpdateScreen();
        esc = !US_LineInput (px,py,str,NULL,true,1,0);
        if (!esc)
        {
            level = atoi (str);
            if (level>=0 && level<=8)
                extravbls = level;
        }
        return 1;
    }
    else if (Keyboard[sc_W])        // W = warp to level
    {
        CenterWindow(26,3);
        PrintY+=6;
		switch (gamestate.episode) {
		case 0:
			US_Print("  Warp to which level(1-38): ");
			break;
		case 1: // 2007
			US_Print("  Warp to which level(1-2): ");
			break;
		case 2: // 2008
			US_Print("  Warp to which level(1-4): ");
			break;
		case 3: // 2009
			US_Print("  Warp to which level(1-4): ");
			break;
		case 4:
			US_Print("  Warp to which level(1-1): ");
			break;
		}
        VW_UpdateScreen();
        esc = !US_LineInput (px,py,str,NULL,true,2,0);
        if (!esc)
        {
            level = atoi (str);
            if (level > 0)
			    {
                switch (gamestate.episode) {
				case 0:
					if (level > 38) return 1;
					gamestate.mapon = level - 1;
					break;
				case 1:
					if (level > 2) return 1;
					gamestate.mapon = 38 + level - 1;
					break;
				case 2:
					if (level > 4) return 1;
					gamestate.mapon = 40 + level - 1;
					break;
				case 3:
					if (level > 4) return 1;
					gamestate.mapon = 44 + level - 1;
					break;
				case 4:
					if (level > 1) return 1;
					gamestate.mapon = 48 + level - 1;
					break;
				}
                if (gamestate.weapon == wp_spear) { // temporary weapon change
                    gamestate.weapons = (WP_KNIFE | WP_PISTOL);
                    gamestate.weapon = wp_pistol;
                    gamestate.primaryammo = STARTAMMO;
                    gamestate.secondaryammo = gamestate.rifleammo = gamestate.rocketammo = gamestate.medkits = 0;
                }
                playstate = ex_warped;
            }
        }
        return 1;
    }
#ifdef USE_CLOUDSKY
    else if(Keyboard[sc_Z])
    {
        char defstr[15];

        CenterWindow(34,4);
        PrintY+=6;
        US_Print("  Recalculate sky with seek: ");
        int seekpx = px, seekpy = py;
        US_PrintUnsigned(curSky->seed);
        US_Print("\n  Use color map (0-");
        US_PrintUnsigned(numColorMaps - 1);
        US_Print("): ");
        int mappx = px, mappy = py;
        US_PrintUnsigned(curSky->colorMapIndex);
        VW_UpdateScreen();

        sprintf(defstr, "%u", curSky->seed);
        esc = !US_LineInput(seekpx, seekpy, str, defstr, true, 10, 0);
        if(esc) return 0;
        curSky->seed = (uint32_t) atoi(str);

        sprintf(defstr, "%u", curSky->colorMapIndex);
        esc = !US_LineInput(mappx, mappy, str, defstr, true, 10, 0);
        if(esc) return 0;
        uint32_t newInd = (uint32_t) atoi(str);
        if(newInd < (uint32_t) numColorMaps)
        {
            curSky->colorMapIndex = newInd;
            InitSky();
        }
        else
        {
            CenterWindow (18,3);
            US_PrintCentered ("Illegal color map!");
            VW_UpdateScreen();
            IN_Ack ();
        }
		return 1;
    }
#endif

    return 0;
}



#ifdef CHRISRULES
/*
===================
=
= OverheadRefresh
=
===================
 */

int xstart, ystart, VIEWTILEX, VIEWTILEY;
int rotateActor[9] = {6,5,4,3,2,1,0,7,0};
int pixelSize, floorSize, TileMMapRatio, TileMWallRatio, mcheckx, mchecky;

int TileMSize = -1;

#define col_floor   25  // Gray Floor
#define col_secret  9   // Blue Floor

#define spr_start   23
#define spr_end     71  // Last sprite in wolf3d (Vines)


void TileMFloor(unsigned sx, unsigned sy, byte color)
{
    VWB_BarScaledCoord(sx, sy, floorSize, floorSize, color);
}

void TileMWall(unsigned sx, unsigned sy, unsigned pagenum)
{
    uint8_t *start;
    start = PM_GetPage(pagenum);

    for (int x=0;x<TileMSize;x++)
    {
        for (int y=0;y<TileMSize;y++)
        {
            VWB_BarScaledCoord((sx+x*pixelSize),(sy+y*pixelSize),pixelSize,pixelSize,(int)*start);
            start = start + TileMWallRatio;
        }

        start = start + TEXTURESIZE*(TileMWallRatio-1);
    }
}

void TileMDoor(unsigned sx, unsigned sy, byte doornum)
{
    int doorpage;
    #define DOORWALL (PMSpriteStart-8)

    if (doorobjlist[doornum].action == dr_open)
    {
        TileMFloor(sx,sy,col_floor);
        return;
    }

    switch(doorobjlist[doornum].lock)
    {
        case dr_normal:
        case dr_auto:
            doorpage = DOORWALL;
            break;
        case dr_rust:
            doorpage = DOORWALL-46;
            break;
        case dr_wood:
            doorpage = DOORWALL-42;
            break;
        case dr_lock1:
            doorpage = DOORWALL-34;
            break;
        case dr_lock2:
            doorpage = DOORWALL-36;
            break;
        case dr_lock3:
            doorpage = DOORWALL+6;
            break;
        case dr_lock4:
            doorpage = DOORWALL-44;
            break;
        case dr_elevator:
            doorpage = DOORWALL+4;
            break;
    }

    TileMWall(sx,sy,doorpage);

}

void TileMSprite(unsigned sx, unsigned sy, int pagenum)
{
    t_compshape   *shape;
    unsigned starty,endy,lpix;
    word *cmdptr;
    byte *line, col;
    short newstart;

    shape = (t_compshape *) PM_GetSprite(pagenum);
    cmdptr=shape->dataofs;

    for(lpix=shape->leftpix;lpix<=shape->rightpix;lpix++,cmdptr++)
    {
        line = (byte *)shape + *cmdptr;

        while((endy = READWORD(line)) != 0)
        {
            endy >>= 1;
            newstart = READWORD(line);
            starty = READWORD(line) >> 1;

            for(;starty<endy;starty++)
            {
                if (lpix%TileMWallRatio==0 && starty%TileMWallRatio==0)
                {
                    col=((byte *)shape)[newstart+starty];
                    VWB_BarScaledCoord(sx+(lpix/TileMWallRatio)*pixelSize,sy+(starty/TileMWallRatio)*pixelSize,pixelSize,pixelSize,col);
                }
            }
        }
    }
}

void SetupMapDefaults ()
{
    // Default Setting (TileM16s)
    if (TileMSize == -1)
        TileMSize = 32;

    pixelSize = 1;

    VIEWTILEX = viewwidth/(TileMSize*pixelSize);
    VIEWTILEY = viewheight/(TileMSize*pixelSize);

    if (VIEWTILEX > MAPSIZE) VIEWTILEX = MAPSIZE;
    if (VIEWTILEY > MAPSIZE) VIEWTILEY = MAPSIZE;

    TileMMapRatio  = MAPSIZE/TileMSize;
    TileMWallRatio = TEXTURESIZE/TileMSize;

    floorSize = pixelSize*TileMSize;

    if (viewsize>19) { xstart = ystart = 0; } else
    {
        xstart = (screenWidth-viewwidth)/2;
        ystart = (screenHeight-STATUSLINES*scaleFactor-viewheight)/2;
    }

        viewtype = actoratview;
    //      button0held = false;

    maporgx = player->tilex - VIEWTILEX/2;
    if (maporgx<0)
        maporgx = 0;
    if (maporgx>MAPSIZE-VIEWTILEX)
        maporgx=MAPSIZE-VIEWTILEX;
    maporgy = player->tiley - VIEWTILEY/2;
    if (maporgy<0)
        maporgy = 0;
    if (maporgy>MAPSIZE-VIEWTILEY)
        maporgy=MAPSIZE-VIEWTILEY;

    if (viewsize == 21)
        VL_ClearScreen(0);
    else
        VWB_BarScaledCoord((screenWidth>>1) - halfviewwidth,((screenHeight - scaleFactor * STATUSLINES) - viewheight) >> 1,viewwidth,viewheight,0x0);
}

void OverheadRefresh (byte exposed)
{
    int        x,y,endx,endy,sx,sy;
    uintptr_t   tile;

    endx =  maporgx+VIEWTILEX;
    endy =  maporgy+VIEWTILEY;

    for (y=maporgy;y<endy;y++)
    {
        for (x=maporgx;x<endx;x++)
        {
            sx = (x-maporgx)*floorSize + xstart;
            sy = (y-maporgy)*floorSize + ystart;

            if (!exposed && !spotsaw[x][y]) { 
                TileMFloor(sx,sy,0);
                continue;
            }

            switch (viewtype)
            {
#if 0
                case mapview:
                    tile = *(mapsegs[0]+farmapylookup[y]+x);
                    break;

                case tilemapview:
                    tile = tilemap[x][y];
                    break;

                case visview:
                    tile = spotvis[x][y];
                    break;
#endif
                case actoratview:
                    tile = (uintptr_t)actorat[x][y];
                    break;
            }

            if (tile && tile < MAXWALLTILES)
            {
                if ((mcheckx || mchecky) && tile == (uintptr_t)actorat[x+mcheckx][y+mchecky])
                    if (!exposed) {
                        if (spotsaw[x+mcheckx][y+mchecky] == spotsaw[x][y])
                             continue; 
                    } else {
                        continue;
                    }
                // Draw Wall
                if (Keyboard[sc_Tab] && exposed && MAPSPOT(x,y,1) == PUSHABLETILE)
                    TileMFloor(sx,sy,col_secret);
                else
                    TileMWall(sx,sy,(tile*2-2));
            }
            else if (tile && tile < MAXWALLTILES*4 && tile != MAXWALLTILES)
            {
                // Draw Door
                TileMDoor(sx, sy,(byte)(tile - MAXWALLTILES*2));
            }
            else
            {
                if (spotvis[x][y] && abs(player->tilex - x) < MAXMAPSIGHT && abs(player->tiley - y) < MAXMAPSIGHT)
                    TileMFloor(sx,sy,(byte)0x12);
                else
                    TileMFloor(sx,sy,(byte)levelinfo.floor1);

                if (statat[x][y]) {
                    if (Keyboard[sc_Tab] && exposed) {
                        switch (statat[x][y]->itemnumber) {
                            case bo_key1:
                            case bo_key2:
                            case bo_key3:
                            case bo_key4:
                                TileMFloor(sx,sy,0x20);
                                break;
                            case bo_cross:
                            case bo_chalice:
                            case bo_bible:
                            case bo_crown:
                            case bo_diamond:
                                TileMFloor(sx,sy,0xE);
                                break;
                        }
                    }

                    statobj_t *stat = (statobj_t *)((uintptr_t)statat[x][y]);
					if (!(stat->flags & FL_INVISIBLE))
						TileMSprite(sx,sy,stat->shapenum);
                }

                if (tile && tile != MAXWALLTILES)
                {
                    // Draw Actor
                    objtype *obj  = (objtype *)(tile);

                    if (exposed || (obj->flags & FL_NONMARK) || CheckLine(obj) ) {
                        int shapenum = ((statetype *)obj->state)->shapenum;
                        if (obj->state->rotate) shapenum += rotateActor[obj->dir];

                        TileMSprite(sx,sy,shapenum);
                    }
                }
            }

            if (player->tilex == x && player->tiley == y) {
                TileMFloor(sx,sy,0xB4);
            }
        }
    }

    fontnumber = 0;
    SETPRINTXY(msgPrintX+1, msgPrintY+1);
    SETFONTCOLOR(0x0,0x0);
    US_Print(mapname);

    SETPRINTXY(msgPrintX, msgPrintY);
    SETFONTCOLOR(DEF_MSG_CLR,0x0);
    US_Print(mapname);

    VW_UpdateScreen();

    if (Keyboard[sc_P])
        PictureGrabber();
}

/*
===================
=
= ViewMap
=
===================
*/

void ViewMap (byte exposed)
{
    SetupMapDefaults();
    if (TileMSize == 1) return;

    viewingmap = 1;

    CA_CacheGrChunk(STARTFONT);

    do
    {
        //
        // let user pan around
        //
        PollControls ();
        mcheckx = mchecky = 0;
        if ((controlx < 0 || controlh < 0) && maporgx>0)
            { maporgx--; mcheckx++; }
        if ((controlx > 0 || controlh > 0) && maporgx<mapwidth-VIEWTILEX)
            { maporgx++; mcheckx--; }
        if (controly < 0 && maporgy>0)
            { maporgy--; mchecky++; }
        if (controly > 0 && maporgy<mapheight-VIEWTILEY)
            { maporgy++; mchecky--; }

#if 0
        if (c.button0 && !button0held)
        {
            button0held = true;
            viewtype++;
            if (viewtype>visview)
                viewtype = mapview;
        }
        if (!c.button0)
            button0held = false;
#endif
        if (Keyboard[SDLK_KP_MINUS] || Keyboard[SDLK_MINUS]) {
            if (TileMSize > 8) {
                TileMSize >>= 1;
                SetupMapDefaults();
                IN_ClearKeysDown();
            }
        } else if (Keyboard[SDLK_KP_PLUS] || Keyboard[SDLK_EQUALS]) {
            if (TileMSize < 64) {
                TileMSize <<= 1;
                SetupMapDefaults();
                IN_ClearKeysDown();
            }
        }

        OverheadRefresh (exposed);

     } while (!Keyboard[sc_Escape] && !Keyboard[sc_Enter]);

    IN_ClearKeysDown ();
    viewingmap = 0;
}
#endif 

#else
#if 0
/*
===================
=
= OverheadRefresh
=
===================
*/

void OverheadRefresh (void)
{
    unsigned        x,y,endx,endy,sx,sy;
    unsigned        tile;


    endx = maporgx+VIEWTILEX;
    endy = maporgy+VIEWTILEY;

    for (y=maporgy;y<endy;y++)
    {
        for (x=maporgx;x<endx;x++)
        {
            sx = (x-maporgx)*16;
            sy = (y-maporgy)*16;

            switch (viewtype)
            {
#if 0
                case mapview:
                    tile = *(mapsegs[0]+farmapylookup[y]+x);
                    break;

                case tilemapview:
                    tile = tilemap[x][y];
                    break;

                case visview:
                    tile = spotvis[x][y];
                    break;
#endif
                case actoratview:
                    tile = (unsigned)actorat[x][y];
                    break;
            }

            if (tile<MAXWALLTILES)
                LatchDrawTile(sx,sy,tile);
            else
            {
                LatchDrawChar(sx,sy,NUMBERCHARS+((tile&0xf000)>>12));
                LatchDrawChar(sx+8,sy,NUMBERCHARS+((tile&0x0f00)>>8));
                LatchDrawChar(sx,sy+8,NUMBERCHARS+((tile&0x00f0)>>4));
                LatchDrawChar(sx+8,sy+8,NUMBERCHARS+(tile&0x000f));
            }
        }
    }
}
#endif

#if 0
/*
===================
=
= ViewMap
=
===================
*/

void ViewMap (void)
{
    boolean         button0held;

    viewtype = actoratview;
    //      button0held = false;


    maporgx = player->tilex - VIEWTILEX/2;
    if (maporgx<0)
        maporgx = 0;
    if (maporgx>MAPSIZE-VIEWTILEX)
        maporgx=MAPSIZE-VIEWTILEX;
    maporgy = player->tiley - VIEWTILEY/2;
    if (maporgy<0)
        maporgy = 0;
    if (maporgy>MAPSIZE-VIEWTILEY)
        maporgy=MAPSIZE-VIEWTILEY;

    do
    {
        //
        // let user pan around
        //
        PollControls ();
        if (controlx < 0 && maporgx>0)
            maporgx--;
        if (controlx > 0 && maporgx<mapwidth-VIEWTILEX)
            maporgx++;
        if (controly < 0 && maporgy>0)
            maporgy--;
        if (controly > 0 && maporgy<mapheight-VIEWTILEY)
            maporgy++;

#if 0
        if (c.button0 && !button0held)
        {
            button0held = true;
            viewtype++;
            if (viewtype>visview)
                viewtype = mapview;
        }
        if (!c.button0)
            button0held = false;
#endif

        OverheadRefresh ();

    } while (!Keyboard[sc_Escape]);

    IN_ClearKeysDown ();
}
#endif
#endif
