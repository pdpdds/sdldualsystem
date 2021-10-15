// WL_GAME.C

#include <math.h>
#include "wl_def.h"
#include "wl_shade.h"
#include <SDL_mixer.h>

#ifdef VARIABLEWEATHER
#include "wl_atmos.h"
#endif

#pragma hdrstop

#ifdef MYPROFILE
#include <TIME.H>
#endif

/*
=============================================================================

                             LOCAL CONSTANTS

=============================================================================
*/


/*
=============================================================================

                             GLOBAL VARIABLES

=============================================================================
*/

boolean         ingame,fizzlein,died;
gametype        gamestate;
levelinfo_t     levelinfo;
byte            bordercol=VIEWCOLOR;        // color of the Change View/Ingame border

#ifdef SPEAR
int32_t         spearx,speary;
unsigned        spearangle;
boolean         spearflag;
#endif

// EoD tile stuff
int warpex,warpey,warpwx,warpwy; // teleporter stuff

void ResetGamestate (void);
void SetupGameLevel (void);
void DrawPlayScreen (void);
void LoadLatchMem (void);
void GameLoop (void);

/*
=============================================================================

                             LOCAL VARIABLES

=============================================================================
*/



//===========================================================================
//===========================================================================


/*
==========================
=
= SetSoundLoc - Given the location of an object (in terms of global
=       coordinates, held in globalsoundx and globalsoundy), munges the values
=       for an approximate distance from the left and right ear, and puts
=       those values into leftchannel and rightchannel.
=
= JAB
=
==========================
*/

int leftchannel, rightchannel;
#define ATABLEMAX 15
byte righttable[ATABLEMAX][ATABLEMAX * 2] = {
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 6, 0, 0, 0, 0, 0, 1, 3, 5, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 6, 4, 0, 0, 0, 0, 0, 2, 4, 6, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 4, 1, 0, 0, 0, 1, 2, 4, 6, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 6, 5, 4, 2, 1, 0, 1, 2, 3, 5, 7, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 5, 4, 3, 2, 2, 3, 3, 5, 6, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 5, 4, 4, 4, 4, 5, 6, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 5, 5, 5, 6, 6, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}
};
byte lefttable[ATABLEMAX][ATABLEMAX * 2] = {
{ 8, 8, 8, 8, 8, 8, 8, 8, 5, 3, 1, 0, 0, 0, 0, 0, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 6, 4, 2, 0, 0, 0, 0, 0, 4, 6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 6, 4, 2, 1, 0, 0, 0, 1, 4, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 7, 5, 3, 2, 1, 0, 1, 2, 4, 5, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 6, 5, 3, 3, 2, 2, 3, 4, 5, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 6, 5, 4, 4, 4, 4, 5, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 6, 6, 5, 5, 5, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}
};

void
SetSoundLoc(fixed gx,fixed gy)
{
    fixed   xt,yt;
    int     x,y;

//
// translate point to view centered coordinates
//
    gx -= viewx;
    gy -= viewy;

//
// calculate newx
//
    xt = FixedMul(gx,viewcos);
    yt = FixedMul(gy,viewsin);
    x = (xt - yt) >> TILESHIFT;

//
// calculate newy
//
    xt = FixedMul(gx,viewsin);
    yt = FixedMul(gy,viewcos);
    y = (yt + xt) >> TILESHIFT;

    if (y >= ATABLEMAX)
        y = ATABLEMAX - 1;
    else if (y <= -ATABLEMAX)
        y = -ATABLEMAX;
    if (x < 0)
        x = -x;
    if (x >= ATABLEMAX)
        x = ATABLEMAX - 1;
    leftchannel  =  lefttable[x][y + ATABLEMAX];
    rightchannel = righttable[x][y + ATABLEMAX];

#if 0
    CenterWindow(8,1);
    US_PrintSigned(leftchannel);
    US_Print(",");
    US_PrintSigned(rightchannel);
    VW_UpdateScreen();
#endif
}

/*
==========================
=
= SetSoundLocGlobal - Sets up globalsoundx & globalsoundy and then calls
=       UpdateSoundLoc() to transform that into relative channel volumes. Those
=       values are then passed to the Sound Manager so that they'll be used for
=       the next sound played (if possible).
=
= JAB
=
==========================
*/
void PlaySoundLocGlobal(word s,fixed gx,fixed gy)
{
    SetSoundLoc(gx, gy);
    SD_PositionSound(leftchannel, rightchannel);

    int channel = SD_PlaySound((soundnames) s);
    if(channel)
    {
        channelSoundPos[channel - 1].globalsoundx = gx;
        channelSoundPos[channel - 1].globalsoundy = gy;
        channelSoundPos[channel - 1].valid = 1;
    }
}

void UpdateSoundLoc(void)
{
/*    if (SoundPositioned)
    {
        SetSoundLoc(globalsoundx,globalsoundy);
        SD_SetPosition(leftchannel,rightchannel);
    }*/

    for(int i = 0; i < MIX_CHANNELS; i++)
    {
        if(channelSoundPos[i].valid)
        {
            SetSoundLoc(channelSoundPos[i].globalsoundx,
                channelSoundPos[i].globalsoundy);
            SD_SetPosition(i, leftchannel, rightchannel);
        }
    }
}

/*
**      JAB End
*/

/*
==========================
=
= ScanInfoPlane
=
= Spawn all actors and mark down special places
=
==========================
*/

