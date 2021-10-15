// WL_ACT2.C

#include <stdio.h>
#include <math.h>
#include "wl_def.h"
#pragma hdrstop

/*
=============================================================================

                               LOCAL CONSTANTS

=============================================================================
*/

#define PROJECTILESIZE  0xc000l
#define BJROCKETSIZE    0x6000l 
#define ROCKETEXPLSIZE  0x20000l
#define BARRELEXPLSIZE  0x35000l

#define BJRUNSPEED      2048
#define BJJUMPSPEED     680

/*
=============================================================================

                              GLOBAL VARIABLES

=============================================================================
*/



/*
=============================================================================

                              LOCAL VARIABLES

=============================================================================
*/

dirtype dirtable[9] = {northwest,north,northeast,west,nodir,east,
    southwest,south,southeast};

short starthitpoints[5][NUMENEMIES] =
//
// BABY MODE
//
{
    {
        40,     // Guard
        80,     // Officer
        140,    // SS
        20,     // Suicide Clone
        80,     // Regular Clone
        5,      // Spectre (snowman, ghost flame)
        2150,   // Angel of Death
        1450,   // Grau (hell)
        1150,   // Superclone
        1450,   // Otto
        1650,   // Baron Von Schmitt (hell)
        1150,   // Ultraclone
        1250,   // Robot
        1650,   // Heinrich

        400,    // Turret
        35,     // Expl. Barrel
        50      // Breakable Column
    },
    //
    // DON'T HURT ME MODE
    //
    {
        45,     // Guard
        85,     // Officer
        155,    // SS
        20,     // Suicide Clone
        90,     // Mutants
        10,      // Spectre (snowman, ghost flame)
        2250,   // Angel of Death
        1550,   // Grau (hell)
        1250,   // Superclone
        1550,    // Otto
        1750,   // Baron Von Schmitt (hell)
        1250,   // Ultraclone
        1350,   // Robot
        1750,   // Heinrich

        400,    // Turret
        35,     // Expl. Barrel
        50      // Breakable Column
    },
    //
    // BRING 'EM ON MODE
    //
    {
        55,     // Guard
        95,     // Officer
        165,    // SS
        35,     // Suicide Clone
        105,     // Mutants
        15,      // Spectre (snowman, ghost flame)
        2350,   // Angel of Death
        1650,   // Grau (hell)
        1350,   // Superclone
        1550,    // Otto
        1850,   // Baron Von Schmitt (hell)
        1350,   // Ultraclone
        1450,   // Robot
        1850,   // Heinrich

        400,    // Turret
        35,     // Expl. Barrel
        50      // Breakable Column
    },
    //
    // DEATH INCARNATE MODE
    //
    {
        65,     // Guard
        105,     // Officer
        175,    // SS
        50,     // Suicide Clone
        115,     // Mutants
        25,      // Spectre (snowman, ghost flame)
        3825,   // Angel of Death
        1550,   // Grau (hell)
        1250,   // Superclone
        1650,    // Otto
        1750,   // Baron Von Schmitt (hell)
        1250,   // Ultraclone
        1350,   // Robot
        1750,   // Heinrich

        400,    // Turret
        35,     // Expl. Barrel
        50      // Breakable Column
    },

    //
    // Tournament
    //
    {
        65,     // Guard
        105,     // Officer
        175,    // SS
        50,     // Suicide Clone
        115,     // Mutants
        25,      // Spectre (snowman, ghost flame)
        3825,   // Angel of Death
        1550,   // Grau (hell)
        1250,   // Superclone
        1650,    // Otto
        1750,   // Baron Von Schmitt (hell)
        1250,   // Ultraclone
        1350,   // Robot
        1750,   // Heinrich

        400,    // Turret
        35,     // Expl. Barrel
        50      // Breakable Column
    }
};

// --- [ TRANSFORMATIONS ] --------------------------------
void    T_Path (objtype *ob);
void    T_Shoot (objtype *ob);
void    T_Launch (objtype *ob);
void    T_SuicideChase (objtype *ob);
void    T_Chase (objtype *ob);
void    T_Otto (objtype *ob);
void    T_Projectile (objtype *ob);
void    T_Stand (objtype *ob);
void    T_Scan (objtype *ob);
void    T_Blink (objtype *ob);
void    T_BlinkReset (objtype *ob);
void    T_Ghosts (objtype *ob);
void    T_ResFlags(objtype *ob);

void    T_Trigger (objtype *ob);
void    T_Defuse (objtype *ob);

// --- [ ACTIONS ] ----------------------------------------
void    A_Dormant (objtype *ob);       
void    A_DeathScream (objtype *ob);
void    A_Explode (objtype *ob);
void    A_Splat (objtype *ob);
void    A_Smoke (objtype *ob);
void    A_Slurpie (objtype *ob);


// --- [ ENEMY BONUS OBJECTS ] ----------------------------
void    CheckEnemyTile (objtype *ob);
void    RemoveObject (statobj_t *check);
void    GetEnemyBonus (statobj_t *objpos, objtype *ob);

// --- [ ENEMY SPAWN TRIGGERS ] ---------------------------
void    SpawnTriggeredActors(enemy_t which, int targetTile);
void    A_SpawnNextBoss(objtype *ob);

// --- [ FIREWALL ROUTINE ] -------------------------------
void    ShootFireballs(int tilex, int tiley, int dir);

#ifdef EODSPECIAL
void CheckGong(void);
#endif

/*
=================================================================================
=
= Projectile & Explosion statetypes
=
=================================================================================
*/
extern  statetype s_rocket;
extern  statetype s_smoke1;
extern  statetype s_smoke2;
extern  statetype s_smoke3;
extern  statetype s_smoke4;
extern  statetype s_boom2;
extern  statetype s_boom3;

extern  statetype s_fireball;
extern  statetype s_firetrail1;
extern  statetype s_firetrail2;
extern  statetype s_firetrail3;
extern  statetype s_firetrail4;
extern  statetype s_fireexpl1;
extern  statetype s_fireexpl2;
extern  statetype s_fireexpl3;

extern  statetype s_spearproj;
extern  statetype s_speartrail1;
extern  statetype s_speartrail2;
extern  statetype s_speartrail3;
extern  statetype s_speartrail4;
extern  statetype s_spearexpl1;
extern  statetype s_spearexpl2;
extern  statetype s_spearexpl3;

extern  statetype s_flametrail1;
extern  statetype s_flametrail2;
extern  statetype s_flametrail3;
extern  statetype s_flametrail4;

// --- [ REGULAR ROCKET STATETYPES ]
statetype s_rocket              = {true,SPR_ROCKET_1,3,(statefunc)T_Projectile,(statefunc)A_Smoke,&s_rocket,true};
statetype s_smoke1              = {false,SPR_SMOKE_1,3,NULL,NULL,&s_smoke2,false};
statetype s_smoke2              = {false,SPR_SMOKE_2,3,NULL,NULL,&s_smoke3,false};
statetype s_smoke3              = {false,SPR_SMOKE_3,3,NULL,NULL,&s_smoke4,false};
statetype s_smoke4              = {false,SPR_SMOKE_4,3,NULL,NULL,NULL,false};

statetype s_boom1               = {false,SPR_BOOM_1,6,NULL,NULL,&s_boom2,true};
statetype s_boom2               = {false,SPR_BOOM_2,6,NULL,NULL,&s_boom3,true};
statetype s_boom3               = {false,SPR_BOOM_3,6,NULL,NULL,NULL,true};

// --- [ FIREBALL STATETYPES ]
statetype s_fireball             = {true,SPR_FIREBALL_1,3,(statefunc)T_Projectile,(statefunc)A_Smoke,&s_fireball,true};
statetype s_firetrail1           = {false,SPR_FIREBALL_TRAIL1,3,NULL,NULL,&s_firetrail2,true};
statetype s_firetrail2           = {false,SPR_FIREBALL_TRAIL2,3,NULL,NULL,&s_firetrail3,true};
statetype s_firetrail3           = {false,SPR_FIREBALL_TRAIL3,3,NULL,NULL,&s_firetrail4,true};
statetype s_firetrail4           = {false,SPR_FIREBALL_TRAIL4,3,NULL,NULL,NULL,true};

statetype s_fireexpl1            = {false,SPR_FIREBALL_EXP1,6,NULL,NULL,&s_fireexpl2,true};
statetype s_fireexpl2            = {false,SPR_FIREBALL_EXP2,6,NULL,NULL,&s_fireexpl3,true};
statetype s_fireexpl3            = {false,SPR_FIREBALL_EXP3,6,NULL,NULL,NULL,true};

// --- [ SPEAR STATETYPES ]
statetype s_spearproj            = {true,SPR_SPEARPROJ_1,3,(statefunc)T_Projectile,(statefunc)A_Smoke,&s_spearproj,true};
statetype s_speartrail1          = {false,SPR_SPEARPROJ_TRAIL1,3,NULL,NULL,&s_speartrail2,true};
statetype s_speartrail2          = {false,SPR_SPEARPROJ_TRAIL2,3,NULL,NULL,&s_speartrail3,true};
statetype s_speartrail3          = {false,SPR_SPEARPROJ_TRAIL3,3,NULL,NULL,&s_speartrail4,true};
statetype s_speartrail4          = {false,SPR_SPEARPROJ_TRAIL4,3,NULL,NULL,NULL,true};

statetype s_spearexpl1            = {false,SPR_SPEARPROJ_EXP1,6,NULL,NULL,&s_spearexpl2,true};
statetype s_spearexpl2            = {false,SPR_SPEARPROJ_EXP2,6,NULL,NULL,&s_spearexpl3,true};
statetype s_spearexpl3            = {false,SPR_SPEARPROJ_EXP3,6,NULL,NULL,NULL,true};

// --- [ FLAME TRAIL STATETYPES ]

statetype s_flametrail1           = {false,SPR_FLAME2_DIE1,3,NULL,NULL,&s_flametrail2,true};
statetype s_flametrail2           = {false,SPR_FLAME2_DIE2,3,NULL,NULL,&s_flametrail3,true};
statetype s_flametrail3           = {false,SPR_FLAME2_DIE3,3,NULL,NULL,&s_flametrail4,true};
statetype s_flametrail4           = {false,SPR_FLAME2_DIE4,3,NULL,NULL,NULL,true};

// --- [ HIGH FIREBALL STATETYPES ]
extern  statetype s_hrocket;
extern  statetype s_hsmoke1;
extern  statetype s_hsmoke2;
extern  statetype s_hsmoke3;
extern  statetype s_hsmoke4;
extern  statetype s_hboom2;
extern  statetype s_hboom3;

statetype s_hrocket             = {true,SPR_HROCKET_1,3,(statefunc)T_Projectile,(statefunc)A_Smoke,&s_hrocket,true};
statetype s_hsmoke1             = {false,SPR_HSMOKE_1,3,NULL,NULL,&s_hsmoke2,true};
statetype s_hsmoke2             = {false,SPR_HSMOKE_2,3,NULL,NULL,&s_hsmoke3,true};
statetype s_hsmoke3             = {false,SPR_HSMOKE_3,3,NULL,NULL,&s_hsmoke4,true};
statetype s_hsmoke4             = {false,SPR_HSMOKE_4,3,NULL,NULL,NULL,true};

statetype s_hboom1              = {false,SPR_HBOOM_1,6,NULL,NULL,&s_hboom2,true};
statetype s_hboom2              = {false,SPR_HBOOM_2,6,NULL,NULL,&s_hboom3,true};
statetype s_hboom3              = {false,SPR_HBOOM_3,6,NULL,NULL,NULL,true};

/*
=================================================================================
=
= Projectile & Explosion Stuff
=
=================================================================================
*/

int Damage (int32_t dx, int32_t dy, int32_t range, int32_t max)
{
    double ddx = (double) dx ;
    double ddy = (double) dy ;
    double drange = (double) range ;
    double dmax = (double) max ;
    double distance = sqrt (ddx * ddx + ddy * ddy) ;

    if (distance > drange)
        return 0 ;
    else
        return (int) (dmax - distance / drange * dmax) ;
}

/*
==================
=
= A_Explode
=
==================
*/
void A_Explode(objtype *ob)
{
    int32_t explosionSize;
    int32_t deltax, deltay;
    objtype	*extracheck;
    int		damage;

    switch (ob->obclass) {
        case barrelobj:
            explosionSize = BARRELEXPLSIZE;
            PlaySoundLocActor(BARRELEXPSND,ob);
            break;
        case rocketobj:
            explosionSize = ROCKETEXPLSIZE;
            PlaySoundLocActor(LANDMINESND,ob);
            break;
        case suicideobj:
			gamestate.killcount++;
            ob->flags &= ~FL_SHOOTABLE;
            explosionSize = BARRELEXPLSIZE;
            PlaySoundLocActor(LONGEXPLOSIONSND,ob);
            break;
        default:
            explosionSize = ROCKETEXPLSIZE;
            break;
    }

    //madenoise = true;

	extracheck = objlist;
	while (extracheck)
	{
		if (   extracheck != ob // skip the projectile // skip bosses (doesnt skip superclones)
            && (extracheck->flags & FL_SHOOTABLE)
            && ( (ob->creator != playerobj && !(extracheck->flags & FL_BOSS) ) || ( (ob->creator == playerobj) && CheckLine2(extracheck,ob) ) )  // player created explosions use checkline, actor created explosions don't - this is for ACTOR damage, not player damage.
           )
		{
			deltax = LABS(ob->x - extracheck->x);
			deltay = LABS(ob->y - extracheck->y);
			if (deltax < explosionSize * 2 && deltay < explosionSize * 2) {
				damage = Damage (deltax, deltay, explosionSize, 300);
				if (damage > 0)
					DamageActor (extracheck, damage);
			}
		}
		extracheck = extracheck->next;
	}

	deltax = LABS(ob->x - player->x);
	deltay = LABS(ob->y - player->y);
	if (deltax < explosionSize * 2 && deltay < explosionSize * 2 && CheckLine(ob)) {
            damage = Damage (deltax, deltay, explosionSize, (US_RndT()>>3) + 30);
			damage += (ob->obclass == barrelobj ? 15 : 0);
		if (damage > 0)
			TakeDamage (damage, ob);
	}
} 

/*
=================
=
= A_Smoke
=
=================
*/

void A_Smoke (objtype *ob)
{
    GetNewActor ();

    switch (ob->obclass) {
         case rocketobj:
            newobj->state = &s_smoke1;
            break;
        case hrocketobj: 
            newobj->state = &s_hsmoke1;
            break;
        case fireballobj:
            newobj->state = &s_firetrail1;
            break;
        case spearprojobj:
            newobj->state = &s_speartrail1;
            break;
		case sparkobj:
			newobj->state = &s_flametrail1;
			break;
    }

    newobj->ticcount = 6;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = inertobj;
    newobj->active = ac_yes;

    newobj->flags = FL_NEVERMARK;
}

/*
===================
=
= ProjectileTryMove
=
= returns true if move ok
===================
*/

#define PROJSIZE        0x2000

