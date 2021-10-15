#include "version.h"

#if defined(USE_STARSKY) || defined(USE_RAIN) || defined(USE_SNOW)

#include "wl_def.h"

#include "wl_atmos.h"

#if defined(USE_RAIN) || defined(USE_SNOW)
    uint32_t rainpos = 0;
#endif

typedef struct {
    int32_t x, y, z;
} point3d_t;

#define MAXPOINTS 500

point3d_t points[MAXPOINTS];

byte moon[100]={
     0,  0, 27, 18, 15, 16, 19, 29,  0,  0,
     0, 22, 16, 15, 15, 16, 16, 18, 24,  0,
    27, 17, 15, 17, 16, 16, 17, 17, 18, 29,
    18, 15, 15, 15, 16, 16, 17, 17, 18, 20,
    16, 15, 15, 16, 16, 17, 17, 18, 19, 21,
    16, 15, 17, 20, 18, 17, 18, 18, 20, 22,
    19, 16, 18, 19, 17, 17, 18, 19, 22, 24,
    28, 19, 17, 17, 17, 18, 19, 21, 25, 31,
     0, 23, 18, 19, 18, 20, 22, 24, 28,  0,
     0,  0, 28, 21, 20, 22, 28, 30,  0,  0 };

/*
======================
=
= Init3DPoints
=
======================
*/
void Init3DPoints()
{
    for(int i = 0; i < MAXPOINTS; i++)
    {
        point3d_t *pt = &points[i];
        pt->x = 16384 - (rand() & 32767);
        pt->z = 16384 - (rand() & 32767);
        float len = sqrt((float)pt->x * pt->x + (float)pt->z * pt->z);
        int j=50;
        do
        {
            pt->y = 1024 + (rand() & 8191);
            j--;
        }
        while(j > 0 && (float)pt->y * 256.F / len >= halfviewheight);
    }
}

#endif

#ifdef USE_STARSKY
/*
======================
=
= DrawStarSky
=
======================
*/
void DrawStarSky(byte *vbuf, uint32_t vbufPitch)
{
    if (!switches.textures && MAPSPOT(player->tilex,player->tiley,2) != 0)
        return;

    byte *ptr = vbuf;
    int i;
    for(i = 0; i < halfviewheight; i++, ptr += vbufPitch)
        memset(ptr, 0, viewwidth);

    for(i = 0; i < MAXPOINTS; i++)
    {
        point3d_t *pt = &points[i];
        int32_t x = pt->x * viewcos + pt->z * viewsin;
        int32_t y = pt->y << 16;
        int32_t z = (pt->z * viewcos - pt->x * viewsin) >> 8;
        if(z <= 0) continue;
        int shade = z >> 18;
        if(shade > 15) continue;
        int32_t xx = x / z * scaleFactor + halfviewwidth;
        int32_t yy = halfviewheight - y / z;
        if(xx >= 0 && xx < viewwidth && yy >= 0 && yy < halfviewheight)
            vbuf[yy * vbufPitch + xx] = shade + 15;
    }

    int32_t x = 16384 * viewcos + 16384 * viewsin;
    int32_t z = (16384 * viewcos - 16384 * viewsin) >> 8;
    if(z <= 0) return;
    int32_t xx = x / z * scaleFactor + halfviewwidth;
    int32_t yy = halfviewheight - ((halfviewheight - (halfviewheight >> 3)) << 22) / z;
    if(xx > -10 && xx < viewwidth)
    {
        int stopx = 10 * scaleFactor, starty = 0, stopy = 10 * scaleFactor;
        i = 0;
        if(xx < 0) {i = -xx; xx = 0;}
        if(xx > viewwidth - 11 * (signed)scaleFactor) stopx = viewwidth - xx;
        if(yy < 0) starty = -yy;
        if(yy > viewheight - 11 * (signed)scaleFactor) stopy = viewheight - yy;
        for(; i < stopx; i++)
            for(int j = starty; j < stopy; j++)
                vbuf[(yy + j) * vbufPitch + xx + i] = moon[j/scaleFactor * 10 + i/scaleFactor];
    }
}

