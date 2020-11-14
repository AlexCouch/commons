#include "timestr.h"
#include "string.h"

int main(){
    string now_str = get_now_time_string();
    printf("%s", now_str.str_data);
    return 0;
}