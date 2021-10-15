// WL_AGENT.C

#include "wl_def.h"
#pragma hdrstop

/*
=============================================================================

                                LOCAL CONSTANTS

=============================================================================
*/

#define MAXMOUSETURN    10


#define MOVESCALE       150l
#define BACKMOVESCALE   100l
#define ANGLESCALE      20

/*
=============================================================================

                                GLOBAL VARIABLES

=============================================================================
*/



//
// player state info
//
int32_t         thrustspeed;
word            plux,pluy;          // player coordinates scaled to unsigned
short           anglefrac;
objtype        *LastAttacker;

extern   statetype s_rocket; 
extern   statetype s_spearproj;

boolean resetAttack = false;

/*
=============================================================================

                                                 LOCAL VARIABLES

=============================================================================
*/


void    T_Player (objtype *ob);
void    T_Attack (objtype *ob);

statetype   s_player = {false,0,0,(statefunc) T_Player,NULL,NULL};
statetype   s_attack = {false,0,0,(statefunc) T_Attack,NULL,NULL};

struct atkinf
{
    int8_t    tics,attack,frame;              // attack is 1 for gun, 2 for knife
} attackinfo[NUMWEAPONS][4] =
{
    { {6,0,1},{6,2,2},{6,0,3},{6,-1,4} }, // knife
    { {6,0,1},{6,1,2},{6,0,3},{6,-1,4} }, // pistol
    { {6,0,1},{6,1,2},{6,3,3},{6,-1,4} }, // mg
    { {6,0,1},{3,1,2},{6,4,3},{6,-1,4} }, // mk3
    { {6,0,1},{2,1,2},{5,4,3},{6,-1,4} }, // mk4 (6,1,3,6)
    { {6,0,1},{6,1,2},{6,0,3},{6,-1,4} }, // rifle
    { {6,0,1},{6,1,2},{6,0,3},{6,-1,4} }, // launcher
    { {6,0,1},{6,1,2},{6,0,3},{6,-1,4} }, // spear
};

//===========================================================================

//----------

void Attack (void);
void Use (void);
void Search (objtype *ob);
void SelectWeapon (void);
void SelectItem (void);

//----------

boolean TryMove (objtype *ob);
void T_Player (objtype *ob);

void ClipMove (objtype *ob, int32_t xmove, int32_t ymove);

void WarpPlayer(int facedir);
void RegenPlayer(void);
int GetPlayerAngle(void);

void SwitchToBestWeapon(void);
boolean HasAmmo(byte weapon);
void SwitchToPreviousWeapon(void);
void SwitchToNextWeapon(void);

/*
=============================================================================

                                CONTROL STUFF

=============================================================================
*/

/*
======================
=
= CheckWeaponChange
=
= Keys 1-4 change weapons
=
======================
*/

void CheckWeaponChange (void)
{
    int newWeapon = -2; // -2: default (nothing was pressed)
                        // -1: pressed but didn't have weapon

    switch (LastScan) {
        case SDLK_1:
            LastScan = 0;
            if (gamestate.weapons & WP_KNIFE)
                newWeapon = wp_knife;
            else
                newWeapon = -1;
            break;

        case SDLK_2:
            LastScan = 0;
            if (gamestate.weapons & WP_PISTOL)
                newWeapon = wp_pistol;
            else
                newWeapon = -1;
            break;

        case SDLK_3:
            LastScan = 0;
            if (gamestate.weapons & WP_MACHINEGUN)
                newWeapon = wp_machinegun;
            else
                newWeapon = -1;
            break;

        case SDLK_4:
            LastScan = 0;
            if (gamestate.weapons & WP_MK3)
                newWeapon = wp_mk3;
            else
                newWeapon = -1;
            break;

        case SDLK_5:
            LastScan = 0;
            if (gamestate.weapons & WP_MK4)
                newWeapon = wp_mk4;
            else
                newWeapon = -1;
            break;

        case SDLK_6:
            LastScan = 0;
            if (gamestate.weapons & WP_RIFLE)
                newWeapon = wp_rifle;
            else
                newWeapon = -1;
            break;

        case SDLK_7:
            LastScan = 0;
            if (gamestate.weapons & WP_LAUNCHER)
                newWeapon = wp_launcher;
            else
                newWeapon = -1;
            break;

        case SDLK_9:
            LastScan = 0;
            if (gamestate.medkits > 0) {
                gamestate.lastweapon = gamestate.weapon;
                newWeapon = wp_medkit;
            } else {
                SD_PlaySound(NOWAYSND);
                GetMessage("You don't have any medkits",DEF_MSG_CLR);
            }
            break;

        default:
            break;

    }

#ifdef _arch_dreamcast
    int joyx, joyy;
    IN_GetJoyFineDelta (&joyx, &joyy);
    if(joyx < -64)
        buttonstate[bt_prevweapon] = true;
    else if(joyx > 64)
        buttonstate[bt_nextweapon] = true;
#endif

    /*if(buttonstate[bt_nextweapon] && !buttonheld[bt_nextweapon])
    {
        newWeapon = gamestate.weapon + 1;
    }
    else if(buttonstate[bt_prevweapon] && !buttonheld[bt_prevweapon])
    {
        newWeapon = gamestate.weapon - 1;
    }*/

    if(newWeapon != -2)
    {
        if (newWeapon == -1) {
            SD_PlaySound(NOWAYSND);
            GetMessage("You don't have this weapon", DEF_MSG_CLR);
        } else if (newWeapon != gamestate.weapon) {
            NEXTWEAPON((weapontype)newWeapon);
        }
    }
}

/*
=======================
=
= HasAmmo
=
=======================
*/
boolean HasAmmo(weapontype weapon) {
    switch (weapon) {
        case wp_knife:
        case wp_spear:
            return true;
            break;
        case wp_pistol:
        case wp_machinegun:
            if (gamestate.primaryammo > 0)
                return true;
            break;
        case wp_mk3:
        case wp_mk4:
            if (gamestate.secondaryammo > 0)
                return true;
            break;
        case wp_rifle:
            if (gamestate.rifleammo > 0)
                return true;
            break;
        case wp_launcher:
            if (gamestate.rocketammo > 0) 
                return true;
            break;
        case wp_medkit:
            if (gamestate.medkits > 0)
                return true;
            break;
    }
    return false;
}
/*
=======================
=
= SwitchToBestWeapon
=
=======================
*/
void SwitchToBestWeapon(void) {
    short bestweapon = wp_launcher; // spear is not considered
    short check = WP_LAUNCHER;

    if (!gamestate.weapons) return;

    while (!(gamestate.weapons & check) || !HasAmmo((weapontype)bestweapon))
    {
        check >>= 1;
        bestweapon--;
    }
    
    if (bestweapon != gamestate.weapon) {
        NEXTWEAPON((weapontype)bestweapon);
    }
}

void SwitchToNextWeapon (void) {
    if (gamestate.attackframe || !gamestate.weapons) return;

    int check = (int)pow((double)2,(int)(gamestate.weapon)+1);
    int nextweapon = gamestate.weapon+1;

    if (!gamestate.weapons) return;

    while ((nextweapon != gamestate.weapon) && (!(gamestate.weapons & check) || !HasAmmo((weapontype)nextweapon))) {
        check <<= 1;
        nextweapon++;
        if (check > WP_SPEAR) {
            check = WP_KNIFE;
            nextweapon = wp_knife;
        }
    }

    if (nextweapon != gamestate.weapon) {
        NEXTWEAPON((weapontype)nextweapon);
    } else {
        SD_PlaySound(NOWAYSND);
    }
}

void SwitchToPreviousWeapon (void) {
    if (gamestate.attackframe || !gamestate.weapons) return;

    int check = (int)pow((double)2,(int)(gamestate.weapon)-1);
    int nextweapon = gamestate.weapon-1;

    while ((nextweapon != gamestate.weapon) && (!(gamestate.weapons & check) || !HasAmmo((weapontype)nextweapon))) {
        check >>= 1;
        nextweapon--;
        if (check < WP_KNIFE) {
            check = WP_SPEAR;
            nextweapon = wp_spear;
        }
    }

    if (nextweapon != gamestate.weapon) {
        NEXTWEAPON((weapontype)nextweapon);
    } else {
        SD_PlaySound(NOWAYSND);
    }
}

/*
=======================
=
= ControlMovement
=
= Takes controlx,controly, and buttonstate[bt_strafe]
=
= Changes the player's angle and position
=
= There is an angle hack because when going 70 fps, the roundoff becomes
= significant
=
=======================
*/

