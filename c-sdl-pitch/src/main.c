#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#define SDL_PITCH 1344 // actual bytes per row

/**

TASKS:

1. fix the function so that it respects `pitch`
2. clip the rectangle to the texture
3. return without writing anything for empty or entirely off-screen rectangles
4. calculate the byte offset of pixel `(319, 199)` when `pitch == 1344`
5. explain why `AddressSanitizer` might not detect the original pitch bug

timebox: 15m

*hint*
treat `pixels` as bytes when advancing between rows. 
convert each individual row to `uint32_t *` only after applying
the byte-based pitch.

notes:
- each pixel is 4 bytes, so a 320-wide image has 1280 bytes per row, presumably.
- however, SDL can add padding after each row for alignment or hardware efficiency.
- so if SDL reports that `pich = 1344`, then we know each row has an extra 64 bytes of padding.

think of `pitch` as the number of bytes from the beginning of one row to
the beginning of the next row.

*/

void fill_rect(
    void *pixels,
    int pitch,
    int texture_width,
    int texture_height,
    int x,
    int y,
    int width,
    int height,
    uint32_t color)
{
    // return without writing anything for empty or entirely off-screen rectangles
    if (pixels == NULL ||
        pitch <= 0 ||
        texture_width <= 0 ||
        texture_height <= 0 ||
        width <= 0 ||
        height <= 0) {
        return;
    }

    // use 64bit arithmetic so x+width and y+height cannot
    // overflow signed int
    int64_t left = x;
    int64_t top = y;
    int64_t right = (int64_t)x + width;
    int64_t bottom = (int64_t)y + height;

    // clip to the bounds of the texture (assuming 0,0 as top left of texture)
    if (left < 0) left = 0;
    if (top < 0) top = 0;
    if (right > texture_width) right = texture_width;
    if (bottom > texture_height) bottom = texture_height;

    if (left >= right || top >= bottom) return;

    uint8_t *base = pixels;

    for (int64_t row = top; row < bottom; row++) {
        uint32_t *dst = (uint32_t *)(base+row * (int64_t)pitch);

        for (int64_t col = left; col < right; col++) {
            dst[col] = color;

            if (row == bottom - 1 && col == right - 1) {
                ptrdiff_t offset = (uint8_t *)&dst[col] - (uint8_t *)pixels;
                printf("offset for coordinates (%lld x %lld) is %td\n", (long long)col, (long long)row, offset);
            }
        }
    }
}

int main() {
    enum { W = 320, H = 200, PITCH = SDL_PITCH };
    _Alignas(uint32_t) static uint8_t pixels[PITCH * H];
    fill_rect(pixels, PITCH, W, H, 10, 10, 320, 200, 0xFF0000FF);
    return 0;
}

/*

Why won't ASan (AddressSanitizer) catch the original bug?

the original bug caused pixel writes based on simple row+col+sizeof(pixel)
math, without factoring in SDL row-level byte padding.

so a write in the original bug would end up _within_ the allocated memory,
even though it was still in correct.

ASan would only detect writes _outside_ the allocated memory.

*/