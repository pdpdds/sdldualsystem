
#
# Tennix! SDL Port
# Copyright (C) 2003, 2007, 2008, 2009 Thomas Perl <thp@thpinfo.com>
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
# MA  02110-1301, USA.
#

CC = g++


ifeq ($(MKCALLGRAPH),1)
  CC        =  nccgen -ncgcc -ncld -ncfabs
  LD        =  nccld
endif

RELEASE   =  1.1

UNAME = $(shell uname)

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
DATAROOTDIR ?= $(PREFIX)/share
DATADIR ?= $(DATAROOTDIR)/games

LIBS =
CFLAGS += -W -Wall -ansi -pedantic -Wcast-qual -Wwrite-strings -DVERSION=\"$(RELEASE)\" -O2 -DPREFIX=\"$(PREFIX)\" -g
CXXFLAGS += $(CFLAGS)

USE_PYTHON ?= 1

ifeq ($(USE_PYTHON),1)
  CFLAGS += `python-config --includes` -DTENNIX_PYTHON
  LIBS += `python-config --libs`
endif

ifeq ($(NONFREE_LOCATIONS),1)
  CFLAGS += -DNONFREE_LOCATIONS
endif

ifeq ($(UPDRECTANGLE),1)
  CFLAGS += -DDRAW_UPDATE_RECTANGLE
endif

ifeq ($(DELUXE),1)
  CFLAGS += -DDELUXE_EDITION
endif

ifeq ($(DEBUG),1)
  CFLAGS += -DDEBUG
endif

ifeq ($(MAEMO),1)
  CFLAGS += -DMAEMO
endif

ifeq ($(UNAME),Darwin)
  SDLLIBS=$$(sdl-config --prefix)/lib
  LIBS += $$(sdl-config --static-libs) $(SDLLIBS)/libSDL_mixer.a $(SDLLIBS)/libSDL_image.a $(SDLLIBS)/libSDL_ttf.a $(SDLLIBS)/libSDL_net.a $$(freetype-config --prefix)/lib/libfreetype.a
  CFLAGS += $$(sdl-config --cflags) -lz
else
  LIBS += $$(sdl-config --libs) -lSDL_mixer -lSDL_image -lSDL_ttf -lSDL_net
  CFLAGS += $$(sdl-config --cflags)
endif

SRC = tennix.cc game.c graphics.cc input.c util.c sound.cc animation.c network.c
OBJ = tennix.o game.o graphics.o input.o util.o sound.o animation.o archive.o SDL_rotozoom.o network.o
ifeq ($(MSYSTEM),MINGW32)
  OBJ += tennixres.o
endif
ifeq ($(USE_PYTHON),1)
  OBJ += tennixpy.o
endif