void ControlMovement (objtype *ob)
{
     int32_t oldx,oldy;
    int     angle;
    int     angleunits;

    thrustspeed = 0;

    oldx = player->x;
    oldy = player->y;

    //
    // turn
    //
    anglefrac += controlh;
    angleunits = anglefrac/ANGLESCALE;
    anglefrac -= angleunits*ANGLESCALE;
    ob->angle -= angleunits;

    if (ob->angle >= ANGLES)
        ob->angle -= ANGLES;
    if (ob->angle < 0)
        ob->angle += ANGLES;

    //
    // forward/backwards move
    //
    if (controly < 0)
    {
        Thrust (ob->angle,-controly*MOVESCALE); // move forwards
    }
    else if (controly > 0)
    {
        angle = ob->angle + ANGLES/2;
        if (angle >= ANGLES)
            angle -= ANGLES;
        Thrust (angle,controly*BACKMOVESCALE);          // move backwards
    }

    if (gamestate.victoryflag)              // watching the BJ actor
        return;

    //
    // strafing
    //
    //
    if (controlx > 0)
    {
        angle = ob->angle - ANGLES/4;
        if (angle < 0)
            angle += ANGLES;
        Thrust (angle,controlx*MOVESCALE);      // move to left
    }
    else if (controlx < 0)
    {
        angle = ob->angle + ANGLES/4;
        if (angle >= ANGLES)
            angle -= ANGLES;
        Thrust (angle,-controlx*MOVESCALE);     // move to right
    } 
}

/*
=============================================================================

                            STATUS WINDOW STUFF

=============================================================================
*/


/*
==================
=
= StatusDrawPic
=
==================
*/

void StatusDrawPic (unsigned x, unsigned y, unsigned picnum)
{
    LatchDrawPicScaledCoord ((screenWidth-scaleFactor*320)/16 + scaleFactor*x,
        screenHeight-scaleFactor*(STATUSLINES-y),picnum);
}

// precision
void StatusDrawPic2 (unsigned x, unsigned y, unsigned picnum)
{
    int destx = x*scaleFactor;
    int desty = y*scaleFactor;

    if (x > 160)
        destx += picHorizAdjust;
    if (y > 100)
        desty += picVertAdjust;

    LatchDrawPicScaledCoord2 (destx,desty,picnum);
}

void StatusDrawFace(unsigned picnum)
{
    if (viewsize == 21) return;
    StatusDrawPic(17, 4, picnum);
    StatusDrawLCD(picnum);
}


/*
==================
=
= DrawFace
=
==================
*/

void DrawFace (void)
{
    if(viewsize == 21 && ingame) return;

    if (SD_SoundPlaying() == GETGATLINGSND)
        StatusDrawFace(GOTGATLINGPIC);
    else if (gamestate.health)
    {
#ifdef SPEAR
        if (gamestate.godmode || gamestate.godcheat)
            StatusDrawFace(GODMODEFACE1PIC+gamestate.faceframe);
        else if (gamestate.fury || gamestate.furycheat)
            StatusDrawFace(GOTGATLINGPIC);
        else if (gamestate.health < 101)
#endif
            StatusDrawFace(FACE1APIC+3*((100-gamestate.health)/16)+gamestate.faceframe);
        else
            StatusDrawFace(FACE1APIC+gamestate.faceframe);
    }
    else
    {
#ifndef SPEAR
        if (LastAttacker && LastAttacker->obclass == needleobj)
            StatusDrawFace(MUTANTBJPIC);
        else
#endif
            StatusDrawFace(FACE8APIC);
    }
}

/*
===============
=
= UpdateFace
=
= Calls draw face if time to change
=
===============
*/

int facecount = 0;
int facetimes = 0;

void UpdateFace (void)
{
    // don't make demo depend on sound playback
    if(demoplayback || demorecord)
    {
        if(facetimes > 0)
        {
            facetimes--;
            return;
        }
    }
    else if(SD_SoundPlaying() == GETGATLINGSND)
        return;

    facecount += tics;
    if (facecount > US_RndT())
    {
        gamestate.faceframe = (US_RndT()>>6);
        if (gamestate.faceframe==3)
            gamestate.faceframe = 1;

        facecount = 0;
        DrawFace ();
    }
}



/*
===============
=
= LatchNumber
=
= right justifies and pads with blanks
=
===============
*/

static void LatchNumber (int x, int y, unsigned width, int32_t number)
{
    if (number == -1) { // special case for knife and spear ammo drawing
        StatusDrawPic(x,y,N_BLANKPIC);
        StatusDrawPic(x+1,y,N_BLANKPIC);
        StatusDrawPic(x+2,y,N_BLANKPIC);
    } else {

        unsigned length,c;
        char    str[20];

        ltoa (number,str,10);
        length = (unsigned) strlen (str);

        while (length<width)
        {
            StatusDrawPic (x,y,N_BLANKPIC);
            x++;
            width--;
        }

        c = length <= width ? 0 : length-width;

        while (c<length)
        {
            StatusDrawPic (x,y,str[c]-'0'+ N_0PIC);
            x++;
            c++;
        }
    }
}

/*
===============
=
= DrawHealth
=
===============
*/

void DrawHealth (void)
{
    if(viewsize == 21 && ingame) {
        if (!fs_drawflag) return;
        StatusDrawPic2(246,182,FS_MEDPIC);

        // Shadow
        SETFONTCOLOR(0x0,0x0);
        SETPRINTXY(231 + printHorizAdjust,169 + printVertAdjust);
        US_PrintUnsignedPadded(gamestate.health,3);

        // Shadow for percent
        SETPRINTXY(263 + printHorizAdjust, 169 + printVertAdjust);
        fontnumber = 0;
        US_Print("%");
        fontnumber = 1;
        
        // Color based on state
        if (gamestate.godmode || gamestate.godcheat) {
            SETFONTCOLOR(0x64,0x0);
        } else if (gamestate.health > 66) {
            SETFONTCOLOR(0x10,0x0);
        } else if (gamestate.health > 33) {
            SETFONTCOLOR(0x44,0x0);
        } else {
            SETFONTCOLOR(0x34,0x0);
        }
        SETPRINTXY(230 + printHorizAdjust,168 + printVertAdjust);
        US_PrintUnsignedPadded(gamestate.health,3);

        // Percent
        SETFONTCOLOR(0x10,0x0);
        SETPRINTXY(262 + printHorizAdjust, 168 + printVertAdjust);
        fontnumber = 0;
        US_Print("%");
        fontnumber = 1;
    } else {
        LatchNumber (21,16,3,gamestate.health);
    }
}


/*
===============
=
= TakeDamage
=
===============
*/

void TakeDamage (int points,objtype *attacker)
{
    LastAttacker = attacker;

    if (gamestate.victoryflag)
        return;
    if (gamestate.difficulty==gd_baby)
        points>>=2;

    if (!gamestate.godmode && !gamestate.godcheat) {
        gamestate.health -= points;
        if (attacker == NULL)
            SD_PlaySound(TAKEDAMAGESND);
    }

    if (gamestate.health<=0)
    {
        gamestate.health = 0;
        playstate = ex_died;
        killerobj = attacker;
    }

    if (!gamestate.godmode && !gamestate.godcheat)
        StartDamageFlash (points);

    DrawHealth ();
    DrawFace ();

    //
    // MAKE BJ'S EYES BUG IF MAJOR DAMAGE!
    //
#ifdef SPEAR
    if (points > 30 && gamestate.health!=0 && !gamestate.godmode && !gamestate.godcheat)
    {
        StatusDrawFace(BJOUCHPIC);
        facecount = 0;
    }
#endif
}

/*
===============
=
= HealSelf
=
===============
*/

void HealSelf (int points)
{
    gamestate.health += points;
    if (gamestate.health > 100)
        gamestate.health = 100;

    DrawHealth ();
    DrawFace ();
}


//===========================================================================


/*
===============
=
= DrawLevel
=
===============
*/

void DrawLevel (void)
{
	int num;

	switch (gamestate.episode) {
	case 0:
		num = gamestate.mapon + 1;
		break;
	case 1:
		num = gamestate.mapon - 37;
		break;
	case 2:
		num = gamestate.mapon - 39;
		break;
	case 3:
		num = gamestate.mapon - 43;
		break;
	case 4:
		num = gamestate.mapon - 47;
		break;
	}

	if (gamestate.weapon == wp_spear) num--;

    if(viewsize == 21 && ingame) {
        if (!fs_drawflag) return;
        StatusDrawPic2(4,174,FS_LEVELPIC);
        fontnumber = 0;

        SETFONTCOLOR(0x0,0x0);
        SETPRINTXY(20,175 + printVertAdjust);
        US_PrintUnsigned(num);

        SETFONTCOLOR(0x10,0x0);
        SETPRINTXY(19,174 + printVertAdjust);
        US_PrintUnsigned(num);

        fontnumber = 1;
    } else {
        LatchNumber (2,16,2,num);
    }

}

//===========================================================================


/*
===============
=
= DrawLives
=
===============
*/

void DrawLives (void)
{
    if(viewsize == 21 && ingame) {
        if (!fs_drawflag) return;
        fontnumber = 0;
        SETFONTCOLOR(0x0,0);
        SETPRINTXY(225 + printHorizAdjust,186 + printVertAdjust);
        US_PrintUnsignedPadded(gamestate.lives,2);

        SETFONTCOLOR(0x10,0);
        SETPRINTXY(224 + printHorizAdjust,185 + printVertAdjust);
        US_PrintUnsignedPadded(gamestate.lives,2);
        fontnumber = 1;
    } else {
        LatchNumber (14,16,2,gamestate.lives);
    }
}