static void ScanInfoPlane(void)
{
    unsigned x,y;
    int      tile;
    word     *start;

    start = mapsegs[1];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *start++;
            if (!tile)
                continue;

            switch (tile)
            {
                case 19:
                case 20:
                case 21:
                case 22:
                    SpawnPlayer(x,y,NORTH+tile-19);
                    break;

                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
                case 30:

                case 31:
                case 32:
                case 33:
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:

                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:

                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:

                case 55:
                case 56:
                case 57:
                case 58:
                case 59:
                case 60:
                case 61:
                case 62:

                case 63:
                case 64:
                case 65:
                case 66:
                case 67:
                case 68:
                case 69:
                case 70:
                case 71:
                case 72:
                case 73:                        // TRUCK AND SPEAR!
                case 74:
                case 75:                        // MK4 cannon
                case 76:
                case 77:
                case 78:
                case 79:
                case 80:
                case 81:
                case 82:
                case 83:
                case 84:
                case 85:
                case 86:
                case 87:
                case 88:
                case 89:
                    SpawnStatic(x,y,tile-23);
                    break;

                case 270:
                case 271:
                case 272:
                case 273:
                case 274:
                case 275:
                case 276:
                case 277:
                case 278:
                case 279:
                case 280:
                case 281:
                case 282:
                case 283:
                case 284:
                case 285:
                case 286:
                case 287:
                case 288:
                case 289:
                case 290:
                case 291:
                case 292:
                case 293:
                case 294:
                case 295:
                case 296:
                case 297:
                case 298:
                case 299:
                case 300:
                case 301:
                case 302:
                case 303:
                case 304:
                case 305:
                case 306:
                case 307:
                case 308:
                case 309:
                case 310:
                case 311:
                case 312:
                case 313:
                // case 314: save gave
                case 315:
                case 316:
                case 317:
                case 318:
                case 319:
                case 320:
                case 321:
                case 322:
                case 323:
                case 324:
                case 325:
                case 326:
                case 327:
                case 328:
                case 329:
                case 330:
                case 331:
                case 332:
                case 333:
                case 334:
                case 335:
                case 336:
                case 337:
                case 338:
                case 339:
                case 340:
                case 341:
                case 342:
                case 343:
                case 344:
                case 345:
                case 346:
                case 347:
                case 348:
                case 349:
                case 350:
                case 351:
                case 352:
                case 353:
                case 354:
                    SpawnStatic(x,y,tile-201);
                    break;

                case 314:
                    if (gamestate.difficulty == gd_extreme)
                        SpawnStatic(x,y,tile-201);
                    break;

//
// WALL SWITCH STUFF
//
                /*
                 * Default state for firewalls: active = false; tics = 0; tilex = -1; tiley = -1;
                 */
                case 400: // Fireball 1
                case 401: // Fireball 2
                case 402: // Fireball 3
                case 403: // Fireball 4
                case 404: // Fireball 5
                case 405: // Fireball 6
                case 406: // Fireball 7
                case 407: // Fireball 8
                    fwall[tile%MAXFIREWALLS].tics = FIREWALLDELAY;
                    fwall[tile%MAXFIREWALLS].tilex = x; // set the x coord of the shooter
                    fwall[tile%MAXFIREWALLS].tiley = y; // set the y coord of the shooter
                    break; 
                case 408: // Fireball 1 switch overlay
                case 409: // Fireball 2 switch overlay
                case 410: // Fireball 3 switch overlay
                case 411: // Fireball 4 switch overlay
                case 412: // Fireball 5 switch overlay
                case 413: // Fireball 6 switch overlay
                case 414: // Fireball 7 switch overlay
                case 415: // Fireball 8 switch overlay
                    fwall[tile%MAXFIREWALLS].active = (MAPSPOT(x,y,0) == SWITCHONTILE) ? true : false; // set active based on switch state
                    break; 

                /* 420-435 are the pushwall direction markers */
                    
//
// P wall
//
                case SECRETTILE:
					SpawnStatic(x,y,154);
					break;
                case SUPERSECRETTILE:
					SpawnStatic(x,y,155);
                    break;

//
// Spawn Triggers
//
				case SPN_TRG_GRD_1:
					SpawnStatic(x,y,156);
					break;
				case SPN_TRG_GRD_2:
					SpawnStatic(x,y,157);
					break;
				case SPN_TRG_SS_1:
					SpawnStatic(x,y,158);
					break;
				case SPN_TRG_SS_2:
					SpawnStatic(x,y,159);
					break;
				case SPN_TRG_OFC_1:
					SpawnStatic(x,y,160);
					break;
				case SPN_TRG_OFC_2:
					SpawnStatic(x,y,161);
					break;

//
// Teleporters
//
                case 100:
                    SpawnStatic(x,y,67);
                    break;
                case 101:
                    warpex = x;
                    warpey = y;
                    break;
                case 102:
                    SpawnStatic(x,y,68);
                    break;
                case 103:
                    warpwx = x;
                    warpwy = y;
                    break;


//
// guard
//
                case 180:
                case 181:
                case 182:
                case 183:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 144:
                case 145:
                case 146:
                case 147:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 108:
                case 109:
                case 110:
                case 111:
                    SpawnStand(en_guard,x,y,tile-108);
                    break;


                case 184:
                case 185:
                case 186:
                case 187:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 148:
                case 149:
                case 150:
                case 151:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 112:
                case 113:
                case 114:
                case 115:
                    SpawnPatrol(en_guard,x,y,tile-112);
                    break;

                case 124:
                    SpawnDeadGuy (en_guard,x,y);
                    break;
//
// officer
//
                case 188:
                case 189:
                case 190:
                case 191:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 152:
                case 153:
                case 154:
                case 155:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 116:
                case 117:
                case 118:
                case 119:
                    SpawnStand(en_officer,x,y,tile-116);
                    break;


                case 192:
                case 193:
                case 194:
                case 195:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 156:
                case 157:
                case 158:
                case 159:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 120:
                case 121:
                case 122:
                case 123:
                    SpawnPatrol(en_officer,x,y,tile-120);
                    break;

                case 231:
                    SpawnDeadGuy(en_officer,x,y);
                    break;


//
// ss
//
                case 198:
                case 199:
                case 200:
                case 201:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 162:
                case 163:
                case 164:
                case 165:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 126:
                case 127:
                case 128:
                case 129:
                    SpawnStand(en_ss,x,y,tile-126);
                    break;


                case 202:
                case 203:
                case 204:
                case 205:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 166:
                case 167:
                case 168:
                case 169:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 130:
                case 131:
                case 132:
                case 133:
                    SpawnPatrol(en_ss,x,y,tile-130);
                    break;

                case 229:
                    SpawnDeadGuy(en_ss,x,y);
                    break;

//
// suicide clones
//
             /*   case 206:
                case 207:
                case 208:
                case 209:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 170:
                case 171:
                case 172:
                case 173:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;*/
                case 134:
                case 135:
                case 136:
                case 137:
                    SpawnStand(en_suicide,x,y,tile-134);
                    break;


                case 210:
                case 211:
                case 212:
                case 213:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 174:
                case 175:
                case 176:
                case 177:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 138:
                case 139:
                case 140:
                case 141:
                    SpawnPatrol(en_suicide,x,y,tile-138);
                    break;

                case 228:
                    SpawnDeadGuy(en_suicide,x,y);
                    break;

//
// boss
//
#ifndef SPEAR
                case 214:
                    SpawnBoss (x,y);
                    break;
                case 197:
                    SpawnGretel (x,y);
                    break;
                case 215:
                    SpawnGift (x,y);
                    break;
                case 179:
                    SpawnFat (x,y);
                    break;
                case 196:
                    SpawnSchabbs (x,y);
                    break;
                case 160:
                    SpawnFakeHitler (x,y);
                    break;
                case 178:
                    SpawnHitler (x,y);
                    break;
#else
                case 104:
                case 260:
                    SpawnBarrel(x,y);
                    break;
                case 105:
                    SpawnSpectre(x,y);
                    break;
                case 224:
                    SpawnSpectre2(x,y);
                    break;
                case 106:
                    SpawnTurret (x,y);
                    break;
              /*  case 107:
                    SpawnAngel (x,y);
                    break;*/
                case 125:
                    SpawnGrau (x,y);
                    break;
                case 142:
                    SpawnSuperClone (x,y);
                    break;
                case 143:
                    SpawnOtto (x,y);
                    break;
             /*   case 161:
                    SpawnBaron (x,y);
                    break;*/
                case 178: // new
                    SpawnRobot(x,y);
                    break;
                case 226:
                    SpawnBlinkingLight(x,y);
                    break;
                case 262:
                    SpawnSnowman(x,y);
                    break;
                case 263:
                    SpawnColumn(x,y);
                    break;
                case 264: 
                    SpawnUltraClone(x,y);
                    break;
                case 265:
                    SpawnHeinrich(x,y);
                    break;
                case 267:
                    SpawnStatic(x,y,12); // the toilet
                    SpawnToiletClone(x,y);
                    break;


#endif

//
// mutants
//
                case 252:
                case 253:
                case 254:
                case 255:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 18;
                case 234:
                case 235:
                case 236:
                case 237:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 18;
                case 216:
                case 217:
                case 218:
                case 219:
                    SpawnStand(en_mutant,x,y,tile-216);
                    break;

                case 256:
                case 257:
                case 258:
                case 259:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 18;
                case 238:
                case 239:
                case 240:
                case 241:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 18;
                case 220:
                case 221:
                case 222:
                case 223:
                    SpawnPatrol(en_mutant,x,y,tile-220);
                    break;

//
// ghosts
//
#ifndef SPEAR
                case 224:
                    SpawnGhosts (en_blinky,x,y);
                    break;
                case 225:
                    SpawnGhosts (en_clyde,x,y);
                    break;
                case 226:
                    SpawnGhosts (en_pinky,x,y);
                    break;
                case 227:
                    SpawnGhosts (en_inky,x,y);
                    break;
#endif
            }
        }
    }
}

