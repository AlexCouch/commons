#pragma once

///A stack allocator is an allocator on the stack. This allocator will allocate 4KB of
///stack/temporary memory to be used. This uses the stack's natural semantics to
///deallocate automatically when it's finished at the end of its declaring scope.
///NOTE: In order to use this, you must make sure that this is alive during its use
///     If you use this after its declaring procedure/scope is finished, it will be deallocated.
/*
    Memory Layout:
    |---------------|---------------------------|
    |     Header    |                           |
    |---------------|       data (4KB)          |
    | u32 stack_ptr |                           |
    |---------------|---------------------------|
*/
struct stack_alloc{
    u32 stack_ptr;
    char data[1024*4];
};
typedef struct stack_alloc stack_alloc;

stack_alloc create_stack_alloc(){
    stack_alloc stack = {0};
    return stack;
}

void* stack_copy_data(stack_alloc* stack, void* data, u32 size){
    char* src = (char*)data;
    u32 DestCounter = stack->stack_ptr;
    while(size--){
        char* dest = (char*)(stack->data + DestCounter);
        *dest = *src++;
        DestCounter++;
    }
    return stack->data + stack->stack_ptr;
}

void* stack_push(stack_alloc* stack, void* data, u32 size){
    void* ptr = stack_copy_data(stack, data, size);
    stack->stack_ptr += size;
    return ptr;
}

void* stack_get(stack_alloc* stack, u32 offset){
    return stack->data + offset;
}

void stack_pop(stack_alloc stack){
    stack.stack_ptr--;
}