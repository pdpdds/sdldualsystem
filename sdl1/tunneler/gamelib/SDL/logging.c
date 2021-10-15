#include <stdio.h>
#include <stdarg.h>
#include "require_sdl.h"

void     gamelib_print (char *str, ...) {
	va_list l;
	va_start(l, str);
	vfprintf(stdout, str, l) ;
	va_end(l);
}

void     gamelib_debug (char *str, ...) {
	va_list l;
	va_start(l, str);
	vfprintf(stderr, str, l) ;
	va_end(l);
}

void     gamelib_error (char *str, ...) {
	va_list l;
	va_start(l, str);
	vfprintf(stderr, str, l) ;
	va_end(l);
}

