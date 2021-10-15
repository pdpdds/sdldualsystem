#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "string.h"
#include "assert.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_image.h"

#include "List.h"
#include "Symbol.h"
#include "2DCMC.h"
#include "auxiliar.h"
#include "GLTile.h"
#include "2DCMC.h"

#include "GLTManager.h"

List<char> font_name;
List< List<GLTile> > font_characters;

// Set this to false to be able to parse multi row font images:
bool ASSUME_SINGLE_ROW = true;

void font_extract(char *name, char *graphic_file, int nc, char *characters)
{
    SDL_Surface *sfc = IMG_Load(graphic_file);
    // assert(sfc != 0);

    if (sfc != 0) {
        int i, j;
        List<int> l_minx, l_miny, l_maxx, l_maxy;
        List<GLTile> l, *l2;
        List<int> l_row_y1, l_row_y2;
        GLTile *t;
        int n = 0;
        Uint32 c;
        bool *visited = new bool[sfc->h * sfc->w];

        for (i = 0;i < sfc->h*sfc->w;i++)
            visited[i] = false;

        for (i = 0;i < sfc->h;i++) {
            for (j = 0;j < sfc->w;j++) {
                c = getpixel(sfc, j, i) & AMASK;
                if (c != 0) {
                    // found one character!!
                    int min_x = j, max_x = j;
                    int min_y = i, max_y = i;
                    int x = j, y = i;
                    int k;
                    int *offs, offs2;
                    int next[4] = {1, -1, 0, 0};
                    List<int> stack;
                    next[2] = -sfc->w;
                    next[3] = sfc->w;

                    // find all the pixels of that character:
                    stack.Insert(new int(x + y*sfc->w));
                    while (!stack.EmptyP()) {
                        offs = stack.ExtractIni();

                        x = (*offs) % sfc->w;
                        y = (*offs) / sfc->w;

                        if (x < min_x)
                            min_x = x;
                        if (x > max_x)
                            max_x = x;
                        if (y < min_y)
                            min_y = y;
                        if (y > max_y)
                            max_y = y;

                        for (k = 0;k < 4;k++) {
                            offs2 = (*offs) + next[k];
                            x = offs2 % sfc->w;
                            y = offs2 / sfc->w;
                            if (x >= 0 && y >= 0 && x < sfc->w && y < sfc->h &&
                                    (getpixel(sfc, x, y)&AMASK) != 0 &&
                                    !visited[offs2] && !stack.MemberP(&offs2))
                                stack.Add(new int(offs2));
                        } // for
                        visited[*offs] = true;
                        delete offs;
                    } // while

                    // Create the character with all the pixels found:
                    t = new GLTile(sfc, min_x, min_y, (max_x - min_x) + 1, (max_y - min_y) + 1);
                    l.Add(t);
                    l_minx.Add(new int(min_x));
                    l_miny.Add(new int(min_y));
                    l_maxx.Add(new int(max_x));
                    l_maxy.Add(new int(max_y));
                    n++;

                    // Remove the found character from the original image so that it is not found again:
                    {
                        SDL_Rect r;
                        r.x = min_x;
                        r.y = min_y;
                        r.w = (max_x - min_x) + 1;
                        r.h = (max_y - min_y) + 1;
                        SDL_FillRect(sfc, &r, 0);
                    }
                } // if
            } // for
        } // for

        delete []visited;

        //  assert(n==nc);

        // sort the found characters (buble sort) as a "human" would sort them
        // according to their position in the image:
        {
            bool change;
            bool any_change = false;

            do
            {
                any_change = false;
                for (i = 0;i < n - 1;i++) {
                    change = false;


					if (ASSUME_SINGLE_ROW) {
						if (*(l_minx[i]) > *(l_maxx[i + 1]))
							change = true;
					} else {
						if (*(l_maxy[i]) > *(l_miny[i + 1]) && *(l_minx[i]) > *(l_maxx[i + 1]))
							change = true;
					} // if


                    if (change) {
                        GLTile *t2;
                        int *intt2;
                        t2 = l[i];
                        l.SetObj(i, l[i + 1]);
                        l.SetObj(i + 1, t2);

                        intt2 = l_minx[i];
                        l_minx.SetObj(i, l_minx[i + 1]);
                        l_minx.SetObj(i + 1, intt2);
                        intt2 = l_miny[i];
                        l_miny.SetObj(i, l_miny[i + 1]);
                        l_miny.SetObj(i + 1, intt2);
                        intt2 = l_maxx[i];
                        l_maxx.SetObj(i, l_maxx[i + 1]);
                        l_maxx.SetObj(i + 1, intt2);
                        intt2 = l_maxy[i];
                        l_maxy.SetObj(i, l_maxy[i + 1]);
                        l_maxy.SetObj(i + 1, intt2);

                        any_change = true;
                    } // if
                } // for
            } while (any_change);
        }

        // Automatically identify the rows of characters:
        {
            int i, j;
            int row_start = 0;
            int prev_y1 = -1, prev_y2 = -1;
            bool first = true;

            for (i = 0;i < n;i++)
            {
                if (first) {
                    row_start = i;
                    prev_y1 = *(l_miny[i]);
                    prev_y2 = *(l_maxy[i]);
                    first = false;
                } else {
                    if (ASSUME_SINGLE_ROW || (*(l_miny[i]) < prev_y2 && *(l_maxy[i]) > prev_y1)) {
                        if (*(l_miny[i]) < prev_y1)
                            prev_y1 = *(l_miny[i]);
                        if (*(l_maxy[i]) < prev_y2)
                            prev_y2 = *(l_maxy[i]);
                    } else {
                        // end of row:
                        for (j = row_start;j < i;j++) {
                            l[j]->set_hotspot(0, (int)(((prev_y1 + prev_y2) / 2 - (*(l_miny[j]) + *(l_maxy[j])) / 2)*0.66f));
                        } // for
                        row_start = i;
                        prev_y1 = *(l_miny[i]);
                        prev_y2 = *(l_maxy[i]);
                    } // if
                } // if

            } // for
            // end of row:
            for (j = row_start;j < i;j++)
            {
                l[j]->set_hotspot(0, (int)(((prev_y1 + prev_y2) / 2 - (*(l_miny[j]) + *(l_maxy[j])) / 2)*0.66f));
            } // for
        }

        // Create a vector of 256 empty characters, and then fill it with the characters found in the image:
        {
            char *name2 = new char[strlen(name) + 1];
            strcpy(name2, name);
            font_name.Add(name2);

            l2 = new List<GLTile>;
            for (i = 0;i < 256;i++)
                l2->Add(0);

            for (i = 0;i < n;i++)
                l2->SetObj(characters[i], l[i]);

			while(!l.EmptyP()) {
				t=l.ExtractIni();
				if (!l2->MemberRefP(t)) {
					delete t;
				} // if

			} // while

            font_characters.Add(l2);
        }

        SDL_FreeSurface(sfc);
    } /* if */
} /* font_extract */


