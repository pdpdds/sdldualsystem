// WL_ACT1.C

#include "wl_def.h"
#pragma hdrstop

/*
=============================================================================

                                                        STATICS

=============================================================================
*/

statobj_t       statobjlist[MAXSTATS];
statobj_t       *laststatobj;

struct
{
    short      picnum;
    wl_stat_t  type;
    uint32_t   specialFlags;    // they are ORed to the statobj_t flags
} statinfo[] =
{
    {SPR_STAT_0},                           // Puddle                23...
    {SPR_STAT_1,block},                     // Red barrel
    {SPR_STAT_2,block},                     // Table/chairs    
    {SPR_STAT_3,block,FL_FULLBRIGHT},       // Blue floor lamp      
    {SPR_STAT_4,none,FL_FULLBRIGHT},        // Chandelier      
    {SPR_STAT_5,block},                     // Hanged bloody man      
    {SPR_STAT_6,bo_alpo},                   // Small health kit  
    {SPR_STAT_7,block},                     // Regular column     
// 8
    {SPR_STAT_8,block},                     // Plant in blue pot
    {SPR_STAT_9},                           // Bloody skeleton
    {SPR_STAT_10,block},                    // Desk with books
    {SPR_STAT_11,block},                    // Plant in red pot
    {SPR_STAT_12,block},                    // Toilet            
    {SPR_STAT_13,block},                    // Bare table     
    {SPR_STAT_14,none,FL_FULLBRIGHT},       // Cyan ceiling light
    {SPR_STAT_15,block},                    // Bloody cage
// 16
    {SPR_STAT_16,block},                    // Suit of armor
    {SPR_STAT_17,block},                    // Empty cage
    {SPR_STAT_18,block},                    // Skeleton cage
    {SPR_STAT_19},                          // Scattered skeleton
    {SPR_STAT_20,bo_key1},                  // Gold key (key 1)
    {SPR_STAT_21,bo_key2},                  // Bronze key (key 2)
    {SPR_STAT_22,block},                    // Bloody table
    {SPR_STAT_23,block},                    // Wooden barrel
// 24
    {SPR_STAT_24,bo_food},                  // Food tray
    {SPR_STAT_25,bo_firstaid},              // First aid
    {SPR_STAT_26,bo_clip},                  // Primary ammo clip
    {SPR_STAT_27,bo_machinegun},            // Machine gun     
    {SPR_STAT_28,bo_mk3},                   // MK3
    {SPR_STAT_29,bo_cross},                 // Cross           
    {SPR_STAT_30,bo_chalice},               // Chalice         
    {SPR_STAT_31,bo_bible},                 // Bible           
// 32
    {SPR_STAT_32,bo_crown},                 // Crown
    {SPR_STAT_33,block},                    // Fire logs
    {SPR_STAT_34,bo_gibs},                  // Really bloody skeleton
    {SPR_STAT_35,block},                    // Table with mugs
    {SPR_STAT_36,block},                    // Table with glasses
    {SPR_STAT_37,block},                    // Table with skull candle
    {SPR_STAT_38,bo_gibs},                  // Blood puddle
    {SPR_STAT_39,block},                    // Flag
// 40
    {SPR_STAT_40,none,FL_FULLBRIGHT},       // Blue ceiling light
    {SPR_STAT_41},                          // Snowy short bush           
    {SPR_STAT_42},                          // Snowy medium bush
    {SPR_STAT_43},                          // Scattered skulls
    {SPR_STAT_44,block},                    // Sink
    {SPR_STAT_45,block},                    // Snowy tall bush
    {SPR_STAT_46,block},                    // Snowy tall & wide tree
    {SPR_STAT_47},                          // Vines
// 48
    {SPR_STAT_48,block},                    // Snowy tall tree
    {SPR_STAT_49,bo_25clip},                // Primary ammo box
    {SPR_STAT_50,block},                    // Boulder
    {SPR_STAT_51,bo_spear},                 // SPEAR OF DESTINY!
    {SPR_STAT_MK4,bo_mk4},                  // MK4
    {SPR_STAT_BLOODKEY,bo_key3},            // Bloody key (key 3)
    {SPR_STAT_ELEKEY,bo_key4},              // Elevator key (key 4)
    {SPR_STAT_LANDMINE,bo_landmine},        // Landmine
// 56
    {SPR_STAT_COLUMNDEAD},                  // Broken column dead
    {SPR_STAT_SECAMMOCLIP,bo_sclip},        // Secondary ammo clip    
    {SPR_STAT_BLACKCOLUMN,block},           // Stone column (black)
    {SPR_STAT_BUTCHERTABLE,block},          // Butcher table
    {SPR_STAT_BLACKDIAMOND,bo_diamond},     // Black diamond
    {SPR_STAT_ROCKS1},                      // Pile of rocks 1
    {SPR_STAT_ROCKS2},                      // Pile of rocks 2
    {SPR_STAT_BLOODYSKELE,bo_gibs},                 // Bloody skelton
// 64
    {SPR_STAT_HANGPLANT},                   // Hanging plant
    {SPR_STAT_HITLERSTATUE,block},          // Pedestal - Hitler
    {SPR_STAT_SECAMMOBOX,bo_s25clip},       // Secondary ammo box      89...
    {SPR_STAT_TELE1,none,FL_FULLBRIGHT},    // Teleport east artifact
    {SPR_STAT_TELE1,none,FL_FULLBRIGHT},    // Teleport west artifact
    {SPR_STAT_NEWLIFE1,bo_fullheal,FL_FULLBRIGHT},        // 1-up animated   70 (270)
    {SPR_STAT_BACKPACK,bo_backpack},                    // Backpack
    {SPR_STAT_STEEL_3DNS,block,FL_DIR_HORIZ_MID},   // Steel directional (N/S)
// 72 
    {SPR_STAT_STEEL_3DEW,block,FL_DIR_VERT_MID},    // Steel directional (E/W)
    {SPR_STAT_UNIPOLE,block},                     // Coatstand
    {SPR_STAT_BARBF_3D,block,FL_DIR_HORIZ_MID}, // Barb fence
    {SPR_STAT_BARBF_3D,block,FL_DIR_VERT_MID},  // Barb fence
    {SPR_STAT_BARBS_3D,block,FL_DIR_HORIZ_MID}, // Barb fence w/ sign
    {SPR_STAT_BARBS_3D,block,FL_DIR_VERT_MID},  // Barb fence w/ sign
    {SPR_STAT_IRONGATE_3D,block,FL_DIR_HORIZ_MID},  // Rusted gate
    {SPR_STAT_IRONGATE_3D,block,FL_DIR_VERT_MID},   // Rusted gate
// 80
    {SPR_STAT_METALGRATE_3D,block,FL_DIR_HORIZ_MID},    // Metal grille
    {SPR_STAT_METALGRATE_3D,block,FL_DIR_VERT_MID},     // Metal grille
    {SPR_STAT_BRNBRKA_3DNS,none,FL_DIR_HORIZ_MID},     // Brown brick arch
    {SPR_STAT_GOODBERRY,bo_goodberry},                               // Good berry bush
    {SPR_STAT_BADBERRY,bo_badberry},                                // Bad berry bush
    {SPR_STAT_BRNBRKA_3DEW,none,FL_DIR_VERT_MID},      // Brown brick arch
    {SPR_STAT_BONES1},                      // Bones 3 (!)
    {SPR_STAT_SKULLS},                      // Bones 4 (!)
// 88
    {SPR_STAT_SNOWYPINE,block},             // Snowy Tree (EMPTY PED??)
    {SPR_STAT_PEDVASE,block},               // Ped w/ vase
    {SPR_STAT_DESK1,block},                 // Desk w/ typewriter
    {SPR_STAT_FLAG1,block},                 // Swastika banner
    {SPR_STAT_BARETREE,block},              // Dead tree
    {SPR_STAT_WELLFULL,block},              // Well full
    {SPR_STAT_ROCKS3},                      // Small rocks/skulls
    {SPR_STAT_BONES1},                      // Bones 5
// 96
    {SPR_STAT_CLEANSKULL},                  // White skull
    {SPR_STAT_BLOODYSKULL},                 // Bloody skull
    {SPR_STAT_POISON,bo_poison},                      // Toxic puddle
    {SPR_DEMO},                             // ??? Warp entry
    {SPR_STAT_WELLEMPTY,block},             // Well empty
    {SPR_STAT_RUSTBARREL,block},            // Rusty barrel
    {SPR_STAT_SPENTBOX},                    // Spent ammo box
    {SPR_STAT_VINES},                       // Hanging vines 2
// 104
    {SPR_STAT_PISTOL,bo_pistol},                      // Pistol
    {SPR_STAT_RIFLE,bo_rifle},                       // Rifle
    {SPR_STAT_LAUNCHER, bo_launcher},                    // Rocket launcher
    {SPR_STAT_RIFLEAMMO,bo_rifleclip},                   // Rifle ammo clip     
    {SPR_STAT_LAUNCHERAMMOBOX,bo_rocketbox},             // Rockets
    {SPR_STAT_WEB1_3D,none,FL_DIR_HORIZ_MID}, // Spider web 1
    {SPR_STAT_WEB1_3D,none,FL_DIR_VERT_MID},  // Spider web 1
    {SPR_STAT_OTORCH1,block,FL_FULLBRIGHT},               // Outdoor torch
// 112
    {SPR_STAT_ITORCH1,block,FL_FULLBRIGHT},               // Indoor torch
    {SPR_STAT_SAVEGAME1,bo_save,FL_FULLBRIGHT},                   // Save artifact
    {SPR_STAT_FURY1,bo_fury,FL_FULLBRIGHT},                       // Berserk (knife) artifact
    {SPR_STAT_DEATH1,bo_death},                      // Death artifact
    {SPR_DEMO},                             // ??? Tele artifact
    {SPR_STAT_GOD1,bo_god,FL_FULLBRIGHT},                        // God artifact
    {SPR_STAT_FLAME1,none,FL_FULLBRIGHT},                      // Stationary Flame
    {SPR_DEATHCAM,block},                   // Snowy barrel
// 120
    {SPR_STAT_SOF_3DEW,block,FL_DIR_VERT_MID}, // SOF window
    {SPR_STAT_REGEN1,none,FL_FULLBRIGHT},                      // Well of health
    {SPR_STAT_SNOWYWELL,block},               // Well snowy
    {SPR_STAT_SPEARRACK_3D,block,FL_DIR_HORIZ_MID}, // Spear rack
    {SPR_STAT_SPEARRACK_3D,block,FL_DIR_VERT_MID},  // Spear rack
    {SPR_STAT_WOODBARRIER_3D,block,FL_DIR_HORIZ_MID},   // Wood barricade
    {SPR_STAT_WOODBARRIER_3D,block,FL_DIR_VERT_MID},    // Wood barricade
    {SPR_STAT_RADIO,block},                       // Radio on table
// 128
    {SPR_STAT_HANGBLOODSKULL},              // Hanging bloody skull
    {SPR_STAT_SOF_3DNS,block,FL_DIR_HORIZ_MID}, // SOF window
    {SPR_STAT_HATPOLE,block},              // Coatstand
    {SPR_STAT_CANDLES,block},               // Candlestand
    {SPR_STAT_WEB2_3D,none,FL_DIR_HORIZ_MID}, // Spider web 2
    {SPR_STAT_WEB2_3D,none,FL_DIR_VERT_MID},  // Spider web 2
    {SPR_STAT_MINESIGN_3D,block,FL_DIR_HORIZ_MID}, // Minen sign
    {SPR_STAT_MINESIGN_3D,block,FL_DIR_VERT_MID}, // Minen sign
// 136
    {SPR_STAT_CONCRETE_3DNS,block,FL_DIR_HORIZ_MID}, // Prison door
    {SPR_STAT_CONCRETE_3DEW,block,FL_DIR_VERT_MID},  // Prison door
    {SPR_STAT_BRNBRKW_3DNS,block,FL_DIR_HORIZ_MID}, // Brick window
    {SPR_STAT_BRNBRKW_3DEW,block,FL_DIR_VERT_MID},  // Brick window
    {SPR_STAT_HANGCLEANSKULL},                      // Hanging skull
    {SPR_STAT_SKULLPOLE1,block},                    // Bloody skull on pole
    {SPR_STAT_SKULLPOLE2,block},                    // Bloody skulls pole
    {SPR_STAT_FLAG_3D,none,FL_DIR_HORIZ_MID},       // Eagle flag
// 144
    {SPR_STAT_FLAG_3D,none,FL_DIR_VERT_MID},        // Eagle flag
    {SPR_STAT_RAIL_3D,block,FL_DIR_HORIZ_MID},      // Low rail fence
    {SPR_STAT_RAIL_3D,block,FL_DIR_VERT_MID},       // Low rail fence
    {SPR_STAT_PORTMED,bo_portmed},                             // Portable medkit
    {SPR_STAT_SNOWBRICK_3DNS,block,FL_DIR_HORIZ_MID},   // Snowbrick window
    {SPR_STAT_SNOWBRICK_3DEW,block,FL_DIR_VERT_MID},    // Snowbrick window
    {SPR_STAT_GREYBRICK_3DNS,block,FL_DIR_HORIZ_MID},   // Grey brick window
    {SPR_STAT_GREYBRICK_3DEW,block,FL_DIR_VERT_MID},    // Grey brick window
// 152
    {SPR_STAT_LIGHTDEAD},                           // Dead ceiling light
    {SPR_STAT_XMASPINE,block},                      // Xmas pine tree
	{2,bo_secret},
	{2,bo_supersecret},
	{2,bo_grdspn1},
	{2,bo_grdspn2},
	{2,bo_ssspn1},
	{2,bo_ssspn2},
// 160
	{2,bo_ofcspn1},
	{2,bo_ofcspn2},
// END MAP OBJECTS

    {SPR_STAT_26,bo_clip2},                 // Clip            "
    {-1}                                    // terminator
};

