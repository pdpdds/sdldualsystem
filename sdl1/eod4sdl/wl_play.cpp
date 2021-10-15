// WL_PLAY.C

#include "wl_def.h"
#pragma hdrstop

#include "wl_cloudsky.h"
#include "wl_shade.h"

#ifdef VARIABLEWEATHER
#include "wl_atmos.h"
#endif

/*
=============================================================================

                                                 LOCAL CONSTANTS

=============================================================================
*/

#define sc_Question     0x35

/*
=============================================================================

                                                 GLOBAL VARIABLES

=============================================================================
*/

boolean madenoise;              // true when shooting or screaming

exit_t playstate;

static musicnames lastmusicchunk = (musicnames) 0;

int DebugOk;

objtype objlist[MAXACTORS];
objtype *newobj, *obj, *player, *lastobj, *objfreelist, *killerobj;

boolean noclip = 0, ammocheat = 0, ratiocheat = 0;
int singlestep, extravbls = 0;

uint16_t tilemap[MAPSIZE][MAPSIZE]; // wall values only
byte spotvis[MAPSIZE][MAPSIZE];
#ifdef REVEALMAP
byte spotsaw[MAPSIZE][MAPSIZE];
#endif
objtype *actorat[MAPSIZE][MAPSIZE];
statobj_t *statat[MAPSIZE][MAPSIZE];

//
// replacing refresh manager
//
int tics;

//
// control info
//
boolean mouseenabled, joystickenabled;
int dirscan[4] = { sc_UpArrow, sc_RightArrow, sc_DownArrow, sc_LeftArrow };
int buttonscan[NUMBUTTONS] = { sc_Control, sc_Alt, sc_LShift, sc_Space,
                               sc_1, sc_2, sc_3, sc_4, 
                               sc_5, sc_6, sc_7, sc_9};

#ifdef MOUSEWHEEL
int buttonmouse[5] = { bt_attack, bt_strafe, bt_use, bt_prevweapon, bt_nextweapon };
#else
int buttonmouse[4] = { bt_attack, bt_strafe, bt_use, bt_nobutton };
#endif

int buttonjoy[32] = {
#ifdef _arch_dreamcast
    bt_attack, bt_strafe, bt_use, bt_altspeed, bt_esc, bt_prevweapon, bt_nobutton, bt_nextweapon,
    bt_pause, bt_strafeleft, bt_straferight, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton,
#else
    bt_attack, bt_strafe, bt_use, bt_altspeed, bt_strafeleft, bt_straferight, bt_esc, bt_pause,
    bt_prevweapon, bt_nextweapon, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton,
#endif
    bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton,
    bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton
};

int viewsize;

boolean buttonheld[NUMBUTTONS];

boolean demorecord, demoplayback;
int8_t *demoptr, *lastdemoptr;
memptr demobuffer;

//
// current user input
//
int controlx, controly, controlh;         // range from -100 to 100 per tic
boolean buttonstate[NUMBUTTONS];

#define   MOVE1FORWARD()   controly = ((switches.alwaysrun && buttonstate[bt_altspeed]) || (!switches.alwaysrun && !buttonstate[bt_altspeed])) ? -BASEMOVE * tics : -RUNMOVE * tics;

#define   MOVE1FORWARDRIGHT()   { \
   controlx = ((switches.alwaysrun && buttonstate[bt_altspeed]) || (!switches.alwaysrun && !buttonstate[bt_altspeed])) ? BASEMOVEDIAG * tics : RUNMOVEDIAG * tics; \
   controly = -controlx; \
}

#define   MOVE1RIGHT()   controlx = BASEMOVE * tics;

#define   MOVE1BACKWARDRIGHT() { \
   controlx = ((switches.alwaysrun && buttonstate[bt_altspeed]) || (!switches.alwaysrun && !buttonstate[bt_altspeed])) ? BASEMOVEDIAG * tics : RUNMOVEDIAG * tics; \
   controly = controlx; \
}

#define   MOVE1BACKWARD()   controly = ((switches.alwaysrun && buttonstate[bt_altspeed]) || (!switches.alwaysrun && !buttonstate[bt_altspeed])) ? BASEMOVE * tics : RUNMOVE * tics;

#define   MOVE1BACKWARDLEFT()   { \
   controly = ((switches.alwaysrun && buttonstate[bt_altspeed]) || (!switches.alwaysrun && !buttonstate[bt_altspeed])) ? BASEMOVEDIAG * tics : RUNMOVEDIAG * tics; \
   controlx = -controly; \
}

#define   MOVE1LEFT()   controlx = -BASEMOVE * tics;

#define   MOVE1FORWARDLEFT()   { \
   controlx = ((switches.alwaysrun && buttonstate[bt_altspeed]) || (!switches.alwaysrun && !buttonstate[bt_altspeed])) ? -BASEMOVEDIAG * tics : -RUNMOVEDIAG * tics; \
   controly = controlx; \
}

#define   TURNLEFT()   controlh = (buttonstate[bt_altspeed]) ? -RUNTURN * tics : -BASETURN * tics;

#define   TURNRIGHT()   controlh = (buttonstate[bt_altspeed]) ? RUNTURN * tics : BASETURN * tics;

