# C + SDL 2/3 development container

This container includes GCC, Clang, Make, CMake, Meson, Ninja, GDB,
`pkg-config`, and the development headers and libraries for both SDL 2 and
SDL 3.

Build the image and compile both included smoke tests:

```sh
docker compose build
docker compose run --rm dev make
```

Run the command-line smoke tests:

```sh
docker compose run --rm dev make run-sdl2
docker compose run --rm dev make run-sdl3
```

Open an interactive development shell:

```sh
docker compose run --rm dev
```

The project directory is mounted at `/workspace`, so files created on the
host or in the container remain in sync. Use `pkg-config --cflags --libs sdl2`
for SDL 2 and `pkg-config --cflags --libs sdl3` for SDL 3 in additional build
targets.

Graphical programs need access to the host display and audio devices. The
exact Docker options for that are host-platform-specific; the default setup
is deliberately portable and is ready for compilation and non-graphical
tests.
