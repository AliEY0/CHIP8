chip8: main.c chip8.c chip8.h
	gcc main.c chip8.c -o chip8 $(shell pkg-config --cflags --libs sdl2)

clean:
	rm -f chip8

