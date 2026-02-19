#ifndef __SIMPLE_MEMORY_H__
#define __SIMPLE_MEMORY_H__

#include "cdes.h"
#include "simple_port.h"

typedef struct {
    simple_port p;
    cdes_task on_received_message;
    cdes_simulation* sim;
} simple_memory;

simple_memory* simple_memory_create(simple_memory* this, cdes_simulation* sim);
void simple_memory_destroy(simple_memory* this);

#endif  // __SIMPLE_MEMORY_H__