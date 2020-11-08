#pragma once

#include "commons.h"

//Named arena_alloc so that variables of this struct can be called `arena`
struct arena_alloc{
    ///The size allocated for the arena
    u32 size;
    ///The first chunk of data allocated to the arena
    void* first;
    ///The last chunk of data allocated to the arena
    void* last;
};
typedef struct arena_alloc arena_alloc;

/*
    Initializes a new arena allocator with the given size

    |-----------------------------------|---------------------------------------|
    |        Header (32 + 8 + 8)        |                                       |
    |-----------------------------------|            Payload (size)             |
    | size (32) | first (8) | last (8)  |                                       |
    |-----------------------------------|---------------------------------------|
*/
arena_alloc* arena_init(u32 size);
void arena_deinit(arena_alloc* arena);

//Puts some data into the arena wherever the arena->last is pointing to by copying from data into arena->last (or arena->first if its fresh)
void* arena_put(arena_alloc* arena, void* data, u32 size);