/*
===============
=
= InitStaticList
=
===============
*/

void InitStaticList (void)
{
    laststatobj = &statobjlist[0];
}

/*
===============
=
= SpawnStatic
=
===============
*/

void SpawnStatic (int tilex, int tiley, int type)
{
    laststatobj->shapenum = statinfo[type].picnum;
    laststatobj->tilex = tilex;
    laststatobj->tiley = tiley;
    laststatobj->visspot = &spotvis[tilex][tiley];
    laststatobj->numruns = 0;

    switch (statinfo[type].type)
    {
        case block:
            actorat[tilex][tiley] = (objtype *) BIT_WALL;          // consider it a blocking tile
        case none:
            laststatobj->flags = 0;
            laststatobj->itemnumber = 0;
            break;

        case    bo_cross:
        case    bo_chalice:
        case    bo_bible:
        case    bo_crown:
        case    bo_diamond:
            if (!loadedgame)
                gamestate.treasuretotal++;

        case    bo_fullheal:
        case    bo_firstaid:
        case    bo_key1:
        case    bo_key2:
        case    bo_key3:
        case    bo_key4:
        case    bo_clip:
        case    bo_sclip: // MK3 / MK4 ammo
        case    bo_25clip:
        case    bo_s25clip:
        case    bo_machinegun:
        case    bo_mk3:
        case    bo_mk4:
        case    bo_food:
        case    bo_alpo:
        case    bo_gibs:
        case    bo_spear:
        case    bo_landmine:
        case    bo_backpack:
        case    bo_goodberry:
        case    bo_badberry:
        case    bo_poison:
        case    bo_pistol:
        case    bo_rifle:
        case    bo_launcher:
        case    bo_rifleclip:
        case    bo_rocketbox:
        case    bo_save:
        case    bo_fury:
        case    bo_death:
        case    bo_god:
        case    bo_portmed:
            laststatobj->flags = FL_BONUS;
            laststatobj->itemnumber = statinfo[type].type;
            break;

		case bo_secret:
		case bo_supersecret:
			if(!loadedgame)
				gamestate.secrettotal++;
		case bo_grdspn1:
		case bo_grdspn2:
		case bo_ssspn1:
		case bo_ssspn2:
		case bo_ofcspn1:
		case bo_ofcspn2:
			laststatobj->flags = FL_BONUS|FL_INVISIBLE;
            laststatobj->itemnumber = statinfo[type].type;
			break;
    }

    laststatobj->flags |= statinfo[type].specialFlags;

    statat[tilex][tiley] = laststatobj;

    laststatobj++;

    if (laststatobj == &statobjlist[MAXSTATS])
        Quit ("Too many static objects!\n");
}


