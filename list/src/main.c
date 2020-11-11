#define DEBUG

#include "debug.h"
#include "list.h"
#include "arena.h"
#include "string.h"

struct Person{
    str name;
    u32 age;
};
typedef struct Person Person;

int main(){
    arena_alloc* arena = arena_init(1024*1024);
    list* _list = create_list(arena);
    Person bob;
    bob.name = "Bob";
    bob.age = 34;
    Person* bob_ptr = list_add(_list, &bob, sizeof(Person));
    string message = create_string("My name is %s, and I am %i years old!", bob_ptr->name, bob_ptr->age);
    string from = create_string("list_test");
    debug_log(&from, &message);
    Person* bob_get_ptr = list_get(_list, 0);
    message = create_string("My name is %s, and I am %i years old!", bob_ptr->name, bob_ptr->age);
    debug_log(&from, &message);
    Person* bob_rem_ptr = list_remove(_list, 0);

    printf("My name is %s, and I am %i years old!\n", bob_rem_ptr->name, bob_rem_ptr->age);
    Person* bob_get_ptr2 = list_get(_list, 0);
    if(bob_get_ptr2 != NULL){
        printf("Expected bob_get_ptr2 to be NULL after getting the 0th index after removing the 0th index.");
    }
    arena_deinit(arena);
    return 0;
}