boolean ProjectileTryMove (objtype *ob)
{
    int      xl,yl,xh,yh,x,y;
    objtype *check;
    int32_t deltax, deltay;

    xl = (ob->x-PROJSIZE) >> TILESHIFT;
    yl = (ob->y-PROJSIZE) >> TILESHIFT;

    xh = (ob->x+PROJSIZE) >> TILESHIFT;
    yh = (ob->y+PROJSIZE) >> TILESHIFT;

    //
    // check for solid walls
    //
    for (y=yl;y<=yh;y++)
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (ob->obclass == fireballobj && check == (objtype *)(uintptr_t)SHOOTERTILE) 
                return true; // fireballs emerge from the wall

            if (check && !ISPOINTER(check))
                return false;
        }

    //
    // check for actors
    //
    for (check=player->next;check;check=check->next)
        if (check->flags & FL_SHOOTABLE && ob->creator != check->obclass)
        {
			if (check->flags & FL_IMMUNE)
				return true;

            deltax = check->x - ob->x;
            deltay = check->y - ob->y;
            if ((LABS(deltax)<PROJECTILESIZE)&&(LABS(deltay)<PROJECTILESIZE)) {
				switch (ob->obclass) {
					case fireballobj:
						check->flags |= FL_GIB;
						KillActor(check);
						if ((check->flags & FL_BOSS) || check->obclass == supercloneobj || check->obclass == columnobj || check->obclass == barrelobj)
							return true;
						else
							return false;
					case spearprojobj:
						DamageActor(check,100);
						return false;

					default:
						if (ob->creator != playerobj && !(check->flags & FL_BOSS)) { // enemy created projectiles
							check->flags |= FL_GIB;
							KillActor(check);
							if (check->obclass == columnobj)
								return true;
							else
								return false;
						} else if (ob->creator != playerobj) { // Boss
							return true;
						} else
							return false; // A_Explode will deal damage
				}
            }
        } 
        return true;
}

/*
=================
=
= T_Projectile
=
=================
*/

void T_Projectile (objtype *ob)
{
    int32_t   deltax,deltay;
    int      damage;
    int32_t   speed;

    speed = (int32_t)ob->speed*tics;

    deltax = FixedMul(speed,costable[ob->angle]);
    deltay = -FixedMul(speed,sintable[ob->angle]);

    if (deltax>0x10000l)
        deltax = 0x10000l;
    if (deltay>0x10000l)
        deltay = 0x10000l;

    ob->x += deltax;
    ob->y += deltay;

    // Hit a wall or actor (NOT PLAYER)
    if (!ProjectileTryMove (ob))
    {
        switch (ob->obclass) {
            case rocketobj:
                ob->state = &s_boom1;
                A_Explode(ob);
                break;

            case hrocketobj: // fireballs
                PlaySoundLocActor(MISSILEHITSND,ob);
                ob->state = &s_hboom1;
                break;

            case fireballobj:
                ob->state = &s_fireexpl1;
                break;

            case spearprojobj:
                PlaySoundLocActor(MISSILEHITSND,ob);
                ob->state = &s_spearexpl1;
                break;

            default:
                ob->state = NULL;
                break;
        }
        return;
    }
    // Check if player hit by anything

    deltax = LABS(ob->x - player->x);
    deltay = LABS(ob->y - player->y);

    if (deltax < PROJECTILESIZE && deltay < PROJECTILESIZE && ob->creator != playerobj)
    {   // hit the player
        damage = (US_RndT() >> 3) + 30;
        TakeDamage (damage,ob);
        ob->state = NULL;      // mark for removal
        return;
    }

    ob->tilex = ob->x >> TILESHIFT;
    ob->tiley = ob->y >> TILESHIFT;

} 

/*
===============
=
= ShootFireballs
=
===============
*/
int sdirs[4][2]={{0,-1},{1,0},{0,1},{-1,0}};

void ShootFireballs() {
    for (int i = 0; i < MAXFIREWALLS; i++)
    {
        if (!fwall[i].active || MAPSPOT(fwall[i].tilex, fwall[i].tiley, 0) != SHOOTERTILE)
            continue;

        if (fwall[i].tics > 0) {
            fwall[i].tics -= tics;
            continue;
        }

        for (int d = NORTH; d <= WEST; d++) { // these walls are 4 way shooters
            if (MAPSPOT(fwall[i].tilex + sdirs[d][0],fwall[i].tiley + sdirs[d][1],0) >= AREATILE)
            {
                GetNewActor ();

                newobj->state = &s_fireball;
                newobj->ticcount = 1;

                newobj->tilex = fwall[i].tilex;
                newobj->tiley = fwall[i].tiley;
                newobj->x = ((int32_t)fwall[i].tilex<<TILESHIFT)+(TILEGLOBAL>>1);
                newobj->y = ((int32_t)fwall[i].tiley<<TILESHIFT)+(TILEGLOBAL>>1);
                newobj->obclass = fireballobj;

                switch (d) {
                    case EAST:
                        newobj->angle = 0;
                        break;
                    case NORTH:
                        newobj->angle = 90;
                        break;
                    case WEST:
                        newobj->angle = 180;
                        break;
                    case SOUTH:
                        newobj->angle = 270;
                        break;
                }
                newobj->speed = 0x2500l;
                newobj->flags = FL_NEVERMARK;
                newobj->active = ac_yes;
            }
        }
        fwall[i].tics = FIREWALLDELAY;
    }
}

// ------------------------------------------------------------------

/*
====================
=
= TakeRandomDamage (FROM SR CODE)
=
====================
*/
void TakeRandomDamage (int x, int y)
{
    switch (gamestate.difficulty)
    {
        case gd_easy:
            TakeDamage (x + (US_RndT()%y),NULL);
            break;
        case gd_medium:
            TakeDamage ((x + 5) + (US_RndT()%y),NULL);
            break;
        default:   //MCS: skill level 1 damage will be divided by 4
            TakeDamage ((x + 10) + (US_RndT()%(y*2)),NULL);
    }
    return;
}  

/*
===========================================
=
= GUARD
=
===========================================
*/
extern  statetype s_grdstand;

extern  statetype s_grdpath1;
extern  statetype s_grdpath1s;
extern  statetype s_grdpath2;
extern  statetype s_grdpath3;
extern  statetype s_grdpath3s;
extern  statetype s_grdpath4;

extern  statetype s_grdpain;
extern  statetype s_grdpain1;

extern  statetype s_grdshoot1;
extern  statetype s_grdshoot2;
extern  statetype s_grdshoot3;
extern  statetype s_grdshoot4;

extern  statetype s_grdchase1;
extern  statetype s_grdchase1s;
extern  statetype s_grdchase2;
extern  statetype s_grdchase3;
extern  statetype s_grdchase3s;
extern  statetype s_grdchase4;

extern  statetype s_grddie1;
extern  statetype s_grddie1d;
extern  statetype s_grddie2;
extern  statetype s_grddie3;
extern  statetype s_grddie4;

statetype s_grdstand            = {true,SPR_GRD_S_1,0,(statefunc)T_Stand,NULL,&s_grdstand,false};

statetype s_grdpath1            = {true,SPR_GRD_W1_1,20,(statefunc)T_Path,NULL,&s_grdpath1s,false};
statetype s_grdpath1s           = {true,SPR_GRD_W1_1,5,NULL,NULL,&s_grdpath2,false};
statetype s_grdpath2            = {true,SPR_GRD_W2_1,15,(statefunc)T_Path,NULL,&s_grdpath3,false};
statetype s_grdpath3            = {true,SPR_GRD_W3_1,20,(statefunc)T_Path,NULL,&s_grdpath3s,false};
statetype s_grdpath3s           = {true,SPR_GRD_W3_1,5,NULL,NULL,&s_grdpath4,false};
statetype s_grdpath4            = {true,SPR_GRD_W4_1,15,(statefunc)T_Path,NULL,&s_grdpath1,false};

statetype s_grdpain             = {2,SPR_GRD_PAIN_1,10,NULL,NULL,&s_grdchase1,false};
statetype s_grdpain1            = {2,SPR_GRD_PAIN_2,10,NULL,NULL,&s_grdchase1,false};

statetype s_grdshoot1           = {false,SPR_GRD_SHOOT1,20,NULL,NULL,&s_grdshoot2,false};
statetype s_grdshoot2           = {false,SPR_GRD_SHOOT2,20,NULL,(statefunc)T_Shoot,&s_grdshoot3,false};
statetype s_grdshoot3           = {false,SPR_GRD_SHOOT3,20,NULL,NULL,&s_grdchase1,true};

statetype s_grdchase1           = {true,SPR_GRD_W1_1,10,(statefunc)T_Chase,NULL,&s_grdchase1s,false};
statetype s_grdchase1s          = {true,SPR_GRD_W1_1,3,NULL,NULL,&s_grdchase2,false};
statetype s_grdchase2           = {true,SPR_GRD_W2_1,8,(statefunc)T_Chase,NULL,&s_grdchase3,false};
statetype s_grdchase3           = {true,SPR_GRD_W3_1,10,(statefunc)T_Chase,NULL,&s_grdchase3s,false};
statetype s_grdchase3s          = {true,SPR_GRD_W3_1,3,NULL,NULL,&s_grdchase4,false};
statetype s_grdchase4           = {true,SPR_GRD_W4_1,8,(statefunc)T_Chase,NULL,&s_grdchase1,false};

statetype s_grddie1             = {false,SPR_GRD_DIE_1,15,NULL,(statefunc)A_DeathScream,&s_grddie2,false};
statetype s_grddie2             = {false,SPR_GRD_DIE_2,15,NULL,NULL,&s_grddie3,false};
statetype s_grddie3             = {false,SPR_GRD_DIE_3,15,NULL,NULL,&s_grddie4,false};
statetype s_grddie4             = {false,SPR_GRD_DEAD,0,NULL,NULL,&s_grddie4,false};

// ----------------------------------------
/*
===========================================
=
= SUICIDE CLONES
=
===========================================
*/
extern  statetype s_suicidestand;

extern  statetype s_suicidepath1;
extern  statetype s_suicidepath1s;
extern  statetype s_suicidepath2;
extern  statetype s_suicidepath3;
extern  statetype s_suicidepath3s;
extern  statetype s_suicidepath4;

extern  statetype s_suicidetrigger1;
extern  statetype s_suicidetrigger2;
extern  statetype s_suicidetrigger3;
extern  statetype s_suicidetrigger4;
extern  statetype s_suicidetrigger5;

extern  statetype s_suicidechase1;
extern  statetype s_suicidechase1s;
extern  statetype s_suicidechase2;
extern  statetype s_suicidechase3;
extern  statetype s_suicidechase3s;
extern  statetype s_suicidechase4;

extern  statetype s_suicidedie0;
extern  statetype s_suicidedie1;
extern  statetype s_suicidedie2;
extern  statetype s_suicidedie3;
extern  statetype s_suicidedead;

statetype s_suicidestand            = {true,SPR_SCLONE_W1_1,0,(statefunc)T_Stand,NULL,&s_suicidestand,false};

statetype s_suicidepath1            = {true,SPR_SCLONE_W1_1,20,(statefunc)T_Path,NULL,&s_suicidepath1s,false};
statetype s_suicidepath1s           = {true,SPR_SCLONE_W1_1,5,NULL,NULL,&s_suicidepath2,false};
statetype s_suicidepath2            = {true,SPR_SCLONE_W2_1,15,(statefunc)T_Path,NULL,&s_suicidepath3,false};
statetype s_suicidepath3            = {true,SPR_SCLONE_W3_1,20,(statefunc)T_Path,NULL,&s_suicidepath3s,false};
statetype s_suicidepath3s           = {true,SPR_SCLONE_W3_1,5,NULL,NULL,&s_suicidepath4,false};
statetype s_suicidepath4            = {true,SPR_SCLONE_W4_1,15,(statefunc)T_Path,NULL,&s_suicidepath1,false};

statetype s_suicidetrigger1            = {false,SPR_SCLONE_JUMP1,10,NULL,NULL,&s_suicidetrigger2,false};
statetype s_suicidetrigger2            = {false,SPR_SCLONE_JUMP2,10,NULL,(statefunc)T_Trigger,&s_suicidetrigger3,false};
statetype s_suicidetrigger3            = {false,SPR_SCLONE_JUMP3,10,NULL,NULL,&s_suicidetrigger4,false};
statetype s_suicidetrigger4            = {false,SPR_SCLONE_JUMP1,10,(statefunc)T_Defuse,NULL,&s_suicidedie1,false};

statetype s_suicidechase1           = {true,SPR_SCLONE_W1_1,10,(statefunc)T_SuicideChase,NULL,&s_suicidechase1s,false};
statetype s_suicidechase1s          = {true,SPR_SCLONE_W1_1,3,NULL,NULL,&s_suicidechase2,false};
statetype s_suicidechase2           = {true,SPR_SCLONE_W2_1,8,(statefunc)T_SuicideChase,NULL,&s_suicidechase3,false};
statetype s_suicidechase3           = {true,SPR_SCLONE_W3_1,10,(statefunc)T_SuicideChase,NULL,&s_suicidechase3s,false};
statetype s_suicidechase3s          = {true,SPR_SCLONE_W3_1,3,NULL,NULL,&s_suicidechase4,false};
statetype s_suicidechase4           = {true,SPR_SCLONE_W4_1,8,(statefunc)T_SuicideChase,NULL,&s_suicidechase1,false};

statetype s_suicidedie1             = {false,SPR_SCLONE_DIE_1,10,NULL,(statefunc)A_Explode,&s_suicidedie2,true};
statetype s_suicidedie2             = {false,SPR_SCLONE_DIE_2,10,NULL,NULL,&s_suicidedie3,false};
statetype s_suicidedie3             = {false,SPR_SCLONE_DIE_3,10,NULL,NULL,&s_suicidedead,false};
statetype s_suicidedead             = {false,SPR_SCLONE_DEAD,0,NULL,NULL,&s_suicidedead,false};

/*
=================
=
= T_SuicideChase
=
=================
*/

