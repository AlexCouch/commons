#pragma once

#include "commons.h"

PUBLIC
EXTENSION(str_data)
INIT(INTERNAL, create_string)
struct string{
    str str_data;
    u32 len;
};
typedef struct string string;

PUBLIC
RECEIVER(str_data)
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

RECEIVER(dest)
PUBLIC
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

PUBLIC
RECEIVER(dest)
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

PUBLIC
RECEIVER(dest)
void str2string(string* dest, str src){
    string src_str = create_string(src);
    string2string(dest, &src_str);
}

PUBLIC
RECEIVER(dest)
void string2str(str dest, string* src){
    // printf("Copying %s into str: %s\n", src->str_data, dest);
    strcat(dest, src->str_data);
}

/*
            STRING COMPARISONS
*/

PUBLIC
RECEIVER(s1)
bool stringeq(string* s1, string* s2){
    bool ran = false;
    bool pass_flag = true;
    u32 counted = 0;
    for(
        u32 i = 0;
        i < s1->len && i < s2->len;
        i++
    ){
        ran = true;
        char c1 = s1->str_data[i];
        char c2 = s2->str_data[i];
        if(c1 != c2){
            // printf("Found a discrepency: %c in s1, %c in s2, at %i", c1, c2, i);
            pass_flag = false;
            break;
        }
        counted++;
    }
    // printf("Counted %i in both strings\n", counted);
    if(counted < s2->len){
        // printf("Counted less than the len of string 2\n");
        pass_flag = false;
    }
    return pass_flag;
}

PUBLIC
RECEIVER(s1)
bool strcmp(str s1, str s2){
    string string1 = create_string(s1);
    string string2 = create_string(s2);
    return stringeq(&string1, &string2);
}