/*
===============
=
= PlaceItemType
=
= Called during game play to drop actors' items.  It finds the proper
= item number based on the item type (bo_???).  If there are no free item
= spots, nothing is done.
=
===============
*/

void PlaceItemType (int itemtype, int tilex, int tiley)
{
    int type;
    statobj_t *spot;

    //
    // find the item number
    //
    for (type=0; ; type++)
    {
        if (statinfo[type].picnum == -1)                    // end of list
            Quit ("PlaceItemType: couldn't find type!");
        if (statinfo[type].type == itemtype)
            break;
    }

    //
    // find a spot in statobjlist to put it in
    //
    for (spot=&statobjlist[0]; ; spot++)
    {
        if (spot==laststatobj)
        {
            if (spot == &statobjlist[MAXSTATS])
                return;                                     // no free spots
            laststatobj++;                                  // space at end
            break;
        }

        if (spot->shapenum == -1)                           // -1 is a free spot
            break;
    }
    //
    // place it
    //
    spot->shapenum = statinfo[type].picnum;
    spot->tilex = tilex;
    spot->tiley = tiley;
    spot->visspot = &spotvis[tilex][tiley];
    spot->flags = FL_BONUS | statinfo[type].specialFlags;
    spot->itemnumber = statinfo[type].type;
}