void T_SuicideChase (objtype *ob)
{
    int32_t move;
    int     dx,dy;
    boolean dodge;

    if (gamestate.victoryflag)
        return;

    dodge = false;
    if (CheckLine(ob))      // got a shot at player?
    {
        ob->hidden = false;
        dodge = true;
    }
    else
        ob->hidden = true;

    if (ob->dir == nodir)
    {
        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {

        dx = player->x - ob->x;
        if (dx<0)
            dx = -dx;
        dx -= move;
        if (dx <= MINACTORDIST)
        {
            dy = player->y - ob->y;
            if (dy<0)
                dy = -dy;
            dy -= move;
            if (dy <= MINACTORDIST)
            {
                if (ob->temp2 == -1)
                    NewState(ob,&s_suicidedie1);
                else
                    NewState (ob,&s_suicidetrigger1);
                return;
            }
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        CheckEnemyTile(ob);

#ifdef EODSPECIAL
        if (gamestate.mapon == 30 && !gamestate.gong) {
            SelectLootDir(ob);
        } else {
#endif
            if (dodge)
                SelectDodgeDir (ob);
            else
                SelectChaseDir (ob);
#ifdef EODSPECIAL
        }
#endif

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

/*
================
=
= T_Trigger
=
================
*/
void T_Trigger (objtype *ob) {
   SD_PlaySound(BOMBTRIGGERSND);
}

/*
================
=
= T_Defuse
=
================
*/

void T_Defuse (objtype *ob) {
    int32_t deltax = abs(player->x - ob->x) - TILEGLOBAL;
    int32_t deltay = abs(player->y - ob->y) - TILEGLOBAL;

    if (deltax > MINACTORDIST || deltay > MINACTORDIST) {
        ob->temp2 = -1;
        NewState(ob,&s_suicidechase1);
    }

}

// ----------------------------------------
/*
===========================================
=
= OFFICER
=
===========================================
*/

extern  statetype s_ofcstand;

extern  statetype s_ofcpath1;
extern  statetype s_ofcpath1s;
extern  statetype s_ofcpath2;
extern  statetype s_ofcpath3;
extern  statetype s_ofcpath3s;
extern  statetype s_ofcpath4;

extern  statetype s_ofcpain;
extern  statetype s_ofcpain1;

extern  statetype s_ofcgiveup;

extern  statetype s_ofcshoot1;
extern  statetype s_ofcshoot2;
extern  statetype s_ofcshoot3;
extern  statetype s_ofcshoot4;

extern  statetype s_ofcchase1;
extern  statetype s_ofcchase1s;
extern  statetype s_ofcchase2;
extern  statetype s_ofcchase3;
extern  statetype s_ofcchase3s;
extern  statetype s_ofcchase4;

extern  statetype s_ofcdie1;
extern  statetype s_ofcdie2;
extern  statetype s_ofcdie3;
extern  statetype s_ofcdie4;
extern  statetype s_ofcdie5;

statetype s_ofcstand            = {true,SPR_OFC_S_1,0,(statefunc)T_Stand,NULL,&s_ofcstand,false};

statetype s_ofcpath1            = {true,SPR_OFC_W1_1,20,(statefunc)T_Path,NULL,&s_ofcpath1s,false};
statetype s_ofcpath1s           = {true,SPR_OFC_W1_1,5,NULL,NULL,&s_ofcpath2,false};
statetype s_ofcpath2            = {true,SPR_OFC_W2_1,15,(statefunc)T_Path,NULL,&s_ofcpath3,false};
statetype s_ofcpath3            = {true,SPR_OFC_W3_1,20,(statefunc)T_Path,NULL,&s_ofcpath3s,false};
statetype s_ofcpath3s           = {true,SPR_OFC_W3_1,5,NULL,NULL,&s_ofcpath4,false};
statetype s_ofcpath4            = {true,SPR_OFC_W4_1,15,(statefunc)T_Path,NULL,&s_ofcpath1,false};

statetype s_ofcpain             = {2,SPR_OFC_PAIN_1,10,NULL,NULL,&s_ofcchase1,false};
statetype s_ofcpain1            = {2,SPR_OFC_PAIN_2,10,NULL,NULL,&s_ofcchase1,false};

statetype s_ofcshoot1           = {false,SPR_OFC_SHOOT1,6,NULL,NULL,&s_ofcshoot2,false};
statetype s_ofcshoot2           = {false,SPR_OFC_SHOOT2,20,NULL,(statefunc)T_Shoot,&s_ofcshoot3,false};
statetype s_ofcshoot3           = {false,SPR_OFC_SHOOT3,10,NULL,NULL,&s_ofcchase1,true};

statetype s_ofcchase1           = {true,SPR_OFC_W1_1,10,(statefunc)T_Chase,NULL,&s_ofcchase1s,false};
statetype s_ofcchase1s          = {true,SPR_OFC_W1_1,3,NULL,NULL,&s_ofcchase2,false};
statetype s_ofcchase2           = {true,SPR_OFC_W2_1,8,(statefunc)T_Chase,NULL,&s_ofcchase3,false};
statetype s_ofcchase3           = {true,SPR_OFC_W3_1,10,(statefunc)T_Chase,NULL,&s_ofcchase3s,false};
statetype s_ofcchase3s          = {true,SPR_OFC_W3_1,3,NULL,NULL,&s_ofcchase4,false};
statetype s_ofcchase4           = {true,SPR_OFC_W4_1,8,(statefunc)T_Chase,NULL,&s_ofcchase1,false};

statetype s_ofcdie1             = {false,SPR_OFC_DIE_1,11,NULL,(statefunc)A_DeathScream,&s_ofcdie2,false};
statetype s_ofcdie2             = {false,SPR_OFC_DIE_2,11,NULL,NULL,&s_ofcdie3,false};
statetype s_ofcdie3             = {false,SPR_OFC_DIE_3,11,NULL,NULL,&s_ofcdie4,false};
statetype s_ofcdie4             = {false,SPR_OFC_DIE_4,11,NULL,NULL,&s_ofcdie5,false};
statetype s_ofcdie5             = {false,SPR_OFC_DEAD,0,NULL,NULL,&s_ofcdie5,false};

// ----------------------------------------
/*
===========================================
=
= REGULAR CLONE
=
===========================================
*/
extern  statetype s_mutstand;

extern  statetype s_mutpath1;
extern  statetype s_mutpath1s;
extern  statetype s_mutpath2;
extern  statetype s_mutpath3;
extern  statetype s_mutpath3s;
extern  statetype s_mutpath4;

extern  statetype s_mutpain;
extern  statetype s_mutpain1;

extern  statetype s_mutgiveup;

extern  statetype s_mutshoot1;
extern  statetype s_mutshoot2;
extern  statetype s_mutshoot3;
extern  statetype s_mutshoot4;

extern  statetype s_mutchase1;
extern  statetype s_mutchase1s;
extern  statetype s_mutchase2;
extern  statetype s_mutchase3;
extern  statetype s_mutchase3s;
extern  statetype s_mutchase4;

extern  statetype s_mutdie1;
extern  statetype s_mutdie2;
extern  statetype s_mutdie3;
extern  statetype s_mutdie4;
extern  statetype s_mutdie5;

statetype s_mutstand            = {true,SPR_MUT_S_1,0,(statefunc)T_Stand,NULL,&s_mutstand,false};
statetype s_mutsit              = {false,SPR_TOILETCLONE,0,(statefunc)T_Stand,NULL,&s_mutsit,false}; // toilet clone

statetype s_mutpath1            = {true,SPR_MUT_W1_1,20,(statefunc)T_Path,NULL,&s_mutpath1s,false};
statetype s_mutpath1s           = {true,SPR_MUT_W1_1,5,NULL,NULL,&s_mutpath2,false};
statetype s_mutpath2            = {true,SPR_MUT_W2_1,15,(statefunc)T_Path,NULL,&s_mutpath3,false};
statetype s_mutpath3            = {true,SPR_MUT_W3_1,20,(statefunc)T_Path,NULL,&s_mutpath3s,false};
statetype s_mutpath3s           = {true,SPR_MUT_W3_1,5,NULL,NULL,&s_mutpath4,false};
statetype s_mutpath4            = {true,SPR_MUT_W4_1,15,(statefunc)T_Path,NULL,&s_mutpath1,false};

statetype s_mutpain             = {2,SPR_MUT_PAIN_1,10,NULL,NULL,&s_mutchase1,false};
statetype s_mutpain1            = {2,SPR_MUT_PAIN_2,10,NULL,NULL,&s_mutchase1,false};

statetype s_mutshoot1           = {false,SPR_MUT_SHOOT1,6,NULL,(statefunc)T_Shoot,&s_mutshoot2,false};
statetype s_mutshoot2           = {false,SPR_MUT_SHOOT2,20,NULL,NULL,&s_mutshoot3,true};
statetype s_mutshoot3           = {false,SPR_MUT_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_mutshoot4,false};
statetype s_mutshoot4           = {false,SPR_MUT_SHOOT4,20,NULL,NULL,&s_mutchase1,true};

statetype s_mutchase1           = {true,SPR_MUT_W1_1,10,(statefunc)T_Chase,NULL,&s_mutchase1s,false};
statetype s_mutchase1s          = {true,SPR_MUT_W1_1,3,NULL,NULL,&s_mutchase2,false};
statetype s_mutchase2           = {true,SPR_MUT_W2_1,8,(statefunc)T_Chase,NULL,&s_mutchase3,false};
statetype s_mutchase3           = {true,SPR_MUT_W3_1,10,(statefunc)T_Chase,NULL,&s_mutchase3s,false};
statetype s_mutchase3s          = {true,SPR_MUT_W3_1,3,NULL,NULL,&s_mutchase4,false};
statetype s_mutchase4           = {true,SPR_MUT_W4_1,8,(statefunc)T_Chase,NULL,&s_mutchase1,false};

statetype s_mutdie1             = {false,SPR_MUT_DIE_1,7,NULL,(statefunc)A_DeathScream,&s_mutdie2,false};
statetype s_mutdie2             = {false,SPR_MUT_DIE_2,7,NULL,NULL,&s_mutdie3,false};
statetype s_mutdie3             = {false,SPR_MUT_DIE_3,7,NULL,NULL,&s_mutdie4,false};
statetype s_mutdie4             = {false,SPR_MUT_DIE_4,7,NULL,NULL,&s_mutdie5,false};
statetype s_mutdie5             = {false,SPR_MUT_DEAD,0,NULL,NULL,&s_mutdie5,false};

// ----------------------------------------
/*
===========================================
=
= SS
=
===========================================
*/
extern  statetype s_ssstand;

extern  statetype s_sspath1;
extern  statetype s_sspath1s;
extern  statetype s_sspath2;
extern  statetype s_sspath3;
extern  statetype s_sspath3s;
extern  statetype s_sspath4;

extern  statetype s_sspain;
extern  statetype s_sspain1;

extern  statetype s_ssshoot1;
extern  statetype s_ssshoot2;
extern  statetype s_ssshoot3;
extern  statetype s_ssshoot4;
extern  statetype s_ssshoot5;
extern  statetype s_ssshoot6;
extern  statetype s_ssshoot7;
extern  statetype s_ssshoot8;
extern  statetype s_ssshoot9;

extern  statetype s_sschase1;
extern  statetype s_sschase1s;
extern  statetype s_sschase2;
extern  statetype s_sschase3;
extern  statetype s_sschase3s;
extern  statetype s_sschase4;

extern  statetype s_ssdie1;
extern  statetype s_ssdie2;
extern  statetype s_ssdie3;
extern  statetype s_ssdie4;

statetype s_ssstand             = {true,SPR_SS_S_1,0,(statefunc)T_Stand,NULL,&s_ssstand,false};

statetype s_sspath1             = {true,SPR_SS_W1_1,20,(statefunc)T_Path,NULL,&s_sspath1s,false};
statetype s_sspath1s            = {true,SPR_SS_W1_1,5,NULL,NULL,&s_sspath2,false};
statetype s_sspath2             = {true,SPR_SS_W2_1,15,(statefunc)T_Path,NULL,&s_sspath3,false};
statetype s_sspath3             = {true,SPR_SS_W3_1,20,(statefunc)T_Path,NULL,&s_sspath3s,false};
statetype s_sspath3s            = {true,SPR_SS_W3_1,5,NULL,NULL,&s_sspath4,false};
statetype s_sspath4             = {true,SPR_SS_W4_1,15,(statefunc)T_Path,NULL,&s_sspath1,false};

statetype s_sspain              = {2,SPR_SS_PAIN_1,10,NULL,NULL,&s_sschase1,false};
statetype s_sspain1             = {2,SPR_SS_PAIN_2,10,NULL,NULL,&s_sschase1,false};

statetype s_ssshoot1            = {false,SPR_SS_SHOOT1,20,NULL,NULL,&s_ssshoot2,false};
statetype s_ssshoot2            = {false,SPR_SS_SHOOT2,20,NULL,(statefunc)T_Shoot,&s_ssshoot3,false};
statetype s_ssshoot3            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot4,true};
statetype s_ssshoot4            = {false,SPR_SS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_ssshoot5,false};
statetype s_ssshoot5            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot6,true};
statetype s_ssshoot6            = {false,SPR_SS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_ssshoot7,false};
statetype s_ssshoot7            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot8,true};
statetype s_ssshoot8            = {false,SPR_SS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_ssshoot9,false};
statetype s_ssshoot9            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_sschase1,true};

statetype s_sschase1            = {true,SPR_SS_W1_1,10,(statefunc)T_Chase,NULL,&s_sschase1s,false};
statetype s_sschase1s           = {true,SPR_SS_W1_1,3,NULL,NULL,&s_sschase2,false};
statetype s_sschase2            = {true,SPR_SS_W2_1,8,(statefunc)T_Chase,NULL,&s_sschase3,false};
statetype s_sschase3            = {true,SPR_SS_W3_1,10,(statefunc)T_Chase,NULL,&s_sschase3s,false};
statetype s_sschase3s           = {true,SPR_SS_W3_1,3,NULL,NULL,&s_sschase4,false};
statetype s_sschase4            = {true,SPR_SS_W4_1,8,(statefunc)T_Chase,NULL,&s_sschase1,false};

statetype s_ssdie1              = {false,SPR_SS_DIE_1,15,NULL,(statefunc)A_DeathScream,&s_ssdie2,false};
statetype s_ssdie2              = {false,SPR_SS_DIE_2,15,NULL,NULL,&s_ssdie3,false};
statetype s_ssdie3              = {false,SPR_SS_DIE_3,15,NULL,NULL,&s_ssdie4,false};
statetype s_ssdie4              = {false,SPR_SS_DEAD,0,NULL,NULL,&s_ssdie4,false};

// ----------------------------------------
/*
===========================================

 SPAWNING ROUTINES

===========================================
*/

/*
===============
=
= SpawnStand
=
===============
*/

