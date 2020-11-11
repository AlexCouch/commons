#pragma once

#include <time.h>
#include "string.h"

string get_now_time_string(){
    string str;
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    str = create_string(asctime(timeinfo));
    str.str_data[str.len - 1] = 0;
    str.len -= 1;
    return str;
}