#endif

#ifdef USE_RAIN
/*
======================
=
= DrawRain
=
======================
*/
void DrawRain(byte *vbuf, uint32_t vbufPitch)
{
#if defined(USE_FLOORCEILINGTEX) && defined(FIXRAINSNOWLEAKS)
    fixed dist;                                // distance to row projection
    fixed gu, gv, floorx, floory;              // global texture coordinates
#endif

    fixed px = (player->y + FixedMul(0x7900, viewsin)) >> 6;
    fixed pz = (player->x - FixedMul(0x7900, viewcos)) >> 6;
    int32_t ax, az, x, y, z, xx, yy, height, actheight;
    int shade;

    rainpos -= tics * 900;
    for(int i = 0; i < MAXPOINTS; i++)
    {
#ifdef VARIABLEWEATHER
        if (i > gamestate.weatherpoints) break;
#endif
        point3d_t *pt = &points[i];
        ax = pt->x + px;
        ax = 0x1fff - (ax & 0x3fff);
        az = pt->z + pz;
        az = 0x1fff - (az & 0x3fff);
        x = ax * viewcos + az * viewsin;
        y = -(57278464) + +((((pt->y << 6) + rainpos) & 0x0ffff) << 11);
        z = (az * viewcos - ax * viewsin) << 8 ;
        if(z <= 0) continue;
        shade = z >> 17;
        if(shade > 13) continue;
        xx = x / z + halfviewwidth;
        if(xx < 0 || xx >= viewwidth) continue;
        actheight = y / z;
        yy = halfviewheight - actheight;
        height = (458227712) / z;
        if(actheight < 0) actheight = -actheight;
        if(actheight < (wallheight[xx] >> 3) && height < wallheight[xx]) continue;

        if(xx >= 0 && xx < viewwidth && yy > 0 && yy < viewheight)
        {
#if defined(USE_FLOORCEILINGTEX) && defined(FIXRAINSNOWLEAKS)
            // Find the rain's tile coordinate
            // NOTE: This sometimes goes over the map edges.
            dist = ((heightnumerator / (( (( heightnumerator << 10) / z)>> 3) + 1)) << 5);
            gu =  viewx + FixedMul(dist, viewcos);
            gv = -viewy + FixedMul(dist, viewsin);
            floorx = (  gu >> TILESHIFT     ) & (MAPSIZE-1);
            floory = (-(gv >> TILESHIFT) - 1) & (MAPSIZE-1);

            // Is there a ceiling tile?
            if(MAPSPOT(floorx, floory, 2) >> 8) continue;
#endif

            vbuf[yy * vbufPitch + xx] = shade+15;
            vbuf[(yy - 1) * vbufPitch + xx] = shade+16;
            if(yy > 2)
                vbuf[(yy - 2) * vbufPitch + xx] = shade+17;
        }
    }
}

#endif

