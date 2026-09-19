CC ?= cc
CFLAGS ?= -O2 -g -Wall -Wextra -Wpedantic -fsanitize=address -fno-omit-frame-pointer

.PHONY: all clean run-sdl2 run-sdl3

all: build/main build/sdl2-smoke build/sdl3-smoke

build:
	mkdir -p $@

build/main: src/main.c | build
	$(CC) $(CFLAGS) $< -o $@

build/sdl2-smoke: src/sdl2-smoke.c | build
	$(CC) $(CFLAGS) $(shell pkg-config --cflags sdl2) $< -o $@ $(shell pkg-config --libs sdl2)

build/sdl3-smoke: src/sdl3-smoke.c | build
	$(CC) $(CFLAGS) $(shell pkg-config --cflags sdl3) $< -o $@ $(shell pkg-config --libs sdl3)

run-main: build/main
	./$<

run-sdl2: build/sdl2-smoke
	./$<

run-sdl3: build/sdl3-smoke
	./$<

clean:
	rm -rf build
