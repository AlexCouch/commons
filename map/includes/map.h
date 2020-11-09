#pragma once

#include "string.h"
#include "arena.h"

/*
    A map is the use of an arena such that entries can be a key followed by a value.
    When putting an entry in the map, map_put must be called.
    map_put will take data for the key of void* followed by a u32 size, and data for the value with u32 size.
    ```
    string key = create_string("Hello, world!");
    map_put(key.str_data, key.len, 5, sizeof(u32));
    u32* value = (u32*)map_get(key.str_data);
    ```
    map_get will create a map_iter instance, and use it to iterate over the map, and check each key against the given search key.
*/

enum map_entry_kind{
    KEY,
    VALUE,
};
typedef enum map_entry_type map_entry_kind;

///What type of data an entry contains. This is a marker for the data
PUBLIC
enum map_entry_type{
    STRING,
    U8,
    U16,
    U32,
    U64,
    OTHER
};
typedef enum map_entry_type map_entry_type;


//A map entry. 
//This has the data map for easily knowing how to treat the entry.
//This also contains the size of the data
//This also has a pointer to the data itself. 
//  This pointer should point to just after this entry's memory location relative to struct size.
//data => The data this entry is representing. The data will always be immediately after this entry. This is mostly a convenience field.
//next => The next entry in the map. This is used for iteration. This is an internal use only.
typedef struct map_entry map_entry;
INTERNAL 
struct map_entry{
    map_entry_kind data_kind;
    map_entry_type data_type;
    u32 size;
    
    HELPER
    void* data;
    
    HELPER 
    map_entry* next;
    
    HELPER 
    map_entry* value;
};


PUBLIC
EXTENSION(arena)
struct map{
    INTERNAL 
    arena_alloc* arena;
    
    INTERNAL HELPER 
    map_entry* first_entry;
    
    INTERNAL HELPER
    map_entry* last_entry;
};
typedef struct map map;

///Creates a new map with a NULL first_entry, indicating it currently has no values.
PUBLIC
RECEIVER(arena)
map* create_map(arena_alloc* arena){
    ///A new map instance. Passed into arena_put to acquire a pointer to its heap counterpart.
    ///MEM: Borrowed
    ///LIFETIME: Borrowed by arena_put to be copied into the greedy arena, where the acquired pointer is returned.
    map _map;
    ///Initialize the arena for the map to the pointer to the given arena_alloc `pointer`
    _map.arena = arena;
    ///set the first_entry to NULL
    _map.first_entry = NULL;
    ///Set the last_entry to first_entry which is set to NULL
    _map.last_entry = _map.first_entry;
    //printf("Putting new map header into arena\n");
    ///Give 
    return arena_put(arena, &_map, sizeof(map));
}

///Creates a map entry. This can either be a key or a value.
///This will be called from map_put. Do not attempt to call this directly.
///_map => The map the new entry is being put into
///kind => What kind of entry is this: KEY or VALUE
///data_type => A flag that is used for knowing what kind of node this is
///data_size => The size of the data for the entry
///data => The data itself this entry is being associated with
INTERNAL
EXTENSION(_map)
map_entry* create_map_entry(
    map* _map,
    map_entry_kind kind,
    map_entry_type data_type,
    u32 data_size,
    void* data
)
{
    ///Create a new entry with the given kind, type, size, and data, while setting next and value to NULL
    ///MEM: Borrow
    ///LIFETIME: This is immediately borrowed by arena_put for copying into the arena. It is then never used again.
    map_entry entry = { kind, data_type, data_size, data, NULL, NULL };
    //printf("Putting new map entry header into arena\n");
    ///Put `entry` into the arena via arena_put. It will be copied into the arena, and the following uses of the entry will be from here.
    ///MEM: (Borrow or Borrow, Borrow), Borrow
    ///LIFETIME: This is borrowed by _map->first_entry or by _map->last_entry->next and _map->last_entry respectively
    map_entry* entry_ptr = (map_entry*)arena_put(_map->arena, &entry, sizeof(map_entry) + data_size);
    if(entry_ptr == NULL){
        //printf("entry_ptr came back null while putting entry into arena\n");
        return NULL;
    }
    //printf("Putting new map entry data into arena\n");
    ///Put the entry data into the arena. This should be immediately following the entry struct header. This is then given to entry_ptr->data.
    ///MEM: Borrow
    ///LIFETIME: This is borrowed by entry_ptr->data, as a helper field. This field is the best way to acquire a handle to this recently allocated data associated with this entry.
    void* entry_data = arena_put(_map->arena, data, data_size);
    ///Borrow entry_data so that entry_ptr->data is used to acquire a handle on the allocated data associated with this entry
    entry_ptr->data = entry_data;
    ///If the first entry in the map is null, then we have a fresh, empty map. Therefore, set the first entry to the newly acquired pointer to this entry
    ///Otherwise, set the last entry's next pointer to this entry, then set the last entry to this entry.
    ///This makes it so that every entry points to not only its value but also the next key-value pair in the map.
    if(kind == KEY){
        if(_map->first_entry == NULL){
            //printf("Map is fresh...setting first and last entry to new map entry\n");
            _map->first_entry = entry_ptr;
            _map->last_entry = _map->first_entry;
        }else{
            //printf("Setting map entry to last_entry->next and last_entry\n");
            _map->last_entry->next = entry_ptr;
            _map->last_entry = entry_ptr;
        }
    }
    ///Return the pointer to this entry
    ///Finished
    return entry_ptr;
}