#define   MOVE2FORWARD()   controly = (buttonstate[bt_altspeed]) ? -RUNMOVE * tics : -BASEMOVE * tics;

#define   MOVE2FORWARDRIGHT()   { \
   controlx = (buttonstate[bt_altspeed]) ? RUNMOVEDIAG * tics : BASEMOVEDIAG * tics; \
   controly = -controlx; \
}

#define   MOVE2RIGHT()   controlx = BASEMOVE * tics;

#define   MOVE2BACKWARDRIGHT()   { \
   controlx = (buttonstate[bt_altspeed]) ? RUNMOVEDIAG * tics : BASEMOVEDIAG * tics; \
   controly = controlx; \
}

#define   MOVE2BACKWARD()   controly = (buttonstate[bt_altspeed]) ? RUNMOVE * tics : BASEMOVE * tics;

#define   MOVE2BACKWARDLEFT()   { \
   controly = (buttonstate[bt_altspeed]) ? RUNMOVEDIAG * tics : BASEMOVEDIAG * tics; \
   controlx = -controly; \
}

#define   MOVE2LEFT()   controlx = -BASEMOVE * tics;

#define   MOVE2FORWARDLEFT()   { \
   controlx = (buttonstate[bt_altspeed]) ? -RUNMOVEDIAG * tics : -BASEMOVEDIAG * tics; \
   controly = controlx; \
} 

int lastgamemusicoffset = 0;


//===========================================================================


void CenterWindow (word w, word h);
void InitObjList (void);
void RemoveObj (objtype * gone);
void PollControls (void);
int StopMusic (void);
void StartMusic (void);
void ContinueMusic (int offs);
void PlayLoop (void);

/*
=============================================================================

                                                 LOCAL VARIABLES

=============================================================================
*/


objtype dummyobj;

/*
=============================================================================

                               USER CONTROL

=============================================================================
*/

/*
===================
=
= PollKeyboardButtons
=
===================
*/

void PollKeyboardButtons (void)
{
    int i;

    for (i = 0; i < NUMBUTTONS; i++)
        if (Keyboard[buttonscan[i]])
            buttonstate[i] = true;
}


/*
===================
=
= PollMouseButtons
=
===================
*/

void PollMouseButtons (void)
{
    int buttons = IN_MouseButtons ();

    if (buttons & 1)
        buttonstate[buttonmouse[0]] = true;
    if (buttons & 2)
        buttonstate[buttonmouse[1]] = true;
    if (buttons & 4)
        buttonstate[buttonmouse[2]] = true;
#ifdef MOUSEWHEEL
    if (buttons & 8) {
        GetMessage("TEST",DEF_MSG_CLR);
        buttonstate[buttonmouse[3]] = true;
    }
    if (buttons & 16)
        buttonstate[buttonmouse[4]] = true;
#endif
}



/*
===================
=
= PollJoystickButtons
=
===================
*/

void PollJoystickButtons (void)
{
    int buttons = IN_JoyButtons();

    for(int i = 0, val = 1; i < JoyNumButtons; i++, val <<= 1)
    {
        if(buttons & val)
            buttonstate[buttonjoy[i]] = true;
    }
}


/*
===================
=
= PollKeyboardMove
=
===================
*/

void PollKeyboardMove (void)
{
   if (Keyboard[sc_Tab] || Keyboard [sc_BackSpace]) return; // annoying move when using debug/cheat keys

   if (Keyboard[dirscan[di_north]])
      if (Keyboard[dirscan[di_south]])
         if (Keyboard[dirscan[di_west]])
         {
            if (!Keyboard[dirscan[di_east]])
               MOVE1LEFT()
         }
         else
         {
            if (Keyboard[dirscan[di_east]])
               MOVE1RIGHT()
         }
      else
         if (Keyboard[dirscan[di_west]])
            if (Keyboard[dirscan[di_east]])
               MOVE1FORWARD()
            else
               MOVE1FORWARDLEFT()
         else
            if (Keyboard[dirscan[di_east]])
               MOVE1FORWARDRIGHT()
            else
               MOVE1FORWARD()
   else
      if (Keyboard[dirscan[di_south]])
         if (Keyboard[dirscan[di_west]])
            if (Keyboard[dirscan[di_east]])
               MOVE1BACKWARD()
            else
               MOVE1BACKWARDLEFT()
         else
            if (Keyboard[dirscan[di_east]])
               MOVE1BACKWARDRIGHT()
            else
               MOVE1BACKWARD()
      else
         if (Keyboard[dirscan[di_west]])
         {
            if (!Keyboard[dirscan[di_east]])
               MOVE1LEFT()
         }
         else
            if (Keyboard[dirscan[di_east]])
               MOVE1RIGHT()
}

