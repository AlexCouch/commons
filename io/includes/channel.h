#pragma once

/**
    A concurrent channel to send data from one fiber to another
    This is meant to be used with fibers.

    This is a simple wrapper with a semaphore around a queue.
    The dequeue is used to control the start and end of the queue.
    When you want to send data, things are received in a first-in-first-out
    order, hence the use of the dequeue.
    

    SEE: libco in build.json
    SEE: queue
*/

struct channel{
    u32 queued;
    queue* data;
};
typedef struct channel channel;