void SetLevelWeapons (boolean reset) {
    if (gamestate.changingweapon) {
        gamestate.weapchange = 0;
        gamestate.changingweapon = gamestate.goingdown = false;
    }

    if (levelinfo.backtolevel > 0 && levelinfo.parSec > 0) { // weapons modifier for secret levels
        gamestate.primaryammo = gamestate.secondaryammo = gamestate.rifleammo = gamestate.rocketammo = 0;
        gamestate.weapons = WP_KNIFE;
        gamestate.medkits = 0;

        if (levelinfo.parSec == 8) {
            gamestate.weapons = WP_SPEAR;
            gamestate.weapon = wp_spear;
        } else {
            if (levelinfo.parSec > 7) levelinfo.parSec = 7;

            for (int x = 1; x < levelinfo.parSec; x++) {
                gamestate.weapons <<= 1;
                gamestate.weapons |= 0x1;
            }
                
            if (gamestate.weapons & WP_PISTOL)
                gamestate.primaryammo = STARTAMMO;
            if (gamestate.weapons & WP_MK4)
                gamestate.secondaryammo = STARTAMMO;
            if (gamestate.weapons & WP_RIFLE)
                gamestate.rifleammo = STARTAMMO;
            if (gamestate.weapons & WP_LAUNCHER)
                gamestate.rocketammo = STARTAMMO;

            gamestate.weapon = (weapontype)(levelinfo.parSec - 1);
        }
    } else {
        switch (gamestate.mapon+1) {
            case 1:
                gamestate.weapons = WP_KNIFE;
                gamestate.weapon = wp_knife;
                gamestate.primaryammo = gamestate.secondaryammo = gamestate.rifleammo 
                    = gamestate.rocketammo = gamestate.medkits = 0;
                break;
            default:
                if (reset || gamestate.weapon == wp_spear) {
                    gamestate.weapons = (WP_KNIFE | WP_PISTOL);
                    gamestate.weapon = wp_pistol;
                    gamestate.primaryammo = STARTAMMO;
                    gamestate.secondaryammo = gamestate.rifleammo = gamestate.rocketammo = gamestate.medkits = 0;
                }
                break;
        }
    }

    DrawWeapon();
    DrawAmmo();
}

//==========================================================================

/*
==================
=
= SetupGameLevel
=
==================
*/

