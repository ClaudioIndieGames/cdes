#ifndef __SIMPLE_SOC_H__
#define __SIMPLE_SOC_H__

#include "cdes.h"
#include "simple_cpu.h"
#include "simple_memory.h"

typedef struct {
    simple_cpu cpu;
    simple_memory mem;
    cdes_simulation* sim;
} simple_soc;

simple_soc* simple_soc_create(simple_soc* this, cdes_simulation* sim);
void simple_soc_destroy(simple_soc* this);

#endif  // __SIMPLE_SOC_H__