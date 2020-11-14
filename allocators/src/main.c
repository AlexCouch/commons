#include "commons.h"
#include "debug.h"
#include "stack.h"

struct Person{
    str name;
    u32 age;
};
typedef struct Person Person;

int main(){
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
    return 0;
}