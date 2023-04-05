#ifndef _INTL_H
#define _INTL_H

#ifndef WIN32
	#include <locale.h>
	#include <libintl.h>

	#define _(x) gettext(x)
	#define N_(x) (x)
#else
	#define _(x) (x)
	#define N_(x) (x)
#endif
#endif