void font_release(void)
{
    font_name.Delete();
    font_characters.Delete();
} /* font_release */



void font_preview(char *prefix)
{
    int k = font_name.Position(prefix);
    List<GLTile> *l = font_characters[k];
    GLTile *t;
    int i = 0;
    int x = 0;

    for (i = 0;i < 256;i++) {
        t = l->operator [](i);
        if (t!=0) {
            t->draw(float(x), 0, 0, 0, 1);
            x += t->get_dx();
        } // if
    } // for
} /* font_preview */



void font_print(char *prefix, char *text)
{
    int k = font_name.Position(prefix);
    List<GLTile> *l = font_characters[k];
    GLTile *t;
    int i = 0;
    int x = 0;

    for (i = 0;text[i] != 0;i++) {
        if (text[i] == ' ') {
            x += l->operator []('a')->get_dx() / 2;
        } else {
            t = l->operator [](text[i]);
            if (t!=0) {
                t->draw(float(x), 0, 0, 0, 1);
                x += t->get_dx();
            } // if
        } // if
    } // for
} /* font_print */


void font_print(char *prefix, char *text, int x_offset, float alpha)
{
    int k = font_name.Position(prefix);
    List<GLTile> *l = font_characters[k];
    GLTile *t;
    int i = 0;
    int x = 0;

    for (i = 0;text[i] != 0;i++) {
        if (text[i] == ' ') {
            x += l->operator []('a')->get_dx() / 2;
        } else {
            t = l->operator [](text[i]);
            if (t!=0) {
                t->draw(1,1,1,alpha,float(x), 0, 0, 0, 1);
                x += t->get_dx();
            } // if
        } // if
        x += x_offset;
    } // for
} /* font_print */


