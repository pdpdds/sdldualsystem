// WL_DRAW.C

#include "wl_def.h"
#pragma hdrstop

#include "wl_cloudsky.h"
#include "wl_atmos.h"
#include "wl_shade.h"
#include <iostream>

/*
=============================================================================

                               LOCAL CONSTANTS

=============================================================================
*/

// the door is the last picture before the sprites
#define DOORWALL        (PMSpriteStart-8)

#define ACTORSIZE       0x4000

/*
=============================================================================

                              GLOBAL VARIABLES

=============================================================================
*/

static byte *vbuf = NULL;
unsigned vbufPitch = 0;

int32_t    lasttimecount;
int32_t    frameon;
int   messagetime=0; // WSJ's message feature
int   messagecolor=0x35;
boolean fpscounter;

boolean fs_drawflag = 0; 

int fps_frames=0, fps_time=0, fps=0;

int *wallheight;
int min_wallheight;

//
// math tables
//
short *pixelangle;
int32_t finetangent[FINEANGLES/4];
fixed sintable[ANGLES+ANGLES/4];
fixed *costable = sintable+(ANGLES/4);

//
// refresh variables
//
fixed   viewx,viewy;                    // the focal point
short   viewangle;
fixed   viewsin,viewcos;

void    TransformActor (objtype *ob);
void    BuildTables (void);
void    ClearScreen (void);
int     CalcRotate (objtype *ob);
void    DrawScaleds (void);
void    CalcTics (void);
void    ThreeDRefresh (void);

void DrawPlayerWeapon(void);

// WSJ's message feature
void   GetMessage (char *lastmessage); 
void   DrawMessage (void);
void    DrawRatios(void);

//
// wall optimization variables
//
int     lastside;               // true for vertical
int32_t    lastintercept;
int     lasttilehit;
int     lasttexture;

//
// ray tracing variables
//
short    focaltx,focalty,viewtx,viewty;
longword xpartialup,xpartialdown,ypartialup,ypartialdown;

short   midangle,angle;

word    tilehit;
int     pixx;

short   xtile,ytile;
short   xtilestep,ytilestep;
int32_t    xintercept,yintercept;
word    xstep,ystep;
word    xspot,yspot;
int     texdelta;

short pw = 0;

word horizwall[MAXWALLTILES],vertwall[MAXWALLTILES];


/*
============================================================================

                           3 - D  DEFINITIONS

============================================================================
*/

/*
========================
=
= TransformActor
=
= Takes paramaters:
=   gx,gy               : globalx/globaly of point
=
= globals:
=   viewx,viewy         : point of view
=   viewcos,viewsin     : sin/cos of viewangle
=   scale               : conversion from global value to screen value
=
= sets:
=   screenx,transx,transy,screenheight: projected edge location and size
=
========================
*/


//
// transform actor
//
void TransformActor (objtype *ob)
{
    fixed gx,gy,gxt,gyt,nx,ny;

//
// translate point to view centered coordinates
//
    gx = ob->x-viewx;
    gy = ob->y-viewy;

//
// calculate newx
//
    gxt = FixedMul(gx,viewcos);
    gyt = FixedMul(gy,viewsin);
    nx = gxt-gyt-ACTORSIZE;         // fudge the shape forward a bit, because
                                    // the midpoint could put parts of the shape
                                    // into an adjacent wall

//
// calculate newy
//
    gxt = FixedMul(gx,viewsin);
    gyt = FixedMul(gy,viewcos);
    ny = gyt+gxt;

//
// calculate perspective ratio
//
    ob->transx = nx;
    ob->transy = ny;

    if (nx<MINDIST)                 // too close, don't overflow the divide
    {
        ob->viewheight = 0;
        return;
    }

    ob->viewx = (word)(centerx + ny*scale/nx);

//
// calculate height (heightnumerator/(nx>>8))
//
    ob->viewheight = (word)(heightnumerator/(nx>>8));
}

//==========================================================================

/*
========================
=
= TransformTile
=
= Takes paramaters:
=   tx,ty               : tile the object is centered in
=
= globals:
=   viewx,viewy         : point of view
=   viewcos,viewsin     : sin/cos of viewangle
=   scale               : conversion from global value to screen value
=
= sets:
=   screenx,transx,transy,screenheight: projected edge location and size
=
= Returns true if the tile is withing getting distance
=
========================
*/

boolean TransformTile (int tx, int ty, short *dispx, short *dispheight)
{
    fixed gx,gy,gxt,gyt,nx,ny;

//
// translate point to view centered coordinates
//
    gx = ((int32_t)tx<<TILESHIFT)+0x8000-viewx;
    gy = ((int32_t)ty<<TILESHIFT)+0x8000-viewy;

//
// calculate newx
//
    gxt = FixedMul(gx,viewcos);
    gyt = FixedMul(gy,viewsin);
    nx = gxt-gyt-0x2000;            // 0x2000 is size of object

//
// calculate newy
//
    gxt = FixedMul(gx,viewsin);
    gyt = FixedMul(gy,viewcos);
    ny = gyt+gxt;


//
// calculate height / perspective ratio
//
    if (nx<MINDIST)                 // too close, don't overflow the divide
        *dispheight = 0;
    else
    {
        *dispx = (short)(centerx + ny*scale/nx);
        *dispheight = (short)(heightnumerator/(nx>>8));
    }

//
// see if it should be grabbed
//
    if (nx<TILEGLOBAL && ny>-TILEGLOBAL/2 && ny<TILEGLOBAL/2)
        return true;
    else
        return false;
}

//==========================================================================

/*
====================
=
= CalcHeight
=
= Calculates the height of xintercept,yintercept from viewx,viewy
=
====================
*/

int CalcHeight()
{
    fixed z = FixedMul(xintercept - viewx, viewcos)
        - FixedMul(yintercept - viewy, viewsin);
    if(z < MINDIST) z = MINDIST;
    int height = heightnumerator / (z >> 8);
    if(height < min_wallheight) min_wallheight = height;
    return height;
}

//==========================================================================

/*
===================
=
= ScalePost
=
===================
*/

byte *postsource;
int postx;
int postwidth;

void ScalePost()
{
    int ywcount, yoffs, yw, yd, yendoffs;
    byte col;

#ifdef USE_SHADING
    byte *curshades = shadetable[GetShade(wallheight[postx])];
#endif

    ywcount = yd = wallheight[postx] >> 3;
    if(yd <= 0) yd = 100;

    yoffs = (halfviewheight - ywcount) * vbufPitch;
    if(yoffs < 0) yoffs = 0;
    yoffs += postx;

    yendoffs = halfviewheight + ywcount - 1;
    yw=TEXTURESIZE-1;

    while(yendoffs >= viewheight)
    {
        ywcount -= TEXTURESIZE/2;
        while(ywcount <= 0)
        {
            ywcount += yd;
            yw--;
        }
        yendoffs--;
    }
    if(yw < 0) return;

#ifdef USE_SHADING
    col = curshades[postsource[yw]];
#else
    col = postsource[yw];
#endif
    yendoffs = yendoffs * vbufPitch + postx;
    while(yoffs <= yendoffs)
    {
        vbuf[yendoffs] = col;
        ywcount -= TEXTURESIZE/2;
        if(ywcount <= 0)
        {
            do
            {
                ywcount += yd;
                yw--;
            }
            while(ywcount <= 0);
            if(yw < 0) break;
#ifdef USE_SHADING
        col = curshades[postsource[yw]];
#else
       col = postsource[yw];
#endif
        }
        yendoffs -= vbufPitch;
    }
}

