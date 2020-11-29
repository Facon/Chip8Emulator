CC=gcc
DFLAGS=-c -ggdb -Wall
CFLAGS=-c -O3 -Wall
FLAGS=$(DFLAGS)
LIBS=-lSDL
OBJ=main.o machine.o

chip8: machine.h main.c machine.c
	$(CC) $(FLAGS) main.c machine.c
	$(CC) main.o machine.o $(LIBS) -o chip8
	
windows:
	i586-mingw32msvc-g++ $(FLAGS) main.c machine.c machine.h
	i586-mingw32msvc-g++ main.o machine.o $(LIBS) -o chip8.exe

clean:
	rm -f -r *~
	rm -f -r *.o
	
clean_all:
	rm -f -r *~
	rm -f -r *.o
	rm -f -r *~.c
	rm -f -r *~.h
	rm -f -r chip8
