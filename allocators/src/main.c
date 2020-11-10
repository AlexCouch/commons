#include "stack.h"

struct Person{
    str name;
    u32 age;
};
typedef struct Person Person;

int main(){
    stack_alloc* stack = stack_init_full(1024);
    Person bob;
    bob.name = "Bob";
    bob.age = 32;

    Person* bob_ptr = stack_push(stack, &bob, sizeof(Person));
    if(bob_ptr == NULL){
        printf("bob_ptr came back null!\n");
        return 1;
    }
    stack_pop(stack, sizeof(Person));
    if(bob_ptr == NULL){
        printf("bob_ptr is null after pop!\n");
        return 1;
    }
    stack_deinit(stack);
    return 0;
}