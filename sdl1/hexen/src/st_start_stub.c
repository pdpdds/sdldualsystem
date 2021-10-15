#include <stdio.h>
#include <stdarg.h>
#include "st_start.h"

void ST_Init(void) {
}
void ST_Done(void) {
}

extern void ST_Message(char *message, ...) {
 va_list v;
 va_start(v,message);
 vprintf(message,v);
 va_end(v);
}

extern void ST_RealMessage(char *message, ...) {
 va_list v;
 va_start(v,message);
 vprintf(message,v);
 va_end(v);
}

extern void ST_Progress(void) {
}

extern void ST_NetProgress(void) {
}

extern void ST_NetDone(void) {
}
