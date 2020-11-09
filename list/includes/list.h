#pragma once

#include "arena.h"

///An entry in the list.
///Each entry holds the size of the data, the next entry in the data
///And a convenience pointer to the data this entry represents
///The data should always be following this object
/*
    |---------------|------------|--------------|----------|
    |   list_entry  |    data    |  list_entry  |   data   |
    |---------------|------------|--------------|----------|
*/
///Each list entry will point to the next or NULL if it's the last
///This makes iteration easier.
///SEE: create_list_iter, list_iter_next
typedef struct list_entry list_entry;
PUBLIC
struct list_entry{
    u32 size;
    list_entry* next;
    void* data;
};

///A list object which is used for collecting an enumerable list of objects.
///This list is mutable, but any entries that are "removed" via list_remove are
///only disconnected from their surrounding entries so that the iterator
///does not see it.
///This list uses a greedy arena, so that means any entry will persist with the arena
///This list favors cpu usage over memory use. The list_remove procedure will
///keep the entry at the given index in memory but it will be inactive in the list
///This is because removing and relocating/shifting the entries to fill the empty
///space will take up a lot of cpu usage compared to just disconnecting it from the rest of the list.
///This prevents the memory from being fragmented.
///This list should only be used if you intend on the lifetime of this list to be shorter
///than the rest of this program. In other words, if you need a list to persist indefinitely,
///with arbitrary add and remove and insert operations, please use c++ vector, or find another
///better implementation.
///This is meant to act as a simple, easy to use list for cases where remove and insert aren't as
///prevalent, or demanded.
///This list's remove is also only intended so that the list may be transferred from one arena
///to another, where any "removed" elements will be deinitialized/free with the rest of this list's
///arena. Any entries that have stuck around will be transferred to another arena.
///If you'd like to transfer this list to another arena, use list_transfer with a new arena pointer.
PUBLIC
EXTENSION(arena_alloc*)
struct list{
    ///The arena being used to allocate to
    INTERNAL
    arena_alloc* arena;
    ///Number of elements currently added to this list
    INTERNAL
    u32 element_count;
    ///The first element in the list
    ///This is used for iteration
    ///This is first set to NULL
    INTERNAL
    list_entry* first_element;
    ///The last entry in the list. This is used to set the `next` pointer in
    ///this entry to the next entry when list_add is called.
    ///Then this field will be set to that newly added entry
    ///This is first set to first_element
    INTERNAL
    list_entry* last_element;
};
typedef struct list list;

///Creates a new list with the given greedy arena pointer
///The arena is used for allocating a new list object at the new available slot in the arena
///When arena_deinit is called with list->arena, this list will also be deinitialized.
PUBLIC
RECEIVER(arena)
list* create_list(arena_alloc* arena){
    list _list;
    _list.arena = arena;
    _list.element_count = 0;
    _list.first_element = NULL;
    _list.last_element = NULL;
    return arena_put(arena, &_list, sizeof(list));
}
///Adds a new entry to the list with the given data and size
///This will automatically put a new list_entry into the list's greedy arena pointer
///This list_entry will immediately be followed by the given data
///The _list->next_element will be filled by the 
PUBLIC
RECEIVER(_list)
void* list_add(list* _list, void* data, u32 size){
    if(size > _list->arena->size){
        ///TODO: Need to make an assertion/debug library and replace this with a debug/assert call. ~alex, 11/8/2020, 11:19 PM PST
        printf("Expected a list data size within the size of the arena but instead found %i", size);
        return NULL;
    }
    ///TODO: Should we do an assert/debug/log to prevent anything from pass NULL to data?
    ///Create a new list_entry and set the size field to size paramter, set next to null
    ///We don't set the entry data field to anything yet
    ///MEM: Borrow
    ///LIFETIME: Copied into the _list->arena; Dropped at the end of scope
    list_entry entry;
    entry.size = size;
    entry.next = NULL;
    ///Put the entry into the arena
    ///MEM: Borrowed-always
    ///LIFETIME: Persistent as long as the list is persistent, which is the same as _list->arena
    ///NOTE: Do not ever attempt to free this. If you'd life to free it, call list_remove, or dealloc the list/arena entirely
    list_entry* entry_ptr = arena_put(_list->arena, &entry, sizeof(list_entry));
    ///Put the entry data into the arena, just after the list_entry
    ///MEM: Borrowed-always
    ///LIFETIME: Borrowed by list_entry `entry_ptr` such that data is always found by the list_entry
    ///NOTE: Do not attempt to free this. See the NOTE for entry_ptr above.
    void* data_ptr = arena_put(_list->arena, data, size);
    entry_ptr->data = data_ptr;
    if(_list->first_element == NULL){
        _list->first_element = entry_ptr;
        _list->last_element = _list->first_element;
    }else{
        _list->last_element = entry_ptr;
    }
    _list->element_count += 1;
    return data_ptr;
}


