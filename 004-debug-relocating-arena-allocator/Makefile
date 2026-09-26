CC ?= cc
CFLAGS ?= -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer

.PHONY: all clean run-main

all: build/main

build:
	mkdir -p $@

build/main: src/main.c | build
	$(CC) $(CFLAGS) $< -o $@

run-main: build/main
	./$<

clean:
	rm -rf build