void SetupGameLevel (void)
{
    int  x,y;
    word *map;
    word tile;

    if (!loadedgame)
    {
#ifdef REVEALMAP
        memset(spotsaw,0,maparea);
#endif
        gamestate.TimeCount
            = gamestate.secrettotal
            = gamestate.killtotal
            = gamestate.treasuretotal
            = gamestate.secretcount
            = gamestate.killcount
            = gamestate.treasurecount = facetimes = 0;

        for (int i = 0; i < MAXPWALLS; i++)
        {
            pwall[i].active = false;
            pwall[i].state = 0;
            pwall[i].pos = 0;
        }

        LastAttacker = NULL;
        killerobj = NULL;

        ResetGamestate();
        ResetFWalls();

#ifdef EODSPECIAL
        gamestate.gong = false;
        gamestate.enemytreasurecount = 0;
#endif
    }

    //US_InitRndT (true);

//
// load the level
//
	CA_CacheMap(gamestate.mapon);
	switch (gamestate.episode) {
		case 1: mapon-=38; break; // 2007
		case 2: mapon-=40; break; // 2008
		case 3: mapon-=44; break; // 2009
		case 4: mapon-=48; break; // 2010
	}
    /*CA_CacheMap (gamestate.mapon+10*gamestate.episode);
    mapon-=gamestate.episode*10;*/

#ifdef USE_FEATUREFLAGS
    // Read feature flags data from map corners and overwrite corners with adjacent tiles
    int tmp = MAPSPOT(0,1,0);
    levelinfo.shadestr      = MAPSPOT(0,0,0) % 32;                  MAPSPOT(0,0,0) = tmp;
    levelinfo.shadeR        = MAPSPOT(1,0,0) % 256;                 MAPSPOT(1,0,0) = tmp;
    levelinfo.shadeG        = MAPSPOT(2,0,0) % 256;                 MAPSPOT(2,0,0) = tmp;
    levelinfo.shadeB        = MAPSPOT(3,0,0) % 256;                 MAPSPOT(3,0,0) = tmp;
    levelinfo.weather       = MAPSPOT(4,0,0) % 16;                  MAPSPOT(4,0,0) = tmp;
    levelinfo.parallax      = MAPSPOT(5,0,0);                       MAPSPOT(5,0,0) = tmp;
    levelinfo.backtolevel   = MAPSPOT(6,0,0) % 60;                  MAPSPOT(6,0,0) = tmp;
    levelinfo.secretlevel   = MAPSPOT(7,0,0) % 60;                  MAPSPOT(7,0,0) = tmp;
    levelinfo.ceiling1      = MAPSPOT(8,0,0) % 256;                 MAPSPOT(8,0,0) = tmp;
    levelinfo.floor1        = MAPSPOT(9,0,0) % 256;                 MAPSPOT(9,0,0) = tmp;
    levelinfo.ceiling2      = MAPSPOT(10,0,0) % 256;                MAPSPOT(10,0,0) = tmp;
    levelinfo.floor2        = MAPSPOT(11,0,0) % 256;                MAPSPOT(11,0,0) = tmp;
    levelinfo.parMin        = MAPSPOT(12,0,0) % 21;                 MAPSPOT(12,0,0) = tmp;
    levelinfo.parSec        = MAPSPOT(13,0,0) % 60;                 MAPSPOT(13,0,0) = tmp;
    levelinfo.timerMin      = MAPSPOT(14,0,0) % 11;                 MAPSPOT(14,0,0) = tmp;
    levelinfo.timerSec      = MAPSPOT(15,0,0) % 60;                 MAPSPOT(15,0,0) = tmp;
    levelinfo.readthischunk = MAPSPOT(16,0,0);                      MAPSPOT(16,0,0) = tmp;
    levelinfo.music         = MAPSPOT(17,0,0);                      MAPSPOT(17,0,0) = tmp;

    // Display level art if 
    if (!loadedgame) {
        if (levelinfo.readthischunk > 0 && !died) {
            assert(T_HELP + levelinfo.readthischunk < ENUMEND);

            if (playstate == ex_warped) VW_FadeOut();

            VL_ClearScreen(0);
            StopMusic();
            LevelText(levelinfo.readthischunk);
            DrawPlayScreen();
            ClearMemory ();
        }

        if (levelinfo.timerMin > 0 || levelinfo.timerSec > 0) {
            int totalSeconds = levelinfo.timerMin * 60 + levelinfo.timerSec;
            lastsec = levelinfo.timerSec % 10; // for 1000 points stuff

            GetTimer(totalSeconds - 1, T_LEVEL);
            gamestate.levelTimer = totalSeconds * 70;
        }

        SetLevelWeapons(false);

#ifdef VARIABLEWEATHER
        if ((levelinfo.weather & FF_SNOW) || (levelinfo.weather & FF_RAIN))
            InitializeWeatherState();
#endif
    }

#endif

//
// copy the wall data to a data segment array
//
    #define NEWTILE_OFFSET 1000

    memset (tilemap,0,sizeof(tilemap));
    memset (actorat,0,sizeof(actorat));
    memset (statat,0,sizeof(statat));

    map = mapsegs[0];
    for (y=0;y<mapheight;y++)
    {

        for (x=0;x<mapwidth;x++)
        {
            tile = *map++;
            if (tile<AREATILE || tile == 144 || tile == 145)
            {
                // solid wall
                tilemap[x][y] = (byte) tile;
                actorat[x][y] = (objtype *)(uintptr_t) tile;
            }
            else if (tile >= NEWTILE_OFFSET && tile < (NEWTILE_OFFSET + MAXWALLTILES))
            {
                tilemap[x][y] = tile - NEWTILE_OFFSET;
                actorat[x][y] = (objtype *)(uintptr_t)(tile - NEWTILE_OFFSET);
            }
            else
            {
                // area floor
                tilemap[x][y] = 0;
                actorat[x][y] = 0;
            }
        }
    }

//
// spawn doors
//
    InitActorList ();                       // start spawning things with a clean slate
    InitDoorList ();
    InitStaticList ();

    map = mapsegs[0];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *map++;
            if ((tile >= 90 && tile <= 105) || tile == 144 || tile == 145)
            {
                // door
                switch (tile)
                {
                    case 90: // Normal E/W
                    case 92: // Gold E/W
                    case 94: // Bronze E/W
                    case 96: // Blood E/W
                    case 98: // Locked elevator E/W
                    case 100: // Elevator E/W
                    case 102: // Rusted E/W
                    case 104: // Wooden E/W
                        SpawnDoor (x,y,1,(tile-90)/2);
                        break;
                    case 91:
                    case 93:
                    case 95:
                    case 97:
                    case 99:
                    case 101:
                    case 103:
                    case 105:
                        SpawnDoor (x,y,0,(tile-91)/2);
                        break;
                    case 144: // Automatic door E/W
                        SpawnDoor (x,y,1,8);
                        break;
                    case 145:
                        SpawnDoor (x,y,0,8);
                        break;
                }
            }
        }
    }