void GlobalScalePost(byte *vidbuf, unsigned pitch)
{
    vbuf = vidbuf;
    vbufPitch = pitch;
    ScalePost();
}

/*
====================
=
= HitVertWall
=
= tilehit bit 7 is 0, because it's not a door tile
= if bit 6 is 1 and the adjacent tile is a door tile, use door side pic
=
====================
*/

void HitVertWall (void)
{
    int wallpic;
    int texture;
    uint16_t tile;
		
    texture = ((yintercept+texdelta)>>TEXTUREFROMFIXEDSHIFT)&TEXTUREMASK;
    if (xtilestep == -1)
    {
        texture = TEXTUREMASK-texture;
        xintercept += TILEGLOBAL;
    }

    if(lastside==1 && lastintercept==xtile && lasttilehit==tilehit && !(lasttilehit & (BIT_WALL | MOVINGIN_N | MOVINGIN_W | MOVINGIN_E | MOVINGIN_S)))
    {
        if((pixx&3) && texture == lasttexture)
        {
            ScalePost();
            postx = pixx;
            wallheight[pixx] = wallheight[pixx-1];
            return;
        }
        ScalePost();
        wallheight[pixx] = CalcHeight();
        postsource+=texture-lasttexture;
        postwidth=1;
        postx=pixx;
        lasttexture=texture;
        return;
    }

    if(lastside!=-1) ScalePost();

    lastside=1;
    lastintercept=xtile;
    lasttilehit=tilehit;
    lasttexture=texture;
    wallheight[pixx] = CalcHeight();

    switch(lasttilehit) {
        case 78: // fireplace
            tilehit+=(frameon>>4)%4; // the 3 is speed, the 4 is frames
            break;
        case 87: // hell wall
            tilehit+=((frameon>>6))%3; // the 7 is speed, the 3 is frames
            break;
    }

    postx = pixx;
    postwidth = 1;

    if (tilehit & BIT_WALL)
    {                        // check for adjacent doors
        ytile = yintercept>>TILESHIFT;
        tile = tilemap[xtile-xtilestep][ytile];
        if (tile & BIT_DOOR)
        {
            switch(doorobjlist[tile & LAST_DOORNUM].lock)
            {
                case dr_rust:
                    wallpic = DOORWALL-37;
                    break;
                case dr_wood:
                    wallpic = DOORWALL-39;
                    break;
                default:
                    wallpic = DOORWALL+3;
                    break;
            }
        }
        else
        wallpic = vertwall[tilehit & ~BIT_WALL];
    }
    else
        wallpic = vertwall[tilehit]; 

    postsource = PM_GetTexture(wallpic) + texture;
}

/*
====================
=
= HitHorizWall
=
= tilehit bit 7 is 0, because it's not a door tile
= if bit 6 is 1 and the adjacent tile is a door tile, use door side pic
=
====================
*/

void HitHorizWall (void)
{
    int wallpic;
    int texture;
    uint16_t tile;

    texture = ((xintercept+texdelta)>>TEXTUREFROMFIXEDSHIFT)&TEXTUREMASK;
    if (ytilestep == -1)
        yintercept += TILEGLOBAL;
    else
        texture = TEXTUREMASK-texture;

    if(lastside==0 && lastintercept==ytile && lasttilehit==tilehit && !(lasttilehit & (BIT_WALL | MOVINGIN_N | MOVINGIN_W | MOVINGIN_E | MOVINGIN_S)))
    {
        if((pixx&3) && texture == lasttexture)
        {
            ScalePost();
            postx=pixx;
            wallheight[pixx] = wallheight[pixx-1];
            return;
        }
        ScalePost();
        wallheight[pixx] = CalcHeight();
        postsource+=texture-lasttexture;
        postwidth=1;
        postx=pixx;
        lasttexture=texture;
        return;
    }

    if(lastside!=-1) ScalePost();

    lastside=0;
    lastintercept=ytile;
    lasttilehit=tilehit;
    lasttexture=texture;
    wallheight[pixx] = CalcHeight();

    switch (lasttilehit)
    {
        case 78: // fireplace
            tilehit+=(frameon>>4)%4; // the 3 is speed, the 4 is frames
            break;
        case 87: // hell wall
            tilehit+=((frameon>>6)%3); // the 7 is speed, the 3 is frames
            break;
    } 

    postx = pixx;
    postwidth = 1;

    if (tilehit & BIT_WALL)
    {                        // check for adjacent doors
        xtile = xintercept>>TILESHIFT;
        tile = tilemap[xtile][ytile-ytilestep];
        if(tile & BIT_DOOR)
        {
            switch(doorobjlist[tile & LAST_DOORNUM].lock)
            {
              case dr_rust:
                   wallpic = DOORWALL-38;
                   break;
              case dr_wood:
                   wallpic = DOORWALL-40;
                   break;
              default:
                   wallpic = DOORWALL+2;
                   break;
            }
        }
        else
        wallpic = horizwall[tilehit & ~BIT_WALL];
    }
    else
        wallpic = horizwall[tilehit]; 

    postsource = PM_GetTexture(wallpic) + texture;
}

//==========================================================================

/*
====================
=
= HitHorizDoor
=
====================
*/

void HitHorizDoor (void)
{
    int doorpage;
    int doornum;
    int texture;

    doornum = tilehit&LAST_DOORNUM;
    texture = ((xintercept-doorposition[doornum])>>TEXTUREFROMFIXEDSHIFT)&TEXTUREMASK;

    if(lasttilehit==tilehit)
    {
        if((pixx&3) && texture == lasttexture)
        {
            ScalePost();
            postx=pixx;
            wallheight[pixx] = wallheight[pixx-1];
            return;
        }
        ScalePost();
        wallheight[pixx] = CalcHeight();
        postsource+=texture-lasttexture;
        postwidth=1;
        postx=pixx;
        lasttexture=texture;
        return;
    }

    if(lastside!=-1) ScalePost();

    lastside=2;
    lasttilehit=tilehit;
    lasttexture=texture;
    wallheight[pixx] = CalcHeight();
    postx = pixx;
    postwidth = 1;

    switch(doorobjlist[doornum].lock)
    {
        case dr_normal:
        case dr_auto:
            doorpage = DOORWALL;
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
        case dr_rust:
            doorpage = DOORWALL-46;
            break;
        case dr_wood:
            doorpage = DOORWALL-42;
            break;
    }

    postsource = PM_GetTexture(doorpage) + texture;
}

//==========================================================================

/*
====================
=
= HitVertDoor
=
====================
*/