void SpawnStand (enemy_t which, int tilex, int tiley, int dir)
{
    word *map;
    word tile;

    switch (which)
    {
        case en_guard:
            SpawnNewObj (tilex,tiley,&s_grdstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_officer:
            SpawnNewObj (tilex,tiley,&s_ofcstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_mutant:
            SpawnNewObj (tilex,tiley,&s_mutstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_ss:
            SpawnNewObj (tilex,tiley,&s_ssstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_suicide:
            SpawnNewObj (tilex,tiley,&s_suicidestand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;
    }


    map = mapsegs[0]+(tiley<<mapshift)+tilex;
    tile = *map;
    if (tile == AMBUSHTILE)
    {
        tilemap[tilex][tiley] = 0;

        if (*(map+1) >= AREATILE)
            tile = *(map+1);
        if (*(map-mapwidth) >= AREATILE)
            tile = *(map-mapwidth);
        if (*(map+mapwidth) >= AREATILE)
            tile = *(map+mapwidth);
        if ( *(map-1) >= AREATILE)
            tile = *(map-1);

        *map = tile;
        newobj->areanumber = tile-AREATILE;

        newobj->flags |= FL_AMBUSH;
    }

    newobj->obclass = (classtype)(guardobj + which);
    newobj->hitpoints = starthitpoints[gamestate.difficulty][which];
    newobj->dir = (dirtype)(dir * 2);
    newobj->flags |= FL_SHOOTABLE;
}

/*
===============
=
= SpawnPatrol
=
===============
*/

void SpawnPatrol (enemy_t which, int tilex, int tiley, int dir)
{
    switch (which)
    {
        case en_guard:
            SpawnNewObj (tilex,tiley,&s_grdpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_officer:
            SpawnNewObj (tilex,tiley,&s_ofcpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_ss:
            SpawnNewObj (tilex,tiley,&s_sspath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_mutant:
            SpawnNewObj (tilex,tiley,&s_mutpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_suicide:
            SpawnNewObj (tilex,tiley,&s_suicidepath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;
    }

    newobj->obclass = (classtype)(guardobj+which);
    newobj->dir = (dirtype)(dir*2);
    newobj->hitpoints = starthitpoints[gamestate.difficulty][which];
    newobj->distance = TILEGLOBAL;
    newobj->flags |= FL_SHOOTABLE;
    newobj->active = ac_yes;

    actorat[newobj->tilex][newobj->tiley] = NULL;           // don't use original spot

    switch (dir)
    {
        case 0:
            newobj->tilex++;
            break;
        case 1:
            newobj->tiley--;
            break;
        case 2:
            newobj->tilex--;
            break;
        case 3:
            newobj->tiley++;
            break;
    }

    actorat[newobj->tilex][newobj->tiley] = newobj;
}

/*
===============
=
= SpawnDeadGuy
=
===============
*/

void SpawnDeadGuy (enemy_t which,int tilex, int tiley)
{
    switch (which) {
        case en_guard:
            SpawnNewObj (tilex,tiley,&s_grddie4);
            break;
        case en_ss:
            SpawnNewObj (tilex,tiley,&s_ssdie4);
            break;
        case en_officer:
            SpawnNewObj (tilex,tiley,&s_ofcdie5);
            break;
        case en_mutant:
            SpawnNewObj (tilex,tiley,&s_mutdie5);
            break;
        case en_suicide:
            SpawnNewObj (tilex,tiley,&s_suicidedead);
            break;
    }
    DEMOIF_SDL
    {
        newobj->flags |= FL_NONMARK;    // walk through moving enemy fix
    }
    newobj->obclass = inertobj;
}

/*
===============
=
= SpawnToiletClone
=
===============
*/

void SpawnToiletClone (int tilex, int tiley) {
    SpawnNewObj (tilex,tiley,&s_mutsit);
    newobj->obclass = toiletobj;
    newobj->speed = SPDPATROL;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_mutant];
    newobj->flags |= FL_SHOOTABLE;
        
    word *map = mapsegs[0]+(tiley<<mapshift)+tilex;
    word tile = *map;
    if (tile == AMBUSHTILE)
    {
        tilemap[tilex][tiley] = 0;

        if (*(map+1) >= AREATILE)
            tile = *(map+1);
        if (*(map-mapwidth) >= AREATILE)
            tile = *(map-mapwidth);
        if (*(map+mapwidth) >= AREATILE)
            tile = *(map+mapwidth);
        if ( *(map-1) >= AREATILE)
            tile = *(map-1);

        *map = tile;
        newobj->areanumber = tile-AREATILE;

        newobj->flags |= FL_AMBUSH;
    }
}

/*
==================
=
= SpawnTriggeredActors
=
==================
*/

void SpawnTriggeredActors(enemy_t which, int targetTile) {
    for (int x = 0; x < mapwidth; x++) {
        for (int y = 0; y < mapwidth; y++) {
            if (MAPSPOT(x,y,1) == targetTile) {
                switch(which) {
                    case en_guard:
                        SpawnNewObj (x,y,&s_grdchase1);
                        newobj->speed = SPDPATROL*3;
                        break;
                    case en_ss:
                        SpawnNewObj (x,y,&s_sschase1);
                        newobj->speed = SPDPATROL*4;
                        break;
                    case en_officer:
                        SpawnNewObj (x,y,&s_ofcchase1);
                        newobj->speed = SPDPATROL*5;
                        break;
                }

                newobj->obclass = (classtype)(guardobj + which);
                newobj->hitpoints = starthitpoints[gamestate.difficulty][which];
                newobj->dir = nodir;
                newobj->distance = TILEGLOBAL;  
                newobj->flags |= FL_SHOOTABLE|FL_FIRSTATTACK|FL_ATTACKMODE;
                newobj->active = ac_yes;
                gamestate.killtotal++; // add to the kill total!
            }
        }
    }           
}

/*
==================
=
= A_DeathScream
=
==================
*/

int grdsounds[9]={ DEATHSCREAM1SND,
                DEATHSCREAM2SND,
                DEATHSCREAM3SND,
#ifndef APOGEE_1_0
                DEATHSCREAM4SND,
                DEATHSCREAM5SND,
                DEATHSCREAM7SND,
                DEATHSCREAM8SND,
                DEATHSCREAM9SND
#endif
            };

void A_DeathScream (objtype *ob)
{
    if (ob->flags & FL_STEALTH) // The stealth death sound
    {
        switch(ob->obclass)
        {
            case guardobj:
            case officerobj:
            case ssobj:
                PlaySoundLocActor(DEATHSCREAM10SND,ob);
                return;
        }
    }

    switch (ob->obclass)
    {
        case mutantobj:
        case toiletobj:
            PlaySoundLocActor(AHHHGSND,ob);
            break;

        case guardobj:
        {
#ifndef UPLOAD
            #ifdef CRAND
            PlaySoundLocActor(grdsounds[Random(8)],ob);
            #else
            PlaySoundLocActor(grdsounds[US_RndT()%8],ob);
            #endif
#else
            PlaySoundLocActor(grdsounds[US_RndT()%2],ob);
#endif
            break;
        }
        case officerobj:
            PlaySoundLocActor(NEINSOVASSND,ob);
            break;
        case ssobj:
            PlaySoundLocActor(LEBENSND,ob); // JAB
            break;
        case suicideobj:
            PlaySoundLocActor(LONGEXPLOSIONSND,ob);      // JAB
            break;
        case spectreobj:
        case ghostobj:
            SD_PlaySound(GHOSTFADESND);
            break;
        case snowmanobj:
            PlaySoundLocActor(FARTSND,ob);
            break;
        case angelobj:
            PlaySoundLocActor(ANGELDEATHSND,ob);
            break;
        case grauobj:
            PlaySoundLocActor(GRAUDEATHSND,ob);
            break;
        case supercloneobj:
        case ultracloneobj:
            PlaySoundLocActor(CLONEBOSSDEATHSND,ob);
            break;
        case ottoobj:
            PlaySoundLocActor(OTTODEATHSND,ob);
            break;
        case baronobj:
            PlaySoundLocActor(BARONDEATHSND,ob);
            break;
        case robotobj:
            PlaySoundLocActor(ROBOTDEATHSND,ob);
            break;
        case heinrichobj:
            PlaySoundLocActor(HEINRICHDEATHSND,ob);
            break;
        case turretobj:
            PlaySoundLocActor(LANDMINESND,ob);
            break;
        case barrelobj:
            PlaySoundLocActor(BARRELEXPSND,ob);
            break;
        case columnobj:
            PlaySoundLocActor(COLUMNBREAKSND,ob);
            break;
    }
}

void A_SpawnNextBoss(objtype *ob) {
    switch (ob->obclass) {

        case grauobj:
            for (int x = 0; x < MAPSIZE; x++) {
                for (int y = 0; y < MAPSIZE; y++) {
                    if (MAPSPOT(x,y,1) == 161) {
                        SpawnBaron(x,y);
                    }
                }
            }
            break;

        case baronobj:
            for (int x = 0; x < MAPSIZE; x++) {
                for (int y = 0; y < MAPSIZE; y++) {
                    if (MAPSPOT(x,y,1) == 107) {
                        SpawnAngel(x,y);
                    }
                }
            }
            break;

    }
}

#ifdef SPEAR

extern  statetype s_angelshoot1;
extern  statetype s_baronshoot1;
extern  statetype s_spark1;

//
// trans
//

extern  statetype s_graustand;

extern  statetype s_grauchase1;
extern  statetype s_grauchase1s;
extern  statetype s_grauchase2;
extern  statetype s_grauchase3;
extern  statetype s_grauchase3s;
extern  statetype s_grauchase4;

extern  statetype s_graudie0;
extern  statetype s_graudie01;
extern  statetype s_graudie1;
extern  statetype s_graudie2;
extern  statetype s_graudie3;
extern  statetype s_graudie4;

extern  statetype s_graushoot1;
extern  statetype s_graushoot2;
extern  statetype s_graushoot3;
extern  statetype s_graushoot4;
extern  statetype s_graushoot5;
extern  statetype s_graushoot6;
extern  statetype s_graushoot7;
extern  statetype s_graushoot8;

statetype s_graustand          = {false,SPR_GRAU_W1,0,(statefunc)T_Stand,NULL,&s_graustand,true};

statetype s_grauchase1         = {false,SPR_GRAU_W1,10,(statefunc)T_Chase,NULL,&s_grauchase1s,true};
statetype s_grauchase1s        = {false,SPR_GRAU_W1,3,NULL,NULL,&s_grauchase2,true};
statetype s_grauchase2         = {false,SPR_GRAU_W2,8,(statefunc)T_Chase,NULL,&s_grauchase3,true};
statetype s_grauchase3         = {false,SPR_GRAU_W3,10,(statefunc)T_Chase,NULL,&s_grauchase3s,true};
statetype s_grauchase3s        = {false,SPR_GRAU_W3,3,NULL,NULL,&s_grauchase4,true};
statetype s_grauchase4         = {false,SPR_GRAU_W4,8,(statefunc)T_Chase,NULL,&s_grauchase1,true};

statetype s_graudie0           = {false,SPR_GRAU_W1,1,NULL,(statefunc)A_DeathScream,&s_graudie01,true};
statetype s_graudie01          = {false,SPR_GRAU_W1,1,NULL,NULL,&s_graudie1,true};
statetype s_graudie1           = {false,SPR_GRAU_DIE1,15,NULL,NULL,&s_graudie2,true};
statetype s_graudie2           = {false,SPR_GRAU_DIE2,15,NULL,NULL,&s_graudie3,true};
statetype s_graudie3           = {false,SPR_GRAU_DIE3,15,NULL,(statefunc)A_SpawnNextBoss,&s_graudie4,true};
statetype s_graudie4           = {false,SPR_GRAU_DEAD,0,NULL,NULL,&s_graudie4,true};

statetype s_graushoot1         = {false,SPR_GRAU_SHOOT1,30,NULL,NULL,&s_graushoot2,true};
statetype s_graushoot2         = {false,SPR_GRAU_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_graushoot3,true};
statetype s_graushoot3         = {false,SPR_GRAU_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_graushoot4,true};
statetype s_graushoot4         = {false,SPR_GRAU_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_graushoot5,true};
statetype s_graushoot5         = {false,SPR_GRAU_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_graushoot6,true};
statetype s_graushoot6         = {false,SPR_GRAU_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_graushoot7,true};
statetype s_graushoot7         = {false,SPR_GRAU_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_graushoot8,true};
statetype s_graushoot8         = {false,SPR_GRAU_SHOOT1,10,NULL,NULL,&s_grauchase1,true};

/*
===============
=
= SpawnGrau
=
===============
*/

void SpawnGrau (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_graustand);
    newobj->obclass = grauobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_grau];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH|FL_BOSS;
    if (!loadedgame)
        gamestate.killtotal++;
}


//
// uber
//
void T_SuperCloneShoot (objtype *ob);

extern  statetype s_superclonestand;

extern  statetype s_superclonechase1;
extern  statetype s_superclonechase1s;
extern  statetype s_superclonechase2;
extern  statetype s_superclonechase3;
extern  statetype s_superclonechase3s;
extern  statetype s_superclonechase4;

extern  statetype s_superclonedie1;
extern  statetype s_superclonedie2;
extern  statetype s_superclonedie3;
extern  statetype s_superclonedie4;
extern  statetype s_superclonedie5;
extern statetype s_supercloneres;

extern  statetype s_supercloneshoot1;
extern  statetype s_supercloneshoot2;
extern  statetype s_supercloneshoot3;
extern  statetype s_supercloneshoot4;
extern  statetype s_supercloneshoot5;
extern  statetype s_supercloneshoot6;
extern  statetype s_supercloneshoot7;

extern statetype s_supercloneres1;
extern statetype s_supercloneres2;
extern statetype s_supercloneres3;
extern statetype s_supercloneres4;
extern statetype  s_supercloneresflag;
extern  statetype s_superclonedead;

statetype s_superclonestand           = {false,SPR_SUPERCLONE_W1,0,(statefunc)T_Stand,NULL,&s_superclonestand,false};

statetype s_superclonechase1          = {false,SPR_SUPERCLONE_W1,10,(statefunc)T_Chase,NULL,&s_superclonechase1s,false};
statetype s_superclonechase1s         = {false,SPR_SUPERCLONE_W1,3,NULL,NULL,&s_superclonechase2,false};
statetype s_superclonechase2          = {false,SPR_SUPERCLONE_W2,8,(statefunc)T_Chase,NULL,&s_superclonechase3,false};
statetype s_superclonechase3          = {false,SPR_SUPERCLONE_W3,10,(statefunc)T_Chase,NULL,&s_superclonechase3s,false};
statetype s_superclonechase3s         = {false,SPR_SUPERCLONE_W3,3,NULL,NULL,&s_superclonechase4,false};
statetype s_superclonechase4          = {false,SPR_SUPERCLONE_W4,8,(statefunc)T_Chase,NULL,&s_superclonechase1,false};

statetype s_superclonedie1            = {false,SPR_SUPERCLONE_DIE1,15,NULL,(statefunc)A_DeathScream,&s_superclonedie2,false};
statetype s_superclonedie2            = {false,SPR_SUPERCLONE_DIE2,15,NULL,NULL,&s_superclonedie3,false};
statetype s_superclonedie3            = {false,SPR_SUPERCLONE_DIE3,15,NULL,NULL,&s_superclonedie4,false};
statetype s_superclonedie4            = {false,SPR_SUPERCLONE_DIE4,15,NULL,NULL,&s_superclonedie5,false};
statetype s_superclonedie5            = {false,SPR_SUPERCLONE_DEAD,300,NULL,NULL,&s_supercloneres,false};
statetype s_supercloneres             = {false,SPR_SUPERCLONE_DEAD,10,(statefunc)A_Dormant,NULL,&s_supercloneres,false};

statetype s_supercloneshoot1          = {false,SPR_SUPERCLONE_SHOOT1,30,NULL,NULL,&s_supercloneshoot2,false};
statetype s_supercloneshoot2          = {false,SPR_SUPERCLONE_SHOOT2,12,NULL,(statefunc)T_SuperCloneShoot,&s_supercloneshoot3,true};
statetype s_supercloneshoot3          = {false,SPR_SUPERCLONE_SHOOT3,12,NULL,(statefunc)T_SuperCloneShoot,&s_supercloneshoot4,true};
statetype s_supercloneshoot4          = {false,SPR_SUPERCLONE_SHOOT4,12,NULL,(statefunc)T_SuperCloneShoot,&s_supercloneshoot5,true};
statetype s_supercloneshoot5          = {false,SPR_SUPERCLONE_SHOOT3,12,NULL,(statefunc)T_SuperCloneShoot,&s_supercloneshoot6,true};
statetype s_supercloneshoot6          = {false,SPR_SUPERCLONE_SHOOT2,12,NULL,(statefunc)T_SuperCloneShoot,&s_supercloneshoot7,true};
statetype s_supercloneshoot7          = {false,SPR_SUPERCLONE_SHOOT1,12,NULL,NULL,&s_superclonechase1,false};

statetype s_supercloneres1            = {false,SPR_SUPERCLONE_DIE4,15,NULL,NULL,&s_supercloneres2,false};
statetype s_supercloneres2            = {false,SPR_SUPERCLONE_DIE3,15,NULL,NULL,&s_supercloneres3,false};
statetype s_supercloneres3            = {false,SPR_SUPERCLONE_DIE2,15,NULL,NULL,&s_supercloneres4,false};
statetype s_supercloneres4            = {false,SPR_SUPERCLONE_DIE1,15,NULL,NULL,&s_supercloneresflag,false};
statetype s_supercloneresflag         = {false,SPR_SUPERCLONE_W1,10,NULL,(statefunc)T_ResFlags,&s_superclonechase1,false};
statetype s_superclonedead            = {false,SPR_SUPERCLONE_DEAD,0,NULL,NULL,&s_superclonedead,false};

/*
===============
=
= SpawnSuperClone
=
===============
*/

void SpawnSuperClone (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_superclonestand);
    newobj->obclass = supercloneobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_superclone];
    newobj->flags |= FL_SHOOTABLE|FL_BONUS;
    newobj->speed = 3000;   
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
===============
=
= T_SuperCloneShoot
=
===============
*/

void T_SuperCloneShoot (objtype *ob)
{
    int     dx,dy,dist;

    T_Shoot (ob);

    dx = abs(ob->tilex - player->tilex);
    dy = abs(ob->tiley - player->tiley);
    dist = dx>dy ? dx : dy;
    if (dist <= 1)
        TakeDamage (10,ob);
}

/*
===============
=
= T_ResFlags
=
===============
*/ 

void T_ResFlags (objtype *ob) {
    ob->flags &= ~(FL_NONMARK | FL_IMMUNE);
    switch (ob->obclass) {
        case supercloneobj:
            ob->hitpoints = starthitpoints[gamestate.difficulty][en_superclone];
            break;
        case snowmanobj:
            ob->hitpoints = starthitpoints[gamestate.difficulty][en_spectre];
            break;
    }
    ob->flags |= FL_SHOOTABLE | FL_FIRSTATTACK | FL_ATTACKMODE;
}
//
// otto
//
extern  statetype s_ottostand;

extern  statetype s_ottochase1;
extern  statetype s_ottochase1s;
extern  statetype s_ottochase2;
extern  statetype s_ottochase3;
extern  statetype s_ottochase3s;
extern  statetype s_ottochase4;

extern  statetype s_ottodie1;
extern  statetype s_ottodie2;
extern  statetype s_ottodie3;
extern  statetype s_ottodie4;
extern  statetype s_ottodie5;
extern  statetype s_ottodie6;

extern  statetype s_ottoshoot1;
extern  statetype s_ottoshoot2;
extern  statetype s_ottoshoot3;
extern  statetype s_ottoshoot4;
extern  statetype s_ottoshoot5;
extern  statetype s_ottoshoot6;


statetype s_ottostand           = {false,SPR_OTTO_W1,0,(statefunc)T_Stand,NULL,&s_ottostand,false};

statetype s_ottochase1          = {false,SPR_OTTO_W1,10,(statefunc)T_Otto,NULL,&s_ottochase1s,false};
statetype s_ottochase1s         = {false,SPR_OTTO_W1,3,NULL,NULL,&s_ottochase2,false};
statetype s_ottochase2          = {false,SPR_OTTO_W2,8,(statefunc)T_Otto,NULL,&s_ottochase3,false};
statetype s_ottochase3          = {false,SPR_OTTO_W3,10,(statefunc)T_Otto,NULL,&s_ottochase3s,false};
statetype s_ottochase3s         = {false,SPR_OTTO_W3,3,NULL,NULL,&s_ottochase4,false};
statetype s_ottochase4          = {false,SPR_OTTO_W4,8,(statefunc)T_Otto,NULL,&s_ottochase1,false};

statetype s_ottodeathcam        = {false,SPR_OTTO_W1,1,NULL,NULL,&s_ottodie1,false};

statetype s_ottodie1            = {false,SPR_OTTO_W1,1,NULL,(statefunc)A_DeathScream,&s_ottodie2,false};
statetype s_ottodie2            = {false,SPR_OTTO_W1,10,NULL,NULL,&s_ottodie3,false};
statetype s_ottodie3            = {false,SPR_OTTO_DIE1,10,NULL,NULL,&s_ottodie4,false};
statetype s_ottodie4            = {false,SPR_OTTO_DIE2,10,NULL,NULL,&s_ottodie5,false};
statetype s_ottodie5            = {false,SPR_OTTO_DIE3,10,NULL,NULL,&s_ottodie6,false};
statetype s_ottodie6            = {false,SPR_OTTO_DEAD,20,NULL,NULL,&s_ottodie6,false};

statetype s_ottoshoot1          = {false,SPR_OTTO_SHOOT1,30,NULL,NULL,&s_ottoshoot2,false};
statetype s_ottoshoot2          = {false,SPR_OTTO_SHOOT2,10,NULL,(statefunc)T_Launch,&s_ottoshoot3,true};
statetype s_ottoshoot3          = {false,SPR_OTTO_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_ottoshoot4,true};
statetype s_ottoshoot4          = {false,SPR_OTTO_SHOOT4,10,NULL,(statefunc)T_Shoot,&s_ottoshoot5,true};
statetype s_ottoshoot5          = {false,SPR_OTTO_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_ottoshoot6,true};
statetype s_ottoshoot6          = {false,SPR_OTTO_SHOOT4,10,NULL,(statefunc)T_Shoot,&s_ottochase1,true};


/*
===============
=
= SpawnOtto
=
===============
*/

void SpawnOtto (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_ottostand);
    newobj->obclass = ottoobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_otto];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH|FL_BOSS;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
================
=
= T_Otto
=
================
*/

void T_Otto (objtype *ob)
{
    int32_t move;
    int     dx,dy,dist;
    boolean dodge;

    dodge = false;
    dx = abs(ob->tilex - player->tilex);
    dy = abs(ob->tiley - player->tiley);
    dist = dx>dy ? dx : dy;

    if (CheckLine(ob))                                              // got a shot at player?
    {
        ob->hidden = false;
        if ( (int) US_RndT() < (tics<<3) && objfreelist)
        {
            //
            // go into attack frame
            //
            if (ob->obclass == ottoobj)
                NewState (ob,&s_ottoshoot1);
            else if (ob->obclass == angelobj)
                NewState (ob,&s_angelshoot1);
            else
                NewState (ob,&s_baronshoot1);
            return;
        }
        dodge = true;
    }
    else
        ob->hidden = true;

    if (ob->dir == nodir)
    {
        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            TryWalk(ob);
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        if (dist <4)
            SelectRunDir (ob);
        else if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

}


//
// baron
//
extern  statetype s_baronstand;

extern  statetype s_baronchase1;
extern  statetype s_baronchase1s;
extern  statetype s_baronchase2;
extern  statetype s_baronchase3;
extern  statetype s_baronchase3s;
extern  statetype s_baronchase4;

extern  statetype s_barondie1;
extern  statetype s_barondie2;
extern  statetype s_barondie3;
extern  statetype s_barondie4;
extern  statetype s_barondie5;
extern  statetype s_barondie6;
extern  statetype s_barondie7;
extern  statetype s_barondie8;
extern  statetype s_barondie9;

extern  statetype s_baronshoot1;
extern  statetype s_baronshoot2;
extern  statetype s_baronshoot3;
extern  statetype s_baronshoot4;
extern  statetype s_baronshoot5;


statetype s_baronstand          = {false,SPR_BARON_W1,0,(statefunc)T_Stand,NULL,&s_baronstand,true};

statetype s_baronchase1         = {false,SPR_BARON_W1,10,(statefunc)T_Otto,NULL,&s_baronchase1s,true};
statetype s_baronchase1s        = {false,SPR_BARON_W1,3,NULL,NULL,&s_baronchase2,true};
statetype s_baronchase2         = {false,SPR_BARON_W2,8,(statefunc)T_Otto,NULL,&s_baronchase3,true};
statetype s_baronchase3         = {false,SPR_BARON_W3,10,(statefunc)T_Otto,NULL,&s_baronchase3s,true};
statetype s_baronchase3s        = {false,SPR_BARON_W3,3,NULL,NULL,&s_baronchase4,true};
statetype s_baronchase4         = {false,SPR_BARON_W4,8,(statefunc)T_Otto,NULL,&s_baronchase1,true};

statetype s_barondie1           = {false,SPR_BARON_W1,1,NULL,(statefunc)A_DeathScream,&s_barondie2,true};
statetype s_barondie2           = {false,SPR_BARON_W1,10,NULL,NULL,&s_barondie3,true};
statetype s_barondie3           = {false,SPR_BARON_DIE1,10,NULL,NULL,&s_barondie4,true};
statetype s_barondie4           = {false,SPR_BARON_DIE2,10,NULL,NULL,&s_barondie5,true};
statetype s_barondie5           = {false,SPR_BARON_DIE3,10,NULL,NULL,&s_barondie6,true};
statetype s_barondie6           = {false,SPR_BARON_DIE4,10,NULL,NULL,&s_barondie7,true};
statetype s_barondie7           = {false,SPR_BARON_DIE5,10,NULL,NULL,&s_barondie8,true};
statetype s_barondie8           = {false,SPR_BARON_DIE6,10,NULL,(statefunc)A_SpawnNextBoss,&s_barondie9,true};
statetype s_barondie9           = {false,SPR_BARON_DEAD,0,NULL,NULL,&s_barondie9,true};

statetype s_baronshoot1         = {false,SPR_BARON_SHOOT1,30,NULL,NULL,&s_baronshoot2,true};
statetype s_baronshoot2         = {false,SPR_BARON_SHOOT2,10,NULL,(statefunc)T_Launch,&s_baronshoot3,true};
statetype s_baronshoot3         = {false,SPR_BARON_SHOOT4,10,NULL,(statefunc)T_Shoot,&s_baronshoot4,true};
statetype s_baronshoot4         = {false,SPR_BARON_SHOOT3,10,NULL,(statefunc)T_Launch,&s_baronshoot5,true};
statetype s_baronshoot5         = {false,SPR_BARON_SHOOT4,10,NULL,(statefunc)T_Shoot,&s_baronchase1,true};


/*
===============
=
= SpawnBaron
=
===============
*/

void SpawnBaron (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_baronstand);
    newobj->obclass = baronobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_baron];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH|FL_BOSS;
    if (!loadedgame)
        gamestate.killtotal++;
}

//
// Ultra clone
//
extern  statetype s_ultraclonestand;

extern  statetype s_ultraclonechase1;
extern  statetype s_ultraclonechase1s;
extern  statetype s_ultraclonechase2;
extern  statetype s_ultraclonechase3;
extern  statetype s_ultraclonechase3s;
extern  statetype s_ultraclonechase4;

extern  statetype s_ultraclonedie1;
extern  statetype s_ultraclonedie2;
extern  statetype s_ultraclonedie3;
extern  statetype s_ultraclonedie4;

extern  statetype s_ultracloneshoot1;
extern  statetype s_ultracloneshoot2;
extern  statetype s_ultracloneshoot3;


statetype s_ultraclonestand          = {false,SPR_ULTRACLONE_W1,0,(statefunc)T_Stand,NULL,&s_ultraclonestand,false};

statetype s_ultraclonechase1         = {false,SPR_ULTRACLONE_W1,10,(statefunc)T_Chase,NULL,&s_ultraclonechase1s,false};
statetype s_ultraclonechase1s        = {false,SPR_ULTRACLONE_W1,3,NULL,NULL,&s_ultraclonechase2,false};
statetype s_ultraclonechase2         = {false,SPR_ULTRACLONE_W2,8,(statefunc)T_Chase,NULL,&s_ultraclonechase3,false};
statetype s_ultraclonechase3         = {false,SPR_ULTRACLONE_W3,10,(statefunc)T_Chase,NULL,&s_ultraclonechase3s,false};
statetype s_ultraclonechase3s        = {false,SPR_ULTRACLONE_W3,3,NULL,NULL,&s_ultraclonechase4,false};
statetype s_ultraclonechase4         = {false,SPR_ULTRACLONE_W4,8,(statefunc)T_Chase,NULL,&s_ultraclonechase1,false};

statetype s_ultraclonedie1          = {false,SPR_ULTRACLONE_DIE1,15,NULL,(statefunc)A_DeathScream,&s_ultraclonedie2,false};
statetype s_ultraclonedie2           = {false,SPR_ULTRACLONE_DIE2,15,NULL,NULL,&s_ultraclonedie3,false};
statetype s_ultraclonedie3           = {false,SPR_ULTRACLONE_DIE3,15,NULL,NULL,&s_ultraclonedie4,false};
statetype s_ultraclonedie4           = {false,SPR_ULTRACLONE_DEAD,0,NULL,NULL,&s_ultraclonedie4,false};

statetype s_ultracloneshoot1         = {false,SPR_ULTRACLONE_AIM,30,NULL,NULL,&s_ultracloneshoot2,false};
statetype s_ultracloneshoot2         = {false,SPR_ULTRACLONE_SHOOT1,10,NULL,(statefunc)T_Launch,&s_ultracloneshoot3,true};
statetype s_ultracloneshoot3         = {false,SPR_ULTRACLONE_SHOOT2,10,NULL,(statefunc)T_Launch,&s_ultraclonechase1,true};

/*
===============
=
= SpawnUltraClone
=
===============
*/

void SpawnUltraClone (int tilex, int tiley) {

    SpawnNewObj (tilex,tiley,&s_ultraclonestand);
    newobj->obclass = ultracloneobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_ultraclone];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH|FL_BOSS;
    if (!loadedgame)
        gamestate.killtotal++;
}

//
// Robot
//
extern  statetype s_robotstand;

extern  statetype s_robotchase1;
extern  statetype s_robotchase1s;
extern  statetype s_robotchase2;
extern  statetype s_robotchase3;
extern  statetype s_robotchase3s;
extern  statetype s_robotchase4;

extern  statetype s_robotdie0;
extern  statetype s_robotdie01;
extern  statetype s_robotdie1;
extern  statetype s_robotdie2;
extern  statetype s_robotdie3;
extern  statetype s_robotdie4;

extern  statetype s_robotshoot1;
extern  statetype s_robotshoot2;
extern  statetype s_robotshoot3;
extern  statetype s_robotshoot4;
extern  statetype s_robotshoot5;
extern  statetype s_robotshoot6;
extern  statetype s_robotshoot7;
extern  statetype s_robotshoot8;


statetype s_robotstand          = {false,SPR_ROBOT_W1,0,(statefunc)T_Stand,NULL,&s_robotstand,false};

statetype s_robotchase1         = {false,SPR_ROBOT_W1,10,(statefunc)T_Chase,NULL,&s_robotchase1s,false};
statetype s_robotchase1s        = {false,SPR_ROBOT_W1,3,NULL,NULL,&s_robotchase2,false};
statetype s_robotchase2         = {false,SPR_ROBOT_W2,8,(statefunc)T_Chase,NULL,&s_robotchase3,false};
statetype s_robotchase3         = {false,SPR_ROBOT_W3,10,(statefunc)T_Chase,NULL,&s_robotchase3s,false};
statetype s_robotchase3s        = {false,SPR_ROBOT_W3,3,NULL,NULL,&s_robotchase4,false};
statetype s_robotchase4         = {false,SPR_ROBOT_W4,8,(statefunc)T_Chase,NULL,&s_robotchase1,false};

statetype s_robotdie1           = {false,SPR_ROBOT_DIE1,15,NULL,(statefunc)A_DeathScream,&s_robotdie2,false};
statetype s_robotdie2           = {false,SPR_ROBOT_DIE2,15,NULL,NULL,&s_robotdie3,false};
statetype s_robotdie3           = {false,SPR_ROBOT_DIE3,15,NULL,NULL,&s_robotdie4,false};
statetype s_robotdie4           = {false,SPR_ROBOT_DEAD,0,NULL,NULL,&s_robotdie4,false};

statetype s_robotshoot1         = {false,SPR_ROBOT_AIM,30,NULL,NULL,&s_robotshoot2,false};
statetype s_robotshoot2         = {false,SPR_ROBOT_SHOOT1,10,NULL,(statefunc)T_Shoot,&s_robotshoot3,true};
statetype s_robotshoot3         = {false,SPR_ROBOT_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_robotshoot4,true};
statetype s_robotshoot4         = {false,SPR_ROBOT_SHOOT1,10,NULL,(statefunc)T_Shoot,&s_robotshoot5,true};
statetype s_robotshoot5         = {false,SPR_ROBOT_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_robotshoot6,true};
statetype s_robotshoot6         = {false,SPR_ROBOT_SHOOT1,10,NULL,(statefunc)T_Shoot,&s_robotchase1,true};

/*
===============
=
= SpawnRobot
=
===============
*/

void SpawnRobot (int tilex, int tiley) {
    SpawnNewObj (tilex,tiley,&s_robotstand);
    newobj->obclass = robotobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_robot];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH|FL_BOSS;
    if (!loadedgame)
        gamestate.killtotal++;
}

//
// Heinrich
//
extern  statetype s_heinrichstand;

extern  statetype s_heinrichchase1;
extern  statetype s_heinrichchase1s;
extern  statetype s_heinrichchase2;
extern  statetype s_heinrichchase3;
extern  statetype s_heinrichchase3s;
extern  statetype s_heinrichchase4;

extern  statetype s_heinrichdie0;
extern  statetype s_heinrichdie01;
extern  statetype s_heinrichdie1;
extern  statetype s_heinrichdie2;
extern  statetype s_heinrichdie3;
extern  statetype s_heinrichdie4;

extern  statetype s_heinrichshoot1;
extern  statetype s_heinrichshoot2;
extern  statetype s_heinrichshoot3;
extern  statetype s_heinrichshoot4;
extern  statetype s_heinrichshoot5;
extern  statetype s_heinrichshoot6;
extern  statetype s_heinrichshoot7;
extern  statetype s_heinrichshoot8;


statetype s_heinrichstand          = {false,SPR_HEINRICH_W1,0,(statefunc)T_Stand,NULL,&s_heinrichstand,false};

statetype s_heinrichchase1         = {false,SPR_HEINRICH_W1,10,(statefunc)T_Chase,NULL,&s_heinrichchase1s,false};
statetype s_heinrichchase1s        = {false,SPR_HEINRICH_W1,3,NULL,NULL,&s_heinrichchase2,false};
statetype s_heinrichchase2         = {false,SPR_HEINRICH_W2,8,(statefunc)T_Chase,NULL,&s_heinrichchase3,false};
statetype s_heinrichchase3         = {false,SPR_HEINRICH_W3,10,(statefunc)T_Chase,NULL,&s_heinrichchase3s,false};
statetype s_heinrichchase3s        = {false,SPR_HEINRICH_W3,3,NULL,NULL,&s_heinrichchase4,false};
statetype s_heinrichchase4         = {false,SPR_HEINRICH_W4,8,(statefunc)T_Chase,NULL,&s_heinrichchase1,false};

statetype s_heinrichdie0           = {false,SPR_HEINRICH_W1,1,NULL,(statefunc)A_DeathScream,&s_heinrichdie01,false};
statetype s_heinrichdie01          = {false,SPR_HEINRICH_W1,1,NULL,NULL,&s_heinrichdie1,false};
statetype s_heinrichdie1           = {false,SPR_HEINRICH_DIE1,15,NULL,NULL,&s_heinrichdie2,false};
statetype s_heinrichdie2           = {false,SPR_HEINRICH_DIE2,15,NULL,NULL,&s_heinrichdie3,false};
statetype s_heinrichdie3           = {false,SPR_HEINRICH_DIE3,15,NULL,NULL,&s_heinrichdie4,false};
statetype s_heinrichdie4           = {false,SPR_HEINRICH_DEAD,0,NULL,NULL,&s_heinrichdie4,false};

statetype s_heinrichshoot1         = {false,SPR_HEINRICH_AIM,30,NULL,NULL,&s_heinrichshoot2,false};
statetype s_heinrichshoot2         = {false,SPR_HEINRICH_SHOOT1,10,NULL,(statefunc)T_Launch,&s_heinrichshoot3,true};
statetype s_heinrichshoot3         = {false,SPR_HEINRICH_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_heinrichshoot4,true};
statetype s_heinrichshoot4         = {false,SPR_HEINRICH_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_heinrichshoot5,true};
statetype s_heinrichshoot5         = {false,SPR_HEINRICH_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_heinrichshoot6,true};
statetype s_heinrichshoot6         = {false,SPR_HEINRICH_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_heinrichshoot7,true};
statetype s_heinrichshoot7         = {false,SPR_HEINRICH_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_heinrichshoot8,true};
statetype s_heinrichshoot8         = {false,SPR_HEINRICH_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_heinrichchase1,true};

/*
===============
=
= SpawnHeinrich
=
===============
*/

void SpawnHeinrich (int tilex, int tiley) {

    SpawnNewObj (tilex,tiley,&s_heinrichstand);
    newobj->obclass = heinrichobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_heinrich];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH|FL_BOSS;
    if (!loadedgame)
        gamestate.killtotal++;
}

/*
==============================================================
=
= Turret
=
==============================================================
*/

extern statetype s_turretidle1;

extern statetype s_turretscan1;

extern statetype s_turretdie1;
extern statetype s_turretdie2;
extern statetype s_turretdie3;
extern statetype s_turretdie4;

extern statetype s_turretshoot0;
extern statetype s_turretshoot1;
extern statetype s_turretshoot2;

statetype s_turretidle1        = {false,SPR_TURRET_S1,0,(statefunc)T_Stand,NULL,&s_turretidle1,false};

statetype s_turretscan1        = {true,SPR_TURRET_S1,25,(statefunc)T_Scan,NULL,&s_turretscan1,false};

statetype s_turretdie1           = {false,SPR_TURRET_DIE1,15,NULL,NULL,&s_turretdie2,true};
statetype s_turretdie2           = {false,SPR_TURRET_DIE2,15,NULL,NULL,&s_turretdie3,false};
statetype s_turretdie3           = {false,SPR_TURRET_DIE3,15,NULL,NULL,&s_turretdie4,false};
statetype s_turretdie4           = {false,SPR_TURRET_DIE4,15,NULL,NULL,&s_turretdie4,false};

statetype s_turretshoot0         = {false,SPR_TURRET_S1,30,NULL,NULL,&s_turretshoot1,false}; // fire quickly after sighting
statetype s_turretshoot1         = {false,SPR_TURRET_FIRE,8,NULL,(statefunc)T_Launch,&s_turretshoot2,true}; // fire
statetype s_turretshoot2         = {false,SPR_TURRET_S1,130,NULL,NULL,&s_turretscan1,false}; // reload

/*
===============
=
= T_Scan
=
===============
*/

void T_Scan (objtype *ob) {

    if (gamestate.victoryflag)
        return;

    if (CheckSight(ob)) { // player is in sight while in scanning mode (being in sight means normal CheckSight behavior   
        if (ob->flags & FL_FIRSTATTACK) {
            ob->dir = (dirtype)((GetCardinalDirection(ob) + 1) % 8);
            ob->flags &= ~FL_FIRSTATTACK;
        } else if (ob->state->shapenum + ob->temp2 == SPR_TURRET_S1) { // only attack if the turret is facing the player; temp2 = offset by CalcRotate on shapenum
            ob->dir = nodir;              // lock onto player until they go out of sight
            NewState(ob,&s_turretshoot0); // begin attacking 
        }
    } else if (ob->dir == nodir) {
        ob->dir = (dirtype)((GetCardinalDirection(ob) + 1) % 8);
    }
}

/*
==================
=
= GetCardinalDirection - gets an actor's cardinal direction based on their current angle (used for turret)
=   
==================
*/
dirtype GetCardinalDirection(objtype *ob) {
    int32_t deltax,deltay;
    float   angle;
    int     iangle;

    if (ob->obclass == playerobj) {
        iangle = player->angle;
    } else {
        deltax = player->x - ob->x;
        deltay = ob->y - player->y;
        angle = (float) atan2 ((float) deltay, (float) deltax);
        if (angle<0)
            angle = (float) (M_PI*2+angle);
        iangle = (int) (angle/(M_PI*2)*ANGLES);
    }

    if (iangle < EASTANGLE || iangle > SOUTHEASTANGLE) {
        return east;
    } else if (iangle < NORTHEASTANGLE) {
        return northeast;
    } else if (iangle < NORTHANGLE) {
        return north;
    } else if (iangle < NORTHWESTANGLE) {
        return northwest;
    } else if (iangle < WESTANGLE) {
        return west;
    } else if (iangle < SOUTHWESTANGLE) {
        return southwest;
    } else if (iangle < SOUTHANGLE) {
        return south;
    } else {
        return southeast;
    }
}

/*
===============
=
= SpawnTurret
=
===============
*/

void SpawnTurret (int tilex, int tiley) {
    SpawnNewObj (tilex,tiley,&s_turretidle1);
    newobj->obclass = turretobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_turret];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH|FL_BOSS;
    newobj->temp2 = 0;
    if (!loadedgame)
        gamestate.killtotal++;
}