///A list iterator. This is used for iterating and keeping track of the current iteration
PUBLIC
EXTENSION(list*)
struct list_iter{
    ///The list being iterated over
    INTERNAL
    list* _list;
    ///The entry of the current iteration
    INTERNAL
    list_entry* curr_entry;
};
typedef struct list_iter list_iter;
///Creates a new list_iter from the given list
///NOTE: This returns a list_iter on the stack
PUBLIC
RECEIVER(_list)
list_iter create_list_iter(list* _list){
    list_iter iter;
    iter._list = _list;
    iter.curr_entry = NULL;
    return iter;
}
///Gets the next entry in the list based on the current list_iter->curr_entry's pointer
///NOTE: This returns a list_entry pointer on the stack
PUBLIC
RECEIVER(iter)
list_entry* list_iter_next(list_iter* iter){
    if(iter->curr_entry == NULL){
        iter->curr_entry = iter->_list->first_element;    
    }else{
        list_entry* curr = iter->curr_entry;
        iter->curr_entry = curr->next;
    }
     return iter->curr_entry;
}

///This will remove an entry in the list
///It doesn't actually "remove" an entry, but instead
///Removes it from iteration by cutting its connection with its previous entry
///and its next entry. This will make it so that the iterator does not see it and cannot
///be found when using list_get or list_index_of
PUBLIC
RECEIVER(_list)
void* list_remove(list* _list, u32 idx){
    if(idx > _list->element_count){
        ///TODO: Replace with a debug/assert with a debug/assert library. ~alex, 11/8/2020, 11:23 PM PST
        printf("Index %i given is not within list indices %i", idx, _list->element_count);
        return NULL;
    }
    ///If the list is empty, return NULL
    if(_list->element_count == 0){
        return NULL;
    }
    ///Create the previous entry. This is used so that we can keep track of the previous entry to the found entry of the given index
    ///This variable's next field will be set to the found entry's next field.
    ///MEM: Borrowed, Borrowed-mut
    ///LIFETIME: This persists as long as an iteration needs it. If an iteration doesn't need it, it will be reassigned to the next variable
    ///LIFETIME: When found_flag is true, this variable will persist until the end of the scope, where it will be mutated so that its next field will be set the next's next field,
    ///~alex, 11/8/2020, 11:28 PM PST
    list_entry* prev_entry = NULL;
    ///The iterator object over the given list object
    ///This is used for getting the next element in the list
    ///MEM: Borrowed-always
    ///LIFETIME: This will persist until the end of the procedure
    ///LIFETIME: This will always be borrowed by list_iter_next
    ///SEE: next, list_iter_next, create_list_iter
    ///~alex, 11/8/2020, 11:29 PM PST
    list_iter iter = create_list_iter(_list);
    ///The next element in the list. The first time this is set, it will be set to the first element in the list, aka, _list->first_element
    ///MEM: Borrowed, Borrowed-mut
    ///LIFETIME: This variable will persist either until the end of the current procedure, or until the end of any iteration.
    ///LIFETIME: This variable is reassigned when its index is not equal to the given idx parameter
    ///LIFETIME: If this variable's index matches the given idx, then it will be used to set prev_entry's next field to this variable's next field.
    ///~alex, 11/8/2020, 11:31 PM PST
    list_entry* next = list_iter_next(&iter);
    ///This is the index we are using to find the element at a given index. If this matches the given idx, then we will "remove" the `next` variable from the list
    ///SEE: next, prev_entry
    ///MEM: Move, Move-mut
    ///LIFETIME: This will persist until the end of the current scope. This will always be copied into a given conditional check for comparison against idx parameter
    ///~alex, 11/8/2020, 11:33 PM PST
    u32 index = 0;
    ///This loops until either next is NULL or until index == idx
    ///Ever iteration, list_iter_next will be called, while index is incremented, and prev_entry is set to next
    while(next != NULL && index != idx){
        prev_entry = next;
        next = list_iter_next(&iter);
        index += 1;
    }

    ///If prev_entry was never advanced, do an alternative
    if(prev_entry == NULL){
        ///Set the first_element to the next element of the first element
        _list->first_element = _list->first_element->next;
    }else{
        ///The previous entry's next field will be set to next's next field
        prev_entry->next = next->next;
        ///next's next field is set NULL
        next->next = NULL;
    }
    _list->element_count -= 1;
    ///Return the data at next
    return next->data;
}

