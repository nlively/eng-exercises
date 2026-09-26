#include <SDL2/SDL.h>
#include <stdio.h>

int main(void)
{
    SDL_version version;
    SDL_GetVersion(&version);
    printf("SDL 2 runtime: %u.%u.%u\n", version.major, version.minor, version.patch);
    return 0;
}