//
// spawn actors
//
    ScanInfoPlane ();

//
// take out the ambush markers
//
    map = mapsegs[0];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *map++;
            if (tile == AMBUSHTILE)
            {
                tilemap[x][y] = 0;
                if ( (unsigned)(uintptr_t)actorat[x][y] == AMBUSHTILE)
                    actorat[x][y] = NULL;

                if (*map >= AREATILE)
                    tile = *map;
                if (*(map-1-mapwidth) >= AREATILE)
                    tile = *(map-1-mapwidth);
                if (*(map-1+mapwidth) >= AREATILE)
                    tile = *(map-1+mapwidth);
                if ( *(map-2) >= AREATILE)
                    tile = *(map-2);

                *(map-1) = tile;
            }
        }
    }


//
// have the caching manager load and purge stuff to make sure all marks
// are in memory
//
    CA_LoadAllSounds ();
}

//==========================================================================

/*
===================
=
= ResetGamestate
=
===================
*/
void ResetGamestate() {
    gamestate.godmode = gamestate.fury = false;
    gamestate.levelTimer = gamestate.godmodetics = gamestate.furytics 
        = gamestate.reloadtics = gamestate.attackcount = gamestate.attackframe
        = gamestate.weaponframe = gamestate.lightson = gamestate.activetimers 
        = messagetime = 0;
    gamestate.lastweapon = gamestate.lastweaponfury = (weapontype)(-1);
    memset(gamestate.timers,0,sizeof(gamestate.timers));
}

//==========================================================================

/*
===================
=
= DrawPlayBorderSides
=
= To fix window overwrites
=
===================
*/
void DrawPlayBorderSides(void)
{
    if(viewsize == 21) return;

	const int sw = screenWidth;
	const int sh = screenHeight;
	const int vw = viewwidth;
	const int vh = viewheight;
	const int px = scaleFactor; // size of one "pixel"

	const int h  = sh - px * STATUSLINES;
	const int xl = (sw >> 1) - (vw >> 1);
	const int yl = (h - vh) >> 1;

    if(xl != 0)
    {
	    VWB_BarScaledCoord(0,            0, xl - px,     h, bordercol);                 // left side
	    VWB_BarScaledCoord(xl + vw + px, 0, xl - px * 2, h, bordercol);                 // right side
    }

    if(yl != 0)
    {
	    VWB_BarScaledCoord(0, 0,            sw, yl - px, bordercol);                    // upper side
	    VWB_BarScaledCoord(0, yl + vh + px, sw, yl - px, bordercol);                    // lower side
    }

    if(xl != 0)
    {
        // Paint game view border lines
	    VWB_BarScaledCoord(xl - px, yl - px, vw + px, px,          0);                      // upper border
	    VWB_BarScaledCoord(xl,      yl + vh, vw + px, px,          bordercol - 2);          // lower border
	    VWB_BarScaledCoord(xl - px, yl - px, px,      vh + px,     0);                      // left border
	    VWB_BarScaledCoord(xl + vw, yl - px, px,      vh + px * 2, bordercol - 2);          // right border
	    VWB_BarScaledCoord(xl - px, yl + vh, px,      px,          bordercol - 3);          // lower left highlight
    }
    else
    {
        // Just paint a lower border line
        VWB_BarScaledCoord(0, yl+vh, vw, px, bordercol-2);       // lower border
    }
}


/*
===================
=
= DrawStatusBorder
=
===================
*/

void DrawStatusBorder (byte color)
{
    int statusborderw = (screenWidth-scaleFactor*320)/2;

    VWB_BarScaledCoord (0,0,screenWidth,screenHeight-scaleFactor*(STATUSLINES-3),color);
    VWB_BarScaledCoord (0,screenHeight-scaleFactor*(STATUSLINES-3),
        statusborderw+scaleFactor*8,scaleFactor*(STATUSLINES-4),color);
    VWB_BarScaledCoord (0,screenHeight-scaleFactor*2,screenWidth,scaleFactor*2,color);
    VWB_BarScaledCoord (screenWidth-statusborderw-scaleFactor*8, screenHeight-scaleFactor*(STATUSLINES-3),
        statusborderw+scaleFactor*8,scaleFactor*(STATUSLINES-4),color);

    VWB_BarScaledCoord (statusborderw+scaleFactor*9, screenHeight-scaleFactor*3,
        scaleFactor*97, scaleFactor*1, color-1);
    VWB_BarScaledCoord (statusborderw+scaleFactor*106, screenHeight-scaleFactor*3,
        scaleFactor*161, scaleFactor*1, color-2);
    VWB_BarScaledCoord (statusborderw+scaleFactor*267, screenHeight-scaleFactor*3,
        scaleFactor*44, scaleFactor*1, color-3);
    VWB_BarScaledCoord (screenWidth-statusborderw-scaleFactor*9, screenHeight-scaleFactor*(STATUSLINES-4),
        scaleFactor*1, scaleFactor*20, color-2);
    VWB_BarScaledCoord (screenWidth-statusborderw-scaleFactor*9, screenHeight-scaleFactor*(STATUSLINES/2-4),
        scaleFactor*1, scaleFactor*14, color-3);
}


