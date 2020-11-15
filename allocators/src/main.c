#include "commons.h"
#include "debug.h"
#include "stack.h"
#include "deque.h"

struct Person{
    str name;
    u32 age;
};
typedef struct Person Person;

void stack_test(void){
    stack_alloc stack = create_stack_alloc();
    Person bob;
    bob.name = "Bob";
    bob.age = 32;
    Person* bob_ptr = stack_push(&stack, &bob, sizeof(Person));
    debug_log("Allocators test","My name is %s, and I am %i years old!", bob_ptr->name, bob_ptr->age);
    Person jill;
    jill.name = "Jill";
    jill.age = 38;
    Person* jill_ptr = stack_push(&stack, &jill, sizeof(Person));
    debug_log("Allocators test", "My name is %s, and I am %i years old!", jill_ptr->name, jill_ptr->age);
    Person* bob_ptr_again = stack_get(&stack, 0);
    debug_log("Allocators test", "My name is %s, and I am %i years old!", bob_ptr_again->name, bob_ptr_again->age);
}

void deque_test(void){
    arena_alloc* arena = arena_init(1024*4);
    deque_result result = deque_create(arena, 1024);
    deque_alloc* deque;
    switch(result.tag){
        case SUCCESS: {
            deque = result.data;
        } break;
        case CREATE_FAILED_TOO_LARGE: {
            debug_log("Deque test", "Failed to create deque as deque size %i is too large for arena size %i", result.size, arena->size - arena->capacity);
        } break;
        case CREATE_FAILED_ARENA_NULL: {
            debug_log("Deque test", "Failed to create deque as arena is NULL.");
        }break;
        default: {
            debug_log("Deque test", "Failed to create deque");
        }break;
    }
}

int main(){
    //stack_test();
    deque_test();
    return 0;
}