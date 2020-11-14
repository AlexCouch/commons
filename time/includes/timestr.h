#pragma once

#include "string_store.h"
#include <time.h>

str get_now_time_string(){
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    str time_str = asctime(timeinfo);
    u32 time_len = string_length(time_str);
    time_str[time_len-1] = 0;
    return time_str;
}