#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <memalloc.h>

#include <android/log.h>

#include "require_android.h"

#define  LOG_TAG  "TunnelTanksNative"

/* Return a mem-alloced copy of the string, with all surrounding spaces removed.
 * Returns NULL if that string is empty: */
static char *strip_string_copy(char *str) {
	unsigned len = strlen(str);
	char *end, *out;
	
	/* Eat from the front: */
	while(*str && isspace((unsigned char)*str)) {
		str++;
		len--;
	}
	
	/* Hit the end? */
	if(*str == '\0') return NULL;
	
	/* Fast-forward end to the end: */
	end = str;
	while(*end) end++;
	
	/* Eat from the back: */
	while(isspace((unsigned char)*(--end))) len--;
	
	/* Get the new string, and copy the stuff in: */
	out = get_mem(sizeof(char)*(len+1));
	strncpy(out, str, len);
	out[len] = '\0';
	
	return out;
}

void gamelib_print(char *str, ...) {
	char *stripped;
	
	va_list l;
	va_start(l, str);
	
	stripped = strip_string_copy(str);
	if(stripped) __android_log_vprint(ANDROID_LOG_INFO, LOG_TAG, stripped, l) ;
	free_mem(stripped);
	
	va_end(l);
}

void gamelib_debug(char *str, ...) {
	char *stripped;
	
	va_list l;
	va_start(l, str);
	
	stripped = strip_string_copy(str);
	if(stripped) __android_log_vprint(ANDROID_LOG_DEBUG, LOG_TAG, stripped, l) ;
	free_mem(stripped);
	
	va_end(l);
}

void gamelib_error(char *str, ...) {
	char *stripped;
	
	va_list l;
	va_start(l, str);
	
	stripped = strip_string_copy(str);
	if(stripped) __android_log_vprint(ANDROID_LOG_ERROR, LOG_TAG, stripped, l) ;
	free_mem(stripped);
	
	va_end(l);
}