// -----------------------------------------------------------
/*
==============================================================
=
= Exploding Barrel
=
==============================================================
*/
extern  statetype s_barrelstand;

extern  statetype s_barreldie1;
extern  statetype s_barreldie2;
extern  statetype s_barreldie3;
extern  statetype s_barreldie4;

statetype s_barrelstand          = {false,SPR_STAT_1,0,NULL,NULL,&s_barrelstand,false};

statetype s_barreldie1           = {false,SPR_BARRELEXP1,8,NULL,(statefunc)A_Explode,&s_barreldie2,true};
statetype s_barreldie2           = {false,SPR_BARRELEXP2,8,NULL,NULL,&s_barreldie3,false};
statetype s_barreldie3           = {false,SPR_BARRELEXP3,8,NULL,NULL,&s_barreldie4,false};
statetype s_barreldie4           = {false,SPR_BARRELDEAD,0,NULL,NULL,&s_barreldie4,false};

/*
===============
=
= SpawnBarrel
=
===============
*/

void SpawnBarrel (int tilex, int tiley) {
    SpawnNewObj (tilex,tiley,&s_barrelstand);
    newobj->obclass = barrelobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_barrel];
    newobj->flags |= FL_SHOOTABLE|FL_ATTACKMODE;
    newobj->creator = barrelobj;
}