#ifdef USE_SNOW
/*
======================
=
= DrawSnow
=
======================
*/
void DrawSnow(byte *vbuf, uint32_t vbufPitch)
{
#if defined(USE_FLOORCEILINGTEX) && defined(FIXRAINSNOWLEAKS)
    fixed dist;                                // distance to row projection
 //   fixed tex_step;                            // global step per one screen pixel
    fixed gu, gv, floorx, floory;              // global texture coordinates
#endif

    fixed px = (player->y + FixedMul(0x7900, viewsin)) >> 6;
    fixed pz = (player->x - FixedMul(0x7900, viewcos)) >> 6;
    int32_t ax, az, x, y, z, xx, yy, height, actheight;
    int shade;

    rainpos -= tics * 256;
    for(int i = 0; i < MAXPOINTS; i++)
    {
#ifdef VARIABLEWEATHER
        if (i > gamestate.weatherpoints) break;
#endif
        point3d_t *pt = &points[i];
        ax = pt->x + px;
        ax = 0x1fff - (ax & 0x3fff);
        az = pt->z + pz;
        az = 0x1fff - (az & 0x3fff);
        x = ax * viewcos + az * viewsin;
        y = -(57278464) + ((((pt->y << 6) + rainpos) & 0x0ffff) << 11);
        z = (az * viewcos - ax * viewsin) >> 8;
        if(z <= 0) continue;
        shade = z >> 17;
        if(shade > 13) continue;
        xx = x / z * scaleFactor + halfviewwidth;
        if(xx < 0 || xx >= viewwidth) continue;
        actheight = y/z;
        yy = halfviewheight - actheight;
        height = (458227712) / z;
        if(actheight < 0) actheight = -actheight;
        if(actheight < (wallheight[xx] >> 3) && height < wallheight[xx]) continue;
        if(xx > 0 && xx < viewwidth && yy > 0 && yy < viewheight)
        {
#if defined(USE_FLOORCEILINGTEX) && defined(FIXRAINSNOWLEAKS)
            // Find the snow's tile coordinate
            // NOTE: This sometimes goes over the map edges.
            dist = ((heightnumerator / ((( heightnumerator << 10) / z) + 1)) << 5);
            gu =  viewx + FixedMul(dist, viewcos);
            gv = -viewy + FixedMul(dist, viewsin);
            floorx = (  gu >> TILESHIFT     ) & (MAPSIZE - 1);
            floory = (-(gv >> TILESHIFT) - 1) & (MAPSIZE - 1);

            // Is there a ceiling tile?
            if(MAPSPOT(floorx, floory, 2) >> 8) continue;
#endif

            if(shade < 10)
            {
                vbuf[yy * vbufPitch + xx] = shade+17;
                vbuf[yy * vbufPitch + xx - 1] = shade+16;
                vbuf[(yy - 1) * vbufPitch + xx] = shade+16;
                vbuf[(yy - 1) * vbufPitch + xx - 1] = shade+15;
            }
            else
                vbuf[yy * vbufPitch + xx] = shade+15;
        }
    }
}

#endif

#ifdef VARIABLEWEATHER
/*
======================
=
= InitializeWeatherState
=
======================
*/
void InitializeWeatherState() {
#ifdef CRAND
    int chance = Random(100);
#else
    int chance = US_RndT();
#endif

    gamestate.weatherchecktics = WEATHERCHANGEDELAY * 70;
    gamestate.weatherdelaytics = WEATHERDELAY * 70;
    gamestate.weatherchange = 0;

#ifdef CRAND
    if (chance < 40) { // 40% no weather
        gamestate.weatherpoints = 0; // no weather
    } else if (chance < 60) { // 20% in-between
        gamestate.weatherpoints = Random(100) + 5;
    } else { // 40% weather
        gamestate.weatherpoints = MAXPOINTS;
    }
#else
    if (chance < 100) { // 40% no weather
        gamestate.weatherpoints = 0; // no weather
    } else if (chance < 125) { // 20% in-between
        gamestate.weatherpoints = US_RndT() + 5;
    } else { // 40% weather
        gamestate.weatherpoints = MAXPOINTS;
    }
#endif

}

/*
======================
=
= PollWeatherChange
=
======================
*/
void PollWeatherChange() {
#ifdef CRAND
    if (Random(100) < 20) {
#else
    if (US_RndT() < 50) {
#endif// 20% chance for weather to change
        if (gamestate.weatherpoints == 0) {
            gamestate.weatherchange = 1;
        } else if (gamestate.weatherpoints == MAXPOINTS) {
            gamestate.weatherchange = -1;
        } else {
#ifdef CRAND
            gamestate.weatherchange = (Random(2) == 0 ? -1 : 1);
#else
            gamestate.weatherchange = (US_RndT() % 2 == 0 ? -1 : 1);
#endif
        }
    }

    gamestate.weatherchecktics = WEATHERCHANGEDELAY * 70;
}
#endif