/*
=============================================================================

                                  DOORS

doorobjlist[] holds most of the information for the doors

doorposition[] holds the amount the door is open, ranging from 0 to 0xffff
        this is directly accessed by AsmRefresh during rendering

The number of doors is limited to 64 because a spot in tilemap holds the
        door number in the low 6 bits, with the high bit meaning a door center
        and bit 6 meaning a door side tile

Open doors conect two areas, so sounds will travel between them and sight
        will be checked when the player is in a connected area.

Areaconnect is incremented/decremented by each door. If >0 they connect

Every time a door opens or closes the areabyplayer matrix gets recalculated.
        An area is true if it connects with the player's current spor.

=============================================================================
*/

#define DOORWIDTH       0x7800
#define OPENTICS        300

doorobj_t       doorobjlist[MAXDOORS],*lastdoorobj;
short           doornum;

word            doorposition[MAXDOORS];             // leading edge of door 0=closed
                                                    // 0xffff = fully open

byte            areaconnect[NUMAREAS][NUMAREAS];

boolean         areabyplayer[NUMAREAS];


/*
==============
=
= ConnectAreas
=
= Scans outward from playerarea, marking all connected areas
=
==============
*/

void RecursiveConnect (int areanumber)
{
    int i;

    for (i=0;i<NUMAREAS;i++)
    {
        if (areaconnect[areanumber][i] && !areabyplayer[i])
        {
            areabyplayer[i] = true;
            RecursiveConnect (i);
        }
    }
}