/*
===============
=
= GiveExtraMan
=
===============
*/

void GiveExtraMan (void)
{
    if (gamestate.lives<9)
        gamestate.lives++;
    DrawLives ();
    SD_PlaySound (BONUS1UPSND);
}

//===========================================================================

/*
===============
=
= DrawScore
=
===============
*/

void DrawScore (void)
{
    if(viewsize == 21 && ingame) {
        if (!fs_drawflag) return;
        StatusDrawPic2(4,186,FS_SCOREPIC);

        fontnumber = 0;
        SETFONTCOLOR(0x0,0x0);
        SETPRINTXY(20,187 + printVertAdjust);
        US_PrintUnsigned(gamestate.score);

        SETFONTCOLOR(0x10,0x0);
        SETPRINTXY(19,186 + printVertAdjust);
        US_PrintUnsigned(gamestate.score);
        fontnumber = 1;
    } else {
        LatchNumber (6,16,7,gamestate.score);
    }
}

/*
===============
=
= GivePoints
=
===============
*/

void GivePoints (int32_t points)
{
	if (gamestate.cheated)
		return;

    gamestate.score += points;
    while (gamestate.score >= gamestate.nextextra)
    {
        switch (gamestate.difficulty) {
            case gd_baby:
                gamestate.nextextra += 40000;
                break;
            case gd_easy:
                gamestate.nextextra += 60000;
                break;
            case gd_medium:
                gamestate.nextextra += 75000;
                break;
            case gd_hard:
            case gd_extreme:
                gamestate.nextextra += 100000;
                break;
        }
        GiveExtraMan ();
    }
    DrawScore ();
}

//===========================================================================

/*
==================
=
= DrawWeapon
=
==================
*/

void DrawWeapon (void)
{
    if(viewsize == 21 && ingame) {
        if (!fs_drawflag) return;
        
        if (gamestate.weapon != -1) StatusDrawPic2 (276,182,FS_KNIFEPIC+gamestate.weapon);
    } else {
        StatusDrawPic (33,16,KNIFEPIC+gamestate.weapon);
    }
}


/*
==================
=
= DrawKeys
=
==================
*/

void DrawKeys (void)
{
    if(viewsize == 21 && ingame) {
        if (!fs_drawflag) return;

        if (gamestate.keys & 1)
            StatusDrawPic2 (4,156,FS_KEY1PIC);

        if (gamestate.keys & 2)
            StatusDrawPic2 (8,156,FS_KEY2PIC);

        if (gamestate.keys & 4)
            StatusDrawPic2 (12,156,FS_KEY3PIC);

        if (gamestate.keys & 8)
            StatusDrawPic2 (16,156,FS_KEY4PIC);
    } else {

        if (gamestate.keys & 1)
            StatusDrawPic (30,4,GOLDKEYPIC);
        else
            StatusDrawPic (30,4,NOKEYPIC);

        if (gamestate.keys & 2)
            StatusDrawPic (30,20,SILVERKEYPIC);
        else
            StatusDrawPic (30,20,NOKEYPIC);

        if (gamestate.keys & 4)
            StatusDrawPic (31,4,BLOODKEYPIC);
        else
            StatusDrawPic (31,4,NOKEYPIC);

        if (gamestate.keys & 8)
            StatusDrawPic (31,20,ELEKEYPIC);
        else
            StatusDrawPic (31,20,NOKEYPIC);
    }
}

/*
==================
=
= GiveWeapon
=
==================
*/

void GiveWeapon (weapontype newWeapon)
{
    boolean hadIt;

    if (gamestate.weapon == -1)
        return;

    switch (newWeapon) {
        case wp_knife:
            hadIt = (gamestate.weapons & WP_KNIFE) ? true : false;
            gamestate.weapons |= WP_KNIFE;
            break;
        case wp_pistol:
            hadIt = (gamestate.weapons & WP_PISTOL) ? true : false;
            gamestate.weapons |= WP_PISTOL;
            GiveAmmo(0,4);
            break;
        case wp_machinegun:
            hadIt = (gamestate.weapons & WP_MACHINEGUN) ? true : false;
            gamestate.weapons |= WP_MACHINEGUN;
            GiveAmmo(0,6);
            break;
        case wp_mk3:
            hadIt = (gamestate.weapons & WP_MK3) ? true : false;
            gamestate.weapons |= WP_MK3;
            GiveAmmo(1,10);
            break;
        case wp_mk4:
            hadIt = (gamestate.weapons & WP_MK4) ? true : false;
            gamestate.weapons |= WP_MK4;
            GiveAmmo(1,10);
            break;
        case wp_rifle:
            hadIt = (gamestate.weapons & WP_RIFLE) ? true : false;
            gamestate.weapons |= WP_RIFLE;
            GiveAmmo(2,5);
            break;
        case wp_launcher:
            hadIt = (gamestate.weapons & WP_LAUNCHER) ? true : false;
            gamestate.weapons |= WP_LAUNCHER;
            GiveAmmo(3,2);
            break;
        case wp_spear:
            hadIt = (gamestate.weapons & WP_SPEAR) ? true : false;
            gamestate.weapons |= WP_SPEAR;
            break;
    }

    if (newWeapon > gamestate.weapon && !hadIt && !gamestate.fury) {
        NEXTWEAPON(newWeapon);
    }
}

//===========================================================================

/*
===============
=
= DrawAmmo
=
===============
*/

void DrawAmmo(void)
{
    if(viewsize == 21 && ingame) {
        if (!fs_drawflag) return;

        switch(gamestate.weapon) {
            case -1:
            case wp_knife: // draw nothing
            case wp_spear:
                break;
            case wp_pistol:
            case wp_machinegun:
                SETFONTCOLOR(0x0,0x0);
                SETPRINTXY(281 + printHorizAdjust,169 + printVertAdjust);
                US_PrintUnsignedPadded(gamestate.primaryammo,3);

                SETFONTCOLOR(0x10,0x0);
                SETPRINTXY(280 + printHorizAdjust,168 + printVertAdjust);
                US_PrintUnsignedPadded(gamestate.primaryammo,3);
                break;
            case wp_mk3:
            case wp_mk4:
                SETFONTCOLOR(0x0,0x0);
                SETPRINTXY(281 + printHorizAdjust,169 + printVertAdjust);
                US_PrintUnsignedPadded(gamestate.secondaryammo,3);

                SETFONTCOLOR(0x10,0x0);
                SETPRINTXY(280 + printHorizAdjust,168 + printVertAdjust);
                US_PrintUnsignedPadded(gamestate.secondaryammo,3);
                break;
            case wp_rifle:
                SETFONTCOLOR(0x0,0x0);
                SETPRINTXY(281 + printHorizAdjust,169 + printVertAdjust);
                US_PrintUnsignedPadded(gamestate.rifleammo,3);

                SETFONTCOLOR(0x10,0x0);
                SETPRINTXY(280 + printHorizAdjust,168 + printVertAdjust);
                US_PrintUnsignedPadded(gamestate.rifleammo,3);
                break;
            case wp_launcher:
                SETFONTCOLOR(0x0,0x0);
                SETPRINTXY(281 + printHorizAdjust,169 + printVertAdjust);
                US_PrintUnsignedPadded(gamestate.rocketammo,3);

                SETFONTCOLOR(0x10,0x0);
                SETPRINTXY(280 + printHorizAdjust,168 + printVertAdjust);
                US_PrintUnsignedPadded(gamestate.rocketammo,3);
                break;
            case wp_medkit:
                SETFONTCOLOR(0x0,0x0);
                SETPRINTXY(281 + printHorizAdjust,169 + printVertAdjust);
                US_PrintUnsignedPadded(gamestate.medkits,3);

                SETFONTCOLOR(0x10,0x0);
                SETPRINTXY(280 + printHorizAdjust,168 + printVertAdjust);
                US_PrintUnsignedPadded(gamestate.medkits,3);
                break;
        }
    } else {
        switch(gamestate.weapon) {
            case -1:
            case wp_knife: // draw nothing
            case wp_spear:
                LatchNumber(26,16,3,-1);
                break;
            case wp_pistol:
            case wp_machinegun:
                LatchNumber(26,16,3,gamestate.primaryammo);
                break;
            case wp_mk3:
            case wp_mk4:
                LatchNumber(26,16,3,gamestate.secondaryammo);
                break;
            case wp_rifle:
                LatchNumber(26,16,3,gamestate.rifleammo);
                break;
            case wp_launcher:
                LatchNumber(26,16,3,gamestate.rocketammo);
                break;
            case wp_medkit:
                LatchNumber(26,16,3,gamestate.medkits);
                break;
       }
    }
}

/*
===============
=
= GiveAmmo
=
===============
*/

