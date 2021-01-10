#pragma once

#include "commons.h"
#include <malloc.h>

///A lazy arena is an arena that does not care about where you allocate things.
///It only cares about whether you're allocating within a given size
///You allocate to it via an offset, data, and size
struct lazy_arena_alloc{
    u32 size; //The size of the arena that's allocated
    void* start; //Convenience field. This is so we can just jump right to the start of the arena
};
typedef struct lazy_arena_alloc lazy_arena_alloc;

//This will initialize and return a pointer to a new lazy arena allocator
lazy_arena_alloc* lazy_arena_init(u32 size){
    ///Allocates a new arena with the given size onto the heap via malloc.
    ///MEM: Borrowed-always
    ///LIFETIME: This will always be borrowed until either the pointer is somehow lost or until it's passed into lazy_arena_deinit
    lazy_arena_alloc* arena = (lazy_arena_alloc*)malloc(sizeof(lazy_arena_alloc) + size);
    arena->size = size;
    arena->start = arena + sizeof(lazy_arena_alloc);
    return arena;
}

///This is a procedure that allows you to allocate whatever kind of data you wish to allocate
///It only takes an offset from the start of the arena within the arena size
///Returns 1 if offset is greater than arena size or if given size is greater than arena size
void* lazy_arena_put(lazy_arena_alloc* arena, u32 offset, void* data, u32 size){
    ///Check that the offset is within the given size to ensure that we put data inside the allocated region of memory
    if(offset > arena->size){
        printf("Expected an offset within size %i but instead got %i", arena->size, offset);
        return NULL;
    }
    ///Check that the size of data being put into the arena is within the given arena size so ensure we dont overflow the arena
    ///We also check that the size plus the offset dont overflow the arena
    if(size > arena->size && size + offset < arena->size){
        printf("Expected data size within size %i but instead got %i with offset %i", arena->size, size, offset);
        return NULL;
    }
    ///Get the new destination as a u8* and use it to copy `data` into the address of the offset
    ///It's casted to a u8* because void* has no known size, so offsetting it to get the address to put the data in will not work.
    ///u8* makes it so that we can offset with byte sized strides, which gives us exactly what we need.
    ///MEM: Borrowed
    ///LIFETIME: Borrowed by memcpy, Borrowed by return/caller
    u8* dest = ((u8*)arena->start) + offset;
    printf("Putting data into addr %p\n", dest);
    memcpy(dest, data, size);
    return (void*)dest;
}

///Deinitialize the lazy_arena by pass it into free
///arena: 
/// MEM: Borrowed
/// LIFETIME: Borrowed by free and then discarded by function
/// NOTE: After calling this function, NEVER attempt to use this pointer
void lazy_arena_deinit(lazy_arena_alloc* arena){
    arena->start = 0;
    free(arena);
}