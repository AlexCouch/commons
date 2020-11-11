#include "timestr.h"
#include "string.h"

int main(){
    string now_str = get_now_time_string();
    printf("%s", now_str.str_data);
    string time_str = create_string("");
    get_now_time_buffered(&time_str);
    printf("%s", time_str.str_data);
    return 0;
}