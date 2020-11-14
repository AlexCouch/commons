#pragma once

#include "stack.h"
#include <stdarg.h>
#include <stdio.h>

struct string{
    u32 length;
    str data;
};
typedef struct string string;

///This is a stack based string store. This is meant to be relatively small and store multiple strings on the stack
///And also meant to be something very temporary on the stack. If you expect you're gonna need more memory for more and larger
///strings, please use a heap based implementation like list or some kind of arena. This is only meant for smaller uses
///For example, the debug_log will use this to create a malleable string on the stack to which can be extended,
///while not overwriting data in the global data section.
struct string_store{
    stack_alloc* stack;
    string* start;
    string* end;
};
typedef struct string_store string_store;

///Calculate the length of the string and return it. This is quite simple, nuff said.
///~alex, 9:04 AM PST, 11/14/2020
u32 string_length(str data){
    u32 length = 0;
    for(u32 i = 0;;i++){
        char c = data[i];
        if(c == '\0'){
            length = i;
            break;
        }
    }
    return length;
}

///Create a new string store with the given stack allocator. This string store is meant to be on the stack to avoid cache misses.
///See [string_store] for more info on how to use this and when to call this.
///~alex, 9:04 AM PST, 11/14/2020
string_store string_store_create(stack_alloc* stack){
    string_store store = { stack, NULL, NULL };
    stack_push(stack, &store, sizeof(string_store));
    return store;
}

///Allocate a new string object with 0 init by default. This will then be returned by reference.
///~alex, 9:22 AM PST, 11/14/2020
string* string_store_alloc(string_store* store){
    string _string = { 0 };
    string* _str_data = stack_push(store->stack, &_string, sizeof(string));
    return _str_data;
}

///Put the string into the store and do string formatting if necessary.
///~alex, 9:20 AM PST, 11/14/2020
string* string_store_put(string_store* store, str str_data, ...){
    va_list args;
    va_start(args, str_data);

    string* _string = string_store_alloc(store);
    u32 length = string_length(str_data);
    str data = stack_push(store->stack, str_data, length);
    
    
    vsprintf(str_data, str_data, args);
    va_end(args);
    
    _string->data = data;
    _string->length = length;
    return _string;
}

void varg_string_store_concat_str(string* dest, str src, va_list args){
    char buffer[1024];
    // str str_data = va_arg(args, str);
    // printf("str_data: %s\n", str_data);
    vsprintf(buffer, src, args);

    char* dest_data = dest->data + dest->length;
    u32 src_len = string_length(buffer);
    for(
        u32 i = 0;
        i < src_len;
        i++
    ){
        dest_data[i] = buffer[i];
        dest->length++;
        // printf("Appending %c\n", src_data[i]);
    }
}

void string_store_concat_str(string* dest, str src, ...){
    va_list args;
    va_start(args, src);
    varg_string_store_concat_str(dest, src, args);
    va_end(args);
}

void string_store_concat(string* dest, string* src, ...){
    va_list args;
    va_start(args, src);

    char* dest_str = dest->data + dest->length;
    char* src_str = src->data + src->length;
    while(src != 0){
        *dest++ = *src++;
        dest->length++;
    }
    vsprintf(dest_str, dest_str, args);
    va_end(args);
}