void ConnectAreas (void)
{
    memset (areabyplayer,0,sizeof(areabyplayer));
    areabyplayer[player->areanumber] = true;
    RecursiveConnect (player->areanumber);
}


void InitAreas (void)
{
    memset (areabyplayer,0,sizeof(areabyplayer));
    if (player->areanumber < NUMAREAS)
        areabyplayer[player->areanumber] = true;
}



/*
===============
=
= InitDoorList
=
===============
*/

void InitDoorList (void)
{
    memset (areabyplayer,0,sizeof(areabyplayer));
    memset (areaconnect,0,sizeof(areaconnect));

    lastdoorobj = &doorobjlist[0];
    doornum = 0;
}


/*
===============
=
= SpawnDoor
=
===============
*/

void SpawnDoor (int tilex, int tiley, boolean vertical, int lock)
{
    word *map;

    if (doornum==MAXDOORS)
        Quit ("64+ doors on level!");

    doorposition[doornum] = 0;              // doors start out fully closed
    lastdoorobj->tilex = tilex;
    lastdoorobj->tiley = tiley;
    lastdoorobj->vertical = vertical;
    lastdoorobj->lock = lock;
    lastdoorobj->action = dr_closed;

    actorat[tilex][tiley] = (objtype *)(uintptr_t)(doornum | BIT_DOOR);   // consider it a solid wall

    //
    // make the door tile a special tile, and mark the adjacent tiles
    // for door sides
    //
    tilemap[tilex][tiley] = doornum | BIT_DOOR;
    map = mapsegs[0] + (tiley<<mapshift) +tilex;
    if (vertical)
    {
        *map = *(map-1);                        // set area number
        tilemap[tilex][tiley-1] |= BIT_WALL;
        tilemap[tilex][tiley+1] |= BIT_WALL;
    }
    else
    {
        *map = *(map-mapwidth);                                 // set area number
        tilemap[tilex-1][tiley] |= BIT_WALL;
        tilemap[tilex+1][tiley] |= BIT_WALL;
    }

    doornum++;
    lastdoorobj++;
}

//===========================================================================

/*
=====================
=
= OpenDoor
=
=====================
*/

void OpenDoor (int door)
{
    if (doorobjlist[door].action == dr_open)
        doorobjlist[door].ticcount = 0;         // reset open time
    else
        doorobjlist[door].action = dr_opening;  // start it opening
}


/*
=====================
=
= CloseDoor
=
=====================
*/

void CloseDoor (int door)
{
    int     tilex,tiley,area;
    objtype *check;

    //
    // don't close on anything solid
    //
    tilex = doorobjlist[door].tilex;
    tiley = doorobjlist[door].tiley;

    if (actorat[tilex][tiley])
        return;

    if (player->tilex == tilex && player->tiley == tiley)
        return;

    if (doorobjlist[door].vertical)
    {
        if ( player->tiley == tiley )
        {
            if ( ((player->x+MINDIST) >>TILESHIFT) == tilex )
                return;
            if ( ((player->x-MINDIST) >>TILESHIFT) == tilex )
                return;
        }
        check = actorat[tilex-1][tiley];
        if (ISPOINTER(check) && ((check->x+MINDIST) >> TILESHIFT) == tilex )
            return;
        check = actorat[tilex+1][tiley];
        if (ISPOINTER(check) && ((check->x-MINDIST) >> TILESHIFT) == tilex )
            return;
    }
    else if (!doorobjlist[door].vertical)
    {
        if (player->tilex == tilex)
        {
            if ( ((player->y+MINDIST) >>TILESHIFT) == tiley )
                return;
            if ( ((player->y-MINDIST) >>TILESHIFT) == tiley )
                return;
        }
        check = actorat[tilex][tiley-1];
        if (ISPOINTER(check) && ((check->y+MINDIST) >> TILESHIFT) == tiley )
            return;
        check = actorat[tilex][tiley+1];
        if (ISPOINTER(check) && ((check->y-MINDIST) >> TILESHIFT) == tiley )
            return;
    }


    //
    // play door sound if in a connected area
    //
    area = *(mapsegs[0] + (doorobjlist[door].tiley<<mapshift)
        +doorobjlist[door].tilex)-AREATILE;
    if (areabyplayer[area])
    {
      switch (doorobjlist[door].lock) {
             case dr_rust:
                  PlaySoundLocTile(CLOSEDOORSND,doorobjlist[door].tilex,doorobjlist[door].tiley);  // JAB
                  break;
             case dr_wood:
                  PlaySoundLocTile(CLOSEWOODDRSND,doorobjlist[door].tilex,doorobjlist[door].tiley);  // JAB
                  break;
              default:
                  PlaySoundLocTile(CLOSENORMALDRSND,doorobjlist[door].tilex,doorobjlist[door].tiley);  // JAB
                  break;
            }
    }

    doorobjlist[door].action = dr_closing;
    //
    // make the door space solid
    //
    actorat[tilex][tiley] = (objtype *)(uintptr_t)(door | BIT_DOOR);
}



