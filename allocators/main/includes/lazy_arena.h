#pragma once

#include "commons.h"

///A lazy arena is an arena that does not care about where you allocate things.
///It only cares about whether you're allocating within a given size
///You allocate to it via an offset, data, and size
struct lazy_arena_alloc{
    u32 size; //The size of the arena that's allocated
    void* start; //Convenience field. This is so we can just jump right to the start of the arena
};
typedef struct lazy_arena_alloc lazy_arena_alloc;

//This will initialize and return a pointer to a new lazy arena allocator
lazy_arena_alloc* lazy_arena_init(u32 size);

///This is a procedure that allows you to allocate whatever kind of data you wish to allocate
///It only takes an offset from the start of the arena within the arena size
///Returns 1 if offset is greater than arena size or if given size is greater than arena size
void* lazy_arena_put(lazy_arena_alloc* arena, u32 offset, void* data, u32 size);
void* lazy_arena_get(lazy_arena_alloc* arena, u32 offset);
void lazy_arena_deinit(lazy_arena_alloc* arena);