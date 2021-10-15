#include "h2def.h"

static char kname[10];

char *MN_GetKeyName(int key) {
 switch(key) {
    case 0: return "???";
    case KEY_RIGHTARROW: return "RIGHT";
    case KEY_LEFTARROW: return "LEFT";
    case KEY_UPARROW:	return "UP";
    case KEY_DOWNARROW: return "DOWN";
    case KEY_ESCAPE: return "ESC";
    case KEY_ENTER: return "ENTER";
    case KEY_TAB: return "TAB";
    case KEY_F1: return "F1";
    case KEY_F2: return "F2";
    case KEY_F3: return "F3";
    case KEY_F4: return "F4";
    case KEY_F5: return "F5";
    case KEY_F6: return "F6";
    case KEY_F7: return "F7";
    case KEY_F8: return "F8";
    case KEY_F9: return "F9";
    case KEY_F10: return "F10";
    case KEY_F11: return "F11";
    case KEY_F12: return "F12";
    case KEY_BACKSPACE: return "BACKSPACE";
    case KEY_PAUSE: return "PAUSE";
    case KEY_EQUALS: return "=";
    case KEY_MINUS: return "-";
    case KEY_RSHIFT: return "SHIFT";
    case KEY_RCTRL: return "CTRL";
    case KEY_RALT: return "ALT";
    case ' ': return "SPACEBAR";
    default:
     if (key>='0' && key<='9') {
      kname[0]=key;
      kname[1]=0;
      return kname;
     }
     else
     if (key>='a' && key<='z') {
      kname[0]=key-('a'-'A');
      kname[1]=0;
      return kname;
     }
     else
     if (key>='A' && key<='Z') {
      kname[0]=key;
      kname[1]=0;
      return kname;
     }
     else {
      sprintf(kname,"CODE %d",key&0xFF);
      return kname;
     }
 }
}
