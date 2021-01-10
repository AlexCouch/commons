#pragma once

#include <stdarg.h>

#include "commons.h"
#include "timestr.h"
#include "stack.h"
#include "string_store.h"

#ifndef DEBUG
#define ASSERT(expr)
#else
#define ASSERT(expr) if(!expr){ (*(int*)0 = 0); }
#endif

void debug_log(str from, str msg, ...){
    va_list args;
    va_start(args, msg);

    stack_alloc stack = create_stack_alloc();
    string_store str_store = string_store_create(&stack);
    string* output = string_store_put(&str_store, "[");
    str time_str = get_now_time_string();
    varg_string_store_concat_str(output, time_str, args);
    varg_string_store_concat_str(output, "]", args);
    varg_string_store_concat_str(output, "[", args);
    varg_string_store_concat_str(output, from, args);
    varg_string_store_concat_str(output, "]: ", args);
    varg_string_store_concat_str(output, msg, args);
    printf("%s\n", output->data);

    va_end(args);
}