void PollKeyboardMoveClassic(void) {
    if (Keyboard[sc_Tab] || Keyboard [sc_BackSpace]) return; // annoying move when using debug/cheat keys

    if (buttonstate[bt_strafe])
    {
        if (Keyboard[dirscan[di_north]])
            if (Keyboard[dirscan[di_south]])
            if (Keyboard[dirscan[di_west]])
            {
                if (!Keyboard[dirscan[di_east]])
                    MOVE2LEFT()
            }
            else
            {
                if (Keyboard[dirscan[di_east]])
                    MOVE2RIGHT()
            }
            else
            if (Keyboard[dirscan[di_west]])
            {
                if (Keyboard[dirscan[di_east]])
                    MOVE2FORWARD()
                else
                    MOVE2FORWARDLEFT()
            }
            else
            {
                if (Keyboard[dirscan[di_east]])
                    MOVE2FORWARDRIGHT()
                else
                    MOVE2FORWARD()
            }
        else
            if (Keyboard[dirscan[di_south]])
            if (Keyboard[dirscan[di_west]])
                if (Keyboard[dirscan[di_east]])
                    MOVE2BACKWARD()
                else
                    MOVE2BACKWARDLEFT()
            else
                if (Keyboard[dirscan[di_east]])
                    MOVE2BACKWARDRIGHT()
                else
                    MOVE2BACKWARD()
            else
            if (Keyboard[dirscan[di_west]])
            {
                if (!Keyboard[dirscan[di_east]])
                    MOVE2LEFT()
            }
            else
            {
                if (Keyboard[dirscan[di_east]])
                    MOVE2RIGHT()
            }
    }
    else
    {
        if (Keyboard[dirscan[di_west]])
        {
            if (!Keyboard[dirscan[di_east]])
            TURNLEFT()
        }
        if (Keyboard[dirscan[di_east]])
            TURNRIGHT()
        if (Keyboard[dirscan[di_north]])
        {
            if (!Keyboard[dirscan[di_south]])
            MOVE2FORWARD()
        }
        else
            if (Keyboard[dirscan[di_south]])
            MOVE2BACKWARD();
    } 
}

/*
===================
=
= PollMouseMove
=
===================
*/
byte viewingmap = 0;

void PollMouseMove (void)
{
    int mousexmove, mouseymove;

    SDL_GetMouseState(&mousexmove, &mouseymove);
    if(IN_IsInputGrabbed())
        IN_CenterMouse();

    mousexmove -= screenWidth / 2;
    mouseymove -= screenHeight / 2;
    if (!switches.strafe || viewingmap)
    {
        controlx += mousexmove * 10 / (13 - mouseadjustment);
        controly += mouseymove * 20 / (13 - mouseadjustment);
    }
    controlh += (mousexmove << 4) / (13 - mouseadjustment); 
}


/*
===================
=
= PollJoystickMove
=
===================
*/

void PollJoystickMove (void)
{
    int joyx, joyy;

    IN_GetJoyDelta (&joyx, &joyy);

   if (buttonstate[bt_strafe])
   {
      if (joyy < -64)
         if (joyx < -64)
            MOVE2FORWARDLEFT()
         else if (joyx > 64)
            MOVE2FORWARDRIGHT()
         else
            MOVE2FORWARD()
      else
         if (joyy > 64)
            if (joyx < -64)
               MOVE2BACKWARDLEFT()
            else if (joyx > 64)
               MOVE2BACKWARDRIGHT()
            else
               MOVE2BACKWARD()
   }
   else
   {
      if (joyx < -64)
         TURNLEFT()
      else if (joyx > 64)
         TURNRIGHT()
      if (joyy < -64)
         MOVE2FORWARD()
      else if (joyy > 64)
         MOVE2BACKWARD()
   }
}

/*
===================
=
= PollControls
=
= Gets user or demo input, call once each frame
=
= controlx              set between -100 and 100 per tic
= controly
= buttonheld[]  the state of the buttons LAST frame
= buttonstate[] the state of the buttons THIS frame
=
===================
*/

void PollControls (void)
{
    int max, min, rmax, rmin, i;
    byte buttonbits;

    IN_ProcessEvents();

//
// get timing info for last frame
//
    if (demoplayback || demorecord)   // demo recording and playback needs to be constant
    {
        // wait up to DEMOTICS Wolf tics
        uint32_t curtime = SDL_GetTicks();
        lasttimecount += DEMOTICS;
        int32_t timediff = (lasttimecount * 100) / 7 - curtime;
        if(timediff > 0)
            SDL_Delay(timediff);

        if(timediff < -2 * DEMOTICS)       // more than 2-times DEMOTICS behind?
            lasttimecount = (curtime * 7) / 100;    // yes, set to current timecount

        tics = DEMOTICS;
    }
    else
        CalcTics ();

    controlx = 0;
    controly = 0;
   controlh = 0;
    memcpy (buttonheld, buttonstate, sizeof (buttonstate));
    memset (buttonstate, 0, sizeof (buttonstate));

    if (demoplayback)
    {
        //
        // read commands from demo buffer
        //
        buttonbits = *demoptr++;
        for (i = 0; i < NUMBUTTONS; i++)
        {
            buttonstate[i] = buttonbits & 1;
            buttonbits >>= 1;
        }

        controlx = *demoptr++;
        controly = *demoptr++;
      controlh = *demoptr++;

        if (demoptr == lastdemoptr)
            playstate = ex_completed;   // demo is done

        controlx *= (int)tics;
        controly *= (int)tics;
      controlh *= (int)tics;

        return;
    }


//
// get button states
//
    PollKeyboardButtons ();

    if (mouseenabled && IN_IsInputGrabbed())
        PollMouseButtons ();

    if (joystickenabled)
        PollJoystickButtons ();

//
// get movements
//
    if (!switches.strafe) {
        PollKeyboardMoveClassic ();
    } else {
        PollKeyboardMove();
    }

    if (mouseenabled && IN_IsInputGrabbed())
        PollMouseMove ();

    if (joystickenabled)
        PollJoystickMove ();

//
// bound movement to a maximum
//
    max = 100 * tics;
    min = -max;
    rmax = max << 4;
    rmin = -rmax;
    if (controlh > rmax)
        controlh = rmax;
    else if (controlh < rmin)
        controlh = rmin;

    if (controlx > max)
        controlx = max;
    else if (controlx < min)
        controlx = min;

    if (controly > max)
        controly = max;
    else if (controly < min)
        controly = min;

    if (demorecord)
    {
        //
        // save info out to demo buffer
        //
        controlx /= (int)tics;
        controly /= (int)tics;
      controlh /= (int)tics;

        buttonbits = 0;

        // TODO: Support 32-bit buttonbits
        for (i = NUMBUTTONS - 1; i >= 0; i--)
        {
            buttonbits <<= 1;
            if (buttonstate[i])
                buttonbits |= 1;
        }

        *demoptr++ = buttonbits;
        *demoptr++ = controlx;
        *demoptr++ = controly;
      *demoptr++ = controlh;

        if (demoptr >= lastdemoptr - 8)
            playstate = ex_completed;
        else
        {
            controlx *= (int)tics;
            controly *= (int)tics;
         controlh *= (int)tics;
        }
    } 
}