/*
===================
=
= DrawPlayBorder
=
===================
*/

void DrawPlayBorder (void)
{
	const int px = scaleFactor; // size of one "pixel"

    if (bordercol != VIEWCOLOR)
        DrawStatusBorder(bordercol);
    else
    {
        const int statusborderw = (screenWidth-px*320)/2;
        VWB_BarScaledCoord (0, screenHeight-px*STATUSLINES,
            statusborderw+px*8, px*STATUSLINES, bordercol);
        VWB_BarScaledCoord (screenWidth-statusborderw-px*8, screenHeight-px*STATUSLINES,
            statusborderw+px*8, px*STATUSLINES, bordercol);
    }

    if((unsigned) viewheight == screenHeight) return;

    VWB_BarScaledCoord (0,0,screenWidth,screenHeight-px*STATUSLINES,bordercol);

    const int xl = screenWidth/2-viewwidth/2;
    const int yl = (screenHeight-px*STATUSLINES-viewheight)/2;
    VWB_BarScaledCoord (xl,yl,viewwidth,viewheight,0);

    if(xl != 0)
    {
        // Paint game view border lines
        VWB_BarScaledCoord(xl-px, yl-px, viewwidth+px, px, 0);                      // upper border
        VWB_BarScaledCoord(xl, yl+viewheight, viewwidth+px, px, bordercol-2);       // lower border
        VWB_BarScaledCoord(xl-px, yl-px, px, viewheight+px, 0);                     // left border
        VWB_BarScaledCoord(xl+viewwidth, yl-px, px, viewheight+2*px, bordercol-2);  // right border
        VWB_BarScaledCoord(xl-px, yl+viewheight, px, px, bordercol-3);              // lower left highlight
    }
    else
    {
        // Just paint a lower border line
        VWB_BarScaledCoord(0, yl+viewheight, viewwidth, px, bordercol-2);       // lower border
    }
}


/*
===================
=
= DrawPlayScreen
=
===================
*/

void DrawPlayScreen (void)
{
    VWB_DrawPicScaledCoord ((screenWidth-scaleFactor*320)/2,screenHeight-scaleFactor*STATUSLINES,STATUSBARPIC);
    DrawPlayBorder ();

    DrawFace ();
    DrawHealth ();
    DrawLives ();
    DrawLevel ();
    DrawAmmo ();
    DrawKeys ();
    DrawWeapon ();
    DrawScore ();
}

// Uses LatchDrawPic instead of StatusDrawPic
void LatchNumberHERE (int x, int y, unsigned width, int32_t number)
{
    unsigned length,c;
    char str[20];

    ltoa (number,str,10);

    length = (unsigned) strlen (str);

    while (length<width)
    {
        LatchDrawPic (x,y,N_BLANKPIC);
        x++;
        width--;
    }

    c = length <= width ? 0 : length-width;

    while (c<length)
    {
        LatchDrawPic (x,y,str[c]-'0'+ N_0PIC);
        x++;
        c++;
    }
}

void DrawFullscreenHUD()
{
    CA_CacheGrChunk(STARTFONT+1);
    fontnumber = 1;
    pSpacing = fs_drawflag = 1; // makes spaces the same width as characters...

    DrawHealth ();
    DrawLives ();
    DrawLevel ();
    DrawAmmo ();
    DrawKeys ();
    DrawWeapon ();
    DrawScore ();

    pSpacing = fs_drawflag = 0;
}


//==========================================================================

/*
==================
=
= StartDemoRecord
=
==================
*/

char    demoname[13] = "DEMO?.";

#ifndef REMDEBUG
#define MAXDEMOSIZE     8192

void StartDemoRecord (int levelnumber)
{
    demobuffer=malloc(MAXDEMOSIZE);
    CHECKMALLOCRESULT(demobuffer);
    demoptr = (int8_t *) demobuffer;
    lastdemoptr = demoptr+MAXDEMOSIZE;

    *demoptr = levelnumber;
    demoptr += 4;                           // leave space for length
    demorecord = true;
}


/*
==================
=
= FinishDemoRecord
=
==================
*/

void FinishDemoRecord (void)
{
    int32_t    length,level;

    demorecord = false;

    length = (int32_t) (demoptr - (int8_t *)demobuffer);

    demoptr = ((int8_t *)demobuffer)+1;
    demoptr[0] = (int8_t) length;
    demoptr[1] = (int8_t) (length >> 8);
    demoptr[2] = 0;

    VW_FadeIn();
    CenterWindow(24,3);
    PrintY+=6;
    fontnumber=0;
    SETFONTCOLOR(0,15);
    US_Print(" Demo number (0-9): ");
    VW_UpdateScreen();

    if (US_LineInput (px,py,str,NULL,true,1,0))
    {
        level = atoi (str);
        if (level>=0 && level<=9)
        {
            demoname[4] = (char)('0'+level);
            CA_WriteFile (demoname,demobuffer,length);
        }
    }

    free(demobuffer);
}

//==========================================================================

/*
==================
=
= RecordDemo
=
= Fades the screen out, then starts a demo.  Exits with the screen faded
=
==================
*/

