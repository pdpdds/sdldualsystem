#include "h2def.h"

#ifdef USE_SDL
int VSDL_InitGraphics(void); 
void VSDL_ShutdownGraphics(void); 
void VSDL_GetEvent(void);
void VSDL_SetPalette(byte *palette);
void VSDL_Stub(void);
void VSDL_FinishUpdate(void);
#endif

#ifdef FBSD_CONSOLE
int FBSD_InitGraphics(void); 
void FBSD_ShutdownGraphics(void); 
void FBSD_GetEvent(void);
void FBSD_SetPalette(byte *palette);
void FBSD_Stub(void);
void FBSD_FinishUpdate(void);
#endif

void (*SW_ShutdownGraphics)(void);
void (*SW_StartTic)(void);
void (*SW_SetPalette)(byte *palette);
void (*SW_StartFrame)(void);
void (*SW_UpdateNoBlit)(void);
void (*SW_FinishUpdate)(void);

void I_InitGraphics(void) {
#ifdef USE_SDL
 if (VSDL_InitGraphics()) {
  SW_ShutdownGraphics=VSDL_ShutdownGraphics; 
  SW_StartTic=VSDL_GetEvent;
  SW_SetPalette=VSDL_SetPalette;
  SW_StartFrame=VSDL_Stub;
  SW_UpdateNoBlit=VSDL_Stub;
  SW_FinishUpdate=VSDL_FinishUpdate;
 }
 else
#endif
#ifdef FBSD_CONSOLE
 if (FBSD_InitGraphics()) {
  SW_ShutdownGraphics=FBSD_ShutdownGraphics; 
  SW_StartTic=FBSD_GetEvent;
  SW_SetPalette=FBSD_SetPalette;
  SW_StartFrame=FBSD_Stub;
  SW_UpdateNoBlit=FBSD_Stub;
  SW_FinishUpdate=FBSD_FinishUpdate;
 }
 else
#endif
 {
  I_Error("Unable to initialize graphics!\n"); 
 }
}

void I_ShutdownGraphics(void) {
  SW_ShutdownGraphics();
}

void I_StartTic(void) {
  SW_StartTic();
}

void I_SetPalette(byte *palette) {
  SW_SetPalette(palette);
}

void I_StartFrame(void) {
  SW_StartFrame();
}

void I_UpdateNoBlit(void) {
  SW_UpdateNoBlit();
}

void I_FinishUpdate(void) {
  SW_FinishUpdate();
}