//==========================================================================



///////////////////////////////////////////////////////////////////////////
//
//      CenterWindow() - Generates a window of a given width & height in the
//              middle of the screen
//
///////////////////////////////////////////////////////////////////////////
#define MAXX    320
#define MAXY    160

void CenterWindow (word w, word h)
{
    US_DrawWindow (((MAXX / 8) - w) / 2, ((MAXY / 8) - h) / 2, w, h);
}

//===========================================================================


/*
=====================
=
= CheckKeys
=
=====================
*/

void CheckKeys (void)
{
    ScanCode scan;


    if (screenfaded || demoplayback)    // don't do anything with a faded screen
        return;

    scan = LastScan;
    //
    // SECRET CHEAT CODE: 'MLI'
    //
    if (Keyboard[sc_M] && !Keyboard[sc_Tab] && !Keyboard[sc_C])
    {
        ViewMap(0);
        /*ClearMemory ();
        SD_PlaySound(BOSSACTIVESND);

        CA_CacheGrChunk (STARTFONT + 1);
        ClearSplitVWB ();

        Message ("Visit www.areyep.com!");

        gamestate.weapon = (weapontype) -1;
        gamestate.weapons = 0;

        DrawWeapon();
        DrawAmmo();

        UNCACHEGRCHUNK (STARTFONT + 1);
        IN_ClearKeysDown ();
        IN_Ack ();

        if (viewsize < 18)
            DrawPlayBorder ();
        

        gamestate.health = 100;
        gamestate.primaryammo = MAXPRIMARYAMMO;
        gamestate.keys = 0xF;
        gamestate.score = 0;
        gamestate.TimeCount += 42000L;
        GiveWeapon (wp_mk3);
        DrawWeapon ();
        DrawHealth ();
        DrawKeys ();
        DrawAmmo ();
        DrawScore ();

        ClearMemory ();
        CA_CacheGrChunk (STARTFONT + 1);
        ClearSplitVWB ();

        Message (STR_CHEATER1 "\n"
                 STR_CHEATER2 "\n\n" STR_CHEATER3 "\n" STR_CHEATER4 "\n" STR_CHEATER5);

        UNCACHEGRCHUNK (STARTFONT + 1);
        IN_ClearKeysDown ();
        IN_Ack ();

        if (viewsize < 17)
            DrawPlayBorder ();*/
    }

    //
    // SECRET CHEAT CODE: 'HAC'
    //
    if (Keyboard[sc_H] && Keyboard[sc_C] && Keyboard[sc_A]) {

    }

    //
    // TRYING THE MCS/RIP CHEAT CODE!
    //
    if (Keyboard[sc_R] && Keyboard[sc_I] && Keyboard[sc_P])
    {
        if (gamestate.difficulty > gd_easy) {
            gamestate.health--;
            DrawHealth();
        }
        
        ClearMemory ();
        SD_PlaySound(TAKEDAMAGESND);

        CA_CacheGrChunk (STARTFONT + 1);
        ClearSplitVWB ();

        Message ("Try MCS!");

        UNCACHEGRCHUNK (STARTFONT + 1);
        IN_ClearKeysDown ();
        IN_Ack ();

        if (viewsize < 18)
            DrawPlayBorder ();
    }

    if (Keyboard[sc_M] && Keyboard[sc_C] && Keyboard[sc_S]) {
        if (gamestate.difficulty > gd_easy) {
            gamestate.health--;
            DrawHealth();
        }
        
        ClearMemory ();
        SD_PlaySound(TAKEDAMAGESND);

        CA_CacheGrChunk (STARTFONT + 1);
        ClearSplitVWB ();

        Message ("Try RIP!");

        UNCACHEGRCHUNK (STARTFONT + 1);
        IN_ClearKeysDown ();
        IN_Ack ();

        if (viewsize < 18)
            DrawPlayBorder ();
    }

    if (Keyboard[sc_O] && Keyboard[sc_E] && Keyboard[sc_D]) {
        ClearMemory ();

        CA_CacheGrChunk (STARTFONT + 1);
        ClearSplitVWB ();

        Message ("End of Destiny SDL v1.0b\n"
                 "---------------------------\n"
                 "Original game by:\n"
                 "    AReyeP and MCS\n"
                 "Ported by:\n"
                 "    Kyle Albert\n"
                 "Visit www.areyep.com!");

        UNCACHEGRCHUNK (STARTFONT + 1);
        IN_ClearKeysDown ();
        IN_Ack ();

        if (viewsize < 18)
            DrawPlayBorder ();
    }

    if (Keyboard[sc_BackSpace]) {
        if (Keyboard[sc_T]) {
            if (switches.textures)
                GetMessage("Textures OFF",DEF_MSG_CLR);
            else
                GetMessage("Textures ON",DEF_MSG_CLR);

            switches.textures ^= 1;

            if (!switches.textures) {
                switches.weather = 0;
                switches.clouds = 0;
            }
            IN_ClearKeysDown();
        } else if (Keyboard[sc_S]) {
            if (switches.shading)
                GetMessage("Shading OFF",DEF_MSG_CLR);
            else
                GetMessage("Shading ON",DEF_MSG_CLR);

            switches.shading ^= 1;
            InitLevelShadeTable();
            IN_ClearKeysDown();
        } else if (Keyboard[sc_W] && switches.textures) {
            if (switches.weather)
                GetMessage("Weather OFF",DEF_MSG_CLR);
            else
                GetMessage("Weather ON",DEF_MSG_CLR);

            switches.weather ^= 1;
            IN_ClearKeysDown();
        } else if (Keyboard[sc_C] && switches.textures) {
            if (switches.clouds)
                GetMessage("Clouds OFF",DEF_MSG_CLR);
            else
                GetMessage("Clouds ON",DEF_MSG_CLR);

            switches.clouds ^= 1;
            IN_ClearKeysDown();
        } else if (Keyboard[sc_B]) {
            char buffer[40];

            if (!gamestate.cheated)
                sprintf(buffer, "Game saved %lu times without cheating", gamestate.saves);
            else
                sprintf(buffer, "Game saved %lu times with cheating", gamestate.saves);

            GetMessage(buffer,DEF_MSG_CLR);
        } else if (Keyboard[sc_F]) {
            if (!fpscounter)
                GetMessage("FPS counter ON",DEF_MSG_CLR);
            else
                GetMessage("FPS counter OFF",DEF_MSG_CLR);

            fpscounter ^= 1;
            IN_ClearKeysDown();
        } else if (Keyboard[sc_N]) {
            if (!switches.messages) {
                GetMessage("Notifications ON",DEF_MSG_CLR);
                IN_ClearKeysDown();
            } else {
                ClearMemory ();
                ClearSplitVWB ();
                Message("Notifications OFF");
                IN_ClearKeysDown();
                IN_Ack();
            }
            switches.messages ^= 1;

        }
    }

    if ((Keyboard[SDLK_KP_PLUS] || Keyboard[SDLK_EQUALS]) && viewsize != 21) {
        NewViewSize(viewsize+1);
        if (viewsize != 21)
            DrawPlayScreen();
        VW_UpdateScreen();
        IN_ClearKeysDown();
    }
    if ((Keyboard[SDLK_KP_MINUS] || Keyboard[SDLK_MINUS]) && viewsize != 4) {
        NewViewSize(viewsize-1);
        if (viewsize != 21)
            DrawPlayScreen();
        VW_UpdateScreen();
        IN_ClearKeysDown();
    }

//
// pause key weirdness can't be checked as a scan code
//
    if(buttonstate[bt_pause]) Paused = true;
    if(Paused)
    {
        int lastoffs = StopMusic();
        LatchDrawPic (20 - 4, 80 - 2 * 8, PAUSEDPIC);
        VH_UpdateScreen();
        IN_Ack ();
        Paused = false;
        ContinueMusic(lastoffs);
        if (MousePresent && IN_IsInputGrabbed())
            IN_CenterMouse();     // Clear accumulated mouse movement
        lasttimecount = GetTimeCount();
        return;
    }

//
// F1-F7/ESC to enter control panel
//
    if (
#ifndef DEBCHECK
           scan == sc_F10 ||
#endif
           scan == sc_F9 || scan == sc_F7 || scan == sc_F8)     // pop up quit dialog
    {
        ClearMemory ();
        ClearSplitVWB ();
        US_ControlPanel (scan);

        DrawPlayBorderSides ();

        SETFONTCOLOR (0, 15);
        IN_ClearKeysDown ();
        return;
    }

    if ((scan >= sc_F1 && scan <= sc_F9) || scan == sc_Escape || scan == sc_F11 ||buttonstate[bt_esc])
    {
        int lastoffs = StopMusic ();
        ClearMemory ();
        VW_FadeOut ();

        US_ControlPanel (buttonstate[bt_esc] ? sc_Escape : scan);

        SETFONTCOLOR (0, 15);
        IN_ClearKeysDown ();
		VW_FadeOut();

        if(viewsize != 21)
            DrawPlayScreen ();
        if (!startgame && !loadedgame)
            ContinueMusic (lastoffs);
        if (loadedgame)
            playstate = ex_abort;
        lasttimecount = GetTimeCount();
        if (MousePresent && IN_IsInputGrabbed())
            IN_CenterMouse();     // Clear accumulated mouse movement
        return;
    }

//
// TAB-? debug keys
//
#ifdef DEBUGKEYS
    if (DebugOk && Keyboard[sc_Tab])
    {
        CA_CacheGrChunk (STARTFONT);
        fontnumber = 0;
        SETFONTCOLOR (0x1E, 0x17); // center window font stuff
        if (gamestate.difficulty != gd_extreme && DebugKeys ()) {
            if (viewsize < 18)
                DrawPlayBorder ();       // dont let the blue borders flash

            gamestate.cheated = true;
            gamestate.score = gamestate.oldscore = 0;
            DrawScore();
        }

        if (MousePresent && IN_IsInputGrabbed())
            IN_CenterMouse();     // Clear accumulated mouse movement

        lasttimecount = GetTimeCount();
        return;
    }
#endif
}


