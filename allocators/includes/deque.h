#pragma once

#include "commons.h"
#include "arena.h"
#include "stdio.h"

typedef enum deque_state deque_state;
typedef struct deque_entry deque_entry;

///A dequeue_alloc is a queue implementation that uses a dynamic array and circular buffer.
///A circular buffer is a buffer that wraps back around to tbe beginning of the buffer.
///The [arena] is the arena that this deque is a part of, which allows for instant monotonicity
/// and great locality.
///The [size] is how many bytes total are in the queue before it becomes "full".
///The [capacity] is the current number of items in the queue.
///The [start] is the start of the queue, which will be used for looping back around the buffer
///The [head] is the next address that will be pushed if queue_push_head is used,
/// and also the place where the head will be popped if queue_pop_head is used.
///The [end] is the end of the buffer that will indicate to the tail or head that it has reached
/// the end of the buffer and to loop back around to the [start]. 
///The [tail] is the last item pushed onto the queue via deque_push_tail is used, and also the
/// place where deque_pop_tail will pop and return the item popped.
///The [state] is the current state of the deque.
///
///SEE: arena_alloc
///SEE: deque_state
///SEE: drque_entry
PUBLIC
EXTENSION(arena)
struct deque_alloc{
    INTERNAL
    arena_alloc* arena;

    INTERNAL
    u32 size;
    
    INTERNAL
    u32 capacity;
    
    INTERNAL
    void* start;
    
    INTERNAL
    deque_entry* head;
    
    INTERNAL
    void* end;
    
    INTERNAL
    deque_entry* tail;

    INTERNAL
    deque_state state;
};
typedef struct deque_alloc deque_alloc;

///This is an entry in the deque, which makes this deque a linked list.
///The [size] is the size of the entry as a sanity check.
///The [data] is the data that proceeds this structure in memory, and acts as a convenience field
///The [next] is the next entry in the deque after this entry.
PUBLIC
struct deque_entry{
    INTERNAL
    u32 size;

    PUBLIC
    HELPER
    void* data;
};

///This represents a state of the deque.
///EMPTY is used for knowing that there is currently no entries in the queue and to be cautious
/// with adding a new entry, because [head] will be NULL.
///FULL is used for rejecting a push due to insufficient space.
enum deque_state{
    EMPTY,
    FULL
};

///A result kind, which is used for tagging the internal union of deque_result
///SUCCESS: We succeeded with pushing or popping. This will be accompanied by [data] in deque_result
///CREATE_FAILED_TOO_LARGE: deque_create was called with a size argument too large for the given arena
///CREATE_FAILED_ARENA_NULL: deque_create was called with a NULL arena argument.
///INSUFFICIENT_SPACE: We failed to push because there's not enough space to push the data to the deque
///DATA_TOO_LARGE: The data size being pushed exceed the capacity of the deque
///NULL_DATA_RECEIVED: A deque_push_* operation was called and it was given NULL for [data] param.
///POP_FAIL_TAIL_NULL: A pop tail operation failed due to the tail being NULL
///POP_FAIL_HEAD_NULL: A pop head operation failed due to the head being NULL
enum deque_result_kind{
    SUCCESS,
    CREATE_FAILED_TOO_LARGE,
    CREATE_FAILED_ARENA_NULL,
    INSUFFICIENT_SPACE,
    DATA_TOO_LARGE,
    NULL_DATA_RECEIVED,
    POP_FAIL_TAIL_NULL,
    POP_FAIL_HEAD_NULL
};
typedef enum deque_result_kind deque_result_kind;

///This represents a result of some deque operation. This uses a tagged union to indicate what happened
///and what data it comes with.
///The [tag] is a result kind of deque_result_kind, this is used for tagging the union.
///The [message] is used for logging if something goes wrong, then one can create a stacktrace if necessary.
///
///SEE: deque_result_kind
struct deque_result{
    deque_result_kind tag;
    union{
        u32 size;
        void* data;
    };
};
typedef struct deque_result deque_result;

///Creates a new deque with the given arena and given size. This will check if size is within
///The capacity of the given arena. If not, then the result tag will become CREATE_FAILED_TOO_LARGE.
///If the arena is NULL, the result tag will be CREATE_FAILED_ARENA_NULL
///Otherwise, the arena will be used to put a new instance of the deque_alloc in the arena
///It will then be initialized to default state of EMPTY and pointers set to NULL
///It will then return with result tag of SUCCESS and the result data field in the union set to the
///Pointer in the arena that the deque_alloc was put to.
///~alex, 9:39 AM PST, 11/15/2020
deque_result deque_create(arena_alloc* arena,  u32 size){
    deque_result result;
    if(arena == NULL){
        result.tag = CREATE_FAILED_ARENA_NULL;
        result.size = size;
        return result;
    }
    if(size > arena->size){
        result.tag = CREATE_FAILED_TOO_LARGE;
        result.size = size;
        return result;
    }
    deque_alloc deque;
    deque.arena = arena;
    deque.size = size;
    deque.capacity = 0;
    deque.start = NULL;
    deque.head = NULL;
    deque.end = NULL;
    deque.tail = NULL;
    deque.state = EMPTY;

    deque_alloc* pdeque = arena_put(arena, &deque, sizeof(deque_alloc) + size);

    result.data = pdeque;
    result.tag = SUCCESS;
    return result;
}
deque_result deque_push_head(deque_alloc* deque, void* data, u32 size);
deque_result deque_push_tail(deque_alloc* deque, void* data, u32 size);
deque_result deque_pop_head(deque_alloc* deque, void* data, u32 size);
deque_result deque_pop_tail(deque_alloc* deque, void* data, u32 size);