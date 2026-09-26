#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct {
    unsigned char *data;
    size_t len;
    size_t cap;
} Arena;

int arena_init(Arena *arena, size_t capacity) {
    arena->data = malloc(capacity);

    if (arena->data == NULL) {
        arena->len = 0;
        arena->cap = 0;
        return 0;
    }

    arena->len = 0;
    arena->cap = capacity;
    return 1;
}

void *arena_alloc(Arena *arena, size_t size, size_t alignment) {
    if (alignment == 0 ||
        alignment & (alignment - 1) != 0 ||
        alignment > _Alignof(max_align_t)) {
            return NULL;
        }
    
    if (arena->len > SIZE_MAX - (alignment - 1))
        return NULL;

    size_t start =
        (arena->len + alignment - 1) & ~(alignment - 1);

    if (start > arena->cap || size > arena->cap - start)
        return NULL;

    arena->len = start + size;
    return arena->data + start;
}

void arena_destroy(Arena *arena) {
    free(arena->data);
    *arena = (Arena){0};
}

// void *arena_alloc(Arena *arena, size_t size, size_t alignment) {
//     // here, `size` should be 17, i think.  1 byte char + 2 8-byte integers.

//     size_t alignment_mask = ~(alignment - 1);
//     // starting position in memory
//     size_t start = (
//             arena->len          //   0 - starts at 0 for our given struct
//             + alignment         // + 4 - address has to be multiple of `alignment` bytes. in our case, 4 (32bits)
//             -1                  // - 1 - accommodate zero index?
//         )   // 3 
//         & alignment_mask;       // a trick to round down to a multiple of `alignment`; 
//     // start is 0
//     size_t end = start + size;  // ending position just extends `size` form starting point
//     // end is 17 bytes because `size_t` is 8 bytes, i think?

//     /*
//     ISSUES FOUND
    
//     1. we hard code first arena cap at 64 in our second reallocation,
//        should probably respect _Alignof(size_t)
//     2. 
//     */


//     // initially false
//     if (end > arena->cap) {
//         size_t new_cap = arena->cap ? arena->cap * 2 : 64;

//         while (new_cap < end) 
//             new_cap *= 2;

//         arena->data = realloc(arena->data, new_cap);
//         arena->cap = new_cap;
//     }

//     // first pass, arena->len = 17
//     arena->len = end;
//     return arena->data + start; // 0 + 0
// }

int main(void) {
    Arena arena;

    if (!arena_init(&arena, 64 * 1024)) 
        return 1;

    uint32_t *header = arena_alloc(&arena, sizeof(*header), _Alignof(uint32_t));

    if (header == NULL) {
        arena_destroy(&arena);
        return 1;
    }

    *header = 0x574c4631;

    // all successful allocations remain inside hte original block

    arena_destroy(&arena);
    /*
    // initialize empty Arena.
    // note: the 0 here explicitly fills `data`, and then
    // `len` and `cap` are implicitly 0
    Arena arena = {0};

    // allocate 32 bits, aligned to uint32
    // arena_alloc gives us a void pointer to memory, 
    // `arena.data` plus the starting offset
    uint32_t *header = arena_alloc(&arena, sizeof(*header), _Alignof(uint32_t));

    // put 4 bytes (32 bits) into the header
    // fill bytes 0-4 of the allocated space with the number
    *header = 0x574c4631;

    for (int i = 0; i < 1000; i++)
        arena_alloc(&arena, 32, 8);

    printf("header = %07x\n", *header);

    // we allocated `header` but we're freeing arena.data only (first byte of struct,
    // whereas `header` is first 4 bytes of struct)
    free(arena.data);
    */
}