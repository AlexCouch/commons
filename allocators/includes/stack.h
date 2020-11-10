#pragma once

#include "lazy_arena.h"

/*
    A stack allocator, which uses a push-and-pop mechanism to push
    data onto the stack and pop them in a first-in-last-out way.
                                                             Bytes that can be pushed/popped
    |--------------------------|---------------------------|---------------------------------|
    |  arena header (24 bytes) |  stack header (16 bytes)  | | | | | | | | | | | | | | | | | |
    |--------------------------|---------------------------|---------------------------------|

    Any time that stack_push is called with given data and size of data, it will be put onto
    the stop of the data by calling lazy_arena_put to put data in the arena using stack_ptr field.
    The stack_ptr field becomes the offset from the start of the lazy arena to put data.
    
    When stack_pop is called, we just decrement the stack_ptr field which then makes it so that
    next time that stack_push is called, the data is put further down on the stack per the
    stack_ptr field.
*/
///~alex, 3:42 AM PST, 11/10/2020
PUBLIC
EXTENSION(arena)
struct stack_alloc{
    ///A lazy arena to be used for putting data into memory at an offset [stack_ptr]
    lazy_arena_alloc*   arena;
    ///The size of the stack. This will be used to allocate the [arena] plus the size of this header.
    u32                 size;
    ///The current pointer to the top of the stack, which acts as the next offset to put data into the arena.
    u32                 stack_ptr;
};
typedef struct stack_alloc stack_alloc;

///Initialize a new stack allocator with a given lazy_arena_alloc pointer, which will return
///a new stack_alloc* with that lazy_arena_alloc [arena]. The [size] parameters is used for
///declaring a stack max size when pushing and popping.
///TODO: Replace null-check with null-assert
///TODO: Replace printing with debug assert/logging
///~alex, 3:48 AM PST, 11/10/2020
RECEIVER(arena)
stack_alloc* stack_init(lazy_arena_alloc* arena, u32 size){
    if(arena == NULL){
        printf("Expected an initialized lazy_arena_alloc*, but instead got NULL!\n");
        return NULL;
    }
    if(size > arena->size){
        printf("Cannot create stack bigger than lazy arena size: %i > %i", size, arena->size);
        return NULL;
    }
    stack_alloc stack;
    stack.size = size;
    stack.stack_ptr = sizeof(stack_alloc);
    stack.arena = arena;
    stack_alloc* stack_ptr = lazy_arena_put(arena, 0, &stack, sizeof(stack_alloc));
    return stack_ptr;
}
///Initialize a new stack allocator without any preinitialized lazy_arena_alloc, which will create
///its own lazy_arena_alloc to be used, then return a new stack_alloc via that lazy_arena_alloc.
///The size is used to create a new lazy_arena_alloc with the given size, so that the lazy arena
///is sized relative to the stack itself.
///~alex, 3:48 AM PST, 11/10/2020
stack_alloc* stack_init_full(u32 size){
    ///Create a new lazy_arena_alloc. This is used by the stack to do push/pop
    ///MEM: Borrowed-always
    ///LIFETIME: This sticks around until the stack is passed into stack_deinit
    lazy_arena_alloc* lazy_arena = lazy_arena_init(sizeof(stack_alloc) + size);
    ///A new stack object on the stack. This will be passed into lazy_arena_put at offset 0, so that
    ///it can be copied to offset 0.
    ///The [size] field is set to [size] parameter
    ///The [arena] field is set to [lazy_arena]* variable;
    ///The [stack_ptr] field is set to the size of the stack_alloc struct, so that the 
    /// bottom of the stack is right after the stack header.
    ///MEM: Borrowed
    ///LIFETIME: This only sticks around so that it can be copied into the lazy arena 
    ///LIFETIME: then discarded at the end of the current scope
    stack_alloc stack;
    stack.size = size;
    stack.arena = lazy_arena;
    stack.stack_ptr = sizeof(stack_alloc);
    ///A pointer to the copied [stack] variable in the arena at offset 0
    ///MEM: Borrowed-always
    ///LIFETIME: This persists until this same address is given to stack_deinit
    stack_alloc* stack_ptr = lazy_arena_put(lazy_arena, 0, &stack, sizeof(stack_alloc));
    return stack_ptr;
}
///Deinitialize the given stack by using the stack->arena to deallocate the entire stack and arena.
///TODO: Replace null-check with null-assert
///~alex, 3:48 AM PST, 11/10/2020
RECEIVER(stack)
void stack_deinit(stack_alloc* stack){
    ///If this is already freed, or was corrupted and set to NULL somehow,
    ///we need to prevent that crashing our program by becoming a use-after-free/null-ptr-crash
    if(stack == NULL){
        return;
    }
    ///Pass the stored lazy_arena_alloc* [stack->arena] field into lazy_arena_deinit, which then
    ///deinitialized the entire stack in the arena.
    lazy_arena_deinit(stack->arena);
}
///Push [data] of size [size] to the top of the stack.
///This will check if the size is within the given remaining size by 
///calculating stack->size - stack->stack_ptr, and if its within that bounds, it will push,
///otherwise, will return NULL
///TODO: replace weak printing and returning with a debug assert/logging
///TODO: Replace null-check with null-assert
///~alex, 3:48 AM PST, 11/10/2020
RECEIVER(stack)
void* stack_push(stack_alloc* stack, void* data, u32 size){
    ///Check if the stack is NULL. This is not OKAY!!!
    ///~alex, 3:57 AM PST, 11/10/2020
    if(stack == NULL){
        ///TODO: Replace with debug assert/logging
        ///~alex, 3:57 AM PST, 11/10/2020
        printf("Stack cannot be null!\n");
        return NULL;
    }
    ///Get the remaining size on the stack. If we have already pushed up the stack
    ///Then whatever the size of the stack is minus the current stack_ptr,
    ///That is what is currently remaining
    ///~alex, 3:57 AM PST, 11/10/2020
    u32 remaining = stack->size - stack->stack_ptr;
    if(size > remaining){
        ///TODO: Replace with an assertion
        ///~alex, 3:53 AM PST, 11/10/2020
        printf("Cannot push data of size %i as there is not enough left on the stack: %i\n", size, remaining);
        return NULL;
    }
    ///The pointer returned by the lazy arena when we put data at the given offset [stack->stack_ptr]
    ///~alex, 4:00 AM PST, 11/10/2020
    ///MEM: Borrowed-always
    ///LIFETIME: This persists until stack_pop is called and this data is the top of the stack
    void* pushed = lazy_arena_put(stack->arena, stack->stack_ptr, data, size);
    ///Increment the [stack->stack_ptr] by the given size
    ///~alex, 4:01 AM PST, 11/10/2020
    stack->stack_ptr += size;
    return pushed;
}