// -----------------------------------------------------------

/*
==============================================================
=
= Breakable Column
=
==============================================================
*/

extern  statetype s_columnstand;

extern  statetype s_columndie1;
extern  statetype s_columndie2;
extern  statetype s_columndie3;
extern  statetype s_columndie4;
extern  statetype s_columndie5;

statetype s_columnstand          = {false,SPR_STAT_7,0,NULL,NULL,&s_columnstand,false};

statetype s_columndie1           = {false,SPR_COLUMNBREAK1,8,NULL,(statefunc)A_DeathScream,&s_columndie2,false};
statetype s_columndie2           = {false,SPR_COLUMNBREAK2,8,NULL,NULL,&s_columndie3,false};
statetype s_columndie3           = {false,SPR_COLUMNBREAK3,8,NULL,NULL,&s_columndie4,false};
statetype s_columndie4           = {false,SPR_COLUMNBREAK4,8,NULL,NULL,&s_columndie5,false};
statetype s_columndie5           = {false,SPR_STAT_COLUMNDEAD,0,NULL,NULL,&s_columndie5,false};

/*
===============
=
= SpawnColumn
=
===============
*/

void SpawnColumn (int tilex, int tiley) {
    SpawnNewObj (tilex,tiley,&s_columnstand);
    newobj->obclass = columnobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_column];
    newobj->flags |= FL_SHOOTABLE|FL_ATTACKMODE;
}

//-------------------------------------------------------------------

/*
=====================================================================
=
= Blinking Light
=
=====================================================================
*/
extern statetype s_lighton;
extern statetype s_blink1;
extern statetype s_blink2;
extern statetype s_blink3;

statetype s_lighton         = {false,SPR_STAT_40,0,(statefunc)T_Blink,NULL,&s_lighton,true};
statetype s_blink1          = {false,SPR_STAT_LIGHTDEAD,3,NULL,NULL,&s_blink2,false};
statetype s_blink2          = {false,SPR_STAT_40,3,NULL,NULL,&s_blink3,true};
statetype s_blink3          = {false,SPR_STAT_LIGHTDEAD,3,NULL,NULL,&s_lighton,false};

void T_Blink(objtype *ob) {
    if (ob->temp2 < 0) {
        ob->temp2 = (US_RndT()%4 + 3) * 70; // seconds til next blinking phase
        NewState(ob,&s_blink1);
    }

    ob->temp2 -= tics;
}


/*
===============
=
= SpawnBlinkingLight
=
===============
*/

void SpawnBlinkingLight (int tilex, int tiley) {
    SpawnNewObj (tilex,tiley,&s_lighton);
    newobj->obclass = inertobj;
    newobj->flags |= FL_NEVERMARK;
    newobj->temp2 = 0;
}