void font_print(char *prefix, char *text, int x_offset, bool y_center)
{
    int k = font_name.Position(prefix);
    List<GLTile> *l = font_characters[k];
    GLTile *t;
    int i = 0;
    int x = 0;
    int y = 0;

    int maxheight = 0;
    int myheight;

    // determine max height of font
    for (i = 0;text[i] != 0;i++) {
        myheight = l->operator [](text[i])->get_dy();
        if (myheight > maxheight) {
            maxheight = myheight;
        }
    }
    myheight = 0;

    for (i = 0;text[i] != 0;i++) {
        if (y_center) {
            myheight = l->operator [](text[i])->get_dy();
            if (myheight < maxheight) {
                y = (maxheight - myheight) / 2;
            } else {
                y = 0;
            }
        }
        if (text[i] == ' ') {
            x += l->operator []('a')->get_dx() / 2;
        } else {
            t = l->operator [](text[i]);
            if (t!=0) {
                t->draw(float(x), float(y), 0, 0, 1);
                x += t->get_dx();
            } // if
        } // if
        x += x_offset;
    } // for
} /* font_print */


void font_print(int x, int y, int z, float angle, float scale, char *font, char *text)
{
    glPushMatrix();
    glTranslatef(float(x), float(y), float(z));
    if (scale != 1)
        glScalef(scale, scale, scale);
    if (angle != 0)
        glRotatef(angle, 0, 0, 1);

    font_print(font, text, 0, float(1));

    glPopMatrix();
} /* font_print */


void font_print(int x, int y, int z, float angle, float scale, char *font, char *text, int x_offset)
{
    glPushMatrix();
    glTranslatef(float(x), float(y), float(z));
    if (scale != 1)
        glScalef(scale, scale, scale);
    if (angle != 0)
        glRotatef(angle, 0, 0, 1);

    font_print(font, text, x_offset, float(1));

    glPopMatrix();
} /* font_print */


void font_print(int x, int y, int z, float angle, float scale, float alpha, char *font, char *text, int x_offset)
{
    glPushMatrix();
    glTranslatef(float(x), float(y), float(z));
    if (scale != 1)
        glScalef(scale, scale, scale);
    if (angle != 0)
        glRotatef(angle, 0, 0, 1);

    font_print(font, text, x_offset, alpha);

    glPopMatrix();
} /* font_print */


void font_print(int x, int y, int z, float angle, float scale, char *font, char *text, int x_offset, bool y_center)
{
    glPushMatrix();
    glTranslatef(float(x), float(y), float(z));
    if (scale != 1)
        glScalef(scale, scale, scale);
    if (angle != 0)
        glRotatef(angle, 0, 0, 1);

    font_print(font, text, x_offset, y_center);

    glPopMatrix();
} /* font_print */

void font_print_c(char *prefix, char *text)
{
    int k = font_name.Position(prefix);
    List<GLTile> *l = font_characters[k];
    GLTile *t;
    int i = 0;
    int x = 0;
    int dx = 0, dy = 0;

    for (i = 0;text[i] != 0;i++) {
        if (text[i] == ' ') {
            dx += l->operator []('a')->get_dx() / 2;
        } else {
            t = l->operator [](text[i]);
            if (t!=0) {
                dx += t->get_dx();
                if (t->get_dy() > dy)
                    dy = t->get_dy();
            } // if
        } // if
    } // for

    glPushMatrix();
    glTranslatef( float(-dx / 2), float(-dy / 2), 0);

    for (i = 0;text[i] != 0;i++) {
        if (text[i] == ' ') {
            x += l->operator []('a')->get_dx() / 2;
        } else {
            t = l->operator [](text[i]);
            if (t!=0) {
                t->draw(float(x), 0, 0, 0, 1);
                x += t->get_dx();
            } // if
        } // if
    } // for

    glPopMatrix();

} /* font_print_c */