///Pop off the top of the stack by decrementing the stack->stack_ptr with an offset of [size]
///This will check if stack_ptr is already sizeof(stack_alloc), and if so, 
///don't do anything and just early return
///~alex, 3:48 AM PST, 11/10/2020
///TODO: Replace null-checks with null-asserts
///TODO: Replace weak printing with debug asserts/logging
RECEIVER(stack)
void stack_pop(stack_alloc* stack, u32 size){
    if(stack == NULL){
        printf("stack* cannot be null!\n");
        return;
    }
    ///This is the current size of data that's already been pushed onto the stack.
    ///It is just the current stack_ptr minus the size of the stack_alloc
    ///Because the stack_ptr starts at an offset of the size of the stack_alloc header
    ///So this will tell us if the given [size] is a valid size to pop
    ///MEM: Moved/Owned
    ///LIFETIME: This is used for checking pop size validity, then discarded at the end of scope
    u32 pushed_size = stack->stack_ptr - sizeof(stack_alloc);
    if(size > stack->stack_ptr - sizeof(stack_alloc)){
        printf("Cannot pop size greater than what is already on the stack: %i", pushed_size);
        return;
    }
}

///This is a secondary procedure that will first pop off the stack and then clear the popped data
///This is inefficient so if you absolutely need to clear the popped data, this is your goto.
///~alex, 6:18 AM PST, 11/10/2020
void stack_pop_and_clear(stack_alloc* stack, u32 size){
    stack_pop(stack, size);
    /*
        The following code paragraph is simply going through a popped data and zeroing it out
        A lot of stack allocators will just decrement the stack pointer but just for brevity
        I am clearing this out for the sake of virtually rending that variable useless.
        Ofc, if someone has a pointer to it, it is still valid, so they can still cast it to
        something, but if later on that pointer is used to write/read data, and the stack is still 
        in use, then eventually the stack will just overwrite it. Clearing it to zero ensures
        that if someone asserts data to not be null after it was popped, it will crash their program,
        essentially allowing others to use this stack allocator with debug asserts.

        ~alex, 4:19 AM PST, 11/10/2020
    */
    ///Decrement the current stack pointer
    stack->stack_ptr -= size;
    ///Get the number of bytes to clear in the data that was popped
    ///This will be iterated over via a ranged-for loop to set all the bytes
    ///That was popped to 0
    u32 num_chunks = size / sizeof(u8);
    for(
        u32 i = 0;
        i < num_chunks;
        i++
    ){
        ///The null data being copied into the current offset to clear out the memory on the stack
        u8 zero = 0;
        ///This value is ignored. Capturing in local variable for documentation/clarity
        ///Clear the memory on the stack at the stack->stack_ptr + i
        ///This makes the memory virtually unusable, if a pointer to it is kept around
        void* ignored = lazy_arena_put(stack->arena, stack->stack_ptr + i, &zero, sizeof(u8));
    }
}