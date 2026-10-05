pong: pong.o
	gcc -o pong.exe pong.o -I include/ -L lib/ -lraylib -lgdi32 -lwinmm

pong.o: pong.c entities.h
	gcc -Wall -W -O -std=c99 -pedantic -c pong.c
