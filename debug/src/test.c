#include "debug.h"
#include "string.h"

int main(){
    string test1 = create_string("test1");
    string test2 = create_string("test2");
    string hello_world = create_string("Hello, world!");
    string test_str = create_string("This is a test debug log!");
    debug_log(&test1, &hello_world);
    debug_log(&test2, &test_str);
    return 0;
}