TOARCHIVE=$(wildcard data/*.ogg data/*.ttf wildcard data/*.png voice/*.ogg *.py)

WIN32LIBS = *.dll
OSXAPP = Tennix.app

DATAFILES = README README.*
DATAFILES_OSX = $(DATAFILES) data/Tennix.icns

tennix: $(OBJ) tennix.tnx
	$(CC) $(CFLAGS)   -o tennix $(OBJ) $(LIBS)
	test -f tennix.exe && upx tennix.exe || true 

ChangeLog:
	git log >ChangeLog

install: tennix
	install -d -m 755 $(DESTDIR)$(BINDIR)
	install -d -m 755 $(DESTDIR)$(DATAROOTDIR)/pixmaps
	install -d -m 755 $(DESTDIR)$(DATAROOTDIR)/applications
	install -d -m 755 $(DESTDIR)$(DATAROOTDIR)/man/man6
	install -d -m 755 $(DESTDIR)$(DATAROOTDIR)/icons/hicolor/scalable/apps
	install -d -m 755 $(DESTDIR)$(DATADIR)/tennix
	install -m 755 tennix $(DESTDIR)$(BINDIR)/
	install -m 644 tennix.6 $(DESTDIR)$(DATAROOTDIR)/man/man6/
	install -m 644 data/icon.png $(DESTDIR)$(DATAROOTDIR)/pixmaps/tennix.png
	install -m 644 data/icon.svg $(DESTDIR)$(DATAROOTDIR)/icons/hicolor/scalable/apps/tennix.svg
	install -m 644 tennix.desktop $(DESTDIR)$(DATAROOTDIR)/applications/
	install -m 644 tennix.tnx $(DESTDIR)$(DATADIR)/tennix/

archive.o: archive.cc archive.cc
tennix.o: tennix.cc tennix.h game.h graphics.h input.h util.h animation.h sound.h locations.h util.h archive.hh
graphics.o: graphics.cc graphics.h tennix.h archive.hh sound.h
game.o: game.c game.h graphics.h tennix.h sound.h input.h util.h network.h
sound.o: sound.cc sound.h tennix.h archive.hh graphics.h
input.o: input.c input.h tennix.h graphics.h game.h util.h tennixpy.h archive.hh
util.o: util.c util.h tennix.h
animation.o: animation.c animation.h graphics.h tennix.h credits.h
tennixpy.o: tennixpy.cc tennix.h game.h archive.hh
network.o: network.c network.h game.h
SDL_rotozoom.o: SDL_rotozoom.c SDL_rotozoom.h

tennixar: archive dump

archive: archivetool.cc archive.o archive.hh

dump: archive
	ln -s archive dump

tennix.tnx: archive $(TOARCHIVE)
	rm -f tennix.tnx
	./archive $@ $(TOARCHIVE)

# Mac OS X-specific targets
release-osx: tennix tennix.tnx ChangeLog
	mkdir -p $(OSXAPP)/Contents/{MacOS,/Resources}
	cp -rpv tennix $(OSXAPP)/Contents/MacOS/Tennix
	cp -rpv tennix.tnx $(DATAFILES_OSX) ChangeLog $(OSXAPP)/Contents/Resources/
	sed -e 's/TENNIX_VERSION/$(RELEASE)/' osxapp.plist >$(OSXAPP)/Contents/Info.plist
	echo 'APPL????' >$(OSXAPP)/Contents/PkgInfo
	zip -r tennix-$(RELEASE)-macosx.zip $(OSXAPP)
# End Mac OS X-specific targets

# Windows-specific targets
release-win32: tennix ChangeLog
	zip tennix-$(RELEASE)-win32.zip tennix.exe $(WIN32LIBS) $(DATAFILES) ChangeLog

tennix-installer.iss: tennix-installer.iss.in
	sed tennix-installer.iss.in -e 's/{version}/$(RELEASE)/g' >tennix-installer.iss

release-win32-setup: tennix ChangeLog tennix-installer.iss
	iscc tennix-installer.iss

tennixres.o: tennix.res
	windres tennix.res tennixres.o
# End Windows-specific targets

release-bin: tennix ChangeLog
	tar czvf tennix-$(RELEASE)-bin.tar.gz tennix $(DATAFILES) ChangeLog

release: distclean ChangeLog
	mkdir -p .release-tmp/tennix-$(RELEASE)/
	cp -rv * .release-tmp/tennix-$(RELEASE)/
	rm -rf .release-tmp/tennix-$(RELEASE)/.git
	tar czvf tennix-$(RELEASE).tar.gz -C .release-tmp tennix-$(RELEASE)
	rm -rf .release-tmp

clean:
	rm -f *.o tennix tennix.exe archive archive.exe dump dump.exe
	rm -rf $(OSXAPP) tennix-*-macosx.zip
	rm -f tennixres.o tennix-installer.iss tennix-*-win32-setup.exe
	rm -f tennix.tnx

distclean: clean
	rm -rf tennix-$(RELEASE).zip tennix-$(RELEASE)-bin.tar.gz ChangeLog .release-tmp tennix-$(RELEASE).tar.gz

.PHONY: distclean clean release-bin release-win32 release-win32-setup release-osx install tennixar

