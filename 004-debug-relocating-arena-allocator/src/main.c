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
    /* 
    failure modes:
    1. alignemnt is zero
    2. alignment is not a power of 2
    3. alignment higher than allowed limit
    */
    if (alignment == 0 ||                       // alignment is zero (violation)
        alignment & (alignment - 1) != 0 ||     // alignment is not a power of 2
        alignment > _Alignof(max_align_t)) {
            return NULL;
        }
    
    // additional failure mode:
    // integer overflow check: 
    // arena length would exceed highest number allowed by size_t
    // note: we don't express as `(arena->len + (alignment - 1) > SIZE_MAX)`
    // because doing the addition might already overflow the bound.
    // then we might end up with a false positive on the check
    if (arena->len > SIZE_MAX - (alignment - 1))
        return NULL;

    // let's say arena->len is 0x3fab11, i.e. (4,172,561)
    // and alignment is 0x4 (i.e., 4)
    // ~(alignment-1) would be 0xfffffffc
    // start = 0x3fab14 & 0xfffffffc
    // translated to binary
    //   0011 1111 1010 1011 0001 0100 
    // & 1111 1111 1111 1111 1111 1100
    // = 0011 1111 1010 1011 0001 0100
    // back to hex:
    // = 0x3fab14

    // ensure `start` is a multiple of the alignment (4)
    // rounding it up as needed
    // note that `start` is measured in bytes (i.e., a byte offset), not bits
    size_t start =
        (arena->len + alignment - 1) & ~(alignment - 1);

    // ensure our requested allocation doesn't start or end outside of
    // our upper bound, as specified when arena was initialized
    if (start > arena->cap || size > arena->cap - start)
        return NULL;

    // adjust arena's total length to match our aligned start offset
    // plus the requested size
    arena->len = start + size;

    // data is a pointer to the existing data in the allocated area.
    // return a new offset pointer
    return arena->data + start;
}

void arena_destroy(Arena *arena) {
    free(arena->data);
    *arena = (Arena){0};
}
/*

1111 1111
1111 1110

*/

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