void GiveAmmo (byte type, short amount)
{
    short maxAmount;

    switch (type) {
        case 0: //primary ammo (mg & pistol)
            gamestate.primaryammo += amount;
            maxAmount = (gamestate.backpack ? MAXPRIMARYAMMO_B : MAXPRIMARYAMMO);
            if (gamestate.primaryammo > maxAmount)
                gamestate.primaryammo = maxAmount;
            break;
        case 1: // secondary ammo (mk3 & mk4)
            gamestate.secondaryammo += amount;
            maxAmount = (gamestate.backpack ? MAXSECONDARYAMMO_B : MAXSECONDARYAMMO);
            if (gamestate.secondaryammo > maxAmount)
                gamestate.secondaryammo = maxAmount;
            break;
        case 2:
            gamestate.rifleammo += amount;
            maxAmount = (gamestate.backpack ? MAXRIFLEAMMO_B : MAXRIFLEAMMO);
            if (gamestate.rifleammo > maxAmount)
                gamestate.rifleammo = maxAmount;
            break;
        case 3:
            gamestate.rocketammo += amount;
            maxAmount = (gamestate.backpack ? MAXROCKETAMMO_B : MAXROCKETAMMO);
            if (gamestate.rocketammo > maxAmount)
                gamestate.rocketammo = maxAmount;
            break;
    }

    /*if (!gamestate.ammo)                            // knife was out
    {
        if (!gamestate.attackframe)
        {
            gamestate.weapon = gamestate.chosenweapon;
            DrawWeapon ();
        }
    }*/
    DrawAmmo();
}

//===========================================================================

/*
==================
=
= GiveKey
=
==================
*/

void GiveKey (int key)
{
    gamestate.keys |= (1<<key);
    DrawKeys ();
}

/*
==================
=
= HasMaxAmmo
=
==================
*/

boolean HasMaxAmmo (weapontype weapon) {
    switch (weapon) {
        case wp_pistol:
        case wp_machinegun:
            if (gamestate.primaryammo == (gamestate.backpack ? MAXPRIMARYAMMO_B : MAXPRIMARYAMMO))
                return true;
            break;
        case wp_mk3:
        case wp_mk4:
           if (gamestate.secondaryammo == (gamestate.backpack ? MAXSECONDARYAMMO_B : MAXSECONDARYAMMO))
                return true;
            break;
        case wp_rifle:
            if (gamestate.rifleammo == (gamestate.backpack ?  MAXRIFLEAMMO_B : MAXRIFLEAMMO))
                return true;
            break;
        case wp_launcher:
            if (gamestate.rocketammo == (gamestate.backpack ? MAXROCKETAMMO_B : MAXROCKETAMMO))
                return true;
            break;
        case wp_medkit:
            if (gamestate.medkits == (gamestate.backpack ? MAXMEDKITS_B : MAXMEDKITS))
                return true;
            break;
        case wp_knife:
        case wp_spear:
            return true;
            break;
    }
    return false;
}



/*
=============================================================================

                                MOVEMENT

=============================================================================
*/


/*
===================
=
= GetBonus
=
===================
*/
void GetBonus (statobj_t *check)
{
    switch (check->itemnumber)
    {

// --- Keys
        case bo_key1:
        case bo_key2:
        case bo_key3:
        case bo_key4:
            GiveKey (check->itemnumber - bo_key1);
            SD_PlaySound (GETKEYSND);
            break;

// --- Treasure objects 
        case bo_cross:
            SD_PlaySound (BONUS1SND);
            GivePoints (100);
            gamestate.treasurecount++;
#ifdef EODSPECIAL
            if (gamestate.mapon == 30)
                CheckGong();
#endif
            break;

        case bo_chalice:
            SD_PlaySound (BONUS2SND);
            GivePoints (500);
            gamestate.treasurecount++;
#ifdef EODSPECIAL
            if (gamestate.mapon == 30)
                CheckGong();
#endif
            break;

        case bo_bible:
            SD_PlaySound (BONUS3SND);
            GivePoints (1000);
            gamestate.treasurecount++;
#ifdef EODSPECIAL
            if (gamestate.mapon == 30)
                CheckGong();
#endif
            break;

        case bo_diamond:
            SD_PlaySound (NOITEMSND);
            GivePoints(2500);
            gamestate.treasurecount++;
#ifdef EODSPECIAL
            if (gamestate.mapon == 30)
                CheckGong();
#endif
            break;

        case bo_crown:
            SD_PlaySound (BONUS4SND);
            GivePoints (5000);
            gamestate.treasurecount++;
#ifdef EODSPECIAL
            if (gamestate.mapon == 30)
                CheckGong();
#endif
            break;

// --- Health objects
        case    bo_gibs:
            if (gamestate.health > 10)
                return;

            SD_PlaySound (SLURPIESND);
            HealSelf (1);
            break;

        case bo_alpo:
            if (gamestate.health >= 100)
                return;

            SD_PlaySound (NAZIHITPLAYERSND);
            HealSelf (4);
            break;

        case bo_food:
            if (gamestate.health >= 100)
                return;

            SD_PlaySound (HEALTH1SND);
            HealSelf (10);
            break;

        case bo_firstaid:
            if (gamestate.health >= 100)
                return;

            SD_PlaySound (HEALTH2SND);
            HealSelf (25);
            break;

        case bo_goodberry:
            if (gamestate.health >= 100)
                return;

            SD_PlaySound (HEALTH1SND);
            HealSelf(10);
            break;

        case bo_badberry:
            TakeDamage(16,NULL);
            break;

        case bo_portmed:
            if (HasMaxAmmo(wp_medkit))
                return;

            SD_PlaySound(HEALTH2SND);
            gamestate.medkits++;
            DrawAmmo();
            break;

// --- Pistol/MG stuff
        case bo_clip:
            if (HasMaxAmmo(wp_pistol)) return;

            SD_PlaySound (GETAMMOSND);
            GiveAmmo (0,8);
            break;

        case bo_clip2:
            if (HasMaxAmmo(wp_pistol)) return;

            SD_PlaySound (GETAMMOSND);
            GiveAmmo (0,4);
            break;

        case bo_25clip:
            if (HasMaxAmmo(wp_pistol)) return;

            SD_PlaySound (GETAMMOBOXSND);
            GiveAmmo (0,25);
            break;

        case bo_pistol:
            if ((gamestate.weapons & WP_PISTOL) && HasMaxAmmo(wp_pistol)) return;
            SD_PlaySound (GETAMMOSND);
            GiveWeapon(wp_pistol);
            GetMessage("You picked up a pistol", DEF_MSG_CLR);
            break;

        case bo_machinegun:
            if ((gamestate.weapons & WP_MACHINEGUN) && HasMaxAmmo(wp_machinegun)) return;
            SD_PlaySound (GETMACHINESND);
            GiveWeapon (wp_machinegun);
            GetMessage("You picked up a machinegun", DEF_MSG_CLR);
            break;

// --- MK3/MK4 stuff

        case bo_sclip:
            if (HasMaxAmmo(wp_mk3)) return;

            SD_PlaySound (GETAMMOSND);
            GiveAmmo (1,8);
            break;

        case bo_s25clip:
            if (HasMaxAmmo(wp_mk3)) return;

            SD_PlaySound (GETAMMOBOXSND);
            GiveAmmo (1,25);
            break;

        case bo_mk3:
            if ((gamestate.weapons & WP_MK4) && HasMaxAmmo(wp_mk3)) return;
            SD_PlaySound (GETGATLINGSND);
            facetimes = 38;
            GiveWeapon (wp_mk3);

            StatusDrawFace (GOTGATLINGPIC);

            facecount = 0;
            GetMessage("You picked up an MCS Mk3", DEF_MSG_CLR);
            break;

        case bo_mk4:
            if ((gamestate.weapons & wp_mk4) && HasMaxAmmo(wp_mk4)) return;
            SD_PlaySound (DONOTHINGSND);
            facetimes = 38;
            GiveWeapon (wp_mk4);

            StatusDrawFace (GOTGATLINGPIC);
            facecount = 0;
            GetMessage("You picked up an MCS Mk4", DEF_MSG_CLR);
            break;

// --- Rifle stuff
        case bo_rifleclip:
            if (HasMaxAmmo(wp_rifle)) return;
            SD_PlaySound(GETAMMOSND);
            GiveAmmo(2, 4);
            break;

        case bo_rifle:
            if ((gamestate.weapons & WP_RIFLE) && HasMaxAmmo(wp_rifle)) return;
            SD_PlaySound(GETMACHINESND);
            facetimes = 38;
            GiveWeapon (wp_rifle);

            StatusDrawFace (GOTGATLINGPIC);
            facecount = 0;
            GetMessage("You picked up a stealth rifle", DEF_MSG_CLR);
            break;

// --- Missile launcher stuff
        case bo_rocketbox:
            if (HasMaxAmmo(wp_launcher)) return;
            SD_PlaySound(GETAMMOSND);
            GiveAmmo(3,2);
            break;

        case bo_launcher:
            if ((gamestate.weapons & WP_LAUNCHER) && HasMaxAmmo(wp_launcher)) return;
            SD_PlaySound(DONOTHINGSND);
            facetimes = 38;
            GiveWeapon (wp_launcher);

            StatusDrawFace (GOTGATLINGPIC);
            facecount = 0;
            GetMessage("You picked up a rocket launcher", DEF_MSG_CLR);
            break;

// Artifacts / Packs
        case bo_backpack:
            SD_PlaySound(GETAMMOBOXSND);
            gamestate.backpack = true;
            GiveAmmo(0,40);
            GiveAmmo(1,20);
            GiveAmmo(2,4);
            GiveAmmo(3,2);
            GetMessage("You picked up a backpack",DEF_MSG_CLR);
            break;

        case bo_fullheal:
            SD_PlaySound (BONUS1UPSND);
            if(gamestate.health < 125)
                gamestate.health = 125;
            else if(gamestate.health < 150)
                gamestate.health = 150;
            else if(gamestate.health < 175)
                gamestate.health = 175;
            else
                gamestate.health = 200;

            DrawHealth();
            if (gamestate.difficulty != gd_extreme) 
                GiveExtraMan ();
            break;

        case bo_fury:
            if (gamestate.furytics < 0)
                gamestate.furytics = FURYLENGTH * 70;
            else
                gamestate.furytics += FURYLENGTH * 70;

            if (!gamestate.fury)
                gamestate.lastweaponfury = gamestate.weapon;

            gamestate.fury = true;
            GetTimer(FURYLENGTH, T_FURY);
            GetMessage("Knife fury!", DEF_MSG_CLR);
            SD_PlaySound(BONUS1UPSND);

            if (gamestate.weapon != wp_knife) {
                NEXTWEAPON(wp_knife);
            }
            break;

        case bo_god:
            if (gamestate.godmodetics < 0)
                gamestate.godmodetics = GODMODELENGTH * 70;
            else
                gamestate.godmodetics += GODMODELENGTH * 70;

            gamestate.godmode = true;
            GetTimer(GODMODELENGTH, T_GOD);
            GetMessage("God mode!", DEF_MSG_CLR);
            SD_PlaySound(SELECTITEMSND);
            break;

        case bo_death:
            if (!gamestate.godmode && !gamestate.godcheat) {
                SD_PlaySound(TAKEDAMAGESND);
                gamestate.health = 0;
                DrawHealth();
                DrawFace();
                playstate = ex_died;
            } else
                SD_PlaySound(AHHHGSND);
            break;

        case bo_save:
            if (gamestate.lives >= 9) 
                return;

            SD_PlaySound (BONUS1UPSND);
            gamestate.lives++;
            DrawLives();
            break;

// Bad stuff

        case bo_landmine:
            SD_PlaySound(LANDMINESND);
            TakeRandomDamage(25,10);
            break;

        case bo_poison:
            TakeRandomDamage(10,10);
            break;


#ifdef SPEAR
        case    bo_spear:
            spearflag = true;
            spearx = player->x;
            speary = player->y;
            spearangle = player->angle;
            playstate = ex_completed;
            break;
#endif
		case bo_secret:
			gamestate.secretcount++;
            GetMessage("You found a secret area", DEF_MSG_CLR);
			break;

		case bo_supersecret:
			gamestate.secretcount++;
            GetMessage("You found a super secret area", SPC_MSG_CLR);
			break;

		case bo_grdspn1:
			SpawnTriggeredActors(en_guard, SPN_TAR_GRD_1);
			break;

		case bo_grdspn2:
			SpawnTriggeredActors(en_guard, SPN_TAR_GRD_2);
			break;

		case bo_ssspn1:
			SpawnTriggeredActors(en_ss, SPN_TAR_SS_1);
			break;

		case bo_ssspn2:
			SpawnTriggeredActors(en_ss, SPN_TAR_SS_2);
			break;

		case bo_ofcspn1:
			SpawnTriggeredActors(en_officer, SPN_TAR_OFC_1);
			break;

		case bo_ofcspn2:
			SpawnTriggeredActors(en_officer, SPN_TAR_OFC_2);
			break;
    }

	switch(check->itemnumber) {
		case bo_secret:
		case bo_supersecret:
		case bo_grdspn1:
		case bo_grdspn2:
		case bo_ssspn1:
		case bo_ssspn2:
		case bo_ofcspn1:
		case bo_ofcspn2:
			break;
		default:
			StartBonusFlash ();
	}
		
    check->flags &= ~FL_BONUS;
    check->shapenum = -1;                   // remove from list
    statat[check->tilex][check->tiley] = 0;
}