//===========================================================================

/*
#############################################################################

                                  The objlist data structure

#############################################################################

objlist containt structures for every actor currently playing.  The structure
is accessed as a linked list starting at *player, ending when ob->next ==
NULL.  GetNewObj inserts a new object at the end of the list, meaning that
if an actor spawn another actor, the new one WILL get to think and react the
same frame.  RemoveObj unlinks the given object and returns it to the free
list, but does not damage the objects ->next pointer, so if the current object
removes itself, a linked list following loop can still safely get to the
next element.

<backwardly linked free list>

#############################################################################
*/


/*
=========================
=
= InitActorList
=
= Call to clear out the actor object lists returning them all to the free
= list.  Allocates a special spot for the player.
=
=========================
*/

int objcount;

void InitActorList (void)
{
    int i;

//
// init the actor lists
//
    for (i = 0; i < MAXACTORS; i++)
    {
        objlist[i].prev = &objlist[i + 1];
        objlist[i].next = NULL;
    }

    objlist[MAXACTORS - 1].prev = NULL;

    objfreelist = &objlist[0];
    lastobj = NULL;

    objcount = 0;

//
// give the player the first free spots
//
    GetNewActor ();
    player = newobj;

}

//===========================================================================

/*
=========================
=
= GetNewActor
=
= Sets the global variable new to point to a free spot in objlist.
= The free spot is inserted at the end of the liked list
=
= When the object list is full, the caller can either have it bomb out ot
= return a dummy object pointer that will never get used
=
=========================
*/