/*
=====================
=
= OperateDoor
=
= The player wants to change the door's direction
=
=====================
*/

void OperateDoor (int door)
{
    int lock;

    lock = doorobjlist[door].lock;
    if (lock >= dr_lock1 && lock <= dr_lock4)
    {
        if ( ! (gamestate.keys & (1 << (lock-dr_lock1) ) ) )
        {
            switch(lock) {
                case dr_lock1:
                    GetMessage("You need the gold key", DEF_MSG_CLR);
                    break;
                case dr_lock2:
                    GetMessage("You need the bronze key", DEF_MSG_CLR);
                    break;
                case dr_lock3:
                    GetMessage("You need the blood key", DEF_MSG_CLR);
                    break;
                case dr_lock4:
                    GetMessage("You need the elevator key", DEF_MSG_CLR);
                    break;
            }
            SD_PlaySound (DOORLOCKEDSND);                // locked
            return;
        }
    }

    switch (doorobjlist[door].action)
    {
        case dr_closed:
        case dr_closing:
            OpenDoor (door);
            break;
        case dr_open:
        case dr_opening:
            CloseDoor (door);
            break;
    }
}


//===========================================================================


/*
===============
=
= IsPlayerNearby - for automatic doors
=
===============
*/

boolean IsPlayerNearby(int door) {
    int tilex = doorobjlist[door].tilex;
    int tiley = doorobjlist[door].tiley;

    if (doorobjlist[door].vertical) {
        if (player->tiley == tiley) {
            if ((player->tilex == tilex - 1 && GetPlayerAngle() == di_east) || (player->tilex == tilex + 1 && GetPlayerAngle() == di_west)) {
                return true;
            }
        }
    } else {
        if (player->tilex == tilex) {
            if ((player->tiley == tiley - 1 && GetPlayerAngle() == di_south) || (player->tiley == tiley + 1 && GetPlayerAngle() == di_north)) {
                return true;
            }
        }
    }

    return false;
}

/*
===============
=
= DoorOpen
=
= Close the door after three seconds
=
===============
*/

void DoorOpen (int door)
{
    if ( (doorobjlist[door].ticcount += (short) tics) >= OPENTICS) {
        if (doorobjlist[door].lock != dr_auto) {
            CloseDoor(door);
        } else if (IsPlayerNearby(door)) {
            doorobjlist[door].ticcount = OPENTICS; // don't want to keep adding to ticcount
        } else {
            CloseDoor(door); // close it when something moves away or the coast is clear
        }
    }  
}



/*
===============
=
= DoorOpening
=
===============
*/

void DoorOpening (int door)
{
    unsigned area1,area2;
    word *map;
    int32_t position;

    position = doorposition[door];
    if (!position)
    {
        //
        // door is just starting to open, so connect the areas
        //
        map = mapsegs[0] + (doorobjlist[door].tiley<<mapshift)
            +doorobjlist[door].tilex;

        if (doorobjlist[door].vertical)
        {
            area1 = *(map+1);
            area2 = *(map-1);
        }
        else
        {
            area1 = *(map-mapwidth);
            area2 = *(map+mapwidth);
        }
        area1 -= AREATILE;
        area2 -= AREATILE;

        if (area1 < NUMAREAS && area2 < NUMAREAS)
        {
            areaconnect[area1][area2]++;
            areaconnect[area2][area1]++;

            if (player->areanumber < NUMAREAS)
                ConnectAreas ();

            if (areabyplayer[area1]) {
                switch (doorobjlist[door].lock) {
                case dr_rust:
                    PlaySoundLocTile(OPENDOORSND,doorobjlist[door].tilex,doorobjlist[door].tiley);  // JAB
                    break;
                case dr_wood:
                    PlaySoundLocTile(OPENWOODDRSND,doorobjlist[door].tilex,doorobjlist[door].tiley);  // JAB
                    break;
                default:
                    PlaySoundLocTile(OPENNORMALDRSND,doorobjlist[door].tilex,doorobjlist[door].tiley);  // JAB
                    break;
               }
            }
        }
    }

    //
    // slide the door by an adaptive amount
    //
    position += tics<<10;
    if (position >= 0xffff)
    {
        //
        // door is all the way open
        //
        position = 0xffff;
        doorobjlist[door].ticcount = 0;
        doorobjlist[door].action = dr_open;
        actorat[doorobjlist[door].tilex][doorobjlist[door].tiley] = 0;
    }

    doorposition[door] = (word) position;
}


