#include "commons.h"
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

void test3(void){
    string example1 = create_string("Hello");
    string example2 = create_string("Hello!@");
    bool eq = stringeq(&example1, &example2);
    if(!eq){
        printf("String compare works as expected!\n");
        return;
    }else{
        printf("String compare doesn't work as expected!\n");
        return;
    }
}

void test4(void){
    string example1 = create_string("   sup!");
    string example2 = create_string("   hello    world  ");
    trim(example1);
    printf("example1 after trim: %s\n", example1.str_data);
    trim(example2);
    printf("example2 after trim: %s\n", example2.str_data);
}

int main(){
    // test1();
    // test2();
    // test3();
    test4();
    return 0;
}