#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef char* str;

//Just a human readable marker that tells the reader/writer 
//that something is meant to be internal use only
#define INTERNAL
//A human readable marker that tells the reader/writer 
//that something is meant to be a convenience field or procedure 
//to help with getting data that would otherwise be acquired inelegantly.
#define HELPER
//A human readable market that tells the reader/writer
//that something is meant to be used/seen publically and to be treated as such.
//This is in contrast to INTERNAL
#define PUBLIC