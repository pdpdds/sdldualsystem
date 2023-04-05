INCLUDES = $(DEPS_CFLAGS) -DLOCALEDIR=\""$(datadir)/locale"\" \
	-DDATADIR=\""$(pkgdatadir)"\"

bin_PROGRAMS = ceferino ceferinosetup ceferinoeditor

ceferino_SOURCES = actor.cc audio.cc barra.cc bloque.cc bomba.cc comojugar.cc\
creditos.cc final.cc fuente.cc gaucho.cc grafico.cc intro.cc item.cc \
item_menu.cc juego.cc libgrafico.cc main.cc marcas.cc menu.cc mundo.cc \
nivel.cc pelota.cc procesos.cc simple_sprite.cc tiro.cc utils.cc fuente2.cc \
opciones.cc

ceferinoeditor_SOURCES = main_ed.cc grafico.cc utils.cc
ceferinosetup_SOURCES = main_op.cc grafico.cc utils.cc

EXTRA_DIST = actor.h audio.h barra.h bloque.h bomba.h ceferino-i18n.h \
comojugar.h creditos.h escena.h final.h fuente.h gaucho.h grafico.h int.h\
intro.h item.h item_menu.h juego.h libgrafico.h marcas.h menu.h mundo.h \
nivel.h pelota.h procesos.h simple_sprite.h tiro.h utils.h fuente2.h\
opciones.h main_ed.h main_op.h
