CFLAGS =	`sdl-config --cflags` -g -Wall -I. -DENABLE_BINRELOC
CPPFLAGS =	$(CFLAGS)

# Uncomment the first LIBS line and comment out the other LIBS line to
# compile without SDL_Mixer
#LIBS =		`sdl-config --libs`
LIBS =		`sdl-config --libs` -lSDL_mixer

RETROINSTALLDIR ?= /opt

RETRO =		retro.o Input.o GameLogic.o Hero.o Fuzz.o Object.o Level.o \
		Coin.o Nest.o ScoreKeeper.o Spawner.o Bullet.o SoundManager.o \
		BrokenBlock.o Explosion.o Smoke.o Enemy.o Counter.o Drone.o \
		Field.o Collectible.o HUD.o Menu.o ScoreScreen.o GameMode.o \
		FileManager.o BulletHandler.o BonusLevel.o BonusScoreScreen.o \
		SpaceViper.o SpaceViperBody.o BountyHunter.o \
		HunterBullet.o DamagingExplosion.o Bonus.o BonusSpawner.o \
		PowerBullet.o LaserBeam.o Helmet.o RedFuzz.o GoldDrone.o \
		DireSpaceViper.o DireSpaceViperBody.o DarkBountyHunter.o \
		DarkHunterBullet.o DarkExplosion.o ScoreSign.o binreloc.o \
		datadir.o \
		remar2d.o tinyxml.o tinystr.o tinyxmlerror.o \
		tinyxmlparser.o Sprite.o Animation.o SpriteInstance.o \
		TileSet.o Tile.o Font.o Character.o SurfaceCache.o


retrobattle:	$(RETRO)
		g++ -o $@ $^ $(LIBS)
		mv $@ ..

clean:
		rm -f *.o *~ core* ../retrobattle

install:	retrobattle
		
		echo Installing in $(RETROINSTALLDIR)
		mkdir -p $(RETROINSTALLDIR)/retrobattle
		cp -rv ../retrobattle ../data $(RETROINSTALLDIR)/retrobattle
