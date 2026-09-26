#include <SDL3/SDL.h>
#include <stdio.h>

int main(void)
{
    int version = SDL_GetVersion();
    printf("SDL 3 runtime: %d.%d.%d\n",
           SDL_VERSIONNUM_MAJOR(version),
           SDL_VERSIONNUM_MINOR(version),
           SDL_VERSIONNUM_MICRO(version));
    return 0;
}
