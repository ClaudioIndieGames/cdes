#ifndef __SIMPLE_CPU_H__
#define __SIMPLE_CPU_H__

#include "cdes.h"
#include "simple_port.h"

typedef struct {
    simple_port p;
    cdes_task on_sim_start;
    cdes_task on_received_message;
    cdes_simulation* sim;
} simple_cpu;

simple_cpu* simple_cpu_create(simple_cpu* this, cdes_simulation* sim);
void simple_cpu_destroy(simple_cpu* this);

#endif  // __SIMPLE_CPU_H__