// ------------------------------------------------------------------

/*
=====================================================================
=
= Splatter statetypes
=
=====================================================================
*/

extern statetype s_splatter1;
extern statetype s_splatter2;
extern statetype s_splatter3;
extern statetype s_splatter4;

statetype s_splatter1         = {false,SPR_EXPLOSIVEDEATH1,10,NULL,(statefunc)A_Splat,&s_splatter2,false};
statetype s_splatter2         = {false,SPR_EXPLOSIVEDEATH2,10,NULL,NULL,&s_splatter3,false};
statetype s_splatter3         = {false,SPR_EXPLOSIVEDEATH3,10,NULL,NULL,&s_splatter4,false};
statetype s_splatter4         = {false,SPR_EXPLOSIVEDEATH4,0,NULL,NULL,&s_splatter4,false};

/*
================
=
= A_Splat (splatter deathscream)
=
================
*/

void A_Splat (objtype *ob) {
    PlaySoundLocActor(AHHHGSND,ob);
}

// ------------------------------------------------------------------

/*
=====================================================================
=
= Angel of Death
=
=====================================================================
*/
void A_Relaunch (objtype *ob);
void A_Victory (objtype *ob);
void A_StartAttack (objtype *ob);
void A_Breathing (objtype *ob);

extern  statetype s_angelstand;

extern  statetype s_angelchase1;
extern  statetype s_angelchase1s;
extern  statetype s_angelchase2;
extern  statetype s_angelchase3;
extern  statetype s_angelchase3s;
extern  statetype s_angelchase4;

extern  statetype s_angeldie1;
extern  statetype s_angeldie11;
extern  statetype s_angeldie2;
extern  statetype s_angeldie3;
extern  statetype s_angeldie4;
extern  statetype s_angeldie5;
extern  statetype s_angeldie6;
extern  statetype s_angeldie7;
extern  statetype s_angeldie8;
extern  statetype s_angeldie9;

extern  statetype s_angelshoot1;
extern  statetype s_angelshoot2;
extern  statetype s_angelshoot3;
extern  statetype s_angelshoot4;
extern  statetype s_angelshoot5;
extern  statetype s_angelshoot6;

extern  statetype s_angeltired;
extern  statetype s_angeltired2;
extern  statetype s_angeltired3;
extern  statetype s_angeltired4;
extern  statetype s_angeltired5;
extern  statetype s_angeltired6;
extern  statetype s_angeltired7;

extern  statetype s_spark1;
extern  statetype s_spark2;
extern  statetype s_spark3;
extern  statetype s_spark4;


statetype s_angelstand          = {false,SPR_ANGEL_W1,0,(statefunc)T_Stand,NULL,&s_angelstand,true};

statetype s_angelchase1         = {false,SPR_ANGEL_W1,10,(statefunc)T_Otto,NULL,&s_angelchase1s,true};
statetype s_angelchase1s        = {false,SPR_ANGEL_W1,3,NULL,NULL,&s_angelchase2,true};
statetype s_angelchase2         = {false,SPR_ANGEL_W2,8,(statefunc)T_Otto,NULL,&s_angelchase3,true};
statetype s_angelchase3         = {false,SPR_ANGEL_W3,10,(statefunc)T_Otto,NULL,&s_angelchase3s,true};
statetype s_angelchase3s        = {false,SPR_ANGEL_W3,3,NULL,NULL,&s_angelchase4,true};
statetype s_angelchase4         = {false,SPR_ANGEL_W4,8,(statefunc)T_Otto,NULL,&s_angelchase1,true};

statetype s_angeldie1           = {false,SPR_ANGEL_W1,1,NULL,(statefunc)A_DeathScream,&s_angeldie11,true};
statetype s_angeldie11          = {false,SPR_ANGEL_W1,1,NULL,NULL,&s_angeldie2,true};
statetype s_angeldie2           = {false,SPR_ANGEL_DIE1,10,NULL,(statefunc)A_Slurpie,&s_angeldie3,true};
statetype s_angeldie3           = {false,SPR_ANGEL_DIE2,10,NULL,NULL,&s_angeldie4,true};
statetype s_angeldie4           = {false,SPR_ANGEL_DIE3,10,NULL,NULL,&s_angeldie5,true};
statetype s_angeldie5           = {false,SPR_ANGEL_DIE4,10,NULL,NULL,&s_angeldie6,true};
statetype s_angeldie6           = {false,SPR_ANGEL_DIE5,10,NULL,NULL,&s_angeldie7,true};
statetype s_angeldie7           = {false,SPR_ANGEL_DIE6,10,NULL,NULL,&s_angeldie8,true};
statetype s_angeldie8           = {false,SPR_ANGEL_DIE7,10,NULL,NULL,&s_angeldie9,true};
statetype s_angeldie9           = {false,SPR_ANGEL_DEAD,130,NULL,(statefunc)A_Victory,&s_angeldie9,true};

statetype s_angelshoot1         = {false,SPR_ANGEL_SHOOT1,10,NULL,(statefunc)A_StartAttack,&s_angelshoot2,true};
statetype s_angelshoot2         = {false,SPR_ANGEL_SHOOT2,20,NULL,(statefunc)T_Launch,&s_angelshoot3,true};
statetype s_angelshoot3         = {false,SPR_ANGEL_SHOOT1,10,NULL,(statefunc)A_Relaunch,&s_angelshoot2,true};

statetype s_angeltired          = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angeltired2,true};
statetype s_angeltired2         = {false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired3,true};
statetype s_angeltired3         = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angeltired4,true};
statetype s_angeltired4         = {false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired5,true};
statetype s_angeltired5         = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angeltired6,true};
statetype s_angeltired6         = {false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired7,true};
statetype s_angeltired7         = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angelchase1,true};

statetype s_spark1              = {false,SPR_STAT_FLAME1,6,(statefunc)T_Projectile,(statefunc)A_Smoke,&s_spark2,true};
statetype s_spark2              = {false,SPR_STAT_FLAME2,6,(statefunc)T_Projectile,(statefunc)A_Smoke,&s_spark3,true};
statetype s_spark3              = {false,SPR_STAT_FLAME3,6,(statefunc)T_Projectile,(statefunc)A_Smoke,&s_spark4,true};
statetype s_spark4              = {false,SPR_STAT_FLAME4,6,(statefunc)T_Projectile,(statefunc)A_Smoke,&s_spark1,true};


void A_Slurpie (objtype *)
{
    SD_PlaySound(SLURPIESND);
}

void A_Breathing (objtype *)
{
    SD_PlaySound(ANGELTIREDSND);
}


/*
===============
=
= SpawnAngel
=
===============
*/

void SpawnAngel (int tilex, int tiley)
{
    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_angeldie11.tictime = 105;

    SpawnNewObj (tilex,tiley,&s_angelstand);
    newobj->obclass = angelobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_angel];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH|FL_BOSS;
    if (!loadedgame)
        gamestate.killtotal++;
}

/*
=================
=
= A_StartAttack
=
=================
*/

void A_StartAttack (objtype *ob)
{
    ob->temp1 = 0;
}

/*
=================
=
= A_Relaunch
=
=================
*/

void A_Relaunch (objtype *ob)
{
    if (++ob->temp1 == 5)
    {
        NewState (ob,&s_angeltired);
        return;
    }

    if (US_RndT()&1)
    {
        NewState (ob,&s_angelchase1);
        return;
    }
}

/*
=================
=
= A_Victory
=
=================
*/

void A_Victory (objtype *)
{
    playstate = ex_victorious;
}

// ------------------------------------------------------------------

/*
=====================================================================
=
= Spectres (Flame w/ eyes, ghost flame, snowman)
=
=====================================================================
*/

/*
=====================================================================
=
= Flame w/ eyes
=
=====================================================================
*/
extern  statetype s_spectrewait1;
extern  statetype s_spectrewait2;
extern  statetype s_spectrewait3;
extern  statetype s_spectrewait4;

extern  statetype s_spectrechase1;
extern  statetype s_spectrechase2;
extern  statetype s_spectrechase3;
extern  statetype s_spectrechase4;

extern  statetype s_spectredie1;
extern  statetype s_spectredie2;
extern  statetype s_spectredie3;
extern  statetype s_spectredie4;

extern  statetype s_spectrewake;

statetype s_spectrewait1        = {false,SPR_FLAME1,10,(statefunc)T_Stand,NULL,&s_spectrewait2,true};
statetype s_spectrewait2        = {false,SPR_FLAME2,10,(statefunc)T_Stand,NULL,&s_spectrewait3,true};
statetype s_spectrewait3        = {false,SPR_FLAME3,10,(statefunc)T_Stand,NULL,&s_spectrewait4,true};
statetype s_spectrewait4        = {false,SPR_FLAME4,10,(statefunc)T_Stand,NULL,&s_spectrewait1,true};

statetype s_spectrechase1       = {false,SPR_FLAME1,10,(statefunc)T_Ghosts,NULL,&s_spectrechase2,true};
statetype s_spectrechase2       = {false,SPR_FLAME2,10,(statefunc)T_Ghosts,NULL,&s_spectrechase3,true};
statetype s_spectrechase3       = {false,SPR_FLAME3,10,(statefunc)T_Ghosts,NULL,&s_spectrechase4,true};
statetype s_spectrechase4       = {false,SPR_FLAME4,10,(statefunc)T_Ghosts,NULL,&s_spectrechase1,true};

statetype s_spectredie1         = {false,SPR_FLAME_DIE1,10,NULL,NULL,&s_spectredie2,true};
statetype s_spectredie2         = {false,SPR_FLAME_DIE2,10,NULL,NULL,&s_spectredie3,true};
statetype s_spectredie3         = {false,SPR_FLAME_DIE3,10,NULL,NULL,&s_spectredie4,true};
statetype s_spectredie4         = {false,SPR_FLAME_DIE4,300,NULL,NULL,&s_spectrewake,true};
statetype s_spectrewake         = {false,SPR_FLAME_DIE4,10,(statefunc)A_Dormant,NULL,&s_spectrewake,true};

/*
===============
=
= SpawnSpectre
=
===============
*/

void SpawnSpectre (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_spectrewait1);
    newobj->obclass = spectreobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_spectre];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH; // |FL_NEVERMARK|FL_NONMARK;
}

/*
=====================================================================
=
= Flame w/o eyes
=
=====================================================================
*/
extern  statetype s_spectre2wait1;
extern  statetype s_spectre2wait2;
extern  statetype s_spectre2wait3;
extern  statetype s_spectre2wait4;

extern  statetype s_spectre2chase1;
extern  statetype s_spectre2chase2;
extern  statetype s_spectre2chase3;
extern  statetype s_spectre2chase4;

extern  statetype s_spectre2die1;
extern  statetype s_spectre2die2;
extern  statetype s_spectre2die3;
extern  statetype s_spectre2die4;

statetype s_spectre2wait1        = {false,SPR_STAT_FLAME1,10,(statefunc)T_Stand,NULL,&s_spectre2wait2,true};
statetype s_spectre2wait2        = {false,SPR_STAT_FLAME1,10,(statefunc)T_Stand,NULL,&s_spectre2wait3,true};
statetype s_spectre2wait3        = {false,SPR_STAT_FLAME3,10,(statefunc)T_Stand,NULL,&s_spectre2wait4,true};
statetype s_spectre2wait4        = {false,SPR_STAT_FLAME4,10,(statefunc)T_Stand,NULL,&s_spectre2wait1,true};

statetype s_spectre2chase1       = {false,SPR_STAT_FLAME1,10,(statefunc)T_Ghosts,NULL,&s_spectre2chase2,true};
statetype s_spectre2chase2       = {false,SPR_STAT_FLAME2,10,(statefunc)T_Ghosts,NULL,&s_spectre2chase3,true};
statetype s_spectre2chase3       = {false,SPR_STAT_FLAME3,10,(statefunc)T_Ghosts,NULL,&s_spectre2chase4,true};
statetype s_spectre2chase4       = {false,SPR_STAT_FLAME4,10,(statefunc)T_Ghosts,NULL,&s_spectre2chase1,true};

statetype s_spectre2die1         = {false,SPR_FLAME2_DIE1,10,NULL,NULL,&s_spectre2die2,true};
statetype s_spectre2die2         = {false,SPR_FLAME2_DIE2,10,NULL,NULL,&s_spectre2die3,true};
statetype s_spectre2die3         = {false,SPR_FLAME2_DIE3,10,NULL,NULL,&s_spectre2die4,true};
statetype s_spectre2die4         = {false,SPR_FLAME2_DIE4,10,NULL,NULL,NULL,true};

/*
===============
=
= SpawnSpectre2 (flame with no eyes)
=
===============
*/

void SpawnSpectre2 (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_spectre2wait1);
    newobj->obclass = ghostobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_spectre];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH; // |FL_NEVERMARK|FL_NONMARK;
}

/*
=====================================================================
=
= Snowman
=
=====================================================================
*/

extern statetype s_snowstand;

extern statetype s_snowchase1;
extern statetype s_snowchase2;

extern statetype s_snowdie1;
extern statetype s_snowdie2;
extern statetype s_snowdie3;
extern statetype s_snowdie4;
extern statetype s_snowwake;

extern statetype s_snowres1;
extern statetype s_snowres2;
extern statetype s_snowres3;

extern statetype s_snowreallydead;

statetype s_snowstand		 = {false,SPR_SNOWMAN_W1,0,(statefunc)T_Stand,NULL,&s_snowstand,false};

statetype s_snowchase1       = {false,SPR_SNOWMAN_W1,10,(statefunc)T_Ghosts,NULL,&s_snowchase2,false};
statetype s_snowchase2       = {false,SPR_SNOWMAN_W2,10,(statefunc)T_Ghosts,NULL,&s_snowchase1,false};

statetype s_snowdie1         = {false,SPR_SNOWMAN_DIE1,10,NULL,(statefunc)A_DeathScream,&s_snowdie2,false};
statetype s_snowdie2         = {false,SPR_SNOWMAN_DIE2,10,NULL,NULL,&s_snowdie3,false};
statetype s_snowdie3         = {false,SPR_SNOWMAN_DIE3,10,NULL,NULL,&s_snowdie4,false};
statetype s_snowdie4         = {false,SPR_SNOWMAN_DEAD,500,NULL,NULL,&s_snowwake,false};
statetype s_snowwake         = {false,SPR_SNOWMAN_DEAD,10,(statefunc)A_Dormant,NULL,&s_snowwake,false};

statetype s_snowres1         = {false,SPR_SNOWMAN_DIE3,10,NULL,NULL,&s_snowres2,false};   
statetype s_snowres2         = {false,SPR_SNOWMAN_DIE2,10,NULL,NULL,&s_snowres3,false}; 
statetype s_snowres3         = {false,SPR_SNOWMAN_DIE1,10,NULL,(statefunc)T_ResFlags,&s_snowchase1,false}; 

statetype s_snowreallydead = {false,SPR_SNOWMAN_DEAD,0,NULL,NULL,&s_snowreallydead,false};

/*
===============
=
= SpawnSnowman
=
===============
*/

