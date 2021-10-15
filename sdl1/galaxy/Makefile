LIBS = -lSDL -lSDL_image -lSDL_mixer -lSDL_ttf
SRCS += \
BTree \
Button \
Fleet \
Game \
Globals \
Graphics \
HeroShip \
Invader \
Main \
Menu \
Object \
SDLTimer \
Scene \
Sounds \
Top10

all: clean build galaxy

build:
	for file in $(SRCS); do \
		echo 'Building file:' $$file.cpp ;\
		echo 'Invoking: GCC C++ Compiler' ;\
		g++ -O3 -Wall -c -o bin/build/$$file.o src/$$file.cpp ;\
		echo 'Finished building:' $$file.o ;\
		echo ' ' ;\
	done

galaxy:
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o bin/galaxy `find bin/build/ -type f -name *.o` $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

clean:
	rm -f bin/build/*
	rm -f bin/galaxy
