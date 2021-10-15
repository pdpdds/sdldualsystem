#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include "windows.h"
#endif

#include <GL/gl.h>

#include "SDL.h"
#include "SDL_image.h"

#include "stdio.h"
#include "math.h"

#include "auxiliar.h"

#include "GLTile.h"
#include "SDL_glutaux.h"

int nearest_2pow(int n)
{
    int res = 2;

    for (res = 2;res < 2048;res *= 2) {
        if (res >= n)
            return res;
    } /* for */

    return res;
} /* nearest_2pow */


GLuint createTexture(SDL_Surface *sfc, float *tx, float *ty)
{
    unsigned int tname = 0;
    int szx, szy;

    if (sfc != 0) {
        SDL_Surface *sfc2 = 0;

        szx = nearest_2pow(sfc->w);
        szy = nearest_2pow(sfc->h);
        *tx = (sfc->w) / float(szx);
        *ty = (sfc->h) / float(szy);

        sfc2 = SDL_CreateRGBSurface(SDL_SWSURFACE, szx, szy, 32, RMASK, GMASK, BMASK, AMASK);
        SDL_SetAlpha(sfc, 0, 0);
        SDL_BlitSurface(sfc, 0, sfc2, 0);

        glGenTextures(1, &tname);
        glPixelStorei(GL_UNPACK_ALIGNMENT, tname);
        glBindTexture(GL_TEXTURE_2D, tname);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, szx, szy, 0, GL_RGBA, GL_UNSIGNED_BYTE, sfc2->pixels);
        SDL_FreeSurface(sfc2);
    } else {
        return 0;
    } /* if */

    return tname;
} /* createTexture */


GLuint createTextureClamp(SDL_Surface *sfc, float *tx, float *ty)
{
    unsigned int tname = 0;
    int szx, szy;

    if (sfc != 0) {
        SDL_Surface *sfc2 = 0;

        szx = nearest_2pow(sfc->w);
        szy = nearest_2pow(sfc->h);
        *tx = (sfc->w) / float(szx);
        *ty = (sfc->h) / float(szy);

        sfc2 = SDL_CreateRGBSurface(SDL_SWSURFACE, szx, szy, 32, RMASK, GMASK, BMASK, AMASK);
        SDL_SetAlpha(sfc, 0, 0);
        SDL_BlitSurface(sfc, 0, sfc2, 0);

        glGenTextures(1, &tname);
        glPixelStorei(GL_UNPACK_ALIGNMENT, tname);
        glBindTexture(GL_TEXTURE_2D, tname);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, szx, szy, 0, GL_RGBA, GL_UNSIGNED_BYTE, sfc2->pixels);
        SDL_FreeSurface(sfc2);
    } else {
        return 0;
    } /* if */

    return tname;
} /* createTextureClamp */


GLuint createTextureSmooth(SDL_Surface *sfc, float *tx, float *ty)
{
    unsigned int tname = 0;
    int szx, szy;

    if (sfc != 0) {
        SDL_Surface *sfc2 = 0;

        szx = nearest_2pow(sfc->w);
        szy = nearest_2pow(sfc->h);
        *tx = (sfc->w) / float(szx);
        *ty = (sfc->h) / float(szy);

        sfc2 = SDL_CreateRGBSurface(SDL_SWSURFACE, szx, szy, 32, RMASK, GMASK, BMASK, AMASK);
        SDL_SetAlpha(sfc, 0, 0);
        SDL_BlitSurface(sfc, 0, sfc2, 0);

        glGenTextures(1, &tname);
        glPixelStorei(GL_UNPACK_ALIGNMENT, tname);
        glBindTexture(GL_TEXTURE_2D, tname);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, szx, szy, 0, GL_RGBA, GL_UNSIGNED_BYTE, sfc2->pixels);
        SDL_FreeSurface(sfc2);
    } else {
        return 0;
    } /* if */

    return tname;
} /* createTextureSmooth */


GLuint createTextureClampSmooth(SDL_Surface *sfc, float *tx, float *ty)
{
    unsigned int tname = 0;
    int szx, szy;

    if (sfc != 0) {
        SDL_Surface *sfc2 = 0;

        szx = nearest_2pow(sfc->w);
        szy = nearest_2pow(sfc->h);
        *tx = (sfc->w) / float(szx);
        *ty = (sfc->h) / float(szy);

        sfc2 = SDL_CreateRGBSurface(SDL_SWSURFACE, szx, szy, 32, RMASK, GMASK, BMASK, AMASK);
        SDL_SetAlpha(sfc, 0, 0);
        SDL_BlitSurface(sfc, 0, sfc2, 0);

        glGenTextures(1, &tname);
        glPixelStorei(GL_UNPACK_ALIGNMENT, tname);
        glBindTexture(GL_TEXTURE_2D, tname);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, szx, szy, 0, GL_RGBA, GL_UNSIGNED_BYTE, sfc2->pixels);
        SDL_FreeSurface(sfc2);
    } else {
        return 0;
    } /* if */

    return tname;
} /* createTextureClampSmooth */


GLuint createTextureFromScreen(int x, int y, int dx, int dy, float *tx, float *ty)
{
    unsigned int tname = 0;
    int szx, szy;

    szx = nearest_2pow(dx);
    szy = nearest_2pow(dy);
    *tx = (dx) / float(szx);
    *ty = (dy) / float(szy);

    glGenTextures(1, &tname);
    glPixelStorei(GL_UNPACK_ALIGNMENT, tname);
    glBindTexture(GL_TEXTURE_2D, tname);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    // glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    // glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, x, y, dx, dy);

    return tname;
} /* createTextureFromScreen */


/*
 
void gl_print_left(int x,int y,char *text,int r,int g,int b,TTF_Font *font)
{
 SDL_Surface *sfc;
 SDL_Color c;
 GLTile *t;
 
 c.r=r;
 c.g=g;
 c.b=b;
 
 sfc=TTF_RenderText_Blended(font,text,c);
 t=new GLTile(sfc);
 t->draw(x,y,0,1.0);
 SDL_FreeSurface(sfc);
 delete t;
} // gl_print_left  
 
 
void gl_print_center(int x,int y,char *text,int r,int g,int b,TTF_Font *font)
{
 SDL_Surface *sfc;
 SDL_Color c;
 GLTile *t;
 
 c.r=r;
 c.g=g;
 c.b=b;
 
 sfc=TTF_RenderText_Blended(font,text,c);
 t=new GLTile(sfc);
 t->set_hotspot(sfc->w/2,0);
 t->draw(x,y,0,1.0);
 SDL_FreeSurface(sfc);
 delete t;
} // gl_print_center  
*/

void gl_line(int x1, int y1, int x2, int y2, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_LINES);
    glVertex3f(float(x1), float(y1), 0);
    glVertex3f(float(x2), float(y2), 0);
    glEnd();
} /* gl_line */