void GetNewActor (void)
{
    if (!objfreelist)
        Quit ("GetNewActor: No free spots in objlist!");

    newobj = objfreelist;
    objfreelist = newobj->prev;
    memset (newobj, 0, sizeof (*newobj));

    if (lastobj)
        lastobj->next = newobj;
    newobj->prev = lastobj;     // new->next is allready NULL from memset

    newobj->active = ac_no;
    lastobj = newobj;

    objcount++;
}

//===========================================================================

/*
=========================
=
= RemoveObj
=
= Add the given object back into the free list, and unlink it from it's
= neighbors
=
=========================
*/

void RemoveObj (objtype * gone)
{
    if (gone == player)
        Quit ("RemoveObj: Tried to remove the player!");

    gone->state = NULL;

//
// fix the next object's back link
//
    if (gone == lastobj)
        lastobj = (objtype *) gone->prev;
    else
        gone->next->prev = gone->prev;

//
// fix the previous object's forward link
//
    gone->prev->next = gone->next;

//
// add it back in to the free list
//
    gone->prev = objfreelist;
    objfreelist = gone;

    objcount--;
}

/*
=============================================================================

                                                MUSIC STUFF

=============================================================================
*/


/*
=================
=
= StopMusic
=
=================
*/
int StopMusic (void)
{
    int lastoffs = SD_MusicOff ();

    UNCACHEAUDIOCHUNK (STARTMUSIC + lastmusicchunk);

    return lastoffs;
}

//==========================================================================


/*
=================
=
= StartMusic
=
=================
*/

void StartMusic ()
{
    SD_MusicOff ();
    lastmusicchunk = (musicnames) levelinfo.music;
    SD_StartMusic(STARTMUSIC + lastmusicchunk);
}

void ContinueMusic (int offs)
{
    SD_MusicOff ();
    lastmusicchunk = (musicnames) levelinfo.music;
    SD_ContinueMusic(STARTMUSIC + lastmusicchunk, offs);
}

/*
=============================================================================

                                        PALETTE SHIFTING STUFF

=============================================================================
*/

#define NUMREDSHIFTS    6
#define REDSTEPS        8

#define NUMWHITESHIFTS  3
#define WHITESTEPS      20
#define WHITETICS       6


SDL_Color redshifts[NUMREDSHIFTS][256];
SDL_Color whiteshifts[NUMWHITESHIFTS][256];

int damagecount, bonuscount;
boolean palshifted;

/*
=====================
=
= InitRedShifts
=
=====================
*/

