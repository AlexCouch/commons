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

typedef i8 nat8;
typedef i16 nat16;
typedef i32 nat32;
typedef i64 nat64;

typedef char* str;

typedef float real32;
typedef double real64;

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

#define OUT
#define IN
#define CALLBACK
#define METHOD

///Denotes that a procedure's parameter is supposed to be treated as the receiver
///This is a high level concept and really just helps me organize my code better
///This is used on procedures
///EXAMPLE: create_list is a procedure which takes an arena_alloc* as a receiver
#define RECEIVER(name)
///This defines a struct as an extension on another struct.
///This is used on structs
///EXAMPLE: list struct is an extension on the arena_alloc struct
#define EXTENSION(name)
///Marks a struct as having an initializer with a certain level of visibility. This can either be INTERNAL or PUBLIC
///This just helps me and anyone using these libraries know how they ought to initialize the marked struct
///`init` is the procedure to use to initialize the marked struct
#define INIT(visibility, init)

#define record(name) typedef struct name name; \
    struct name

#define variant(name) typedef enum name name; \
    enum name
