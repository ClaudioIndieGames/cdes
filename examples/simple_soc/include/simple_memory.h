#ifndef __SIMPLE_MEMORY_H__
#define __SIMPLE_MEMORY_H__

#include "cdes.h"
#include "simple_socket.h"

typedef struct {
    simple_target_socket in;
    cdes_task on_request_received;
    cdes_simulation* sim;
} simple_memory;

simple_memory* simple_memory_create(simple_memory* this, cdes_simulation* sim);
void simple_memory_destroy(simple_memory* this);

#endif  // __SIMPLE_MEMORY_H__