void InitRedShifts (void)
{
    SDL_Color *workptr, *baseptr;
    int i, j, delta;


//
// fade through intermediate frames
//
    for (i = 1; i <= NUMREDSHIFTS; i++)
    {
        workptr = redshifts[i - 1];
        baseptr = gamepal;

        for (j = 0; j <= 255; j++)
        {
            delta = 256 - baseptr->r;
            workptr->r = baseptr->r + delta * i / REDSTEPS;
            delta = -baseptr->g;
            workptr->g = baseptr->g + delta * i / REDSTEPS;
            delta = -baseptr->b;
            workptr->b = baseptr->b + delta * i / REDSTEPS;
            baseptr++;
            workptr++;
        }
    }

    for (i = 1; i <= NUMWHITESHIFTS; i++)
    {
        workptr = whiteshifts[i - 1];
        baseptr = gamepal;

        for (j = 0; j <= 255; j++)
        {
            delta = 256 - baseptr->r;
            workptr->r = baseptr->r + delta * i / WHITESTEPS;
            delta = 248 - baseptr->g;
            workptr->g = baseptr->g + delta * i / WHITESTEPS;
            delta = 0-baseptr->b;
            workptr->b = baseptr->b + delta * i / WHITESTEPS;
            baseptr++;
            workptr++;
        }
    }
}


/*
=====================
=
= ClearPaletteShifts
=
=====================
*/

void ClearPaletteShifts (void)
{
    bonuscount = damagecount = 0;
    palshifted = false;
}


/*
=====================
=
= StartBonusFlash
=
=====================
*/

void StartBonusFlash (void)
{
    bonuscount = NUMWHITESHIFTS * WHITETICS;    // white shift palette
}


/*
=====================
=
= StartDamageFlash
=
=====================
*/

void StartDamageFlash (int damage)
{
    damagecount += damage;
}


/*
=====================
=
= UpdatePaletteShifts
=
=====================
*/

void UpdatePaletteShifts (void)
{
    int red, white;

    if (bonuscount)
    {
        white = bonuscount / WHITETICS + 1;
        if (white > NUMWHITESHIFTS)
            white = NUMWHITESHIFTS;
        bonuscount -= tics;
        if (bonuscount < 0)
            bonuscount = 0;
    }
    else
        white = 0;


    if (damagecount)
    {
        red = damagecount / 10 + 1;
        if (red > NUMREDSHIFTS)
            red = NUMREDSHIFTS;

        damagecount -= tics;
        if (damagecount < 0)
            damagecount = 0;
    }
    else
        red = 0;

    if (red)
    {
        VL_SetPalette (redshifts[red - 1], false);
        palshifted = true;
    }
    else if (white)
    {
        VL_SetPalette (whiteshifts[white - 1], false);
        palshifted = true;
    }
    else if (palshifted)
    {
        VL_SetPalette (gamepal, false);        // back to normal
        palshifted = false;
    }
}


/*
=====================
=
= FinishPaletteShifts
=
= Resets palette to normal if needed
=
=====================
*/

void FinishPaletteShifts (void)
{
    if (palshifted)
    {
        palshifted = 0;
        VL_SetPalette (gamepal, true);
    }
}


/*
=============================================================================

                                                CORE PLAYLOOP

=============================================================================
*/


/*
=====================
=
= DoActor
=
=====================
*/

void DoActor (objtype * ob)
{
    void (*think) (objtype *);

    if (!ob->active && !areabyplayer[ob->areanumber])
        return;

    if (!(ob->flags & (FL_NONMARK | FL_NEVERMARK)) && MAPSPOT(ob->tilex,ob->tiley,1) != CLONETOILETTILE)
        actorat[ob->tilex][ob->tiley] = NULL;
//
// non transitional object
//

    if (!ob->ticcount)
    {
        think = (void (*)(objtype *)) ob->state->think;
        if (think)
        {
            think (ob);
            if (!ob->state)
            {
                RemoveObj (ob);
                return;
            }
        }

        if (ob->flags & FL_NEVERMARK)
            return;

        if ((ob->flags & FL_NONMARK) && actorat[ob->tilex][ob->tiley])
            return;

        actorat[ob->tilex][ob->tiley] = ob;
        return;
    }
//
// transitional object
//
    ob->ticcount -= (short) tics;
    while (ob->ticcount <= 0)
    {
        think = (void (*)(objtype *)) ob->state->action;        // end of state action
        if (think)
        {
            think (ob);
            if (!ob->state)
            {
                RemoveObj (ob);
                return;
            }
        }

        ob->state = ob->state->next;

        if (!ob->state)
        {
            RemoveObj (ob);
            return;
        }

        if (!ob->state->tictime)
        {
            ob->ticcount = 0;
            goto think;
        }

        ob->ticcount += ob->state->tictime;

        if (ob->obclass == turretobj && ob->dir != nodir) {
            ob->dir = (dirtype)((ob->dir + 1) % 8);
        }

    }

think:
    //
    // think
    //
    think = (void (*)(objtype *)) ob->state->think;
    if (think)
    {
        think (ob);
        if (!ob->state)
        {
            RemoveObj (ob);
            return;
        }
    }

    if (ob->flags & FL_NEVERMARK)
        return;

    if ((ob->flags & FL_NONMARK) && actorat[ob->tilex][ob->tiley])
        return;

    actorat[ob->tilex][ob->tiley] = ob;
}

