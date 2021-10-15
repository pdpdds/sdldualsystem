#include "version.h"

#ifdef USE_FLOORCEILINGTEX

#include "wl_def.h"
#include "wl_shade.h"

//
// Texture split across doorways
// Credit: Codetech84
//

unsigned GetSplitTexture (int u, int v, int x, int y, boolean vert)
{
    unsigned ret;

    if (vert) {
        if (u < 32) {
            ret = MAPSPOT(x-1,y,2);
        }else{
            ret = MAPSPOT(x+1,y,2);
        }
    } else {
        if (v < 32) {
            ret = MAPSPOT(x,y+1,2);
        }
        else {
            ret = MAPSPOT(x,y-1,2);
        }
    }
    return ret;
} 

// Textured Floor and Ceiling by DarkOne
// With multi-textured floors and ceilings stored in lower and upper bytes of
// according tile in third mapplane, respectively.
void DrawFloorAndCeiling(byte *vbuf, unsigned vbufPitch, int min_wallheight)
{
    fixed dist;                                // distance to row projection
    fixed tex_step;                            // global step per one screen pixel
    fixed gu, gv, du, dv;                      // global texture coordinates
    int u, v;                                  // local texture coordinates
    byte *toptex, *bottex;
    unsigned lasttoptex = 0xffffffff, lastbottex = 0xffffffff;
    uint16_t tile; 

    int y0 = min_wallheight >> 3;              // starting y value
    if(y0 > halfviewheight)
        return;                                // view obscured by walls
    if(!y0) y0 = 1;                            // don't let division by zero
    unsigned bot_offset0 = vbufPitch * (halfviewheight + y0);
    unsigned top_offset0 = vbufPitch * (halfviewheight - y0 - 1);

    // draw horizontal lines
    for(int y = y0, bot_offset = bot_offset0, top_offset = top_offset0;
        y < halfviewheight; y++, bot_offset += vbufPitch, top_offset -= vbufPitch)
    {
        dist = (heightnumerator / (y + 1)) << 5;
        gu =  viewx + FixedMul(dist, viewcos);
        gv = -viewy + FixedMul(dist, viewsin);
        tex_step = (dist << 8) / viewwidth / 175;
        du =  FixedMul(tex_step, viewsin);
        dv = -FixedMul(tex_step, viewcos);
        gu -= halfviewwidth * du;
        gv -= halfviewwidth * dv; // starting point (leftmost)
#ifdef USE_SHADING
        byte *curshades = shadetable[GetShade(y << 3)];
#endif
        for(int x = 0, bot_add = bot_offset, top_add = top_offset;
            x < viewwidth; x++, bot_add++, top_add++)
        {
            if(wallheight[x] >> 3 <= y)
            {
                int curx = (gu >> TILESHIFT) & MAPSIZE_M1;
                int cury = (-(gv >> TILESHIFT) - 1) & MAPSIZE_M1;
                unsigned curtex = MAPSPOT(curx, cury, 2);
                if(curtex)
                {
                    u = (gu >> (TILESHIFT - TEXTURESHIFT)) & (TEXTURESIZE - 1);
                    v = (gv >> (TILESHIFT - TEXTURESHIFT)) & (TEXTURESIZE - 1);
                    tile = tilemap[curx][cury];
                    unsigned curtoptex;

                    if (tile & BIT_DOOR) {
                        curtoptex = GetSplitTexture(u,v, curx, cury,doorobjlist[tile & LAST_DOORNUM].vertical)>>8;
                    }
                    else 
                        curtoptex = curtex >> 8;
                    if (curtoptex != lasttoptex)
                    {
                        lasttoptex = curtoptex;
                        toptex = PM_GetTexture(curtoptex);
                    }

                    tile = tilemap[curx][cury];
                    unsigned curbottex;

                    if (tile & BIT_DOOR)
                        curbottex = GetSplitTexture(u,v, curx, cury,doorobjlist[tile & LAST_DOORNUM].vertical)&0xff;
                    else 
                        curbottex = curtex & 0xff;

                    if (curbottex != lastbottex)
                    {
                        lastbottex = curbottex;
                        bottex = PM_GetTexture(curbottex);
                    }
                    unsigned texoffs = (u << TEXTURESHIFT) + (TEXTURESIZE - 1) - v;
#ifdef USE_SHADING
                    if(curtoptex)
                        vbuf[top_add] = curshades[toptex[texoffs]];
                    if(curbottex)
                        vbuf[bot_add] = curshades[bottex[texoffs]];
#else
                    if(curtoptex)
                        vbuf[top_add] = toptex[texoffs];
                    if(curbottex)
                        vbuf[bot_add] = bottex[texoffs];
#endif
                }
            }
            gu += du;
            gv += dv;
        }
    }
}

#endif