void RecordDemo (void)
{
    int level,esc,maps;

    CenterWindow(26,3);
    PrintY+=6;
    CA_CacheGrChunk(STARTFONT);
    fontnumber=0;
    SETFONTCOLOR(0,15);
#ifndef SPEAR
#ifdef UPLOAD
    US_Print("  Demo which level(1-10): "); maps = 10;
#else
    US_Print("  Demo which level(1-60): "); maps = 60;
#endif
#else
    US_Print("  Demo which level(1-21): "); maps = 21;
#endif
    VW_UpdateScreen();
    VW_FadeIn ();
    esc = !US_LineInput (px,py,str,NULL,true,2,0);
    if (esc)
        return;

    level = atoi (str);
    level--;

    if (level >= maps || level < 0)
        return;

    VW_FadeOut ();

#ifndef SPEAR
    NewGame (gd_hard,level/10);
    gamestate.mapon = level%10;
#else
    NewGame (gd_hard,0);
    gamestate.mapon = level;
#endif

    StartDemoRecord (level);

    DrawPlayScreen ();
    VW_FadeIn ();

    startgame = false;
    demorecord = true;

    SetupGameLevel ();
    StartMusic ();

    if(usedoublebuffering) VH_UpdateScreen();
    fizzlein = true;

    PlayLoop ();

    demoplayback = false;

    StopMusic ();
    VW_FadeOut ();
    ClearMemory ();

    FinishDemoRecord ();
}
#else
void FinishDemoRecord (void) {return;}
void RecordDemo (void) {return;}
#endif



//==========================================================================

/*
==================
=
= PlayDemo
=
= Fades the screen out, then starts a demo.  Exits with the screen unfaded
=
==================
*/

void PlayDemo (int demonumber)
{
    int length;
#ifdef DEMOSEXTERN
// debug: load chunk
#ifndef SPEARDEMO
    int dems[4]={T_DEMO0,T_DEMO1,T_DEMO2,T_DEMO3};
#else
    int dems[1]={T_DEMO0};
#endif

    CA_CacheGrChunk(dems[demonumber]);
    demoptr = (int8_t *) grsegs[dems[demonumber]];
#else
    demoname[4] = '0'+demonumber;
    CA_LoadFile (demoname,&demobuffer);
    demoptr = (int8_t *)demobuffer;
#endif

    NewGame (1,0);
    gamestate.mapon = *demoptr++;
    gamestate.difficulty = gd_hard;
    length = READWORD(*(uint8_t **)&demoptr);
    // TODO: Seems like the original demo format supports 16 MB demos
    //       But T_DEM00 and T_DEM01 of Wolf have a 0xd8 as third length size...
    demoptr++;
    lastdemoptr = demoptr-4+length;

    VW_FadeOut ();

    SETFONTCOLOR(0,15);
    DrawPlayScreen ();

    startgame = false;
    demoplayback = true;

    SetupGameLevel ();
    StartMusic ();

    PlayLoop ();

#ifdef DEMOSEXTERN
    UNCACHEGRCHUNK(dems[demonumber]);
#else
    MM_FreePtr (&demobuffer);
#endif

    demoplayback = false;

    StopMusic ();
    ClearMemory ();
}

//==========================================================================

/*
==================
=
= Died
=
==================
*/

#define DEATHROTATE 2

void Died (void)
{
    float   fangle;
    int32_t dx,dy;
    int     iangle,curangle,clockwise,counter,change;
    weapontype oldweapon; // for timed levels

    if (screenfaded)
    {
        ThreeDRefresh ();
        VW_FadeIn ();
    }

    oldweapon = gamestate.weapon;
    gamestate.weapon = (weapontype) -1;                     // take away weapon
    DrawWeapon();
    DrawAmmo();
    SD_PlaySound (PLAYERDEATHSND);

    //
    // swing around to face attacker
    //
    if(killerobj)
    {
        dx = killerobj->x - player->x;
        dy = player->y - killerobj->y;

        fangle = (float) atan2((float) dy, (float) dx);     // returns -pi to pi
        if (fangle<0)
            fangle = (float) (M_PI*2+fangle);

        iangle = (int) (fangle/(M_PI*2)*ANGLES);
    }
    else
    {
        iangle = player->angle + ANGLES / 2;
        if(iangle >= ANGLES) iangle -= ANGLES;
    }

    if (player->angle > iangle)
    {
        counter = player->angle - iangle;
        clockwise = ANGLES-player->angle + iangle;
    }
    else
    {
        clockwise = iangle - player->angle;
        counter = player->angle + ANGLES-iangle;
    }

    curangle = player->angle;

    if (clockwise<counter)
    {
        //
        // rotate clockwise
        //
        if (curangle>iangle)
            curangle -= ANGLES;
        do
        {
            change = tics*DEATHROTATE;
            if (curangle + change > iangle)
                change = iangle-curangle;

            curangle += change;
            player->angle += change;
            if (player->angle >= ANGLES)
                player->angle -= ANGLES;

            ThreeDRefresh ();
            CalcTics ();
        } while (curangle != iangle);
    }
    else
    {
        //
        // rotate counterclockwise
        //
        if (curangle<iangle)
            curangle += ANGLES;
        do
        {
            change = -(int)tics*DEATHROTATE;
            if (curangle + change < iangle)
                change = iangle-curangle;

            curangle += change;
            player->angle += change;
            if (player->angle < 0)
                player->angle += ANGLES;

            ThreeDRefresh ();
            CalcTics ();
        } while (curangle != iangle);
    }

    //
    // fade to red
    //
    FinishPaletteShifts ();

    if(usedoublebuffering) VH_UpdateScreen();

    VL_BarScaledCoord (viewscreenx,viewscreeny,viewwidth,viewheight,4);

    IN_ClearKeysDown ();

    FizzleFade(screenBuffer,viewscreenx,viewscreeny,viewwidth,viewheight,70,false);

    IN_UserInput(100);
    SD_WaitSoundDone ();
    ClearMemory();

    //
    // RESET ON DEATH
    //
    if (levelinfo.timerMin > 0 || levelinfo.timerSec > 0) { // timed levels don't kill the player
        gamestate.health = 100;
        DrawHealth();
        DrawFace();
        playstate = ex_completed;

        gamestate.weapon = oldweapon;
        gamestate.attackframe = gamestate.attackcount = gamestate.weaponframe = 0;
    } else {
        if (gamestate.difficulty != gd_extreme) // lives is used as a saves counter
            gamestate.lives--;

        if (gamestate.lives > -1)
        {
            gamestate.health = 100;

            SetLevelWeapons(true);
            gamestate.backpack = false;
            gamestate.keys = 0;

            for (int i = 0; i < MAXPWALLS; i++)
            {
                pwall[i].active = false;
                pwall[i].state = 0;
                pwall[i].pos = 0;
            } 

            gamestate.attackframe = gamestate.attackcount =
                gamestate.weaponframe = 0;

            ResetGamestate();

            DrawKeys ();
            DrawWeapon ();
            DrawAmmo ();
            DrawHealth ();
            DrawFace ();
            DrawLives ();

        }
    }
}

