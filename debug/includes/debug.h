#pragma once

#include "commons.h"
#include "string.h"
#include "timestr.h"

#ifndef DEBUG
#define ASSERT(expr)
#else
#define ASSERT(expr) if(!expr){ (*(int*)0 = 0); }
#endif

void debug_log(string* from, string* msg){
#ifdef DEBUG
    char output[512] = "[";
    string time_str = get_now_time_string();
    string2str(output, &time_str);
    strcat(output, "]");
    strcat(output, "[");
    string2str(output, from);
    strcat(output, "]: ");
    string2str(output, msg);
    printf("%s\n", output);
#endif
}