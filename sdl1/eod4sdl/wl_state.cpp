// WL_STATE.C

#include "wl_def.h"
#pragma hdrstop

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


static const dirtype opposite[9] =
    {west,southwest,south,southeast,east,northeast,north,northwest,nodir};

static const dirtype diagonal[9][9] =
{
    /* east */  {nodir,nodir,northeast,nodir,nodir,nodir,southeast,nodir,nodir},
                {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
    /* north */ {northeast,nodir,nodir,nodir,northwest,nodir,nodir,nodir,nodir},
                {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
    /* west */  {nodir,nodir,northwest,nodir,nodir,nodir,southwest,nodir,nodir},
                {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
    /* south */ {southeast,nodir,nodir,nodir,southwest,nodir,nodir,nodir,nodir},
                {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
                {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir}
};

char    dx8dir[9]= { 1, 1, 0, -1, -1, -1,  0,  1, 0};  // dx & dy based on direction
char    dy8dir[9]= { 0, 1, 1,  1,  0, -1, -1, -1, 0};  // used mainly in wl_enemy_ai.cpp 

void    SpawnNewObj (unsigned tilex, unsigned tiley, statetype *state);
void    NewState (objtype *ob, statetype *state);

boolean TryWalk (objtype *ob);
void    MoveObj (objtype *ob, int32_t move);

void    KillActor (objtype *ob);
void    DamageActor (objtype *ob, unsigned damage);

boolean CheckLine (objtype *ob);
void    FirstSighting (objtype *ob);
boolean CheckSight (objtype *ob);

/*
=============================================================================

                                LOCAL VARIABLES

=============================================================================
*/



//===========================================================================


/*
===================
=
= SpawnNewObj
=
= Spaws a new actor at the given TILE coordinates, with the given state, and
= the given size in GLOBAL units.
=
= newobj = a pointer to an initialized new actor
=
===================
*/

void SpawnNewObj (unsigned tilex, unsigned tiley, statetype *state)
{
    GetNewActor ();
    newobj->state = state;
    if (state->tictime)
        #ifdef USE_REAL_RAND
        newobj->ticcount = DEMOCHOOSE_ORIG_SDL(
                US_RndT () % state->tictime,
                US_RndT () % state->tictime + 1);     // Chris' moonwalk bugfix ;D
        #else
        newobj->ticcount = DEMOCHOOSE_ORIG_SDL(
                US_RndT () % state->tictime,
                US_RndT () % state->tictime + 1);     // Chris' moonwalk bugfix ;D
        #endif
    else
        newobj->ticcount = 0;

    newobj->tilex = (short) tilex;
    newobj->tiley = (short) tiley;
    newobj->x = ((int32_t)tilex<<TILESHIFT)+TILEGLOBAL/2;
    newobj->y = ((int32_t)tiley<<TILESHIFT)+TILEGLOBAL/2;
    newobj->dir = nodir;

    actorat[tilex][tiley] = newobj;
    newobj->areanumber =
        *(mapsegs[0] + (newobj->tiley<<mapshift)+newobj->tilex) - AREATILE;
}



/*
===================
=
= NewState
=
= Changes ob to a new state, setting ticcount to the max for that state
=
===================
*/

void NewState (objtype *ob, statetype *state)
{
    ob->state = state;
    ob->ticcount = state->tictime;
}



/*
=============================================================================

                        ENEMY TILE WORLD MOVEMENT CODE

=============================================================================
*/

boolean CheckMovementOK(int x, int y, int *doornum, objtype *ob)
{
    uintptr_t temp = (uintptr_t)actorat[x][y];

    if (MAPSPOT(x,y,1) == ENEMYBLOCKTILE)
        return true;

    if (temp) {
        if (temp<BIT_DOOR && doornum != NULL) {
            return true;
        }
        if (temp<BIT_ALLTILES && doornum == NULL) {
            return true;
        } else if (temp<BIT_ALLTILES && doornum != NULL) {
        #ifdef PLAYDEMOLIKEORIGINAL       
            if(DEMOCOND_ORIG) 
                *doornum = temp & 63;
        #endif         
                *doornum = temp & LAST_DOORNUM;
        }
        else if (((objtype *)(temp))->flags&FL_SHOOTABLE) {
            return true;
        }
    }

    return false;
} 

dirtype SwapVal (dirtype *value1, dirtype *value2)
{
  dirtype temp = *value1;
  *value1 = *value2;
  *value2 = temp;
  return *value1, *value2;
} 

/*
==================================
=
= TryWalk
=
= Attempts to move ob in its current (ob->dir) direction.
=
= If blocked by either a wall or an actor returns FALSE
=
= If move is either clear or blocked only by a door, returns TRUE and sets
=
= ob->tilex         = new destination
= ob->tiley
= ob->areanumber    = the floor tile number (0-(NUMAREAS-1)) of destination
= ob->distance      = TILEGLOBAl, or -doornumber if a door is blocking the way
=
= If a door is in the way, an OpenDoor call is made to start it opening.
= The actor code should wait until
=       doorobjlist[-ob->distance].action = dr_open, meaning the door has been
=       fully opened
=
==================================
*/

boolean TryWalk(objtype *ob) {
  int     oldx, oldy, newx, newy, doornum = -1;
 
   if(ob->dir == nodir) return false;
   oldx=ob->tilex;
   oldy=ob->tiley;
   newx= oldx + dx8dir[ob->dir];
   newy= oldy - dy8dir[ob->dir];   
  if (ob->dir & 1)  // Check Diagonal Movement
  {
      if(CheckMovementOK(newx, oldy, NULL, ob) ||
          CheckMovementOK(oldx, newy, NULL, ob) ||
          CheckMovementOK(newx, newy, NULL, ob)) return false;
   }
   else                   // Check Cardinal Movement
   {
      switch (ob->obclass)
      {
        case spectreobj:          
        case suicideobj:
            if(CheckMovementOK(newx, newy, NULL, ob)) 
                return false;
            break;
        default:
           if(CheckMovementOK(newx, newy, &doornum, ob)) 
            return false;
           break;   
      }
  }
  if (doornum != -1)
  {
    OpenDoor(doornum);
    ob->distance = -doornum-1;
    return true;
  }
  else ob->distance = TILEGLOBAL; // go ahead, its clear
  ob->tilex=newx;
  ob->tiley=newy;
  ob->areanumber=MAPSPOT(newx, newy, 0)-AREATILE;
  return true; 
}

/*
==================================
=
= SelectDodgeDir
=
= Attempts to choose and initiate a movement for ob that sends it towards
= the player while dodging
=
= If there is no possible move (ob is totally surrounded)
=
= ob->dir           =       nodir
=
= Otherwise
=
= ob->dir           = new direction to follow
= ob->distance      = TILEGLOBAL or -doornumber
= ob->tilex         = new destination
= ob->tiley
= ob->areanumber    = the floor tile number (0-(NUMAREAS-1)) of destination
=
==================================
*/

void SelectDodgeDir (objtype *ob)
{
  int     i;
  dirtype dirtry[5], turnaround;
  // turning around is only ok the very first time
  // after noticing the player
  if (ob->flags & FL_FIRSTATTACK) { turnaround = nodir; ob->flags &= ~FL_FIRSTATTACK; }
  else turnaround=opposite[ob->dir];
  // arange 5 direction choices in order of preference
  // the four cardinal directions plus the diagonal
  // straight towards the player
  if (player->tilex - ob->tilex > 0) { dirtry[1]= east;  dirtry[3]= west; }
  else                               { dirtry[1]= west;  dirtry[3]= east; }
  if (player->tiley - ob->tiley > 0) { dirtry[2]= south; dirtry[4]= north; }
  else                               { dirtry[2]= north; dirtry[4]= south; }
  // randomize a bit for dodging
  if (abs(player->tilex - ob->tilex) > abs(player->tiley - ob->tiley))
  { SwapVal (&dirtry[1], &dirtry[2]); SwapVal (&dirtry[3], &dirtry[4]); }
  if (US_RndT() < 128)
  { SwapVal (&dirtry[1], &dirtry[2]); SwapVal (&dirtry[3], &dirtry[4]); }
  dirtry[0] = diagonal[dirtry[1]][dirtry[2]];
  for (i=0;i<5;i++) // try the directions util one works
  {
    if ( dirtry[i] == nodir || dirtry[i] == turnaround) continue;
    ob->dir = dirtry[i];
    if (TryWalk(ob)) return;
  }
  if (turnaround != nodir) // turn around only as a last resort
  { ob->dir = turnaround; if (TryWalk(ob)) return; }
  ob->dir = nodir; 
}


/*
============================
=
= SelectChaseDir
=
= As SelectDodgeDir, but doesn't try to dodge
=
============================
*/

void SelectChaseDir (objtype *ob)
{
    dirtype d[3];
    dirtype tdir, olddir, turnaround;

    olddir=ob->dir;
    turnaround=opposite[olddir];
    d[1]=nodir;
    d[2]=nodir;

    if (player->tilex - ob->tilex != 0)
        d[1] = (player->tilex - ob->tilex > 0 ? east:west);

    if (player->tiley - ob->tiley != 0)
        d[2] = (player->tiley - ob->tiley > 0 ? south:north);

    if (abs(player->tiley - ob->tiley) > abs(player->tilex - ob->tilex))
        SwapVal ( &d[1], &d[2] );

    if (d[1] == turnaround) d[1]=nodir;

    if (d[2] == turnaround) d[2]=nodir;

    if (d[1]!=nodir) /*either moved forward or attacked*/
    { ob->dir=d[1]; if (TryWalk(ob)) return; }
    if (d[2]!=nodir)
    { ob->dir=d[2]; if (TryWalk(ob)) return; }
    /* there is no direct path to the player, so pick another direction */
    if (olddir!=nodir)
    { ob->dir=olddir; if (TryWalk(ob)) return; }
    if (US_RndT()>128)      /*randomly determine direction of search*/
    {
    for (tdir=east; tdir<=south; tdir=(dirtype)(tdir+2))
    {
        if (tdir!=turnaround) { ob->dir=tdir; if ( TryWalk(ob) ) return; }
    }
    }
    else
    {
    for (tdir=south; tdir>=east; tdir=(dirtype)(tdir-2))
    {
        if (tdir!=turnaround) { ob->dir=tdir; if ( TryWalk(ob) ) return; }
    }
    }
    if (turnaround != nodir)
    {
    ob->dir=turnaround;
    if (ob->dir != nodir) { if ( TryWalk(ob) ) return; }
    }
    ob->dir = nodir; // can't move 
}


/*
============================
=
= SelectRunDir
=
= Run Away from player
=
============================
*/

void SelectRunDir (objtype *ob)
{
  dirtype d[3];
  dirtype tdir;

  if (player->tilex - ob->tilex < 0) d[1]= east; else d[1]= west;
  if (player->tiley - ob->tiley < 0) d[2]=south; else d[2]=north;
  if (abs(player->tiley - ob->tiley) > abs(player->tilex - ob->tilex))
  { SwapVal (&d[1], &d[2]); }
  ob->dir=d[1];
  if (TryWalk(ob)) return; /*either moved forward or attacked*/
  ob->dir=d[2];
  if (TryWalk(ob)) return;
  /* there is no direct path to the player, so pick another direction */
  if (US_RndT()>128)      /*randomly determine direction of search*/
  {
    for (tdir=east; tdir<=south; tdir=(dirtype)(tdir+2))
    { ob->dir=tdir; if ( TryWalk(ob) ) return; }
  }
  else
  {
    for (tdir=south; tdir>=east; tdir=(dirtype)(tdir-2)) 
    { ob->dir=tdir; if ( TryWalk(ob) ) return; }
  }
  ob->dir = nodir;                // can't move 
}

#ifdef EODSPECIAL
/*
==================
=
= SelectLootDir
=
==================
*/
void SelectLootDir (objtype *ob) {
    dirtype tdir;
    dirtype idir;

    if (ob->flags & FL_FIRSTATTACK) {
        ob->flags &= ~FL_FIRSTATTACK;
        ob->dir = (dirtype)((ob->dir + 4) % 8);
        if ( TryWalk(ob) )
            return;
    } else {
        if ( !TryWalk(ob) ) {
            idir = ob->dir;
            for (tdir=east; tdir<=south; tdir=(dirtype)(tdir+2)) { 
                if (tdir == (dirtype)((idir+4)%8)) continue;
                ob->dir=tdir; 
                if ( TryWalk(ob) ) 
                    return; 
            }
        }
    }
}
#endif

/*
=================
=
= MoveObj
=
= Moves ob be move global units in ob->dir direction
= Actors are not allowed to move inside the player
= Does NOT check to see if the move is tile map valid
=
= ob->x                 = adjusted for new position
= ob->y
=
=================
*/

void MoveObj (objtype *ob, int32_t move)
{
    int32_t    deltax,deltay;

    switch (ob->dir)
    {
        case north:
            ob->y -= move;
            break;
        case northeast:
            ob->x += move;
            ob->y -= move;
            break;
        case east:
            ob->x += move;
            break;
        case southeast:
            ob->x += move;
            ob->y += move;
            break;
        case south:
            ob->y += move;
            break;
        case southwest:
            ob->x -= move;
            ob->y += move;
            break;
        case west:
            ob->x -= move;
            break;
        case northwest:
            ob->x -= move;
            ob->y -= move;
            break;

        case nodir:
            return;

        default:
            Quit ("MoveObj: bad dir!");
    }

    //
    // check to make sure it's not on top of player
    //
    if (areabyplayer[ob->areanumber])
    {
        deltax = ob->x - player->x;
        if (deltax < -MINACTORDIST || deltax > MINACTORDIST)
            goto moveok;
        deltay = ob->y - player->y;
        if (deltay < -MINACTORDIST || deltay > MINACTORDIST)
            goto moveok;

        if (ob->hidden)          // move closer until he meets CheckLine
            goto moveok;

        if (ob->obclass == ghostobj || ob->obclass == spectreobj || ob->obclass == snowmanobj)
            TakeDamage (tics*2,ob);

        //
        // back up
        //
        switch (ob->dir)
        {
            case north:
                ob->y += move;
                break;
            case northeast:
                ob->x -= move;
                ob->y += move;
                break;
            case east:
                ob->x -= move;
                break;
            case southeast:
                ob->x -= move;
                ob->y -= move;
                break;
            case south:
                ob->y -= move;
                break;
            case southwest:
                ob->x += move;
                ob->y -= move;
                break;
            case west:
                ob->x += move;
                break;
            case northwest:
                ob->x += move;
                ob->y += move;
                break;

            case nodir:
                return;
        }
        return;
    }
moveok:
    ob->distance -=move;
}

/*
=============================================================================

                                STUFF

=============================================================================
*/

/*
===============
=
= DropItem
=
= Tries to drop a bonus item somewhere in the tiles surrounding the
= given tilex/tiley
=
===============
*/

void DropItem (wl_stat_t itemtype, int tilex, int tiley)
{
    int     x,y,xl,xh,yl,yh;

    //
    // find a free spot to put it in
    //
    if (!actorat[tilex][tiley])
    {
        PlaceItemType (itemtype, tilex,tiley);
        return;
    }

    xl = tilex-1;
    xh = tilex+1;
    yl = tiley-1;
    yh = tiley+1;

    for (x=xl ; x<= xh ; x++)
    {
        for (y=yl ; y<= yh ; y++)
        {
            if (!actorat[x][y])
            {
                PlaceItemType (itemtype, x,y);
                return;
            }
        }
    }
}



/*
===============
=
= KillActor
=
===============
*/

void KillActor (objtype *ob)
{
    int     tilex,tiley;

    if (ob->obclass != supercloneobj && ob->obclass != snowmanobj && ob->obclass != spectreobj) {
        tilex = ob->tilex = (word)(ob->x >> TILESHIFT);         // drop item on center
        tiley = ob->tiley = (word)(ob->y >> TILESHIFT);
    }

    switch (ob->obclass)
    {
        case guardobj:
            GivePoints (200);
            if (ob->flags & FL_GIB)
                NewState (ob,&s_splatter1);
            else
                NewState (ob,&s_grddie1);

            if (gamestate.weapons & WP_PISTOL)
                PlaceItemType (bo_clip2,tilex,tiley);
            else
                PlaceItemType (bo_pistol,tilex,tiley);
            break;

        case officerobj:
            GivePoints (400);
            if (ob->flags & FL_GIB)
                NewState (ob,&s_splatter1);
            else
                NewState (ob,&s_ofcdie1);

            if (gamestate.weapons & WP_PISTOL)
                PlaceItemType (bo_clip2,tilex,tiley);
            else
                PlaceItemType (bo_pistol,tilex,tiley);
            break;

        case mutantobj:
        case toiletobj:
            GivePoints (500);
            NewState (ob,&s_mutdie1);

            if (gamestate.weapons & WP_PISTOL)
                PlaceItemType (bo_clip2,tilex,tiley);
            else
                PlaceItemType (bo_pistol,tilex,tiley);
            break;

        case ssobj:
            GivePoints (500);
            if (ob->flags & FL_GIB)
                NewState (ob,&s_splatter1);
            else
                NewState (ob,&s_ssdie1);

            if (gamestate.weapons & WP_MACHINEGUN)
                PlaceItemType (bo_clip2,tilex,tiley);
            else
                PlaceItemType (bo_machinegun,tilex,tiley);
            break;

        case suicideobj:
            GivePoints (200);
            NewState (ob,&s_suicidedie1);
            break;

        case spectreobj:
            NewState (ob,&s_spectredie1);
            break;

        case ghostobj:
            GivePoints(500);
            NewState(ob,&s_spectre2die1);
            break;

        case snowmanobj:
            GivePoints (500);       // Get points once for each
            ob->flags &= ~FL_BONUS;

            ob->temp2 = Random(100);
            if (ob->temp2 > 19) {
                int tmp = Random(100);
                int randtreasure;

                if (tmp < 75) // 75% chance nothing
                    randtreasure = 0;
                else if (tmp < 85) // 10% chance cross
                    randtreasure = bo_cross;
                else if (tmp < 92) // 7% chance
                    randtreasure = bo_chalice;
                else if (tmp < 97) // 5% chance bible
                    randtreasure = bo_bible;
                else               // 3% crown
                    randtreasure = bo_crown;
                 
                if (randtreasure)
                    PlaceItemType(randtreasure,(word)(ob->x >> TILESHIFT),(word)(ob->y >> TILESHIFT));
            }
            NewState (ob,&s_snowdie1);
            break;

        case angelobj:
            GivePoints (10000);
            NewState (ob,&s_angeldie1);
            break;

        case grauobj:
            GivePoints (7000);
            NewState (ob,&s_graudie0);
            break;

        case supercloneobj:
            GivePoints (3000);
            NewState (ob,&s_superclonedie1);
#ifdef CRAND
            ob->temp2 = Random(100);
#else
            ob->temp2 = US_RndT();   
#endif
            break;

        case ottoobj:
            GivePoints (6000);
            NewState (ob,&s_ottodie1);
            PlaceItemType (bo_key4,tilex,tiley);
            break;

        case baronobj:
            GivePoints (8000);
            NewState (ob,&s_barondie1);
            break;

        case ultracloneobj:
            GivePoints (3000);
            NewState (ob,&s_ultraclonedie1);
#ifdef EODSPECIAL
            if (gamestate.mapon == 6)
                PlaceItemType (bo_key1,tilex,tiley);
#endif
            break;

        case robotobj:
            GivePoints (3000);
            NewState (ob,&s_robotdie1);
            break;

        case heinrichobj:
            GivePoints (7000);
            NewState (ob,&s_heinrichdie1);
            PlaceItemType (bo_key1,tilex,tiley);
            break;

        case turretobj:
            GivePoints(800);
            NewState (ob,&s_turretdie1);
            break;

        case barrelobj:
            if (MAPSPOT(ob->tilex,ob->tiley,1) == 104) // barrel spawns fire
                SpawnStatic(ob->tilex,ob->tiley, 118); // spawn an animated flame
#ifdef CRAND
            if (Random(100) < 15)
#else
            if ((US_RndT() % 16) == 0)
#endif
                SpawnStatic(ob->tilex,ob->tiley, 98); //poison
            NewState(ob,&s_barreldie1);
            break;

        case columnobj:
            NewState(ob,&s_columndie1);
            break;
    }

    switch (ob->obclass) {
        case supercloneobj:
            gamestate.killcount++;
            actorat[ob->tilex][ob->tiley] = NULL;
            ob->flags &= ~FL_SHOOTABLE;
            break;
        default:
            gamestate.killcount++;
        case columnobj:
        case barrelobj:    
        case snowmanobj:
        case spectreobj:
        case ghostobj:
		case suicideobj: // gives kill count in A_Explode
            actorat[ob->tilex][ob->tiley] = NULL;
            ob->flags &= ~FL_SHOOTABLE;
            ob->flags |= FL_NONMARK;
            break;
    }
}



/*
===================
=
= DamageActor
=
= Called when the player succesfully hits an enemy.
=
= Does damage points to enemy ob, either putting it into a stun frame or
= killing it.
=
===================
*/

void DamageActor (objtype *ob, unsigned damage)
{
    //madenoise = true;

    //
    // do double damage if shooting a non attack mode actor
    //
    if ( !(ob->flags & FL_ATTACKMODE) )
        damage <<= 1;
    if (ob->hitpoints > 0) {
        ob->hitpoints -= (short)damage;

        if (ob->hitpoints<=0) {
            if ( !(ob->flags & FL_STEALTH) && damage > 99) // sniper rifle doesn't gib!
                ob->flags |= FL_GIB;

            KillActor (ob);
        }
        else
        {
            if (! (ob->flags & FL_ATTACKMODE) )
                FirstSighting (ob);             // put into combat mode

            ob->flags &= ~FL_STEALTH;

            switch (ob->obclass)                // dogs only have one hit point
            {
                case guardobj:
                    if (ob->hitpoints&1)
                        NewState (ob,&s_grdpain);
                    else
                        NewState (ob,&s_grdpain1);
                    break;

                case officerobj:
                    if (ob->hitpoints&1)
                        NewState (ob,&s_ofcpain);
                    else
                        NewState (ob,&s_ofcpain1);
                    break;

                case mutantobj:
                    if (ob->hitpoints&1)
                        NewState (ob,&s_mutpain);
                    else
                        NewState (ob,&s_mutpain1);
                    break;

                case ssobj:
                    if (ob->hitpoints&1)
                        NewState (ob,&s_sspain);
                    else
                        NewState (ob,&s_sspain1);

                    break;

                case toiletobj:
                    if (ob->hitpoints&1 && ob->flags & FL_ATTACKMODE)
                        NewState (ob,&s_mutpain);
                    else
                        NewState (ob,&s_mutpain1);
                    break;
            }
        }
    }
}

/*
=============================================================================

                                CHECKSIGHT

=============================================================================
*/


/*
=====================
=
= CheckLine
=
= Returns true if a straight line between the player and ob is unobstructed
=
=====================
*/

boolean CheckLine (objtype *ob)
{
    int         x1,y1,xt1,yt1,x2,y2,xt2,yt2;
    int         x,y;
    int         xdist,ydist,xstep,ystep;
    int         partial,delta;
    int32_t     ltemp;
    int         xfrac,yfrac,deltafrac;
    unsigned    value,intercept;

    x1 = ob->x >> UNSIGNEDSHIFT;            // 1/256 tile precision
    y1 = ob->y >> UNSIGNEDSHIFT;
    xt1 = x1 >> 8;
    yt1 = y1 >> 8;

    x2 = plux;
    y2 = pluy;
    xt2 = player->tilex;
    yt2 = player->tiley;

    xdist = abs(xt2-xt1);

    if (xdist > 0)
    {
        if (xt2 > xt1)
        {
            partial = 256-(x1&0xff);
            xstep = 1;
        }
        else
        {
            partial = x1&0xff;
            xstep = -1;
        }

        deltafrac = abs(x2-x1);
        delta = y2-y1;
        ltemp = ((int32_t)delta<<8)/deltafrac;
        if (ltemp > 0x7fffl)
            ystep = 0x7fff;
        else if (ltemp < -0x7fffl)
            ystep = -0x7fff;
        else
            ystep = ltemp;
        yfrac = y1 + (((int32_t)ystep*partial) >>8);

        x = xt1+xstep;
        xt2 += xstep;
        do
        {
            y = yfrac>>8;
            yfrac += ystep;

            value = (unsigned)tilemap[x][y];
            x += xstep;

            if (!value)
                continue;

            if (value != BIT_WALL && (value<BIT_DOOR || value>BIT_ALLTILES))
                return false;

            //
            // see if the door is open enough
            //
            value &= ~BIT_DOOR;
            intercept = yfrac-ystep/2;

            if (intercept>doorposition[value])
                return false;

        } while (x != xt2);
    }

    ydist = abs(yt2-yt1);

    if (ydist > 0)
    {
        if (yt2 > yt1)
        {
            partial = 256-(y1&0xff);
            ystep = 1;
        }
        else
        {
            partial = y1&0xff;
            ystep = -1;
        }

        deltafrac = abs(y2-y1);
        delta = x2-x1;
        ltemp = ((int32_t)delta<<8)/deltafrac;
        if (ltemp > 0x7fffl)
            xstep = 0x7fff;
        else if (ltemp < -0x7fffl)
            xstep = -0x7fff;
        else
            xstep = ltemp;
        xfrac = x1 + (((int32_t)xstep*partial) >>8);

        y = yt1 + ystep;
        yt2 += ystep;
        do
        {
            x = xfrac>>8;
            xfrac += xstep;

            value = (unsigned)tilemap[x][y];
            y += ystep;

            if (!value)
                continue;

            if (value != BIT_WALL && (value<BIT_DOOR || value>BIT_ALLTILES))
                return false;

            //
            // see if the door is open enough
            //
            value &= ~BIT_DOOR;
            intercept = xfrac-xstep/2;

            if (intercept>doorposition[value])
                return false;
        } while (y != yt2);
    }

    return true;
}

/*
================
=
= CheckLine for any 2 actors
= Used for explosions
=
= Credit: Matthew
=
===============
*/

boolean CheckLine2 (objtype *ob1, objtype *ob2) {
   int   x1,y1,xt1,yt1,x2,y2,xt2,yt2;
   int   x,y;
   int   xdist,ydist,xstep,ystep;
   int   partial,delta;
   int32_t   ltemp;
   int   xfrac,yfrac,deltafrac;
   unsigned   value,intercept;

   x1 = ob1->x >> UNSIGNEDSHIFT;      // 1/256 tile precision
   y1 = ob1->y >> UNSIGNEDSHIFT;
   xt1 = x1 >> 8;
   yt1 = y1 >> 8;

   x2 = ob2->x >> UNSIGNEDSHIFT;
   y2 = ob2->y >> UNSIGNEDSHIFT;
   xt2 = x2 >> 8;
   yt2 = y2 >> 8;

   xdist = abs(xt2-xt1);

   if (xdist > 0)
   {
      if (xt2 > xt1)
      {
         partial = 256-(x1&0xff);
         xstep = 1;
      }
      else
      {
         partial = x1&0xff;
         xstep = -1;
      }

      deltafrac = abs(x2-x1);
      delta = y2-y1;
      ltemp = ((int32_t)delta<<8)/deltafrac;
      if (ltemp > 0x7fffl)
         ystep = 0x7fff;
      else if (ltemp < -0x7fffl)
         ystep = -0x7fff;
      else
         ystep = ltemp;
      yfrac = y1 + (((int32_t)ystep*partial) >>8);

      x = xt1+xstep;
      xt2 += xstep;
      do
      {
         y = yfrac>>8;
         yfrac += ystep;

         value = (unsigned)tilemap[x][y];
         x += xstep;

         if (!value)
            continue;

         if (value<BIT_DOOR || value>BIT_ALLTILES)
            return false;

         //
         // see if the door is open enough
         //
         value &= ~BIT_DOOR;
         intercept = yfrac-ystep/2;

         if (intercept>doorposition[value])
            return false;

      } while (x != xt2);
   }

   ydist = abs(yt2-yt1);

   if (ydist > 0)
   {
      if (yt2 > yt1)
      {
         partial = 256-(y1&0xff);
         ystep = 1;
      }
      else
      {
         partial = y1&0xff;
         ystep = -1;
      }

      deltafrac = abs(y2-y1);
      delta = x2-x1;
      ltemp = ((int32_t)delta<<8)/deltafrac;
      if (ltemp > 0x7fffl)
         xstep = 0x7fff;
      else if (ltemp < -0x7fffl)
         xstep = -0x7fff;
      else
         xstep = ltemp;
      xfrac = x1 + (((int32_t)xstep*partial) >>8);

      y = yt1 + ystep;
      yt2 += ystep;
      do
      {
         x = xfrac>>8;
         xfrac += xstep;

         value = (unsigned)tilemap[x][y];
         y += ystep;

         if (!value)
            continue;

         if (value<BIT_DOOR || value>BIT_ALLTILES)
            return false;

         //
         // see if the door is open enough
         //
         value &= ~BIT_DOOR;
         intercept = xfrac-xstep/2;

         if (intercept>doorposition[value])
            return false;
      } while (y != yt2);
   }

   return true;
} 


/*
================
=
= CheckSight
=
= Checks a straight line between player and current object
=
= If the sight is ok, check alertness and angle to see if they notice
=
= returns true if the player has been spoted
=
================
*/

#define MINSIGHT        0x18000l

boolean CheckSight (objtype *ob)
{
    int32_t deltax,deltay;

    //
    // don't bother tracing a line if the area isn't connected to the player's
    //
    if (!areabyplayer[ob->areanumber])
        return false;

    //
    // if the player is real close, sight is automatic
    //
        deltax = player->x - ob->x;
        deltay = player->y - ob->y;
    if (ob->obclass != turretobj) {
        if (deltax > -MINSIGHT && deltax < MINSIGHT
            && deltay > -MINSIGHT && deltay < MINSIGHT) {
            if (gamestate.weapon == wp_knife)
                ob->flags |= FL_STEALTH;
            return true;
        }
    }

    //
    // see if they are looking in the right direction
    //
    switch (ob->dir)
    {
        case north:
            if (deltay > 0)
                return false;
            break;

        case east:
            if (deltax < 0)
                return false;
            break;

        case south:
            if (deltay < 0)
                return false;
            break;

        case west:
            if (deltax > 0)
                return false;
            break;

        // check diagonal moving guards fix

        case northwest:
            if (DEMOCOND_SDL && deltay > -deltax)
                return false;
            break;

        case northeast:
            if (DEMOCOND_SDL && deltay > deltax)
                return false;
            break;

        case southwest:
            if (DEMOCOND_SDL && deltax > deltay)
                return false;
            break;

        case southeast:
            if (DEMOCOND_SDL && -deltax > deltay)
                return false;
            break;
    }

    //
    // trace a line to check for blocking tiles (corners)
    //
    return CheckLine (ob);
}


/*
===============
=
= FirstSighting
=
= Puts an actor into attack mode and possibly reverses the direction
= if the player is behind it
=
===============
*/

void FirstSighting (objtype *ob)
{
    //
    // react to the player
    //
    switch (ob->obclass)
    {
        case guardobj:
            if ( ! (ob->flags & FL_STEALTH) ) PlaySoundLocActor(HALTSND,ob);
            NewState (ob,&s_grdchase1);
            ob->speed *= 4;                 // go faster when chasing player
            break;

        case officerobj:
            if ( ! (ob->flags & FL_STEALTH) ) PlaySoundLocActor(SPIONSND,ob);
            NewState (ob,&s_ofcchase1);
            ob->speed *= 5;                 // go faster when chasing player
            break;

        case mutantobj:
            NewState (ob,&s_mutchase1);
            ob->speed *= 3;                 // go faster when chasing player
            break;

        case toiletobj:
            ob->dir = GetCardinalDirection(ob);
            NewState (ob,&s_mutchase1);
            ob->speed *= 3;                 // go faster when chasing player
            break;

        case ssobj:
            if ( ! (ob->flags & FL_STEALTH) ) PlaySoundLocActor(SCHUTZADSND,ob);
            NewState (ob,&s_sschase1);
            ob->speed *= 4;                 // go faster when chasing player
            break;

        case suicideobj:
      //      PlaySoundLocActor(DOGBARKSND,ob);
            NewState (ob,&s_suicidechase1);
            ob->speed *= 7;                 // go faster when chasing player
            break;

        case spectreobj:
            SD_PlaySound(GHOSTSIGHTSND);
            NewState (ob,&s_spectrechase1);
            ob->speed = 800;                        // go faster when chasing player
            break;

        case ghostobj:
            SD_PlaySound(GHOSTSIGHTSND);
            NewState (ob,&s_spectre2chase1);
            ob->speed = 800;                        // go faster when chasing player
            break; 

        case angelobj:
            SD_PlaySound(ANGELSIGHTSND);
            NewState (ob,&s_angelchase1);
            ob->speed = 1536;                       // go faster when chasing player
            break;

        case grauobj:
            SD_PlaySound(GRAUSIGHTSND);
            NewState (ob,&s_grauchase1);
            ob->speed = 1536;                       // go faster when chasing player
            break;

        case supercloneobj:
            NewState (ob,&s_superclonechase1);
            ob->speed = 3000;                       // go faster when chasing player
            break;

        case ottoobj:
            SD_PlaySound(OTTOSIGHTSND);
            NewState (ob,&s_ottochase1);
            ob->speed = 2048;                       // go faster when chasing player
            break;

        case baronobj:
            SD_PlaySound(BARONSIGHTSND);
            NewState (ob,&s_baronchase1);
            ob->speed = 2048;                       // go faster when chasing player
            break;

        case ultracloneobj:
            NewState (ob,&s_ultraclonechase1);
            ob->speed = 3000;                       // go faster when chasing player
            break;

        case robotobj:
            SD_PlaySound(ROBOTSIGHTSND);
            NewState (ob,&s_robotchase1);
            ob->speed = 2048;                       // go faster when chasing player
            break;

        case heinrichobj:
            SD_PlaySound(HEINRICHSIGHTSND);
            NewState (ob,&s_heinrichchase1);
            ob->speed = 2048;                       // go faster when chasing player
            break;

        case turretobj:
            ob->temp2 = 1;
            ob->dir = (dirtype)((GetCardinalDirection(ob) + 1) % 8);
            NewState (ob,&s_turretscan1);
            ob->speed = 0;
            break;

        case snowmanobj:
            NewState (ob,&s_snowchase1);
            ob->speed = 800;
            break;
    }

    if (ob->distance < 0)
        ob->distance = 0;       // ignore the door opening command

    ob->flags |= FL_ATTACKMODE|FL_FIRSTATTACK;
    ob->flags &= ~FL_STEALTH;
}



/*
===============
=
= SightPlayer
=
= Called by actors that ARE NOT chasing the player.  If the player
= is detected (by sight, noise, or proximity), the actor is put into
= it's combat frame and true is returned.
=
= Incorporates a random reaction delay
=
===============
*/

boolean SightPlayer (objtype *ob)
{
    if (ob->flags & FL_ATTACKMODE)
        Quit ("An actor in ATTACKMODE called SightPlayer!");

    if (ob->temp2)
    {
        //
        // count down reaction time
        //
        ob->temp2 -= (short) tics;
        if (ob->temp2 > 0)
            return false;
        ob->temp2 = 0;                                  // time to react
    }
    else
    {
        if (!areabyplayer[ob->areanumber])
            return false;

        if (ob->flags & FL_AMBUSH)
        {
            if (!CheckSight (ob))
                return false;
            ob->flags &= ~FL_AMBUSH;
        }
        else
        {
            if (!madenoise && !CheckSight (ob))
                return false;
        }

        switch (ob->obclass)
        {
            case guardobj:
                ob->temp2 = 1+US_RndT()/4;
                break;

            case officerobj:
                ob->temp2 = 2;
                break;
            case mutantobj:
                ob->temp2 = 1+US_RndT()/6;
                break;
            case ssobj:
                ob->temp2 = 1+US_RndT()/6;
                break;
            case suicideobj:
                ob->temp2 = 1;
                break;
            case toiletobj:
                ob->temp2 = 50+US_RndT()/4;
                break;  
            case spectreobj:
            case ghostobj:
            case angelobj:
            case grauobj:
            case supercloneobj:
            case ottoobj:
            case baronobj:
            case ultracloneobj:
            case robotobj:
            case heinrichobj:
            case snowmanobj:
                ob->temp2 = 1;
                break;
            case turretobj:
                FirstSighting(ob);
                return true;
                break;
        }
        return false;
    }

    FirstSighting (ob);

    return true;
}
