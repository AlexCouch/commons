#include "map.h"
#include "string.h"
#include "arena.h"

bool string_eq(void* key1, void* key2){
    string* k1 = (string*)key1;
    string* k2 = (string*)key2;
    return stringeq(k1, k2);
}

int main(){
    string example_key = create_string("Hello");
    string example_value = create_string("World!");
    arena_alloc* arena = arena_init(1024);
    map* _map = create_map(arena);
    string* key = (string*)map_put(_map, &example_key, sizeof(string), OTHER, &example_value, sizeof(string), OTHER);
    if(key == NULL){
        return;
    }
    string* value = (string*)map_get(_map, key, OTHER, sizeof(string), &string_eq);
    if(value == NULL){
        return;
    }
    printf("%s%s\n", key->str_data, value->str_data);
    return 0;
}