//==========================================================================

/*
===================
=
= GameLoop
=
===================
*/

void GameLoop (void)
{
    boolean endflag = 0;
#ifdef MYPROFILE
    clock_t start,end;
#endif

restartgame:
    ClearMemory ();
    SETFONTCOLOR(0,15);
    VW_FadeOut();
    DrawPlayScreen ();
    died = false;
    do
    {
        if (!loadedgame)
            gamestate.score = gamestate.oldscore;
        if(!died /*|| viewsize != 21*/) DrawScore();

        startgame = false;
        if (!loadedgame) {
            SetupGameLevel ();
        }

        ingame = true;
        if(loadedgame)
        {
            ContinueMusic(lastgamemusicoffset);
            loadedgame = false;
        }
        else StartMusic ();

        if (!died)
            PreloadGraphics ();             // TODO: Let this do something useful!
        else
        {
            died = false;
            fizzlein = true;
        }
      //  DrawLevel ();

#ifdef SPEAR
startplayloop:
#endif
        PlayLoop ();

#ifdef SPEAR
        if (spearflag)
        {
            SD_StopSound();
            SD_PlaySound(GETSPEARSND);
            if (DigiMode != sds_Off)
            {
                Delay(150);
            }
            else
                SD_WaitSoundDone();

            ClearMemory ();
            gamestate.oldscore = gamestate.score;
            gamestate.mapon++;

            SetupGameLevel ();
            StartMusic ();
            player->x = spearx;
            player->y = speary;
            player->angle = (short)spearangle;
            spearflag = false;
            Thrust (0,0);
            goto startplayloop;
        }
#endif

        StopMusic ();
        ingame = false;

        if (demorecord && playstate != ex_warped)
            FinishDemoRecord ();       

        if (startgame || loadedgame)
            goto restartgame;

updateplaystate:
        switch (playstate)
        {
            case ex_completed:
            case ex_secretlevel:
                if(viewsize == 21) DrawPlayScreen();
                
                if(gamestate.difficulty == gd_extreme)
                    gamestate.lives++; // new save for tournament

                gamestate.keys = 0;

                DrawLives();
                DrawKeys ();
                VW_FadeOut ();

                ClearMemory ();

                LevelCompleted ();              // do the intermission

                if(viewsize == 21) DrawPlayScreen();

                gamestate.oldscore = gamestate.score;

                //
                // GOING TO SECRET LEVEL
                //
                if (playstate == ex_secretlevel) {
                    if (levelinfo.secretlevel) {
                        gamestate.mapon = levelinfo.secretlevel - 1; // goto the secret level
                    } else {
                        gamestate.mapon++;
                    }
                } else { // regular elevator
                    if (levelinfo.backtolevel) { // this will be set on secret levels
                        gamestate.mapon = levelinfo.backtolevel - 1;
                    } else {
                        gamestate.mapon++; // default action
                    }
                }

				switch (gamestate.episode) {
				case 0:
					if (gamestate.mapon == 38) {
						endflag = 1;
					}
					break;
				case 1:
					if (gamestate.mapon == 40) {
						endflag = 1;
					}
					break;
				case 2:
					if (gamestate.mapon == 44) {
						endflag = 1;
					}
					break;
				case 3:
					if (gamestate.mapon == 48) {
						endflag = 1;
					}
					break;
				case 4:
					if (gamestate.mapon == 49) {
						endflag = 1;
					}
					break;
				}

				if (!endflag) {
					DrawPlayScreen();
					ClearMemory();
				} else {
					VW_FadeOut ();
				if(screenHeight % 200 != 0)
					VL_ClearScreen(0);
					ClearMemory();
#ifndef JAPAN
					strcpy(MainMenu[viewscores].string,STR_VS);
#endif
					MainMenu[viewscores].routine = CP_ViewScores;
					return;
				}
                break;
                
            case ex_died:
                Died ();
                if (levelinfo.timerMin > 0 || levelinfo.timerSec > 0) goto updateplaystate;
                died = true;                    // don't "get psyched!"

                if (gamestate.lives > -1)
                    break;                          // more lives left

                VW_FadeOut ();
                if(screenHeight % 200 != 0)
                    VL_ClearScreen(0);

#ifdef _arch_dreamcast
                DC_StatusClearLCD();
#endif

                ClearMemory ();

                CheckHighScore (gamestate.score,gamestate.mapon+1);
#ifndef JAPAN
                strcpy(MainMenu[viewscores].string,STR_VS);
#endif
                MainMenu[viewscores].routine = CP_ViewScores;
                return;

            case ex_victorious:
                if(viewsize == 21) DrawPlayScreen();
#ifndef SPEAR
                VW_FadeOut ();
#else
                VL_FadeOut (0,255,10,10,10,300);
#endif
                ClearMemory ();

                Victory ();

                ClearMemory ();

                CheckHighScore (gamestate.score,gamestate.mapon+1);
#ifndef JAPAN
                strcpy(MainMenu[viewscores].string,STR_VS);
#endif
                MainMenu[viewscores].routine = CP_ViewScores;
                return;

            case ex_warped:
            default:
                ResetGamestate();
                DrawPlayScreen();
                ClearMemory ();
                break;
        }
    } while (1);
}

void ResetFWalls (void) {
    for (int i = 0; i < MAXFIREWALLS; i++) {
        fwall[i].active = true;
        fwall[i].tics = FIREWALLDELAY;
        fwall[i].tilex = 0;
        fwall[i].tiley = 0;
    }
}