/*
===============
=
= DoorClosing
=
===============
*/

void DoorClosing (int door)
{
    unsigned area1,area2;
    word *map;
    int32_t position;
    int tilex,tiley;

    tilex = doorobjlist[door].tilex;
    tiley = doorobjlist[door].tiley;

    if ( ((int)(uintptr_t)actorat[tilex][tiley] != (door | BIT_DOOR))
        || (player->tilex == tilex && player->tiley == tiley) )
    {                       // something got inside the door
        OpenDoor (door);
        return;
    };

    if (doorobjlist[door].lock == dr_auto && IsPlayerNearby(door)) {
        OpenDoor(door);
        return;
    };

    position = doorposition[door];

    //
    // slide the door by an adaptive amount
    //
    position -= tics<<10;
    if (position <= 0)
    {
        //
        // door is closed all the way, so disconnect the areas
        //
        position = 0;

        doorobjlist[door].action = dr_closed;

        map = mapsegs[0] + (doorobjlist[door].tiley<<mapshift) + doorobjlist[door].tilex;

        if (doorobjlist[door].vertical)
        {
            area1 = *(map+1);
            area2 = *(map-1);
        }
        else
        {
            area1 = *(map-mapwidth);
            area2 = *(map+mapwidth);
        }
        area1 -= AREATILE;
        area2 -= AREATILE;

        if (area1 < NUMAREAS && area2 < NUMAREAS)
        {
            areaconnect[area1][area2]--;
            areaconnect[area2][area1]--;

            if (player->areanumber < NUMAREAS)
                ConnectAreas ();
        }
    }

    doorposition[door] = (word) position;
}




/*
=====================
=
= MoveDoors
=
= Called from PlayLoop
=
=====================
*/

void MoveDoors (void)
{
    int door;

    if (gamestate.victoryflag)              // don't move door during victory sequence
        return;

    for (door = 0; door < doornum; door++)
    {
            if (doorobjlist[door].lock == dr_auto && doorobjlist[door].action == dr_closed && IsPlayerNearby(door)) { // scan for objects
                OpenDoor(door);
            }

            switch (doorobjlist[door].action)
            {
                case dr_open:
                    DoorOpen (door);
                    break;

                case dr_opening:
                    DoorOpening(door);
                    break;

                case dr_closing:
                    DoorClosing(door);
                    break;
             }
    }
}


/*
=============================================================================

                                PUSHABLE WALLS

=============================================================================
*/

pwall_t pwall[MAXPWALLS];
int dirs[4][2]={{0,-1},{1,0},{0,1},{-1,0}};
/*
===============
=
= PushWall
=
===============
*/

int GetFreePush(void)
{
   for (int i = 0; i < MAXPWALLS; i++)
      if (!pwall[i].active)
         return i;
   return -1;
}

