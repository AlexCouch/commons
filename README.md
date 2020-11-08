# Commons
These are mostly header only libraries I'm making for myself. They will have their own test modules and they are managed through [cman](https://github.com/alexcouch/cman).

## commons/includes/commons.h
This right now just has some typedefs to make my life easier I guess

## string/includes/string.h
Some string functions such as a way to represent string as a string that keeps track of its own length. Some concatenation functions too. It's recommended that you use the string representation instead of str. strcat is slower than string2string. If you try to make everything into a string object via create_string, you will thank me later. Let me know if you get different results.

For me, cman tells me that using string2string cat function is about 20-25 ms faster than strcat.

## map/includes/map.h
A map implementation that is cache-friendly and simple to use.
```c
string key = create_string("Hello");
string value = create_string("World");
arena_alloc* arena = arena_init(1024);
map* _map = create_map(arena);
string* key_ptr = (string*)map_put(_map, &key, sizeof(string), OTHER, &value, sizeof(string), OTHER);
if(key == NULL){
    return;
}
string* value_ptr = (string*)map_get(_map, key_ptr, OTHER, sizeof(string), &string_eq);
if(value == NULL){
    return;
}
printf("%s%s\n", key->str_data, value->str_data);
```
This code can execute on my machine within 30 ms on average. However, I have not tested it with a larget scale use. I hope to do so soon.