///A map iterator struct. This is just a record that is kept for iterating and finding a key of a given data and return the corresponding value
EXTENSION(map)
struct map_iter{
    ///The map being iterated
    map* map;
    ///The current entry being iterated.
    map_entry* curr_entry;
};
typedef struct map_iter map_iter;

///Creates a map iter on the stack. This will record the iteration details over the map.
///This will init to the start of the map according to the beginning of the map, aka _map->first_entry
RECEIVER(_map)
map_iter create_map_iter(map* _map){
    map_iter iter;
    iter.map = _map;
    iter.curr_entry = _map->first_entry;
    return iter;
}

RECEIVER(iter)
map_entry* next_entry(map_iter iter){
    map_entry* curr = iter.curr_entry;
    iter.curr_entry = curr->next;
    return iter.curr_entry;
}

PUBLIC
RECEIVER(_map)
void* map_get(
    ///The map we want to get a key-value from
    map* _map, 
    ///The key metadata we need. key is the data of the key, key_type is the type of data, and size is the size of data in case we need it
    void* key, map_entry_type key_type, u32 size, 
    ///An equality check for other data types. If the key_type is OTHER, this will be called.
    ///If you pass NULL to this, and key_type is not OTHER, it will be ignored.
    bool (*eq_check)(void*, void*)
){
    map_iter iter = create_map_iter(_map);
    map_entry* next = next_entry(iter);
    while(next != NULL){
        if(next->data_type == key_type){
            switch(key_type){
            case U8: {
                u8* entry_data = (u8*)next->data;
                u8* key_data = (u8*)key;
                if(entry_data == key_data){
                    return next->value->data;
                }
            } break;
            case U16: {
                u16* entry_data = (u16*)next->data;
                u16* key_data = (u16*)key;
                if(entry_data == key_data){
                    return next->value->data;
                }
            } break;
            case U32: {
                u32* entry_data = (u32*)next->data;
                u32* key_data = (u32*)key;
                if(*entry_data == *key_data){
                    return next->value->data;
                }
            } break;
            case U64: {
                u64* entry_data = (u64*)next->data;
                u64* key_data = (u64*)key;
                if(*entry_data == *key_data){
                    return next->value->data;
                }
            } break;
            case STRING:{
                str entry_data = (str)next->data;
                str key_data = (str)key;
                if(strcmp(entry_data, key_data)){
                    return next->value->data;
                }
            } break;
            case OTHER:{
                void* entry_data = next->data;
                void* key_data = key;
                if(eq_check(entry_data, key_data)){
                    return next->value->data;
                }
            } break;
            }
        }
        next = next_entry(iter);
    }
    return NULL;
}

PUBLIC
RECEIVER(_map)
void* map_put(
    map* _map, 
    ///Key data, size, and type of data
    void* key, u32 key_size, map_entry_type key_type,
    ///Value data, size, and type of data
    void* value, u32 val_size, map_entry_kind val_type
){
    map_entry* key_entry = create_map_entry(_map, KEY, key_type, key_size, key);
    if(key_entry == NULL){
        printf("Couldn't create map entry...see console!\n");
        return key_entry;
    }
    //printf("Created map entry key\n");
    map_entry* value_entry = create_map_entry(_map, VALUE, val_type, val_size, value);
    //printf("Created map entry value\n");
    key_entry->value = value_entry;
    _map->last_entry->next = key_entry;
    _map->last_entry = key_entry;
    return key_entry->data;
}