void PushWall (int checkx, int checky, int dir)
{
    int oldtile, dx, dy;

    int index = GetFreePush();
    if (index == -1)
        return;

    oldtile = tilemap[checkx][checky];
    if (!oldtile)
        return;

    dx = dirs[dir][0];
    dy = dirs[dir][1];

    if (actorat[checkx+dx][checky+dy])
    {
        SD_PlaySound (NOWAYSND);
        return;
    }
    actorat[checkx+dx][checky+dy] = (objtype *)(uintptr_t) (tilemap[checkx+dx][checky+dy] = oldtile);

    pwall[index].x = checkx;
    pwall[index].y = checky;
    pwall[index].dir = dir;
    pwall[index].state = 1;
    pwall[index].pos = 0;
    pwall[index].tile = tilemap[checkx][checky];
    pwall[index].active = true;
    tilemap[checkx][checky] = BIT_WALL;
    switch (dir) {
        case NORTH:
            tilemap[checkx+dx][checky+dy] = MOVINGIN_N;
            break;
        case EAST:
            tilemap[checkx+dx][checky+dy] = MOVINGIN_E;
            break;
        case SOUTH:
            tilemap[checkx+dx][checky+dy] = MOVINGIN_S;
            break;
        case WEST:
            tilemap[checkx+dx][checky+dy] = MOVINGIN_W;
            break;
    }
    
    if (MAPSPOT(checkx,checky,1) == PUSHABLETILE)
        MAPSPOT(checkx,checky,1) = 0;

    if (MAPSPOT(checkx + 1, checky,0) >= AREATILE)
        MAPSPOT(checkx,checky,0) = MAPSPOT(checkx + 1,checky,0);
    else if (MAPSPOT(checkx - 1, checky,0) >= AREATILE)
        MAPSPOT(checkx,checky,0) = MAPSPOT(checkx - 1,checky,0);
    else if (MAPSPOT(checkx, checky + 1,0) >= AREATILE)
        MAPSPOT(checkx,checky,0) = MAPSPOT(checkx,checky + 1,0);
    else if (MAPSPOT(checkx, checky - 1,0) >= AREATILE)
        MAPSPOT(checkx,checky,0) = MAPSPOT(checkx,checky - 1,0);
    else
        GetMessage("No floor code found", DEF_MSG_CLR);

 //   *(mapsegs[0]+(checky<<mapshift)+checkx) = *(mapsegs[0]+(player->tiley<<mapshift)+player->tilex); // set correct floorcode (BrotherTank's fix)

    int32_t deltax = abs(player->tilex - checkx);
    int32_t deltay = abs(player->tiley - checky);

    if (deltax < 10 && deltay < 10)
        PlaySoundLocTile(PUSHWALLSND,checkx,checky);
}

/*
=================
=
= MovePWalls
=
=================
*/

void MovePWalls (void)
{
    for (int i = 0; i < MAXPWALLS; i++)
    {
        int oldblock,oldtile;

        if (!pwall[i].state || !pwall[i].active)
            continue;

        oldblock = pwall[i].state/128;

        pwall[i].state += (word)tics;

        if (pwall[i].state/128 != oldblock)
        {
            // block crossed into a new block
            oldtile = pwall[i].tile;

            //
            // the tile can now be walked into
            //
            tilemap[pwall[i].x][pwall[i].y] = 0;
            actorat[pwall[i].x][pwall[i].y] = 0;
            *(mapsegs[0]+(pwall[i].y<<mapshift)+pwall[i].x) = player->areanumber+AREATILE;

            int dx=dirs[pwall[i].dir][0], dy=dirs[pwall[i].dir][1];
            //
            // see if it should be pushed farther
            //
            if (pwall[i].state>=384)
            {
                //
                // the block has been pushed two tiles
                //
                pwall[i].state = 0;
                pwall[i].active = false;
                tilemap[pwall[i].x+dx][pwall[i].y+dy] = oldtile;
                return;
            }
            else
            {
                int xl,yl,xh,yh;
                xl = (player->x-PLAYERSIZE) >> TILESHIFT;
                yl = (player->y-PLAYERSIZE) >> TILESHIFT;
                xh = (player->x+PLAYERSIZE) >> TILESHIFT;
                yh = (player->y+PLAYERSIZE) >> TILESHIFT;

                pwall[i].x += dx;
                pwall[i].y += dy;

                if (actorat[pwall[i].x+dx][pwall[i].y+dy]
                    || xl<=pwall[i].x+dx && pwall[i].x+dx<=xh && yl<=pwall[i].y+dy && pwall[i].y+dy<=yh)
                {
                    pwall[i].state = 0;
                    pwall[i].active = false;
                    tilemap[pwall[i].x][pwall[i].y] = oldtile;
                    return;
                }

                tilemap[pwall[i].x][pwall[i].y] = BIT_WALL;
                actorat[pwall[i].x+dx][pwall[i].y+dy] = (objtype *)(uintptr_t) (tilemap[pwall[i].x+dx][pwall[i].y+dy] = oldtile);

                switch(pwall[i].dir) {
                    case NORTH:
                        tilemap[pwall[i].x+dx][pwall[i].y+dy] = MOVINGIN_N;
                        break;
                    case EAST:
                        tilemap[pwall[i].x+dx][pwall[i].y+dy] = MOVINGIN_E;
                        break;
                    case WEST:
                        tilemap[pwall[i].x+dx][pwall[i].y+dy] = MOVINGIN_W;
                        break;
                    case SOUTH:
                        tilemap[pwall[i].x+dx][pwall[i].y+dy] = MOVINGIN_S;
                        break;
                }
            }

        }
        pwall[i].pos = (pwall[i].state/2)&63;
    } 
}

/*
=============================================================================

                                SHOOTING WALLS

=============================================================================
*/

fwall_t fwall[MAXFIREWALLS];