void HitVertDoor (void)
{
    int doorpage;
    int doornum;
    int texture;

    doornum = tilehit&LAST_DOORNUM;
    texture = ((yintercept-doorposition[doornum])>>TEXTUREFROMFIXEDSHIFT)&TEXTUREMASK;

    if(lasttilehit==tilehit)
    {
        if((pixx&3) && texture == lasttexture)
        {
            ScalePost();
            postx=pixx;
            wallheight[pixx] = wallheight[pixx-1];
            return;
        }
        ScalePost();
        wallheight[pixx] = CalcHeight();
        postsource+=texture-lasttexture;
        postwidth=1;
        postx=pixx;
        lasttexture=texture;
        return;
    }

    if(lastside!=-1) ScalePost();

    lastside=2;
    lasttilehit=tilehit;
    lasttexture=texture;
    wallheight[pixx] = CalcHeight();
    postx = pixx;
    postwidth = 1;

    switch(doorobjlist[doornum].lock)
    {
        case dr_normal:
        case dr_auto:
            doorpage = DOORWALL+1;
            break;
        case dr_lock1:
            doorpage = DOORWALL-33;
            break;
        case dr_lock2:
            doorpage = DOORWALL-35;
            break;
        case dr_lock3:
            doorpage = DOORWALL+7;
            break;
        case dr_lock4:
            doorpage = DOORWALL-43;
            break;
        case dr_elevator:
            doorpage = DOORWALL+5;
            break;
        case dr_rust:
            doorpage = DOORWALL-45;
            break;
        case dr_wood:
            doorpage = DOORWALL-41;
            break;
    }

    postsource = PM_GetTexture(doorpage) + texture;
}

//==========================================================================

/*
=====================
=
= VGAClearScreen
=
=====================
*/

void VGAClearScreen (void)
{
    short ceiling=levelinfo.ceiling1;
    short floor=levelinfo.floor1;

    int y;
    byte *ptr = vbuf;

    for(y = 0; y < halfviewheight; y++, ptr += vbufPitch)
        memset(ptr, shadetable[GetShade((halfviewheight - y) << 3)][ceiling], viewwidth);
    for(; y < viewheight; y++, ptr += vbufPitch)
        memset(ptr, shadetable[GetShade((y - halfviewheight) << 3)][floor], viewwidth);

}

//==========================================================================

/*
=====================
=
= CalcRotate
=
=====================
*/

int CalcRotate (objtype *ob)
{
    int angle, viewangle, result;

    // this isn't exactly correct, as it should vary by a trig value,
    // but it is close enough with only eight rotations

    viewangle = player->angle + 319*(centerx - ob->viewx)/(centerx<<3);

    if (ob->obclass == rocketobj || ob->obclass == hrocketobj || ob->obclass == fireballobj || ob->obclass == spearprojobj)
        angle = (viewangle-180) - ob->angle;
    else
        angle = (viewangle-180) - dirangle[ob->dir];

    angle+=ANGLES/16;
    while (angle>=ANGLES)
        angle-=ANGLES;
    while (angle<0)
        angle+=ANGLES;

    result = angle/(ANGLES/8);

    if (ob->obclass == turretobj)
        ob->temp2 = result;

    if (ob->state->rotate == 2)             // 2 rotation pain frame
        return 0;               // pain with shooting frame bugfix

    return result;
}

void ScaleShape (int xcenter, int shapenum, unsigned height, uint32_t flags)
{
    t_compshape *shape;
    unsigned scale,pixheight;
    unsigned starty,endy;
    word *cmdptr;
    byte *cline;
    byte *line;
    byte *vmem;
    int actx,i,upperedge;
    short newstart;
    int scrstarty,screndy,lpix,rpix,pixcnt,ycnt;
    unsigned j;
    byte col;

#ifdef USE_SHADING
    byte *curshades;
    if(flags & FL_FULLBRIGHT)
        curshades = shadetable[0];
    else
        curshades = shadetable[GetShade(height)];

#endif

    shape = (t_compshape *) PM_GetSprite(shapenum);

    scale=height>>3;                 // low three bits are fractional
    if(!scale) return;   // too close or far away

    pixheight=scale*SPRITESCALEFACTOR;
    actx=xcenter-scale;
    upperedge=halfviewheight-scale;

    cmdptr=(word *) shape->dataofs;

    for(i=shape->leftpix,pixcnt=i*pixheight,rpix=(pixcnt>>6)+actx;i<=shape->rightpix;i++,cmdptr++)
    {
        lpix=rpix;
        if(lpix>=viewwidth) break;
        pixcnt+=pixheight;
        rpix=(pixcnt>>6)+actx;
        if(lpix!=rpix && rpix>0)
        {
            if(lpix<0) lpix=0;
            if(rpix>viewwidth) rpix=viewwidth,i=shape->rightpix+1;
            cline=(byte *)shape + *cmdptr;
            while(lpix<rpix)
            {
                if(wallheight[lpix]<=(int)height)
                {
                    line=cline;
                    while((endy = READWORD(line)) != 0)
                    {
                        endy >>= 1;
                        newstart = READWORD(line);
                        starty = READWORD(line) >> 1;
                        j=starty;
                        ycnt=j*pixheight;
                        screndy=(ycnt>>6)+upperedge;
                        if(screndy<0) vmem=vbuf+lpix;
                        else vmem=vbuf+screndy*vbufPitch+lpix;
                        for(;j<endy;j++)
                        {
                            scrstarty=screndy;
                            ycnt+=pixheight;
                            screndy=(ycnt>>6)+upperedge;
                            if(scrstarty!=screndy && screndy>0)
                            {
#ifdef USE_SHADING
                                col=curshades[((byte *)shape)[newstart+j]];
#else
                                col=((byte *)shape)[newstart+j];
#endif
                                if(scrstarty<0) scrstarty=0;
                                if(screndy>viewheight) screndy=viewheight,j=endy;

                                while(scrstarty<screndy)
                                {
                                    *vmem=col;
                                    vmem+=vbufPitch;
                                    scrstarty++;
                                }
                            }
                        }
                    }
                }
                lpix++;
            }
        }
    }
}

void SimpleScaleShape (int xcenter, int shapenum, unsigned height)
{
    t_compshape   *shape;
    unsigned scale,pixheight;
    unsigned starty,endy;
    word *cmdptr;
    byte *cline;
    byte *line;
    int actx,i,upperedge;
    short newstart;
    int scrstarty,screndy,lpix,rpix,pixcnt,ycnt;
    unsigned j;
    byte col;
    byte *vmem;

    shape = (t_compshape *) PM_GetSprite(shapenum);

    scale=height>>1;
    pixheight=scale*SPRITESCALEFACTOR;
    actx=xcenter-scale;
    upperedge=halfviewheight-scale;

    cmdptr=shape->dataofs;

    for(i=shape->leftpix,pixcnt=i*pixheight,rpix=(pixcnt>>6)+actx;i<=shape->rightpix;i++,cmdptr++)
    {
        lpix=rpix;
        if(lpix>=viewwidth) break;
        pixcnt+=pixheight;
        rpix=(pixcnt>>6)+actx;
        if(lpix!=rpix && rpix>0)
        {
            if(lpix<0) lpix=0;
            if(rpix>viewwidth) rpix=viewwidth,i=shape->rightpix+1;
            cline = (byte *)shape + *cmdptr;
            while(lpix<rpix)
            {
                line=cline;
                while((endy = READWORD(line)) != 0)
                {
                    endy >>= 1;
                    newstart = READWORD(line);
                    starty = READWORD(line) >> 1;
                    j=starty;
                    ycnt=j*pixheight;
                    screndy=(ycnt>>6)+upperedge;
                    if(screndy<0) vmem=vbuf+lpix;
                    else vmem=vbuf+screndy*vbufPitch+lpix;
                    for(;j<endy;j++)
                    {
                        scrstarty=screndy;
                        ycnt+=pixheight;
                        screndy=(ycnt>>6)+upperedge;
                        if(scrstarty!=screndy && screndy>0)
                        {
                            col=((byte *)shape)[newstart+j];
                            if(scrstarty<0) scrstarty=0;
                            if(screndy>viewheight) screndy=viewheight,j=endy;

                            while(scrstarty<screndy)
                            {
                                *vmem=col;
                                vmem+=vbufPitch;
                                scrstarty++;
                            }
                        }
                    }
                }
                lpix++;
            }
        }
    }
}

