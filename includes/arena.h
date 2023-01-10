#pragma once

#include "commons.h"
#include <string.h>
#include <malloc.h>


//Named arena_alloc so that variables of this struct can be called `arena`
struct arena_alloc{
    ///The size allocated for the arena
    u32 size;
    ///The currently used capacity of the arena.
    u32 capacity;
    ///The first chunk of data allocated to the arena
    void* first;
    ///The next chunk of data allocated to the arena
    void* next;
};
typedef struct arena_alloc arena_alloc;

/*
    Initializes a new arena allocator with the given size

    |-----------------------------------|---------------------------------------|
    |        Header (32 + 8 + 8)        |                                       |
    |-----------------------------------|            Payload (size)             |
    | size (32) | first (8) | next (8)  |                                       |
    |-----------------------------------|---------------------------------------|
*/

arena_alloc* arena_init(u32 size){
    //Allocate an arena_alloc struct on heap with given size
    //This will allows be a borrowed data object
    u32 size_alloc = size + sizeof(arena_alloc);
    // printf("size_alloc: %i\n", size_alloc);
    // printf("arena size: %zu\n", sizeof(arena_alloc));
    ///Allocate a new arena on the heap with the given `size_alloc`
    ///MEM: Borrowed-always
    ///LIFETIME: Borrowed by anything, after its returned to the caller, that needs to put data into the allocator until it's passed into arena_deinit.
    arena_alloc* arena = (arena_alloc*)malloc(size_alloc);
    // printf("Arena addr: %p\n", arena);
    arena->first = ((char*)arena)+sizeof(arena_alloc);
    // printf("Arena->first addr: %p\n", arena->first);
    arena->next = arena->first;
    arena->size = size;
    arena->capacity = 0;
    return arena;
}

///Deinitializes the given arena_alloc by passing it into free
///arena:
/// MEM: Borrowed
/// LIFETIME: Borrowed by free and then discarded by the system.
/// NOTE: After calling this function, NEVER attempt to use this pointer to read/write memory. It will result in a use-after-free.
void arena_deinit(arena_alloc* arena){
    arena->first = 0;
    arena->next = 0;
    free(arena);
}

void* arena_put(arena_alloc* arena, void* data, u32 size){
    if(arena->capacity + size > arena->size){
        printf("Exceeded allocator size! Cannot put data into arena!\n");
        return NULL;
    }
    //Check if the first and next pointers are the same, if so, this is a fresh arena
    if(arena->first == arena->next){
        //Copy the memory from data into arena->first
        memcpy(arena->first, data, size);
    }else{
        //Copy the memory from data into arena-next
        memcpy(arena->next, data, size);
    }
    ///Borrow the pointer to arena->next, which should be called `next`, to be used for initializing the pointer to the newly copied data in the arena
    ///MEM: Borrowed-always
    ///LIFETIME: The data copied into this address is persistent as long as the arena remains alive. This data's lifetime depends on the arena's.
    void* ret = arena->next;
    // printf("Current addr copied to %p\n", (u8*)arena->next);
    // printf("Next addr predicted: %p\n", ((u8*)arena->next) + size);
    arena->next = (void*)(((u8*)ret) + size);
    arena->capacity += size;
    return ret;
}

///This will reserve memory and return a pointer to the reserved memory.
///This is for reading in files effectively without undefined behavior or weird glitches
///~alex, 12:01 AM PST, 11/22/2020
void* arena_reserve(arena_alloc* arena, u32 size){
    if(arena->capacity + size > arena->size){
        printf("Cannot reserve %i bytes of space as there is not enough room in arena!\n", size);
        return NULL;
    }
    ///Borrow the pointer to arena->next, which should be called `next`, to be used for initializing the pointer to the newly copied data in the arena
    ///MEM: Borrowed-always
    ///LIFETIME: The data copied into this address is persistent as long as the arena remains alive. This data's lifetime depends on the arena's.
    void* ret = arena->next;
    if(arena->first == arena->next){
        ///Borrow the pointer to arena->next, which should be called `next`, to be used for initializing the pointer to the newly copied data in the arena
        ///MEM: Borrowed-always
        ///LIFETIME: The data copied into this address is persistent as long as the arena remains alive. This data's lifetime depends on the arena's.
        void* ret = arena->first;    
    }
    // printf("Current addr copied to %p\n", (u8*)arena->next);
    // printf("Next addr predicted: %p\n", ((u8*)arena->next) + size);
    arena->next = (void*)(((u8*)ret) + size);
    arena->capacity += size;
    return ret;
}