int pdirs[4][2]={{0,-1},{1,0},{0,1},{-1,0}}; 
/*
===================
=
= TryMove
=
= returns true if move ok
= debug: use pointers to optimize
===================
*/

boolean TryMove (objtype *ob)
{
    int         xl,yl,xh,yh,x,y;
    objtype    *check;
    int32_t     deltax,deltay;

    xl = (ob->x-PLAYERSIZE) >>TILESHIFT;
    yl = (ob->y-PLAYERSIZE) >>TILESHIFT;

    xh = (ob->x+PLAYERSIZE) >>TILESHIFT;
    yh = (ob->y+PLAYERSIZE) >>TILESHIFT;

#define PUSHWALLMINDIST PLAYERSIZE

    //
    // check for solid walls
    //
    for (y=yl;y<=yh;y++)
    {
        for (x=xl;x<=xh;x++)
        {
            if (MAPSPOT(x,y,1) == CLONETOILETTILE || MAPSPOT(x,y,1) == TURRETTILE) 
                return false;

            check = actorat[x][y];
            if (check && !ISPOINTER(check))
            {
                for (int i = 0; i < MAXPWALLS; i++) {
                    if (pwall[i].x == x && pwall[i].y == y)
                    {
                        pw = i;
                        break;
                    } 
                }

                if(tilemap[x][y]==BIT_WALL && x==pwallx && y==pwally)
                {
                    switch(pwalldir)
                    {
                        case di_north:
                            if(ob->y-PUSHWALLMINDIST<=(pwally<<TILESHIFT)+((63-pwallpos)<<10))
                                return false;
                            break;
                        case di_west:
                            if(ob->x-PUSHWALLMINDIST<=(pwallx<<TILESHIFT)+((63-pwallpos)<<10))
                                return false;
                            break;
                        case di_east:
                            if(ob->x+PUSHWALLMINDIST>=(pwallx<<TILESHIFT)+(pwallpos<<10))
                                return false;
                            break;
                        case di_south:
                            if(ob->y+PUSHWALLMINDIST>=(pwally<<TILESHIFT)+(pwallpos<<10))
                                return false;
                            break;
                    }
                }
                else return false;
            }
        }
    }

    //
    // check for actors
    //
    if (yl>0)
        yl--;
    if (yh<MAPSIZE-1)
        yh++;
    if (xl>0)
        xl--;
    if (xh<MAPSIZE-1)
        xh++;

    for (y=yl;y<=yh;y++)
    {
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (ISPOINTER(check) && check != player && (check->flags & FL_SHOOTABLE) )
            {
                deltax = ob->x - check->x;
                if (deltax < -MINPLAYERDIST || deltax > MINPLAYERDIST)
                    continue;
                deltay = ob->y - check->y;
                if (deltay < -MINPLAYERDIST || deltay > MINPLAYERDIST)
                    continue;

                return false;
            }
        }
    }

    return true;
}


/*
===================
=
= ClipMove
=
===================
*/

void ClipMove (objtype *ob, int32_t xmove, int32_t ymove)
{
    int32_t    basex,basey;

    basex = ob->x;
    basey = ob->y;

    ob->x = basex+xmove;
    ob->y = basey+ymove;
    if (TryMove (ob))
        return;

#ifndef REMDEBUG
    if (noclip && ob->x > 2*TILEGLOBAL && ob->y > 2*TILEGLOBAL
        && ob->x < (((int32_t)(mapwidth-1))<<TILESHIFT)
        && ob->y < (((int32_t)(mapheight-1))<<TILESHIFT) )
        return;         // walk through walls
#endif

    ob->x = basex+xmove;
    ob->y = basey;
    if (TryMove (ob))
        return;

    ob->x = basex;
    ob->y = basey+ymove;
    if (TryMove (ob))
        return;

    ob->x = basex;
    ob->y = basey;
}

//==========================================================================

/*
===================
=
= VictoryTile
=
===================
*/

void VictoryTile (void)
{
#ifndef SPEAR
    SpawnBJVictory ();
#endif

    gamestate.victoryflag = true;
}

/*
===================
=
= Thrust
=
===================
*/

// For player movement in demos exactly as in the original Wolf3D v1.4 source code
static fixed FixedByFracOrig(fixed a, fixed b)
{
    int sign = 0;
    if(b == 65536) b = 65535;
    else if(b == -65536) b = 65535, sign = 1;
    else if(b < 0) b = (-b), sign = 1;

    if(a < 0)
    {
        a = -a;
        sign = !sign;
    }
    fixed res = (fixed)(((int64_t) a * b) >> 16);
    if(sign)
        res = -res;
    return res;
}