void SpawnSnowman (int tilex, int tiley) {
    SpawnNewObj (tilex,tiley,&s_snowstand);
    newobj->obclass = snowmanobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_spectre];
    newobj->flags |= FL_SHOOTABLE; // |FL_NEVERMARK|FL_NONMARK;
    newobj->speed = 800;
 //   if (!loadedgame)
 //       gamestate.killtotal++;
}


/*
===============
=
= A_Dormant
=
===============
*/

void A_Dormant (objtype *ob)
{
    int32_t     deltax,deltay;
    int         xl,xh,yl,yh;
    int         x,y;
    uintptr_t   tile;

    deltax = ob->x - player->x;
    if (deltax < -MINACTORDIST || deltax > MINACTORDIST)
        goto moveok;
    deltay = ob->y - player->y;
    if (deltay < -MINACTORDIST || deltay > MINACTORDIST)
        goto moveok;

    return;
moveok:

    xl = (ob->x-MINDIST) >> TILESHIFT;
    xh = (ob->x+MINDIST) >> TILESHIFT;
    yl = (ob->y-MINDIST) >> TILESHIFT;
    yh = (ob->y+MINDIST) >> TILESHIFT;

    for (y=yl ; y<=yh ; y++)
        for (x=xl ; x<=xh ; x++)
        {
            tile = (uintptr_t)actorat[x][y];
            if (!tile)
                continue;
            if (!ISPOINTER(tile))
                return;
            if (((objtype *)tile)->flags&FL_SHOOTABLE)
                return;
        }


    switch (ob->obclass) {
        case supercloneobj:
            if (ob->temp2 > 14) {
                ob->flags |= FL_NONMARK;
                NewState(ob,&s_superclonedead);
            } else { // 15% chance to resurrect
                ob->hitpoints = 10000;
                ob->flags |= FL_SHOOTABLE | FL_ATTACKMODE | FL_IMMUNE; // so it blocks while rising
                gamestate.killcount--;
                NewState(ob,&s_supercloneres1);
            }
            break;

        case snowmanobj:
            if (ob->temp2 > 19) {
                ob->flags |= FL_NONMARK;
                NewState(ob,&s_snowreallydead);
            } else { // 20% to resurrect
                ob->hitpoints = 10000;
                ob->flags |= FL_SHOOTABLE | FL_ATTACKMODE | FL_IMMUNE;
                NewState(ob,&s_snowres1);
            }
            break;

        case spectreobj: // flame with eyes
             ob->flags |= FL_SHOOTABLE | FL_ATTACKMODE;
             ob->flags &= ~FL_NONMARK;      // stuck bugfix 1
             ob->dir = nodir;
             ob->hitpoints = starthitpoints[gamestate.difficulty][en_spectre];
             NewState (ob,&s_spectrechase1);
             break;

    }
}
#endif

/*
=====================================================================

 ENEMY MOVEMENT STATETYPE ROUTINES

=====================================================================
*/

/*
===============
=
= T_Stand
=
===============
*/
void T_Stand (objtype *ob)
{
    SightPlayer (ob);
}

/*
=================
=
= T_Chase
=
=================
*/

void T_Chase (objtype *ob)
{
    int32_t move,target;
    int     dx,dy,dist,chance;
    boolean dodge;

    if (gamestate.victoryflag)
        return;

    dodge = false;
    if (CheckLine(ob))      // got a shot at player?
    {
        ob->hidden = false;
        dx = abs(ob->tilex - player->tilex);
        dy = abs(ob->tiley - player->tiley);
        dist = dx>dy ? dx : dy;

#ifdef PLAYDEMOLIKEORIGINAL
        if(DEMOCOND_ORIG)
        {
            if(!dist || (dist == 1 && ob->distance < 0x4000))
                chance = 300;
            else
                chance = (tics<<4)/dist;
        }
        else
#endif
        {
            if (dist)
                if (ob->flags & FL_BOSS)
                    chance = ((tics<<4)+30)/dist; // bosses shoot more
                else
                    chance = (tics<<4)/dist;
            else
                chance = 300;

            if (dist == 1)
            {
                target = abs(ob->x - player->x);
                if (target < 0x14000l)
                {
                    target = abs(ob->y - player->y);
                    if (target < 0x14000l)
                        chance = 300;
                }
            }
        }

        if (US_RndT()<chance)
        {
            //
            // go into attack frame
            //
            switch (ob->obclass)
            {
                case guardobj:
                    NewState (ob,&s_grdshoot1);
                    break;
                case officerobj:
                    NewState (ob,&s_ofcshoot1);
                    break;
                case mutantobj:
                case toiletobj:
                    NewState (ob,&s_mutshoot1);
                    break;
                case ssobj:
                    NewState (ob,&s_ssshoot1);
                    break;
#ifndef SPEAR
                case bossobj:
                    NewState (ob,&s_bossshoot1);
                    break;
                case gretelobj:
                    NewState (ob,&s_gretelshoot1);
                    break;
                case mechahitlerobj:
                    NewState (ob,&s_mechashoot1);
                    break;
                case realhitlerobj:
                    NewState (ob,&s_hitlershoot1);
                    break;
#else
                case angelobj:
                    NewState (ob,&s_angelshoot1);
                    break;
                case grauobj:
                    NewState (ob,&s_graushoot1);
                    break;
                case supercloneobj:
                    NewState (ob,&s_supercloneshoot1);
                    break;
                case ottoobj:
                    NewState (ob,&s_ottoshoot1);
                    break;
                case baronobj:
                    NewState (ob,&s_baronshoot1);
                    break;
                case ultracloneobj:
                    NewState (ob,&s_ultracloneshoot1);
                    break;
                case robotobj:
                    NewState (ob,&s_robotshoot1);
                    break;
                case heinrichobj:
                    NewState (ob,&s_heinrichshoot1);
                    break;
#endif
            }
            return;
        }
        dodge = true;
    }
    else
        ob->hidden = true;

    if (ob->dir == nodir)
    {
        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            DEMOIF_SDL
            {
                TryWalk(ob);
            }
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        CheckEnemyTile(ob);

        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

/*
=================
=
= T_Ghosts
=
=================
*/

void T_Ghosts (objtype *ob)
{
    int32_t move;

    if (ob->dir == nodir)
    {
        SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

/*
===============
=
= SelectPathDir
=
===============
*/

void SelectPathDir (objtype *ob)
{
    unsigned spot;

    spot = MAPSPOT(ob->tilex,ob->tiley,1)-ICONARROWS;

    if (spot<8)
    {
        // new direction
        ob->dir = (dirtype)(spot);
    }

    ob->distance = TILEGLOBAL;

    if (!TryWalk (ob))
        ob->dir = nodir;
}


/*
===============
=
= T_Path
=
===============
*/

void T_Path (objtype *ob)
{
    int32_t    move;

    if (SightPlayer (ob))
        return;

    if (ob->dir == nodir)
    {
        SelectPathDir (ob);
        if (ob->dir == nodir)
            return;                                 // all movement is blocked
    }


    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            DEMOIF_SDL
            {
                TryWalk(ob);
            }
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        if (ob->tilex>MAPSIZE || ob->tiley>MAPSIZE)
        {
            sprintf (str, "T_Path hit a wall at %u,%u, dir %u",
                ob->tilex,ob->tiley,ob->dir);
            Quit (str);
        }

        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
        move -= ob->distance;

        CheckEnemyTile(ob);

        SelectPathDir (ob);

        if (ob->dir == nodir)
            return;                                 // all movement is blocked
    }
}


/*
=============================================================================

 ENEMY ATTACK STATETYPE ROUTINES

=============================================================================
*/

/*
===============
=
= T_Shoot
=
= Try to damage the player, based on skill level and player's speed
=
===============
*/

void T_Shoot (objtype *ob)
{
    int     dx,dy,dist;
    int     hitchance,damage;

    hitchance = 128;

    if (!areabyplayer[ob->areanumber])
        return;

    if (CheckLine (ob))                    // player is not behind a wall
    {
        dx = abs(ob->tilex - player->tilex);
        dy = abs(ob->tiley - player->tiley);
        dist = dx>dy ? dx:dy;

        if (ob->obclass == ssobj)
            dist = dist*2/3;                                        // ss are better shots

        if (thrustspeed >= RUNSPEED)
        {
            if (ob->flags&FL_VISABLE)
                hitchance = 160-dist*16;                // player can see to dodge
            else
                hitchance = 160-dist*8;
        }
        else
        {
            if (ob->flags&FL_VISABLE)
                hitchance = 256-dist*16;                // player can see to dodge
            else
                hitchance = 256-dist*8;
        }

        // see if the shot was a hit

        if (US_RndT()<hitchance)
        {
            if (dist<2)
                damage = US_RndT()>>2;
            else if (dist<4)
                damage = US_RndT()>>3;
            else
                damage = US_RndT()>>4;

            TakeDamage (damage,ob);
        }
    }

    switch(ob->obclass)
    {
        case ssobj:
            PlaySoundLocActor(SSFIRESND,ob);
            break;
        case heinrichobj:
        case supercloneobj:
        case baronobj:
            PlaySoundLocActor(BOSSFIRESND,ob);
            break;
        default:
            PlaySoundLocActor(NAZIFIRESND,ob);
    }
}

/*
===============
=
= T_Launch
=
===============
*/
void T_Launch (objtype *ob)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;

    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2 ((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float) (M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);

    if (ob->obclass == baronobj || ob->obclass == ultracloneobj)
    {
        if (ob->obclass == baronobj) T_Shoot (ob);
        if (ob->state == &s_baronshoot2 || ob->state == &s_ultracloneshoot3)
        {
            iangle-=4;
            if (iangle<0)
                iangle+=ANGLES;
        }
        else
        {
            iangle+=4;
            if (iangle>=ANGLES)
                iangle-=ANGLES;
        }
    }

    GetNewActor ();
    newobj->creator = ob->obclass; // don't let the enemies kill themselves
    newobj->state = &s_rocket;
    newobj->ticcount = 1;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = rocketobj;
    switch(ob->obclass)
    {
        case baronobj:
            newobj->state = &s_hrocket;
            newobj->obclass = hrocketobj;
            PlaySoundLocActor (KNIGHTMISSILESND,newobj);
            break;
        case angelobj:
            newobj->state = &s_spark1;
            newobj->obclass = sparkobj;
            PlaySoundLocActor (ANGELFIRESND,newobj);
            break;
        default:
            PlaySoundLocActor (TURRETFIRESND,newobj);
    }

    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->speed = 0x2000l;
    newobj->flags = FL_NEVERMARK;
    newobj->active = ac_yes;
}

// --------------------------------------------------------
#ifdef EODSPECIAL
void CheckGong() {
    if ((gamestate.enemytreasurecount + gamestate.treasurecount) == gamestate.treasuretotal) {
        SD_PlaySound(GONGSND);
        gamestate.gong = true;
        madenoise = true;
        for (int x = 0; x < MAPSIZE; x++) {
            for (int y = 0; y < MAPSIZE; y++) {
                if (MAPSPOT(x,y,1) == ENEMYBLOCKTILE)
                    MAPSPOT(x,y,1) = 0;
            }
        }
        SpawnStatic(31, 33, 54); // elevator key for level 31
    }
}
#endif

void WarpActor (objtype *ob, int type) {
    
    if (type == EAST) {
        ob->tilex = warpex;
        ob->tiley = warpey;
        ob->dir = east;
    } else {
        ob->tilex = warpwx;
        ob->tiley = warpwy;
        ob->dir = west;
    }

    ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
    ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

    ob->areanumber =
        *(mapsegs[0] + (ob->tiley<<mapshift)+ob->tilex) - AREATILE;
}


/*
**
**  Enemy Bonus Routines (MCS)
**
**/

void CheckEnemyTile (objtype *ob) {
    statobj_t (*objpos);
    // Check for special tiles
    switch (MAPSPOT(ob->tilex, ob->tiley, 1)) {
        case WARPEASTTILE:
            PlaySoundLocActor(WALK1SND,ob);
            WarpActor(ob, EAST);
            break;
        case WARPWESTTILE:
            PlaySoundLocActor(WALK1SND,ob);
            WarpActor(ob, WEST);
            break;
    }

    // Check for static bonus objects (objects the player can pickup)
    for (objpos = &statobjlist[0] ; objpos !=laststatobj ; objpos++) {
        if (!(objpos->flags & FL_BONUS))
            continue;
        if (objpos->tilex == ob->tilex && objpos->tiley == ob->tiley && objpos->flags & FL_BONUS)
            GetEnemyBonus (objpos, ob);
    }
}

void RemoveObject (statobj_t *check) {
    check->flags &= ~FL_BONUS;
  //  
    check->shapenum = -1;
}

/*
==================
=
= GetEnemyBonus
=
==================
*/
void GetEnemyBonus (statobj_t *objpos, objtype *ob) {
    switch(objpos->itemnumber) {
        case bo_landmine:
            RemoveObject (objpos);

            if (ob->flags & FL_VISABLE)
                StartBonusFlash ();

            PlaySoundLocActor (LANDMINESND,ob);
            KillActor (ob);
        break;

        case bo_firstaid:
            switch (ob->obclass) {
                case guardobj:
                case ssobj:
                case officerobj:
                    RemoveObject (objpos);
                    PlaySoundLocActor (HEALTH2SND,ob);
                    ob->hitpoints += 25;
                    break;
                default:
                    break;
            }
            break;

        case bo_food:
            ob->hitpoints += 10;
            RemoveObject (objpos);
            break;
        case bo_alpo:
            ob->hitpoints += 5;
            RemoveObject (objpos);
            break;
        case bo_death:
            KillActor(ob);
            objpos->numruns = objpos->numruns + 1;
            if (objpos->numruns == 4)
               RemoveObject(objpos);
            break;
#ifdef EODSPECIAL
        case bo_cross:
                if (gamestate.mapon == 30) {
                    PlaySoundLocActor(BONUS1SND,ob);
                    RemoveObject(objpos);
                    
                    gamestate.enemytreasurecount++;
                    CheckGong();    
                }
                break;
        case bo_chalice:
               if (gamestate.mapon == 30) {
                    PlaySoundLocActor(BONUS2SND,ob);
                    RemoveObject(objpos);

                    gamestate.enemytreasurecount++;
                    CheckGong();
                }
                break;
        case bo_bible:
                if (gamestate.mapon == 30) {
                    PlaySoundLocActor(BONUS3SND,ob);
                    RemoveObject(objpos);

                    gamestate.enemytreasurecount++;
                    CheckGong();
                }
                break;
        case bo_crown:
                if (gamestate.mapon == 30) {
                    PlaySoundLocActor(BONUS4SND,ob);
                    RemoveObject(objpos);

                    gamestate.enemytreasurecount++;
                    CheckGong();
                }
                break;
        case bo_diamond:
                if (gamestate.mapon == 30) {
                    PlaySoundLocActor(BONUS4SND,ob);
                    RemoveObject(objpos);

                    gamestate.enemytreasurecount++;
                    CheckGong();
                }
                break;
#endif
   }
}
