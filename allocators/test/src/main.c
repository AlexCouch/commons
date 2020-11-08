#include "arena.h"
#include "lazy_arena.h"
#include "commons.h"

struct Person{
    char* name;
    u32 age;
};
typedef struct Person Person;

int main(){
    printf("Testing greedy arena!\n");
    arena_alloc* arena = arena_init(sizeof(Person)*100);
    printf("Size of Person struct %i\n", sizeof(Person));
    for(
        u32 i = 0;
        i < 100;
        i++
    ){
        Person person = {
            "bob",
            32
        };
        Person* bob = (Person*)arena_put(arena, &person, sizeof(Person));
        printf("My name %s and I am %i years old. My addr is %p\n", bob->name, bob->age, bob);
    }
    arena_deinit(arena);
    printf("Allocating 100 u32's\n");
    arena_alloc* u32_arena = arena_init(sizeof(u32)*100);
    for(
        u32 i = 0;
        i < 100;
        i++
    ){
        u32* data = (u32*)arena_put(u32_arena, &i, sizeof(u32));
        printf("Data: %i; Addr: %p\n", *data, data);
    }
    arena_deinit(u32_arena);
    printf("Greedy arena tests complete!\n");
    printf("Testing lazy arena!\n");
    lazy_arena_alloc* lazy_arena = lazy_arena_init(sizeof(u32)*1024);
    printf("End of lazy arena: %p\n", lazy_arena + sizeof(u32)*1024);
    for(
        u32 i = 0;
        i < 1024;
        i++
    ){
        void* addr = lazy_arena_put(lazy_arena, i + sizeof(u32), &i, sizeof(u32));
        printf("Added data %i to addr %p\n", i, addr);
    }
    
    lazy_arena_deinit(lazy_arena);
    printf("\n");
    printf("Lazy arena tests complete!\n");
    return 0;
}