/*
=====================
=
= Thurst
=
=====================
*/

void Thrust (int angle, int32_t speed)
{
    int32_t xmove,ymove;
    unsigned offset;


    //
    // ZERO FUNNY COUNTER IF MOVED!
    //
#ifdef SPEAR
    if (speed)
        funnyticount = 0;
#endif

    thrustspeed += speed;
    //
    // moving bounds speed
    //
    if (speed >= MINDIST*2)
        speed = MINDIST*2-1;

    xmove = DEMOCHOOSE_ORIG_SDL(
                FixedByFracOrig(speed, costable[angle]),
                FixedMul(speed,costable[angle]));
    ymove = DEMOCHOOSE_ORIG_SDL(
                -FixedByFracOrig(speed, sintable[angle]),
                -FixedMul(speed,sintable[angle]));

    ClipMove(player,xmove,ymove);

    player->tilex = (short)(player->x >> TILESHIFT);                // scale to tile values
    player->tiley = (short)(player->y >> TILESHIFT);

    offset = (player->tiley<<mapshift)+player->tilex;
    player->areanumber = *(mapsegs[0] + offset) -AREATILE;

    switch(*(mapsegs[1] + offset)) {
        case EXITTILE:
            VictoryTile();
            break;

        case WARPEASTTILE:
            WarpPlayer(EAST);
            break;

        case WARPWESTTILE:
            WarpPlayer(WEST);
            break;

        case INVISWARPTILE: // special for hell level
            WarpPlayer(WESTINSTANT);
            break;

        case ENDLEVELTILE:
            SD_PlaySound(YEAHSND);
            SD_WaitSoundDone();
            if (MAPSPOT(player->tilex,player->tiley,0) == ALTELEVATORTILE) // secret floor code
                playstate = ex_secretlevel;
            else
                playstate = ex_completed;
            break;
    }

}

/*
===================
=
= WarpPlayer
=
===================
*/

void WarpPlayer (int facedir)
{
    objtype  *check;

    gamestate.attackframe = 0;
    gamestate.attackcount = 0;
    gamestate.weaponframe = 0;

    if (facedir != WESTINSTANT) {
        SD_PlaySound (WALK1SND);
        SDL_Delay(500);
        fizzlein = true;
    } else {
        SD_PlaySound (SWISHSND);
    }

    int temp1 = levelinfo.ceiling1;
    int temp2 = levelinfo.floor1;

    levelinfo.ceiling1 = levelinfo.ceiling2;
    levelinfo.floor1 = levelinfo.floor2;
    levelinfo.ceiling2 = temp1;
    levelinfo.floor2 = temp2;
    
    switch (facedir) {
        case EAST:
            for (int x = 0; x < 2; x++) {
                check = (objtype *)((uintptr_t)actorat[warpex+x][warpey]);
                if (check > objlist && (check->flags & FL_SHOOTABLE)) {
                    KillActor(check);
                }
            }
             SpawnPlayer(warpex,warpey,EAST);
             break;
        case WEST:
        case WESTINSTANT:
            for (int x = 0; x < 2; x++) {
                check = (objtype *)((uintptr_t)actorat[warpwx-x][warpwy]);
                if (check > objlist && (check->flags & FL_SHOOTABLE)) {
                    KillActor(check);
                }
            }
            SpawnPlayer(warpwx,warpwy,WEST);
            break;
    }
    resetAttack = true;
}

/*
=================
=
= Regen/Burn Player
=
=================
*/
int regenTics = 0;

void RegenPlayer(void)
{
    if (gamestate.health < 100)
        regenTics += tics;

    if (regenTics > 70) {
        HealSelf(2);
        SD_PlaySound(ENDBONUS2SND);
        regenTics = 0;
    }
}

// -----------------------
int burnTics = 0;

void BurnPlayer(void) {
    if (gamestate.godmode || gamestate.godcheat) return;

    burnTics += tics;

    if (burnTics > 70) {
        gamestate.health--;
        DrawHealth();
        SD_PlaySound(TAKEDAMAGESND);
        burnTics = 0;
    }
}

/*
===============
=
= GetPlayerAngle
=
===============
*/
int GetPlayerAngle (void) {
    if (player->angle < ANGLES/8 || player->angle > 7*ANGLES/8) {
        return di_east;
    } else if (player->angle < 3*ANGLES/8) {
        return di_north;
    } else if (player->angle < 5*ANGLES/8) {
        return di_west;
    } else {
        return di_south;
    }
}
/*
=============================================================================

                                ACTIONS

=============================================================================
*/


/*
===============
=
= Cmd_Fire
=
===============
*/

void Cmd_Fire (void)
{
    if (gamestate.changingweapon || gamestate.reloadtics > 0)
        return; 

    buttonheld[bt_attack] = true;

    if (gamestate.weapon == wp_medkit) {
        if (gamestate.health < 100) {
            StartBonusFlash();
            SD_PlaySound(NAZIHITPLAYERSND);
            HealSelf(25);
            gamestate.medkits--;
            DrawAmmo();
            if (!gamestate.medkits) {
                NEXTWEAPON(gamestate.lastweapon);
            }
        } else {
            NEXTWEAPON(gamestate.lastweapon);
        }
        return;
    }

    gamestate.weaponframe = 0;

    player->state = &s_attack;

    gamestate.attackframe = 0;
    gamestate.attackcount =
        attackinfo[gamestate.weapon][gamestate.attackframe].tics;
    gamestate.weaponframe =
        attackinfo[gamestate.weapon][gamestate.attackframe].frame;
}

//===========================================================================

/*
===============
=
= Cmd_Use
=
===============
*/

