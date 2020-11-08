#include "arena.h"
#include "lazy_arena.h"
#include <malloc.h>

arena_alloc* arena_init(u32 size){
    //Allocate an arena_alloc struct on heap with given size
    //This will allows be a borrowed data object
    u32 size_alloc = size + sizeof(arena_alloc);
    // printf("size_alloc: %i\n", size_alloc);
    // printf("arena size: %i\n", sizeof(arena_alloc));
    arena_alloc* arena = (arena_alloc*)malloc(size_alloc);
    // printf("Arena addr: %p\n", arena);
    arena->first = ((char*)arena)+sizeof(arena_alloc);
    // printf("Arena->first addr: %p\n", arena->first);
    arena->last = arena->first;
    arena->size = size;
    return arena;
}

void arena_deinit(arena_alloc* arena){
    arena->first = 0;
    arena->last = 0;
    free(arena);
}

void* arena_put(arena_alloc* arena, void* data, u32 size){
    //Check if the first and last pointers are the same, if so, this is a fresh arena
    if(arena->first == arena->last){
        //Copy the memory from data into arena->first
        memcpy(arena->first, data, size);
    }else{
        //Copy the memory from data into arena-last
        memcpy(arena->last, data, size);
    }
    void* ret = arena->last;
    // printf("Current addr copied to %p\n", (u8*)arena->last);
    // printf("Next addr predicted: %p\n", ((u8*)arena->last) + size);
    arena->last = (void*)(((u8*)ret) + size);
    return ret;
}

lazy_arena_alloc* lazy_arena_init(u32 size){
    lazy_arena_alloc* arena = (lazy_arena_alloc*)malloc(sizeof(lazy_arena_alloc) + size);
    arena->size = size;
    arena->start = arena + sizeof(lazy_arena_alloc);
    return arena;
}

void* lazy_arena_put(lazy_arena_alloc* arena, u32 offset, void* data, u32 size){
    if(offset > arena->size){
        printf("Expected an offset within size %i but instead got %i", arena->size, offset);
        return NULL;
    }
    if(size > arena->size){
        printf("Expected data size within size %i but instead got %i", arena->size, size);
        return NULL;
    }
    u8* dest = ((u8*)arena->start) + offset;
    // printf("Putting data into addr %p\n", dest);
    memcpy(dest, data, size);
    return (void*)dest;
}

void* lazy_arena_get(lazy_arena_alloc* arena, u32 offset){
    if(offset > arena->size){
        printf("Expected an offset within size %i but instead got %i", arena->size, offset);
        return NULL;
    }
    u8* dest = ((u8*)arena->start) + offset;
    return (void*)dest;
}

void lazy_arena_deinit(lazy_arena_alloc* arena){
    arena->start = 0;
    free(arena);
}