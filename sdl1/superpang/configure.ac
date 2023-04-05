AC_INIT(ceferino, 0.97.8)
AM_INIT_AUTOMAKE
AC_PREREQ([2.50])
AM_CONFIG_HEADER(config.h)

AC_PROG_CXX


dnl gettext, soporte para idiomas
AM_GNU_GETTEXT
AC_SUBST(localedir)
ALL_LINGUAS="es fr ca"

GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
dnl AC_DEFINE([GETTEXT_PACKAGE],[],[...])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["${GETTEXT_PACKAGE}"], [gettext domain])

dnl localedir="${datadir}/locale"







dnl soporte de internacionalización
dnl ALL_LINGUAS = "es"
dnl AM_GNU_GETTEXT
dnl GETTEXT_PACKAGE=ceferino
dnl AC_SUBST (GETTEXT_PACKAGE)
dnl AC_DEFINE_UNQUOTED (GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [The gettext package])



dnl Busca las bibliotecas SDL
AC_CHECK_LIB(SDL, SDL_Init, , \
	AC_MSG_ERROR([* Can't find SDL library: http://www.libsdl.org]))

AC_CHECK_LIB(SDL_image, IMG_Load, , \
	AC_MSG_ERROR([* Can't find SDL_image library: http://www.libsdl.org]))

AC_CHECK_LIB(SDL_mixer, Mix_OpenAudio, , \
	AC_MSG_ERROR([* Can't find SDL_mixer library: http://www.libsdl.org]))


AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AC_OUTPUT([
	Makefile
	src/Makefile
	data/Makefile
	po/Makefile.in
	intl/Makefile
	data/ima/Makefile
	data/levels/Makefile
	data/music/Makefile
	data/sounds/Makefile
])