void Cmd_Use (void)
{
    int     checkx,checky,doornum,dir,currentTile;
    boolean elevatorok;

    //
    // find which cardinal direction the player is facing
    //
    dir = GetPlayerAngle();

    switch (dir) {
        case di_east:
            checkx = player->tilex + 1;
            checky = player->tiley;
            elevatorok = true;
        break;

        case di_north:
            checkx = player->tilex;
            checky = player->tiley-1;
            elevatorok = false;
        break;

        case di_west:
            checkx = player->tilex - 1;
            checky = player->tiley;
            elevatorok = true;
        break;

        case di_south:
            checkx = player->tilex;
            checky = player->tiley + 1;
            elevatorok = false;
        break;
    }

    doornum = tilemap[checkx][checky];
    currentTile = MAPSPOT(checkx,checky,1);

    //
    // OBJECT CHECKS
    //
    switch (currentTile) {
        case PUSHABLETILE:
            PushWall (checkx,checky,dir);
            return;
            break;

        case ARMORTILE:
            if (!buttonheld[bt_use])
                SD_PlaySound(YOURANGSND);
            break;

        case TOILETTILE:
        case CLONETOILETTILE:
            if (!buttonheld[bt_use])
                SD_PlaySound(TOILETSND);
            break;

        case RADIOTILE:
            if (!buttonheld[bt_use])
                SD_PlaySound(RADIOCASTSND);
            break;

        // Wall switch stuff
        case SW_FIREWALL_1:
        case SW_FIREWALL_2:
        case SW_FIREWALL_3:
        case SW_FIREWALL_4:
        case SW_FIREWALL_5:
        case SW_FIREWALL_6:
        case SW_FIREWALL_7:
        case SW_FIREWALL_8:
            if (!buttonheld[bt_use] && (doornum == SWITCHOFFTILE || doornum == SWITCHONTILE)) {
                SD_PlaySound(LEVELDONESND); // play the sound
                
                if (doornum == SWITCHOFFTILE) // change the tile graphic
                    tilemap[checkx][checky]++;
                else
                    tilemap[checkx][checky]--;

                fwall[currentTile-SW_FIREWALL_1].active ^= 1;
            }
            break;

        case SW_PUSHWALL_1:
            if (!buttonheld[bt_use] && (doornum == SWITCHOFFTILE || doornum == SWITCHONTILE)) {

                SD_PlaySound(LEVELDONESND);
                if (doornum == SWITCHOFFTILE)
                    tilemap[checkx][checky]++; // turn the switch on
                else
                    tilemap[checkx][checky]--;

                for(int x = 0; x < MAPSIZE; x++) {
                    for(int y = 0; y < MAPSIZE; y++) {
                        switch(MAPSPOT(x,y,1)) {
                            case PUSHWALL_1_E:
                                PushWall(x,y,EAST);
                                break;
                            case PUSHWALL_1_N:
                                PushWall(x,y,NORTH);
                                break;
                            case PUSHWALL_1_W:
                                PushWall(x,y,WEST);
                                break;
                            case PUSHWALL_1_S:
                                PushWall(x,y,SOUTH);
                                break;
                        }

                    }
                }
            }
            break;

        case SW_PUSHWALL_2:
            if (!buttonheld[bt_use] && (doornum == SWITCHOFFTILE || doornum == SWITCHONTILE)) {
                SD_PlaySound(LEVELDONESND);
                if (doornum == SWITCHOFFTILE)
                    tilemap[checkx][checky]++; // turn the switch on
                else
                    tilemap[checkx][checky]--;

                for(int x = 0; x < MAPSIZE; x++) {
                    for(int y = 0; y<MAPSIZE; y++) {
                        switch(MAPSPOT(x,y,1)) {
                            case PUSHWALL_2_E:
                                PushWall(x,y,EAST);
                                break;
                            case PUSHWALL_2_N:
                                PushWall(x,y,NORTH);
                                break;
                            case PUSHWALL_2_W:
                                PushWall(x,y,WEST);
                                break;
                            case PUSHWALL_2_S:
                                PushWall(x,y,SOUTH);
                                break;
                        }

                    }
                }
            }
            break;

       case SW_PUSHWALL_3:
            if (!buttonheld[bt_use] && (doornum == SWITCHOFFTILE || doornum == SWITCHONTILE)) {
                SD_PlaySound(LEVELDONESND);
                if (doornum == SWITCHOFFTILE)
                    tilemap[checkx][checky]++; // turn the switch on
                else
                    tilemap[checkx][checky]--;

                for(int x = 0; x < MAPSIZE; x++) {
                    for(int y = 0; y<MAPSIZE; y++) {
                        switch(MAPSPOT(x,y,1)) {
                            case PUSHWALL_3_E:
                                PushWall(x,y,EAST);
                                break;
                            case PUSHWALL_3_N:
                                PushWall(x,y,NORTH);
                                break;
                            case PUSHWALL_3_W:
                                PushWall(x,y,WEST);
                                break;
                            case PUSHWALL_3_S:
                                PushWall(x,y,SOUTH);
                                break;
                        }

                    }
                }
            }
            break;

      case SW_PUSHWALL_4:
            if (!buttonheld[bt_use] && (doornum == SWITCHOFFTILE || doornum == SWITCHONTILE)) {
                SD_PlaySound(LEVELDONESND);
                if (doornum == SWITCHOFFTILE)
                    tilemap[checkx][checky]++; // turn the switch on
                else
                    tilemap[checkx][checky]--;

                for(int x = 0; x < MAPSIZE; x++) {
                    for(int y = 0; y < MAPSIZE; y++) {
                        switch(MAPSPOT(x,y,1)) {
                            case PUSHWALL_4_E:
                                PushWall(x,y,EAST);
                                break;
                            case PUSHWALL_4_N:
                                PushWall(x,y,NORTH);
                                break;
                            case PUSHWALL_4_W:
                                PushWall(x,y,WEST);
                                break;
                            case PUSHWALL_4_S:
                                PushWall(x,y,SOUTH);
                                break;
                        }

                    }
                }
            }
            break;

      case SW_LIGHTS:
            if (!buttonheld[bt_use] && (doornum == SWITCHOFFTILE || doornum == SWITCHONTILE)) {
                int before, after;

                SD_PlaySound(LEVELDONESND);
                if (doornum == SWITCHOFFTILE) {
                    tilemap[checkx][checky]++; // turn the switch on
                    before = SPR_STAT_LIGHTDEAD;
                    after = SPR_STAT_40;
                    gamestate.lightson = 1;
                    InitLevelShadeTable();
                } else {
                    tilemap[checkx][checky]--;
                    before = SPR_STAT_40;
                    after = SPR_STAT_LIGHTDEAD;
                    gamestate.lightson = 0;
                    InitLevelShadeTable();
                }

                for (int x = 0; x < MAPSIZE; x++) {
                    for (int y = 0; y < MAPSIZE; y++) {
                        if (x == checkx && y == checky) continue;

                        if (MAPSPOT(x,y,1) == SW_LIGHTS) {
                            if (gamestate.lightson) {
                                tilemap[x][y] = SWITCHONTILE; // turn the switch on
                            } else {
                                tilemap[x][y] = SWITCHOFFTILE;
                            }
                        }
                        if (statat[x][y] && statat[x][y]->shapenum == before)
                            statat[x][y]->shapenum = after;
                    }
                }
            }
            break;
          break;
    }
    //
    // ELEVATOR
    //
    if (!buttonheld[bt_use] && doornum == ELEVATORTILE && elevatorok)
    {
        buttonheld[bt_use] = true;

        tilemap[checkx][checky]++;              // flip switch
        if (*(mapsegs[0]+(player->tiley<<mapshift)+player->tilex) == ALTELEVATORTILE)
            playstate = ex_secretlevel;
        else
            playstate = ex_completed;
        SD_PlaySound (LEVELDONESND);
        SD_WaitSoundDone();
    }
    //
    // DOORS
    //
    else if (!buttonheld[bt_use] && doornum & BIT_DOOR)
    {
        buttonheld[bt_use] = true;
        int targetDoor = doornum & ~BIT_DOOR;
        if (doorobjlist[targetDoor].lock != dr_auto)
            OperateDoor (targetDoor);
    }
}

/*
=============================================================================

                                PLAYER CONTROL

=============================================================================
*/



/*
===============
=
= SpawnPlayer
=
===============
*/

void SpawnPlayer (int tilex, int tiley, int dir)
{
    player->obclass = playerobj;
    player->active = ac_yes;
    player->tilex = tilex;
    player->tiley = tiley;
    player->areanumber = (byte) *(mapsegs[0]+(player->tiley<<mapshift)+player->tilex);
    player->x = ((int32_t)tilex<<TILESHIFT)+TILEGLOBAL/2;
    player->y = ((int32_t)tiley<<TILESHIFT)+TILEGLOBAL/2;
    player->state = &s_player;
    player->angle = (1-dir)*90;
    if (player->angle<0)
        player->angle += ANGLES;
    player->flags = FL_NEVERMARK;
    Thrust (0,0);                           // set some variables

    InitAreas ();
}


//===========================================================================

/*
===============
=
= MissleAttack (for player rocket launcher)
=
===============
*/
void MissileAttack (objtype *ob)
{
    madenoise = true;
    SD_PlaySound (TURRETFIRESND); //change to any sound you have
   
    GetNewActor ();

    newobj->creator = player->obclass;
    newobj->state = &s_rocket;
    newobj->ticcount = 1;

    newobj->x = ob->x ;
    newobj->y = ob->y ;
    newobj->tilex = newobj->x >> TILESHIFT;
    newobj->tiley = newobj->y >> TILESHIFT;

    newobj->obclass = rocketobj;
    newobj->dir = nodir;
    newobj->angle = ob->angle;
    newobj->speed = 0x2600l;
    newobj->flags = FL_NEVERMARK | FL_BONUS;
    newobj->active = ac_yes;
} 

/*
===============
=
= SpearAttack (for player spear)
=
===============
*/
void SpearAttack (objtype *ob)
{
    SD_PlaySound (KNIGHTMISSILESND); //change to any sound you have
   
    GetNewActor ();

    newobj->creator = player->obclass;
    newobj->state = &s_spearproj;
    newobj->ticcount = 1;

    newobj->x = ob->x ;
    newobj->y = ob->y ;
    newobj->tilex = newobj->x >> TILESHIFT;
    newobj->tiley = newobj->y >> TILESHIFT;

    newobj->obclass = spearprojobj;
    newobj->dir = nodir;
    newobj->angle = ob->angle;
    newobj->speed = 0x2600l;
    newobj->flags = FL_NEVERMARK | FL_BONUS;
    newobj->active = ac_yes;
} 

/*
===============
=
= T_KnifeAttack
=
= Update player hands, and try to do damage when the proper frame is reached
=
===============
*/

void KnifeAttack (objtype *ob)
{
    objtype *check,*closest;
    int32_t  dist;

    if (gamestate.fury || gamestate.furycheat)
        SD_PlaySound (SWISHSND);
    else
        SD_PlaySound (ATKKNIFESND);

    // actually fire
    dist = 0x7fffffff;
    closest = NULL;
    for (check=ob->next; check; check=check->next)
    {
        if ( (check->flags & FL_SHOOTABLE) && (check->flags & FL_VISABLE)
            && abs(check->viewx-centerx) < shootdelta)
        {
            if (check->transx < dist)
            {
                dist = check->transx;
                closest = check;
            }
        }
    }

    if (!closest || dist > 0x18000l)
        return; // missed

    closest->flags |= FL_STEALTH;

    if (gamestate.fury || gamestate.furycheat) {
        KillActor(closest);
        return;
    }

    // Stealth knife kills
    if ( !(closest->flags & FL_ATTACKMODE) ) {
        switch (closest->dir) {
            case north:
                if (player->y > closest->y && player->tilex == closest->tilex) {
                    KillActor(closest);
                    return;
                }
                break;

            case east:
                if (player->x < closest->x && player->tiley == closest->tiley) {
                    KillActor(closest);
                    return;
                }
                break;

            case south:
                if (player->y < closest->y && player->tilex == closest->tilex) {
                    KillActor(closest);
                    return;
                }
                break;

            case west:
                if (player->x > closest->x && player->tiley == closest->tiley) {
                    KillActor(closest);
                    return;
                }
                break;

            default:
                break;
        }
    }

    // hit something
    DamageActor (closest,US_RndT() >> 4);
}

