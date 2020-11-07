#include "string.h"

void test2(void){
    char example[] = "\0";
    char hello[] = "Hello";
    strcat(example, hello);
    str world = ", world!";
    strcat(example, world);
    printf("%s", example);
}

void test1(void){
    string example = create_string("");
    string hello = create_string("Hello");
    string2string(&example, &hello);
    // printf("'Hello' string copied into example string\n");
    printf("str_data: %s\nstr_len: %i\n", example.str_data, example.len);
    str2string(&example, ", world!");
    printf("str_data: %s\nstr_len: %i\n", example.str_data, example.len);
}

int main(){
    test1();
    // test2();
    return 0;
}