///Gets the data at the given index, if it exists, otherwise will return NULL
///Step1: Check if idx is within the elemen_count of _list
///Step1a: If it isn't return with NULL
///Step1b: If it is proceed to Step2
///Step2: Call create_list_iter with this _list
///Step3: Loop over list_iter_next while keeping track of the current iteration count, called i
///Step4: If i == idx, return the data in the current entry, from list_iter_next
///Step5: return NULL
PUBLIC
RECEIVER(_list)
void* list_get(list* _list, u32 idx){
    if(idx > _list->element_count){
        ///TODO: Replace with a debug/assert with a debug/assert library. ~alex, 11/8/2020, 11:23 PM PST
        printf("Index %i given is not within list indices %i", idx, _list->element_count);
        return NULL;
    }
    ///If the list is empty, return NULL
    if(_list->element_count == 0){
        return NULL;
    }
    ///The iterator object over the given list object
    ///This is used for getting the next element in the list
    ///MEM: Borrowed
    ///LIFETIME: This pointer will persist until the end of the procedure
    ///LIFETIME: This will always be borrowed by list_iter_next 
    ///SEE: next, list_iter_next, create_list_iter
    ///~alex, 11/8/2020, 11:43 PM PST
    list_iter iter = create_list_iter(_list);
    ///The next element in the list. The first time this is set, it will be set to the first element in the list, aka, _list->first_element
    ///MEM: Borrowed, Borrowed-mut
    ///LIFETIME: This variable will persist either until the end of the current procedure, or until the end of any iteration.
    ///LIFETIME: This variable is reassigned when its index is not equal to the given idx parameter
    ///LIFETIME: If this variable's index matches the given idx, then it will be used to set prev_entry's next field to this variable's next field.
    ///~alex, 11/8/2020, 11:43 PM PST
    list_entry* next = list_iter_next(&iter);
    ///This is the index we are using to find the element at a given index. If this matches the given idx, then we will "remove" the `next` variable from the list
    ///SEE: next, prev_entry
    ///MEM: Move, Move-mut
    ///LIFETIME: This will persist until the end of the current scope. This will always be copied into a given conditional check for comparison against idx parameter
    ///~alex, 11/8/2020, 11:43 PM PST
    u32 index = 0;
    ///This loops until either next is NULL or until index == idx
    ///Ever iteration, list_iter_next will be called, while index is incremented, and prev_entry is set to next
    while(next != NULL && index != idx){
        next = list_iter_next(&iter);
        index += 1;
    }
    
    ///Return the data at next
    return next->data;
}