/*
================
=
= GunAttack
=
================
*/

void    GunAttack (objtype *ob)
{
    objtype *check,*closest,*oldclosest;
    int      damage;
    int      dx,dy,dist;
    int32_t  viewdist;

    switch (gamestate.weapon)
    {
        case wp_pistol:
            if (!ammocheat)
                gamestate.primaryammo--;
            SD_PlaySound (ATKPISTOLSND);
            madenoise = true;
            break;
        case wp_machinegun:
            if (!ammocheat)
                gamestate.primaryammo--;
            SD_PlaySound (ATKMACHINEGUNSND);
            madenoise = true;
            break;
        case wp_mk3:
            if (!ammocheat)
                gamestate.secondaryammo--;
            SD_PlaySound (ATKGATLINGSND);
            madenoise = true;
            break;
        case wp_mk4:
            if (!ammocheat)
                gamestate.secondaryammo--;
            SD_PlaySound (BOSSFIRESND);
            madenoise = true;
            break;
        case wp_rifle:
            if (!ammocheat)
                gamestate.rifleammo--;
            SD_PlaySound (SILENCEDSND);
            madenoise = false;
            break;
        case wp_launcher:
            if (!ammocheat)
                gamestate.rocketammo--;
            MissileAttack(ob);
            DrawAmmo ();
            return;
            break;
        case wp_spear:
           SpearAttack(ob);
           return;
           break;
    }

    DrawAmmo ();

    //
    // find potential targets
    //
    viewdist = 0x7fffffffl;
    closest = NULL;

    while (1)
    {
        oldclosest = closest;

        for (check=ob->next ; check ; check=check->next)
        {
            if ((check->flags & FL_SHOOTABLE) && (check->flags & FL_VISABLE)
                && abs(check->viewx-centerx) < shootdelta)
            {
                if (check->transx < viewdist)
                {
                    viewdist = check->transx;
                    closest = check;
                }
            }
        }

        if (closest == oldclosest)
            return;                                         // no more targets, all missed

        //
        // trace a line from player to enemey
        //
        if (CheckLine(closest))
            break;
    }

    //
    // hit something
    //
    dx = ABS(closest->tilex - player->tilex);
    dy = ABS(closest->tiley - player->tiley);
    dist = dx>dy ? dx:dy;

    switch (gamestate.weapon) {
        case wp_rifle:
            if (closest->flags & FL_BOSS || closest->obclass == supercloneobj)
                return; // bosses are immune to the rifle

            // stealth flag -> killactor
            if (dist < 21) {
                closest->flags |= FL_STEALTH; 
                KillActor(closest);
            }
            return;
        default:
            if (dist<2)
                damage = US_RndT() / 4;
            else if (dist<4)
                damage = US_RndT() / 6;
            else
            {
                if ( (US_RndT() / 12) < dist)           // missed
                    return;
                damage = US_RndT() / 6;
            }
    }
    if (gamestate.weapon == wp_mk4)
        damage = (int)((double)damage * (double)1.25);

    DamageActor (closest,damage);
}

//===========================================================================

/*
===============
=
= VictorySpin
=
===============
*/

void VictorySpin (void)
{
    int32_t    desty;

    if (player->angle > 270)
    {
        player->angle -= (short)(tics * 3);
        if (player->angle < 270)
            player->angle = 270;
    }
    else if (player->angle < 270)
    {
        player->angle += (short)(tics * 3);
        if (player->angle > 270)
            player->angle = 270;
    }

    desty = (((int32_t)player->tiley-5)<<TILESHIFT)-0x3000;

    if (player->y > desty)
    {
        player->y -= tics*4096;
        if (player->y < desty)
            player->y = desty;
    }
}


//===========================================================================

/*
===============
=
= T_Attack
=
===============
*/

void    T_Attack (objtype *ob)
{
    struct  atkinf  *cur;

    UpdateFace ();

    if (gamestate.victoryflag)              // watching the BJ actor
    {
        VictorySpin ();
        return;
    }

    if ( buttonstate[bt_use] && !buttonheld[bt_use] )
        buttonstate[bt_use] = false;

    if ( buttonstate[bt_attack] && !buttonheld[bt_attack])
        buttonstate[bt_attack] = false;

    ControlMovement (ob);

    if (gamestate.victoryflag)             // watching the BJ actor
        return;

    if (resetAttack) {
        resetAttack = false;
        return;
    }

    plux = (word) (player->x >> UNSIGNEDSHIFT);                     // scale to fit in unsigned
    pluy = (word) (player->y >> UNSIGNEDSHIFT);
    player->tilex = (short)(player->x >> TILESHIFT);                // scale to tile values
    player->tiley = (short)(player->y >> TILESHIFT);

    //
    // change frame and fire
    //
    gamestate.attackcount -= (short) tics;
    while (gamestate.attackcount <= 0)
    {
        cur = &attackinfo[gamestate.weapon][gamestate.attackframe];
        switch (cur->attack)
        {
            
            case -1: // primary ammo after-fire check
                ob->state = &s_player;
                switch(gamestate.weapon) {
                    case wp_spear:
                        gamestate.reloadtics = RELOADDELAY;
                    case wp_knife: // infinite ammo for these
                        break;
                    case wp_pistol:
                    case wp_machinegun:
                        if (!gamestate.primaryammo) {
                            SwitchToBestWeapon();
                        }
                        break;
                    case wp_mk3:
                    case wp_mk4:
                        if (!gamestate.secondaryammo) {
                            SwitchToBestWeapon();
                        }
                        break;
                    case wp_rifle:
                        if (!gamestate.rifleammo) {
                            SwitchToBestWeapon();
                        }
                        break;
                    case wp_launcher:
                        if (!gamestate.rocketammo) {
                            SwitchToBestWeapon();
                        } else {
                            gamestate.reloadtics = RELOADDELAY;
                        }
                        break;
                }
                gamestate.attackframe = gamestate.weaponframe = 0;
                return;

           /* case 4: // 
                if (gamestate.secondaryammo && buttonstate[bt_attack])
                    gamestate.attackframe -= 2;*/
            case 1: // single shot case
                GunAttack (ob);
                break;

            case 2: // knife only
                KnifeAttack (ob);
                break;

            case 3: // single shot but automatic (mg)
                if (gamestate.primaryammo && buttonstate[bt_attack])
                    gamestate.attackframe -= 2;
                break;

             case 4: // single shot but automatic (mk3 & mk4)
                if (gamestate.secondaryammo && buttonstate[bt_attack])
                    gamestate.attackframe -= 2;
                break;
        }

        gamestate.attackcount += cur->tics;
        gamestate.attackframe++;
        gamestate.weaponframe =
            attackinfo[gamestate.weapon][gamestate.attackframe].frame;
    }
}



//===========================================================================

/*
===============
=
= T_Player
=
===============
*/

void    T_Player (objtype *ob)
{
    if (gamestate.victoryflag)              // watching the BJ actor
    {
        VictorySpin ();
        return;
    }

    UpdateFace ();
    CheckWeaponChange ();

    if ( buttonstate[bt_use] )
        Cmd_Use ();

    if ( buttonstate[bt_attack] && !buttonheld[bt_attack]) {
        if (HasAmmo(gamestate.weapon)) {
            Cmd_Fire ();
        } else {
            if (gamestate.weapon != wp_medkit) {
                SD_PlaySound(EMPTYCLIPSND);
                SwitchToBestWeapon();
            } else {
                NEXTWEAPON(gamestate.lastweapon);
            }
        }
    }

    ControlMovement (ob);
    if (gamestate.victoryflag)              // watching the BJ actor
        return;

    if (resetAttack)
        resetAttack = false;

    plux = (word) (player->x >> UNSIGNEDSHIFT);                     // scale to fit in unsigned
    pluy = (word) (player->y >> UNSIGNEDSHIFT);
    player->tilex = (short)(player->x >> TILESHIFT);                // scale to tile values
    player->tiley = (short)(player->y >> TILESHIFT);

    if (MAPSPOT(player->tilex,player->tiley,1) == REGENTILE)
        RegenPlayer();

    if (MAPSPOT(player->tilex + 1,player->tiley,0) == HELLTILE || MAPSPOT(player->tilex + 1,player->tiley,0) == FIREPLACETILE
        || MAPSPOT(player->tilex - 1,player->tiley,0) == HELLTILE || MAPSPOT(player->tilex - 1,player->tiley,0) == FIREPLACETILE
        || MAPSPOT(player->tilex,player->tiley + 1,0) == HELLTILE || MAPSPOT(player->tilex,player->tiley + 1,0) == FIREPLACETILE
        || MAPSPOT(player->tilex,player->tiley - 1,0) == HELLTILE || MAPSPOT(player->tilex,player->tiley - 1,0) == FIREPLACETILE)
        BurnPlayer();
}