/*
=====================
=
= DrawScaleds
=
= Draws all objects that are visable
=
=====================
*/

#define MAXVISABLE 400

typedef struct
{
    short      viewx,
               viewheight,
               shapenum;
    short      flags;          // this must be changed to uint32_t, when you
                               // you need more than 16-flags for drawing
#ifdef USE_DIR3DSPR
    statobj_t *transsprite;
#endif
} visobj_t;

visobj_t vislist[MAXVISABLE];
visobj_t *visptr,*visstep,*farthest;

void DrawScaleds (void)
{
    int      i,least,numvisable,height;
    byte     *visspot;
    uint16_t *tilespot;
    unsigned spotloc;

    statobj_t *statptr;
    objtype   *obj;

    visptr = &vislist[0];

    frameon+=tics;
    frameon%=2520; 
//
// place static objects
//
    for (statptr = &statobjlist[0] ; statptr !=laststatobj ; statptr++)
    {
        if ((visptr->shapenum = statptr->shapenum) == -1)
            continue;                                               // object has been deleted

        if (!*statptr->visspot)
            continue;                                               // not visable

        switch (statptr->shapenum)
        {
            case SPR_STAT_OTORCH1:     
            case SPR_STAT_ITORCH1:    
            case SPR_STAT_FLAME1:                
                visptr->shapenum+=int((frameon/12)%4);                               
                break;
            case SPR_STAT_GOD1:
            case SPR_STAT_TELE1:
            case SPR_STAT_FURY1:
            case SPR_STAT_DEATH1:
            case SPR_STAT_SAVEGAME1:
            case SPR_STAT_NEWLIFE1: 
            case SPR_STAT_REGEN1:
                visptr->shapenum+=int((frameon/10)%4);
                break;    
         } 

        if (TransformTile (statptr->tilex,statptr->tiley,
            &visptr->viewx,&visptr->viewheight) && statptr->flags & FL_BONUS)
        {
            GetBonus (statptr);
            if(statptr->shapenum == -1)
                continue;                                           // object has been taken
        }

		if (statptr->flags & FL_INVISIBLE)
			continue;

        if (!visptr->viewheight)
            continue;                                               // to close to the object

#ifdef USE_DIR3DSPR
        if(statptr->flags & FL_DIR_MASK)
            visptr->transsprite=statptr;
        else
            visptr->transsprite=NULL;
#endif

        if (visptr < &vislist[MAXVISABLE-1])    // don't let it overflow
        {
            visptr->flags = (short) statptr->flags;
            visptr++;
        }
    }

//
// place active objects
//
    for (obj = player->next;obj;obj=obj->next)
    {
        if ((visptr->shapenum = obj->state->shapenum)==0)
            continue;                                               // no shape

        spotloc = (obj->tilex<<mapshift)+obj->tiley;   // optimize: keep in struct?
        visspot = &spotvis[0][0]+spotloc;
        tilespot = &tilemap[0][0]+spotloc;

        //
        // could be in any of the nine surrounding tiles
        //
        if (*visspot
            || ( *(visspot-1) && !*(tilespot-1) )
            || ( *(visspot+1) && !*(tilespot+1) )
            || ( *(visspot-(MAPSIZE+1)) && !*(tilespot-(MAPSIZE+1)) )
            || ( *(visspot-(MAPSIZE)) && !*(tilespot-(MAPSIZE)) )
            || ( *(visspot-(MAPSIZE-1)) && !*(tilespot-(MAPSIZE-1)) )
            || ( *(visspot+(MAPSIZE+1)) && !*(tilespot+(MAPSIZE+1)) )
            || ( *(visspot+(MAPSIZE)) && !*(tilespot+(MAPSIZE)) )
            || ( *(visspot+(MAPSIZE-1)) && !*(tilespot+(MAPSIZE-1)) ) ) 
        {
            obj->active = ac_yes;
            TransformActor (obj);
            if (!obj->viewheight)
                continue;                                               // too close or far away

            visptr->viewx = obj->viewx;
            visptr->viewheight = obj->viewheight;
            if (visptr->shapenum == -1)
                visptr->shapenum = obj->temp1;  // special shape

            if (obj->state->rotate)
                visptr->shapenum += CalcRotate (obj);

            if (obj->state->fullbright ) {
                obj->flags |= FL_FULLBRIGHT;
            } else {
                obj->flags &= ~FL_FULLBRIGHT;
            }

            if (visptr < &vislist[MAXVISABLE-1])    // don't let it overflow
            {
                visptr->flags = (short) obj->flags;
#ifdef USE_DIR3DSPR
                visptr->transsprite = NULL;
#endif
                visptr++;
            }
            obj->flags |= FL_VISABLE;
        }
        else
            obj->flags &= ~FL_VISABLE;
    }

//
// draw from back to front
//
    numvisable = (int) (visptr-&vislist[0]);

    if (!numvisable)
        return;                                                                 // no visable objects

    for (i = 0; i<numvisable; i++)
    {
        least = 32000;
        for (visstep=&vislist[0] ; visstep<visptr ; visstep++)
        {
            height = visstep->viewheight;
            if (height < least)
            {
                least = height;
                farthest = visstep;
            }
        }
        //
        // draw farthest
        //

        if(farthest->transsprite)
            Scale3DShape(vbuf, vbufPitch, farthest->transsprite);
        else
            ScaleShape(farthest->viewx, farthest->shapenum, farthest->viewheight, farthest->flags);

        farthest->viewheight = 32000;
    }
}

//==========================================================================

/*
==============
=
= DrawPlayerWeapon
=
= Draw the player's hands
=
==============
*/

int weaponscale[NUMWEAPONS] = {SPR_KNIFEREADY, SPR_PISTOLREADY,
    SPR_MACHINEGUNREADY, SPR_CHAINREADY, SPR_MK4READY, SPR_RIFLEREADY, SPR_LAUNCHERREADY, SPR_SPEARREADY, NULL};

void DrawPlayerWeapon (void)
{
    int shapenum;

#ifndef SPEAR
    if (gamestate.victoryflag)
    {
#ifndef APOGEE_1_0
        if (player->state == &s_deathcam && (GetTimeCount()&32) )
            SimpleScaleShape(viewwidth/2,SPR_DEATHCAM,viewheight+1);
#endif
        return;
    }
#endif

    if (gamestate.weapon != -1 && gamestate.weapon != wp_medkit)
    {
        shapenum = weaponscale[gamestate.weapon]+gamestate.weaponframe;
        SimpleScaleShape(halfviewwidth,shapenum,viewheight+1+gamestate.weapchange);
    }

    if (demorecord || demoplayback)
        SimpleScaleShape(viewwidth/2,SPR_DEMO,viewheight+1);
}