//==========================================================================

/*
===================
=
= DoTimers
=
===================
*/
int32_t funnyticount = 0;

void DoTimers(void) {
        if (gamestate.godmodetics > 0) {
            StartBonusFlash();
            gamestate.godmodetics -= tics;
            if (gamestate.godmodetics <= 0) {
                SD_PlaySound(TAKEDAMAGESND);
                gamestate.godmode = false;
            }
        }

        if (gamestate.furytics > 0) {
            StartBonusFlash();
            gamestate.furytics -= tics;
            if (gamestate.furytics <= 0) {
                SD_PlaySound(TAKEDAMAGESND);
                gamestate.fury = false;
                if (gamestate.weapon == wp_knife && gamestate.lastweaponfury != wp_knife) {
                    NEXTWEAPON(gamestate.lastweaponfury);
                }
            }
        }

        if(gamestate.changingweapon && gamestate.attackframe == 0) {
            if(!gamestate.goingdown) {
                gamestate.weapchange -= 50 * tics; // 60
                if(gamestate.weapchange <= 0) {
                    gamestate.weapchange = 0;
                    gamestate.changingweapon = false;
                }
            } else {
                gamestate.weapchange += 50 * tics; // 60
                if(gamestate.weapchange >= 1200) { // 700
                    gamestate.goingdown = false;
                    gamestate.weapon = gamestate.nextweapon;
                    DrawWeapon();
                    DrawAmmo();
                }
            }
        } 

        if (gamestate.levelTimer > 0) {
            gamestate.levelTimer -= tics;

            if (gamestate.levelTimer <= 0) {
                playstate=ex_completed;
            }
        }

        if (gamestate.reloadtics > 0)
            gamestate.reloadtics-=tics;

#ifdef VARIABLEWEATHER
        if (switches.weather) {
            if (gamestate.weatherchecktics > 0) {
                gamestate.weatherchecktics -= tics;

                if (gamestate.weatherchecktics <= 0)
                    PollWeatherChange();
            }

            if (gamestate.weatherchange == -1) {
                if (gamestate.weatherpoints != 0) {
                    gamestate.weatherdelaytics -= tics;

                    if (gamestate.weatherdelaytics <= 0) {
                        gamestate.weatherdelaytics = WEATHERDELAY * 70;
                        gamestate.weatherpoints-=10;

                        if (gamestate.weatherpoints < 0)
                            gamestate.weatherpoints = 0;
                    }
                } else {
                    gamestate.weatherchange = 0;
                }
            } else if (gamestate.weatherchange == 1) {
                if (gamestate.weatherpoints != 500) {
                    gamestate.weatherdelaytics -= tics;

                    if (gamestate.weatherdelaytics <= 0) {
                        gamestate.weatherdelaytics = WEATHERDELAY * 70;
                        gamestate.weatherpoints+=10;

                        if (gamestate.weatherpoints > 500)
                            gamestate.weatherpoints = 500;
                    }
                } else {
                    gamestate.weatherchange = 0;
                }
            }
        }
#endif

        //
        // MAKE FUNNY FACE IF BJ DOESN'T MOVE FOR AWHILE
        //
#ifdef SPEAR
        funnyticount += tics;
        if (funnyticount > 2100)
        {
            funnyticount = 0;
            StatusDrawFace(BJWAITING1PIC + (US_RndT () & 1));
            facecount = 0;
        }
#endif
}
/*
===================
=
= PlayLoop
=
===================
*/

void PlayLoop (void)
{
#if defined(USE_FEATUREFLAGS) && defined(USE_CLOUDSKY)
    if(levelinfo.weather & FF_CLOUDSKY)
        InitSky();
#endif

#ifdef USE_SHADING
    InitLevelShadeTable();
#endif

    playstate = ex_stillplaying;
    lasttimecount = GetTimeCount();
    frameon = 0;
    anglefrac = 0;
    facecount = 0;
    funnyticount = 0;
    memset (buttonstate, 0, sizeof (buttonstate));
    ClearPaletteShifts ();

    if (MousePresent && IN_IsInputGrabbed())
        IN_CenterMouse();         // Clear accumulated mouse movement

    if (demoplayback)
        IN_StartAck ();

// Level beginning stuff
    do
    {
        PollControls ();

//
// actor thinking
//
        madenoise = false;

        MoveDoors ();
        MovePWalls ();
        ShootFireballs();
        DoTimers();

        for (obj = player; obj; obj = obj->next) {
            DoActor (obj);
        }

        UpdatePaletteShifts ();

        ThreeDRefresh ();

        gamestate.TimeCount += tics;

        UpdateSoundLoc ();      // JAB
        if (screenfaded)
            VW_FadeIn ();

		CheckKeys ();

//
// debug aids
//
        if (singlestep)
        {
            VW_WaitVBL (singlestep);
            lasttimecount = GetTimeCount();
        }
        if (extravbls)
            VW_WaitVBL (extravbls);

        if (demoplayback)
        {
            if (IN_CheckAck ())
            {
                IN_ClearKeysDown ();
                playstate = ex_abort;
            }
        }
    }
    while (!playstate && !startgame);

    if (playstate != ex_died)
        FinishPaletteShifts ();
}