void font_print_c(int x, int y, int z, float angle, float scale, char *font, char *text)
{
    glPushMatrix();
    glTranslatef(float(x), float(y), float(z));
    if (scale != 1)
        glScalef(scale, scale, scale);
    if (angle != 0)
        glRotatef(angle, 0, 0, 1);

    font_print_c(font, text);

    glPopMatrix();
} /* font_print */


void font_box_c(char *prefix, char *text, int *px, int *py, int *pdx, int *pdy)
{
    int k = font_name.Position(prefix);
    List<GLTile> *l = font_characters[k];
    GLTile *t;
    int i = 0;
    // int x=0;
    int dx = 0, dy = 0;

    for (i = 0;text[i] != 0;i++) {
        if (text[i] == ' ') {
            dx += l->operator []('a')->get_dx() / 2;
        } else {
            t = l->operator [](text[i]);
            if (t!=0) {
                dx += t->get_dx();
                if (t->get_dy() > dy)
                    dy = t->get_dy();
            } // if
        } // if
    } // for

    *px = -(dx / 2);
    *py = -(dy / 2);
    *pdx = dx;
    *pdy = dy;
} /* font_box_c */



void font_box_c(char *prefix, char *text, int *px, int *py, int *pdx, int *pdy, int x_offset)
{
    int k = font_name.Position(prefix);
    List<GLTile> *l = font_characters[k];
    GLTile *t;
    int i = 0;
    // int x=0;
    int dx = 0, dy = 0;

    for (i = 0;text[i] != 0;i++) {
        if (text[i] == ' ') {
            dx += l->operator []('a')->get_dx() / 2;
        } else {
            t = l->operator [](text[i]);
            if (t!=0) {
                dx += t->get_dx();
                if (t->get_dy() > dy)
                    dy = t->get_dy();
            } // if
        } // if
        dx += x_offset;
    } // for

    *px = -(dx / 2);
    *py = -(dy / 2);
    *pdx = dx;
    *pdy = dy;
} /* font_box_c */

void font_print_c(char *prefix, char *text, int x_offset,float alpha)
{
    int k = font_name.Position(prefix);
    List<GLTile> *l = font_characters[k];
    GLTile *t;
    int i = 0;
    int x = 0;
    int dx = 0, dy = 0;

    for (i = 0;text[i] != 0;i++) {
        if (text[i] == ' ') {
            dx += l->operator []('a')->get_dx() / 2;
        } else {
            t = l->operator [](text[i]);
            if (t!=0) {
                dx += t->get_dx();
                if (t->get_dy() > dy)
                    dy = t->get_dy();
            } // if
        } // if
        dx += x_offset;
    } // for

    glPushMatrix();
    glTranslatef(float(-dx / 2), float(-dy / 2), 0);

    for (i = 0;text[i] != 0;i++) {
        if (text[i] == ' ') {
            x += l->operator []('a')->get_dx() / 2;
        } else {
            t = l->operator [](text[i]);
            if (t!=0) {
                t->draw(1,1,1,alpha,float(x), 0, 0, 0, 1);
                x += t->get_dx();
            } // if
        } // if
        x += x_offset;
    } // for

    glPopMatrix();

} /* font_print_c */


void font_print_c(int x, int y, int z, float angle, float scale, char *font, char *text, int x_offset)
{
    glPushMatrix();
    glTranslatef(float(x), float(y), float(z));
    if (scale != 1)
        glScalef(scale, scale, scale);
    if (angle != 0)
        glRotatef(angle, 0, 0, 1);

    font_print_c(font, text, x_offset, 1);

    glPopMatrix();
} /* font_print */


void font_print_c(int x, int y, int z, float angle, float scale, float alpha, char *font, char *text, int x_offset)
{
    glPushMatrix();
    glTranslatef(float(x), float(y), float(z));
    if (scale != 1)
        glScalef(scale, scale, scale);
    if (angle != 0)
        glRotatef(angle, 0, 0, 1);

    font_print_c(font, text, x_offset, alpha);

    glPopMatrix();
} /* font_print */