///Gets the index of the pointer to the given data. 
///If the data is equivalent to the data at an index, that index will be returned
///-1 is returned otherwise
///TODO: Test this procedure
PUBLIC
RECEIVER(_list)
i32 list_index_of(list* _list, void* data, bool (*eq_check)(void*, void*)){
    ///The iterator object over the given list object
    ///This is used for getting the next element in the list
    ///MEM: Borrowed
    ///LIFETIME: This pointer will persist until the end of the procedure
    ///LIFETIME: This will always be borrowed by list_iter_next 
    ///SEE: next, list_iter_next, create_list_iter
    ///~alex, 11/8/2020, 11:43 PM PST
    list_iter iter = create_list_iter(_list);
    ///The next element in the list. The first time this is set, it will be set to the first element in the list, aka, _list->first_element
    ///MEM: Borrowed, Borrowed-mut
    ///LIFETIME: This variable will persist either until the end of the current procedure, or until the end of any iteration.
    ///LIFETIME: This variable is reassigned when its index is not equal to the given idx parameter
    ///LIFETIME: If this variable's index matches the given idx, then it will be used to set prev_entry's next field to this variable's next field.
    ///~alex, 11/8/2020, 11:43 PM PST
    list_entry* next = list_iter_next(&iter);
    ///This is the index we are using to find the element at a given index. If this matches the given idx, then we will "remove" the `next` variable from the list
    ///SEE: next, prev_entry
    ///MEM: Move, Move-mut
    ///LIFETIME: This will persist until the end of the current scope. This will always be copied into a given conditional check for comparison against idx parameter
    ///~alex, 11/8/2020, 11:43 PM PST
    u32 index = 0;
    ///Whether the index for the given data is found
    ///MEM: Move-mut
    ///LIFETIME: This will persist until the end of the procedure
    ///~alex, 11/8/2020, 11:49 PM PST
    bool found_flag = false;
    ///This loops until either next is NULL or until index == idx
    ///Ever iteration, list_iter_next will be called, while index is incremented, and prev_entry is set to next
    while(next != NULL){
        if(eq_check(next->data, data)){
            found_flag = true;
            break;
        }
        next = list_iter_next(&iter);
        index += 1;
    }
    if(found_flag){
        return index;
    }
    
    ///Return the data at next
    return -1;
}

///This will transfer all the persistent entries in _list to a new list, with the new_arena
///and will be returned
///Step1: Create a new list via create_list by passing in new_arena
///Step2: Iterate over _list
///Step3: Take every `next` entry in _list and add it to the newly created list via list_add
///Step4: After iteration is complete, call arena_deinit on the old _list->arena
///NOTE: Step4 is deinit'ing the _list->arena which will also deinit the list itself.
///TEST: This hasn't been tested yet but will be tested in the future
///TODO: Test this procedure
PUBLIC
RECEIVER(_list)
list* list_transfer(list* _list, arena_alloc* new_arena){
    ///The new list we are transferring to in which will be returned
    ///MEM: Borrowed-always
    ///LIFETIME: This will persist as long as the user needs it, and will persist until it's pass into arena_deinit
    ///LIFETIME: This is used to transfer the old list, whatever is iterable, into this list, and then returned
    ///~alex, 11/9/2020, 12:11 AM PST
    list* new_list = create_list(new_arena);
    ///The iterator object over the given list object
    ///This is used for getting the next element in the list
    ///MEM: Borrowed
    ///LIFETIME: This pointer will persist until the end of the procedure
    ///LIFETIME: This will always be borrowed by list_iter_next 
    ///SEE: next, list_iter_next, create_list_iter
    ///~alex, 11/9/2020, 12:11 AM PST
    list_iter iter = create_list_iter(_list);
    ///The next element in the list. The first time this is set, it will be set to the first element in the list, aka, _list->first_element
    ///MEM: Borrowed, Borrowed-mut
    ///LIFETIME: This variable will persist either until the end of the current procedure, or until the end of any iteration.
    ///LIFETIME: This variable is reassigned when its index is not equal to the given idx parameter
    ///LIFETIME: If this variable's index matches the given idx, then it will be used to set prev_entry's next field to this variable's next field.
    ///~alex, 11/9/2020, 12:11 AM PST
    list_entry* next = list_iter_next(&iter);
    ///This loops until either next is NULL or until index == idx
    ///Ever iteration, list_iter_next will be called, while index is incremented, and prev_entry is set to next
    while(next != NULL){
        ///The return value is ignored
        ///MEM: Ignored
        ///LIFETIME: Ignored
        ///~alex, 11/9/2020, 12:14 AM PST
        void* ignored_value = list_add(new_list, next, next->size);
        next = list_iter_next(&iter);
    }
    ///TODO: Is this really necessary tho? Should we keep it alive until the user decides to free it?
    ///     although, the whole point of transferring iterable elements of one list to another is to get rid of the old one.
    arena_deinit(_list->arena);
    return new_list;
}