#include <stdint.h>

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
    uint32_t *p = pixels;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            p[(y+row) * texture_width + (x+col)] = color;
        }
    }
}