//==========================================================================


/*
=====================
=
= CalcTics
=
=====================
*/

void CalcTics (void)
{
//
// calculate tics since last refresh for adaptive timing
//
    if (lasttimecount > (int32_t) GetTimeCount())
        lasttimecount = GetTimeCount();    // if the game was paused a LONG time

    uint32_t curtime = SDL_GetTicks();
    tics = (curtime * 7) / 100 - lasttimecount;
    if(!tics)
    {
        // wait until end of current tic
        SDL_Delay(((lasttimecount + 1) * 100) / 7 - curtime);
        tics = 1;
    }

    lasttimecount += tics;

    if (tics>MAXTICS)
        tics = MAXTICS;
}


//==========================================================================

short GetCurPush(int x, int y)
{
   for (int i = 0; i < MAXPWALLS; i++)
       if (pwall[i].x == x && pwall[i].y == y)
            return i;
   return -1;
} 

#define HitHorizBorder() HitHorizWall()
#define HitVertBorder() HitVertWall()

void AsmRefresh()
{
    int32_t xstep,ystep;
    longword xpartial,ypartial;
    boolean playerInPushwallBackTile = tilemap[focaltx][focalty] == BIT_WALL;

    for(pixx=0;pixx<viewwidth;pixx++)
    {
        pw = 0;
        short angl=midangle+pixelangle[pixx];
        if(angl<0) angl+=FINEANGLES;
        if(angl>=3600) angl-=FINEANGLES;
        if(angl<900)
        {
            xtilestep=1;
            ytilestep=-1;
            xstep=finetangent[900-1-angl];
            ystep=-finetangent[angl];
            xpartial=xpartialup;
            ypartial=ypartialdown;
        }
        else if(angl<1800)
        {
            xtilestep=-1;
            ytilestep=-1;
            xstep=-finetangent[angl-900];
            ystep=-finetangent[1800-1-angl];
            xpartial=xpartialdown;
            ypartial=ypartialdown;
        }
        else if(angl<2700)
        {
            xtilestep=-1;
            ytilestep=1;
            xstep=-finetangent[2700-1-angl];
            ystep=finetangent[angl-1800];
            xpartial=xpartialdown;
            ypartial=ypartialup;
        }
        else if(angl<3600)
        {
            xtilestep=1;
            ytilestep=1;
            xstep=finetangent[angl-2700];
            ystep=finetangent[3600-1-angl];
            xpartial=xpartialup;
            ypartial=ypartialup;
        }
        yintercept=FixedMul(ystep,xpartial)+viewy;
        xtile=focaltx+xtilestep;
        xspot=(word)((xtile<<mapshift)+((uint32_t)yintercept>>16));
        xintercept=FixedMul(xstep,ypartial)+viewx;
        ytile=focalty+ytilestep;
        yspot=(word)((((uint32_t)xintercept>>16)<<mapshift)+ytile);
        texdelta=0;

        // Special treatment when player is in back tile of pushwall
        if(playerInPushwallBackTile)
        {
            pw = GetCurPush(focaltx, focalty);

            if(    pwalldir == di_east && xtilestep ==  1
                || pwalldir == di_west && xtilestep == -1)
            {
                int32_t yintbuf = yintercept - ((ystep * (64 - pwallpos)) >> 6);
                if((yintbuf >> 16) == focalty)   // ray hits pushwall back?
                {
                    if(pwalldir == di_east)
                        xintercept = (focaltx << TILESHIFT) + (pwallpos << 10);
                    else
                        xintercept = (focaltx << TILESHIFT) - TILEGLOBAL + ((64 - pwallpos) << 10);
                    yintercept = yintbuf;
                    ytile = (short) (yintercept >> TILESHIFT);
                    tilehit = pwalltile;
                    HitVertWall();
                    continue;
                }
            }
            else if(pwalldir == di_south && ytilestep ==  1
                ||  pwalldir == di_north && ytilestep == -1)
            {
                int32_t xintbuf = xintercept - ((xstep * (64 - pwallpos)) >> 6);
                if((xintbuf >> 16) == focaltx)   // ray hits pushwall back?
                {
                    xintercept = xintbuf;
                    if(pwalldir == di_south)
                        yintercept = (focalty << TILESHIFT) + (pwallpos << 10);
                    else
                        yintercept = (focalty << TILESHIFT) - TILEGLOBAL + ((64 - pwallpos) << 10);
                    xtile = (short) (xintercept >> TILESHIFT);
                    tilehit = pwalltile;
                    HitHorizWall();
                    continue;
                }
            }
        }

        do
        {
            if(ytilestep==-1 && (yintercept>>16)<=ytile) goto horizentry;
            if(ytilestep==1 && (yintercept>>16)>=ytile) goto horizentry;
vertentry:
            if((uint32_t)yintercept>(mapheight<<16)-1 || (word)xtile>=mapwidth)
            {
                if(xtile<0) xintercept=0, xtile=0;
                else if(xtile>=mapwidth) xintercept=mapwidth<<TILESHIFT, xtile=mapwidth-1;
                else xtile=(short) (xintercept >> TILESHIFT);
                if(yintercept<0) yintercept=0, ytile=0;
                else if(yintercept>=(mapheight<<TILESHIFT)) yintercept=mapheight<<TILESHIFT, ytile=mapheight-1;
                yspot=0xffff;
                tilehit=0;
                HitHorizBorder();
                break;
            }
            if(xspot>=maparea) break;
            tilehit=((uint16_t *)tilemap)[xspot];

            if(tilehit)
            {
                if(tilehit & BIT_DOOR)
                {
                    int32_t yintbuf=yintercept+(ystep>>1);
                    if((yintbuf>>16)!=(yintercept>>16))
                        goto passvert;
                    if((word)yintbuf<doorposition[tilehit&LAST_DOORNUM])
                        goto passvert;
                    yintercept=yintbuf;
                    xintercept=(xtile<<TILESHIFT)|0x8000;
                    ytile = (short) (yintercept >> TILESHIFT);
                    HitVertDoor();
                }
                else
                {
                    if(tilehit >= MOVINGIN_W && tilehit <= BIT_WALL)
                    {
                        switch (tilehit) {
                            case MOVINGIN_N:
                                pw = GetCurPush(xspot >> mapshift, (xspot & (MAPSIZE-1)) + 1);
                                break;
                            case MOVINGIN_S:
                                pw = GetCurPush(xspot >> mapshift, (xspot & (MAPSIZE-1)) - 1);
                                break;
                            case MOVINGIN_W:
                                pw = GetCurPush((xspot >> mapshift) + 1, xspot & (MAPSIZE-1));
                                break;
                            case MOVINGIN_E:
                                pw = GetCurPush((xspot >> mapshift) - 1, xspot & (MAPSIZE-1));
                                break;
                            case BIT_WALL:
                                pw = GetCurPush(xspot >> mapshift, xspot & (MAPSIZE-1));
                                break;
                        } 

                        if(pwalldir==di_west || pwalldir==di_east)
                        {
	                        int32_t yintbuf;
                            int pwallposnorm;
                            int pwallposinv;
                            if(pwalldir==di_west)
                            {
                                pwallposnorm = 64-pwallpos;
                                pwallposinv = pwallpos;
                            }
                            else
                            {
                                pwallposnorm = pwallpos;
                                pwallposinv = 64-pwallpos;
                            }
                            if(pwalldir == di_east && xtile==pwallx && ((uint32_t)yintercept>>16)==pwally
                                || pwalldir == di_west && !(xtile==pwallx && ((uint32_t)yintercept>>16)==pwally))
                            {
                                yintbuf=yintercept+((ystep*pwallposnorm)>>6);
                                if((yintbuf>>16)!=(yintercept>>16))
                                    goto passvert;

                                xintercept=(xtile<<TILESHIFT)+TILEGLOBAL-(pwallposinv<<10);
                                yintercept=yintbuf;
                                ytile = (short) (yintercept >> TILESHIFT);
                                tilehit=pwalltile;
                                HitVertWall();
                            }
                            else
                            {
                                yintbuf=yintercept+((ystep*pwallposinv)>>6);
                                if((yintbuf>>16)!=(yintercept>>16))
                                    goto passvert;

                                xintercept=(xtile<<TILESHIFT)-(pwallposinv<<10);
                                yintercept=yintbuf;
                                ytile = (short) (yintercept >> TILESHIFT);
                                tilehit=pwalltile;
                                HitVertWall();
                            }
                        }
                        else
                        {
                            int pwallposi = pwallpos;
                            if(pwalldir==di_north) pwallposi = 64-pwallpos;
                            if(pwalldir==di_south && (word)yintercept<(pwallposi<<10)
                                || pwalldir==di_north && (word)yintercept>(pwallposi<<10))
                            {
                                if(((uint32_t)yintercept>>16)==pwally && xtile==pwallx)
                                {
                                    if(pwalldir==di_south && (int32_t)((word)yintercept)+ystep<(pwallposi<<10)
                                            || pwalldir==di_north && (int32_t)((word)yintercept)+ystep>(pwallposi<<10))
                                        goto passvert;

                                    if(pwalldir==di_south)
                                        yintercept=(yintercept&0xffff0000)+(pwallposi<<10);
                                    else
                                        yintercept=(yintercept&0xffff0000)-TILEGLOBAL+(pwallposi<<10);
                                    xintercept=xintercept-((xstep*(64-pwallpos))>>6);
                                    xtile = (short) (xintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitHorizWall();
                                }
                                else
                                {
                                    texdelta = -(pwallposi<<10);
                                    xintercept=xtile<<TILESHIFT;
                                    ytile = (short) (yintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitVertWall();
                                }
                            }
                            else
                            {
                                if(((uint32_t)yintercept>>16)==pwally && xtile==pwallx)
                                {
                                    texdelta = -(pwallposi<<10);
                                    xintercept=xtile<<TILESHIFT;
                                    ytile = (short) (yintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitVertWall();
                                }
                                else
                                {
                                    if(pwalldir==di_south && (int32_t)((word)yintercept)+ystep>(pwallposi<<10)
                                            || pwalldir==di_north && (int32_t)((word)yintercept)+ystep<(pwallposi<<10))
                                        goto passvert;

                                    if(pwalldir==di_south)
                                        yintercept=(yintercept&0xffff0000)-((64-pwallpos)<<10);
                                    else
                                        yintercept=(yintercept&0xffff0000)+((64-pwallpos)<<10);
                                    xintercept=xintercept-((xstep*pwallpos)>>6);
                                    xtile = (short) (xintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitHorizWall();
                                }
                            }
                        }
                    }
                    else
                    {
                        xintercept=xtile<<TILESHIFT;
                        ytile = (short) (yintercept >> TILESHIFT);
                        if ( (abs(ytile-player->tiley) < MAXMAPSIGHT) && abs(xtile-player->tilex) < MAXMAPSIGHT )
                            *((byte *)spotsaw+xspot)= 1;
                        HitVertWall();
                    }
                }
                break;
            }
passvert:
            *((byte *)spotvis+xspot)=1;
			
            if ( (abs((xspot >> mapshift)-player->tilex) < MAXMAPSIGHT) && (abs((xspot & (MAPSIZE-1))-player->tiley) < MAXMAPSIGHT ) )
                *((byte *)spotsaw+xspot)= 1;

            xtile+=xtilestep;
            yintercept+=ystep;
            xspot=(word)((xtile<<mapshift)+((uint32_t)yintercept>>16));
        }
        while(1);
        continue;

        do
        {
            if(xtilestep==-1 && (xintercept>>16)<=xtile) goto vertentry;
            if(xtilestep==1 && (xintercept>>16)>=xtile) goto vertentry;
horizentry:
            if((uint32_t)xintercept>(mapwidth<<16)-1 || (word)ytile>=mapheight)
            {
                if(ytile<0) yintercept=0, ytile=0;
                else if(ytile>=mapheight) yintercept=mapheight<<TILESHIFT, ytile=mapheight-1;
                else ytile=(short) (yintercept >> TILESHIFT);
                if(xintercept<0) xintercept=0, xtile=0;
                else if(xintercept>=(mapwidth<<TILESHIFT)) xintercept=mapwidth<<TILESHIFT, xtile=mapwidth-1;
                xspot=0xffff;
                tilehit=0;
                HitVertBorder();
                break;
            }
            if(yspot>=maparea) break;
            tilehit=((uint16_t *)tilemap)[yspot];
                
            if(tilehit)
            {
                if(tilehit & BIT_DOOR)
                {
                    int32_t xintbuf=xintercept+(xstep>>1);
                    if((xintbuf>>16)!=(xintercept>>16))
                        goto passhoriz;
                    if((word)xintbuf<doorposition[tilehit&LAST_DOORNUM])
                        goto passhoriz;
                    xintercept=xintbuf;
                    yintercept=(ytile<<TILESHIFT)+0x8000;
                    xtile = (short) (xintercept >> TILESHIFT);
                    HitHorizDoor();
                }
                else
                {
                    if(tilehit >= MOVINGIN_W && tilehit <= BIT_WALL)
                    {
                        switch (tilehit) {
                            case MOVINGIN_N:
                                pw = GetCurPush(yspot >> mapshift, (yspot & (MAPSIZE-1)) + 1);
                                break;
                            case MOVINGIN_S:
                                pw = GetCurPush(yspot >> mapshift, (yspot & (MAPSIZE-1)) - 1);
                                break;
                            case MOVINGIN_W:
                                pw = GetCurPush((yspot >> mapshift) + 1, yspot & (MAPSIZE-1));
                                break;
                            case MOVINGIN_E:
                                pw = GetCurPush((yspot >> mapshift) - 1, yspot & (MAPSIZE-1));
                                break;
                            case BIT_WALL:
                                pw = GetCurPush(yspot >> mapshift, yspot & (MAPSIZE-1));
                                break;
                        } 

                        if(pwalldir==di_north || pwalldir==di_south)
                        {
                            int32_t xintbuf;
                            int pwallposnorm;
                            int pwallposinv;
                            if(pwalldir==di_north)
                            {
                                pwallposnorm = 64-pwallpos;
                                pwallposinv = pwallpos;
                            }
                            else
                            {
                                pwallposnorm = pwallpos;
                                pwallposinv = 64-pwallpos;
                            }
                            if(pwalldir == di_south && ytile==pwally && ((uint32_t)xintercept>>16)==pwallx
                                || pwalldir == di_north && !(ytile==pwally && ((uint32_t)xintercept>>16)==pwallx))
                            {
                                xintbuf=xintercept+((xstep*pwallposnorm)>>6);
                                if((xintbuf>>16)!=(xintercept>>16))
                                    goto passhoriz;

                                yintercept=(ytile<<TILESHIFT)+TILEGLOBAL-(pwallposinv<<10);
                                xintercept=xintbuf;
                                xtile = (short) (xintercept >> TILESHIFT);
                                tilehit=pwalltile;
                                HitHorizWall();
                            }
                            else
                            {
                                xintbuf=xintercept+((xstep*pwallposinv)>>6);
                                if((xintbuf>>16)!=(xintercept>>16))
                                    goto passhoriz;

                                yintercept=(ytile<<TILESHIFT)-(pwallposinv<<10);
                                xintercept=xintbuf;
                                xtile = (short) (xintercept >> TILESHIFT);
                                tilehit=pwalltile;
                                HitHorizWall();
                            }
                        }
                        else
                        {
                            int pwallposi = pwallpos;
                            if(pwalldir==di_west) pwallposi = 64-pwallpos;
                            if(pwalldir==di_east && (word)xintercept<(pwallposi<<10)
                                    || pwalldir==di_west && (word)xintercept>(pwallposi<<10))
                            {
                                if(((uint32_t)xintercept>>16)==pwallx && ytile==pwally)
                                {
                                    if(pwalldir==di_east && (int32_t)((word)xintercept)+xstep<(pwallposi<<10)
                                            || pwalldir==di_west && (int32_t)((word)xintercept)+xstep>(pwallposi<<10))
                                        goto passhoriz;

                                    if(pwalldir==di_east)
                                        xintercept=(xintercept&0xffff0000)+(pwallposi<<10);
                                    else
                                        xintercept=(xintercept&0xffff0000)-TILEGLOBAL+(pwallposi<<10);
                                    yintercept=yintercept-((ystep*(64-pwallpos))>>6);
                                    ytile = (short) (yintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitVertWall();
                                }
                                else
                                {
                                    texdelta = -(pwallposi<<10);
                                    yintercept=ytile<<TILESHIFT;
                                    xtile = (short) (xintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitHorizWall();
                                }
                            }
                            else
                            {
                                if(((uint32_t)xintercept>>16)==pwallx && ytile==pwally)
                                {
                                    texdelta = -(pwallposi<<10);
                                    yintercept=ytile<<TILESHIFT;
                                    xtile = (short) (xintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitHorizWall();
                                }
                                else
                                {
                                    if(pwalldir==di_east && (int32_t)((word)xintercept)+xstep>(pwallposi<<10)
                                            || pwalldir==di_west && (int32_t)((word)xintercept)+xstep<(pwallposi<<10))
                                        goto passhoriz;

                                    if(pwalldir==di_east)
                                        xintercept=(xintercept&0xffff0000)-((64-pwallpos)<<10);
                                    else
                                        xintercept=(xintercept&0xffff0000)+((64-pwallpos)<<10);
                                    yintercept=yintercept-((ystep*pwallpos)>>6);
                                    ytile = (short) (yintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitVertWall();
                                }
                            }
                        }
                    }
                    else
                    {
                        yintercept=ytile<<TILESHIFT;
                        xtile = (short) (xintercept >> TILESHIFT);
                        if ( (abs(ytile-player->tiley) < MAXMAPSIGHT) && abs(xtile-player->tilex) < MAXMAPSIGHT )
                            *((byte *)spotsaw+yspot)= 1;
                        HitHorizWall();
                    }
                }
                break;
            }
passhoriz:
            *((byte *)spotvis+yspot)=1;

            if ( (abs((yspot >> mapshift)-player->tilex) < MAXMAPSIGHT) && (abs((yspot & (MAPSIZE - 1))-player->tiley) < MAXMAPSIGHT) )
                *((byte *)spotsaw+yspot)= 1;

            ytile+=ytilestep;
            xintercept+=xstep;
            yspot=(word)((((uint32_t)xintercept>>16)<<mapshift)+ytile);
        }
        while(1);
    }
}

/*
====================
=
= WallRefresh
=
====================
*/

void WallRefresh (void)
{
    pw = 0; // multiple pushwalls
    xpartialdown = viewx&(TILEGLOBAL-1);
    xpartialup = TILEGLOBAL-xpartialdown;
    ypartialdown = viewy&(TILEGLOBAL-1);
    ypartialup = TILEGLOBAL-ypartialdown;

    min_wallheight = viewheight;
    lastside = -1;                  // the first pixel is on a new wall
    AsmRefresh ();
    ScalePost ();                   // no more optimization on last post
}

void CalcViewVariables()
{
    viewangle = player->angle;
    midangle = viewangle*(FINEANGLES/ANGLES); // a finer
    // scale: [0,1] -> [0,65536] (scaled to game world)
    viewsin = sintable[viewangle];
    viewcos = costable[viewangle];
    // offsets the camera view based on cos/sin so the player does not facemash the walls
    viewx = player->x - FixedMul(focallength,viewcos);
    viewy = player->y + FixedMul(focallength,viewsin);

    // the tilex and tiley of the player eye (offset by the above)
    focaltx = (short)(viewx>>TILESHIFT);
    focalty = (short)(viewy>>TILESHIFT);

    viewtx = (short)(player->x >> TILESHIFT);
    viewty = (short)(player->y >> TILESHIFT);
}

//==========================================================================

/*
========================
=
= ThreeDRefresh
=
========================
*/

void    ThreeDRefresh (void)
{
//
// clear out the traced array
//
    memset(spotvis,0,maparea);
    spotvis[player->tilex][player->tiley] = 1;       // Detect all sprites over player fix
#ifdef REVEALMAP
    spotsaw[player->tilex][player->tiley] = 1;
#endif

    vbuf = VL_LockSurface(screenBuffer);
    vbuf+=screenofs;
    vbufPitch = bufferPitch;

    CalcViewVariables();

//
// follow the walls from there to the right, drawing as we go
//
    VGAClearScreen ();
#if defined(USE_FEATUREFLAGS) && defined(USE_STARSKY)
    if((levelinfo.weather & FF_STARSKY) && !levelinfo.parallax &&switches.weather)
        DrawStarSky(vbuf, vbufPitch);
#endif

    WallRefresh ();

#if defined(USE_FEATUREFLAGS) && defined(USE_PARALLAX)
    if(levelinfo.parallax && switches.weather)
        DrawParallax(vbuf, vbufPitch);
#endif
#if defined(USE_FEATUREFLAGS) && defined(USE_CLOUDSKY)
    if((levelinfo.weather & FF_CLOUDSKY) && switches.clouds)
        DrawClouds(vbuf, vbufPitch, min_wallheight);
#endif
#ifdef USE_FLOORCEILINGTEX
    if (switches.textures)
        DrawFloorAndCeiling(vbuf, vbufPitch, min_wallheight);
#endif

//
// draw all the scaled images
//
    DrawScaleds();                  // draw scaled stuff

#if defined(USE_FEATUREFLAGS) && defined(USE_RAIN)
    if((levelinfo.weather & FF_RAIN) && switches.weather)
        DrawRain(vbuf, vbufPitch);
#endif

#if defined(USE_FEATUREFLAGS) && defined(USE_SNOW)
    if((levelinfo.weather & FF_SNOW) && switches.weather)
        DrawSnow(vbuf, vbufPitch);
#endif

    DrawPlayerWeapon ();    // draw player's hands

    if (messagetime > 0 && switches.messages) // WSJ's message feature
        DrawMessage (); 

    if (gamestate.activetimers && switches.messages) {
        DrawTimer();
    }

    if (ratiocheat && ingame) {
        DrawRatios();
    }

    if(viewsize == 21 && gamestate.health > 0)
        DrawFullscreenHUD();

    VL_UnlockSurface(screenBuffer);
    vbuf = NULL;

//
// show screen and time last cycle
//

    if (fizzlein) // TODO, not set timecount on teleport?
    {
        FizzleFade(screenBuffer, 0, 0,
            screenWidth, screenHeight, 20, false);
        fizzlein = false;

        lasttimecount = GetTimeCount();          // don't make a big tic count
    }
    else
    {
#ifndef REMDEBUG
        if (fpscounter)
        {
            fontnumber = 0;
            SETFONTCOLOR(0x0,0x0);
            SETPRINTXY(msgPrintX+1,msgPrintY + 11);
            US_PrintSigned(fps);
            US_Print(" fps");

            SETFONTCOLOR(0x10,0x0);
            SETPRINTXY(msgPrintX,msgPrintY + 10);
            US_PrintSigned(fps);
            US_Print(" fps");
        }
#endif
        SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
        SDL_Flip(screen);
    }

#ifndef REMDEBUG
    if (fpscounter)
    {
        fps_frames++;
        fps_time+=tics;

        if(fps_time>35)
        {
            fps_time-=35;
            fps=fps_frames<<1;
            fps_frames=0;
        }
    }
#endif
}

//
// In-game messages | WSJ's routine
//
/*
========================
=
= GetMessage
=
= gets ingame messages
=
========================
*/

void GetMessage (char *lastmessage, int color)
{
   messagetime = 150; // time for message to display
   messagecolor = color;
   strcpy(gamestate.message, lastmessage);
}   

/*
========================
=
= DrawMessage
=
= displays ingame messages
=
========================
*/

void DrawMessage (void)
{

   fontnumber = 0;
   // shadow
   SETFONTCOLOR(0x0,0x0);
   SETPRINTXY(msgPrintX + 1, msgPrintY + 1);
   US_Print(gamestate.message);

   SETFONTCOLOR(messagecolor,0x0);
   SETPRINTXY(msgPrintX, msgPrintY);
   US_Print(gamestate.message);

   if (messagetime <= 0)
        DrawPlayBorderSides();
   else
        messagetime-=tics;
} 

/*
========================
=
= GetTimer
=
= gets ingame messages
=
========================
*/

void GetTimer (int seconds, int eventtype)
{
    if (gamestate.activetimers > MAXTIMERS) return;

    // Find if a timer for the event exists
    for (int t = 0; t < gamestate.activetimers; t++) {
        if (gamestate.timers[t].type == eventtype) {
            gamestate.timers[t].tics += seconds * 70;
            return;
        }
    }

    // Timer doesn't already exist for event
    gamestate.timers[gamestate.activetimers].tics = seconds * 70;       // increment the correct timer
    gamestate.timers[gamestate.activetimers].type = eventtype;

    switch (eventtype) { // set the color based on event type
        case T_LEVEL:
            gamestate.timers[gamestate.activetimers].color = LVL_TMR_CLR;
            lastsec = (gamestate.timers[gamestate.activetimers].tics / 70) % 60;
            break;
        case T_GOD:
            gamestate.timers[gamestate.activetimers].color = GOD_TMR_CLR;
            break;
        case T_FURY:
            gamestate.timers[gamestate.activetimers].color = FUR_TMR_CLR;
            break;
    }
    gamestate.activetimers++;                   // increase the number of active timers
}  

/*
========================
=
= DrawMessage
=
= displays ingame messages
=
========================
*/
int lastsec = -1;

void DrawTimer (void)
{
   for (int x = 0; x < gamestate.activetimers; x++) {
       // Calculate timer values (tics -> min/sec)
       if (gamestate.timers[x].tics <= 0) { // a timer is done
           gamestate.activetimers--;

            for (int t = x; t < gamestate.activetimers; t++) { // shift the relatively new timers up
                gamestate.timers[t].tics = gamestate.timers[t+1].tics;
                gamestate.timers[t].color = gamestate.timers[t+1].color;
                gamestate.timers[t].type = gamestate.timers[t+1].type;
            }
       }

       if (gamestate.timers[x].tics <= 0) return;

       int sec = gamestate.timers[x].tics / 70;
       int min = sec / 60;
       int yOffset = (x << 3) + x;
       sec %= 60;

       // Give 1000 pts/sec for timed levels (level timer always in first spot if it is set)
       if (x == 0 && (levelinfo.timerMin > 0 || levelinfo.timerSec > 0) && lastsec != sec) {
           GivePoints(1000);
           lastsec = sec;
       }
       
       // Draw timer shadow
       fontnumber = 0;
       SETFONTCOLOR(0x0,0x0); // set the color{
       SETPRINTXY(timerPrintX+1,timerPrintY + yOffset + 1);
       US_PrintUnsignedTimer(min); US_Print(":"); US_PrintUnsignedTimer(sec); // print message

       // Draw timer
       SETFONTCOLOR(gamestate.timers[x].color,0x0); // set the color
       SETPRINTXY(timerPrintX,timerPrintY + yOffset);
       US_PrintUnsignedTimer(min); US_Print(":"); US_PrintUnsignedTimer(sec);  // print message

       if (gamestate.timers[x].tics > 0)
            gamestate.timers[x].tics -= tics;
   }
} 

void DrawRatios (void) {
    char ratiobuf[32];
    fontnumber = 0;

    sprintf(ratiobuf, "T: %lu / %lu", gamestate.treasurecount, gamestate.treasuretotal);
    SETFONTCOLOR(0x0,0x0);
    SETPRINTXY(ratioPrintX + 1, ratioPrintY + 1);
    US_Print(ratiobuf);
    SETFONTCOLOR(0x10,0x0);
    SETPRINTXY(ratioPrintX, ratioPrintY);
    US_Print(ratiobuf);

    sprintf(ratiobuf, "S: %lu / %lu", gamestate.secretcount, gamestate.secrettotal);
    SETFONTCOLOR(0x0,0x0);
    SETPRINTXY(ratioPrintX + 1, ratioPrintY - 9);
    US_Print(ratiobuf);
    SETFONTCOLOR(0x10,0x0);
    SETPRINTXY(ratioPrintX, ratioPrintY - 10);
    US_Print(ratiobuf);

    sprintf(ratiobuf, "K: %lu / %lu", gamestate.killcount, gamestate.killtotal);
    SETFONTCOLOR(0x0,0x0);
    SETPRINTXY(ratioPrintX + 1, ratioPrintY - 19);
    US_Print(ratiobuf);
    SETFONTCOLOR(0x10,0x0);
    SETPRINTXY(ratioPrintX, ratioPrintY - 20);
    US_Print(ratiobuf);
}