#pragma once

#include "commons.h"

struct string{
    char* str_data;
    u32 len;
};
typedef struct string string;

string create_string(str str_data){
    string str;
    str.str_data = str_data;
    for(u32 i = 0;;i++){
        char c = str_data[i];
        if(c == '\0'){
            str.len = i;
            break;
        }
    }
    return str;
}

void strcat(str dest, str src){
    // printf("Concatenating %s into %s\n", src, dest);
    u32 dest_end = 0;
    for(
        u32 i = 0;
        ;
        i++
    ){
        char c = dest[i];
        // printf("Checking char in dest: %c\n", c);
        if(c == '\0'){
            dest_end = i;
            break;
        }
    }
    // printf("Found dest_end: %i\n", dest_end);
    u32 src_end = 0;
    for(
        u32 i = 0;
        ;
        i++
    ){
        char c = src[i];
        // printf("Checking char in src: %c\n", c);
        if(c == '\0'){
            src_end = i;
            break;
        }
    }
    // printf("Found src_end: %i\n", src_end);
    for(
        u32 i = 0;
        i < src_end;
        i++
    ){
        char c = src[i];
        if(c == '\0'){
            break;
        }
        dest[dest_end + i] = c;
        // printf("Copying %c to destination\n", c);
        
    }
    dest[dest_end + src_end] = '\0';
}

void string2string(string* dest, string* src){
    char* dest_data = dest->str_data;
    char* src_data = src->str_data;
    for(
        u32 i = 0;
        i < src->len;
        i++
    ){
        dest_data[dest->len + i] = src_data[i];
        // printf("Appending %c\n", src_data[i]);
    }
    dest->str_data = dest_data;
    dest->len += src->len;
}

void str2string(string* dest, str src){
    string src_str = create_string(src);
    string2string(dest, &src_str);
}

void string2str(str dest, string* src){
    // printf("Copying %s into str: %s\n", src->str_data, dest);
    strcat(dest, src->str_data);
}