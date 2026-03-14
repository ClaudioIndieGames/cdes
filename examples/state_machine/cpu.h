#ifndef __CPU_H__
#define __CPU_H__

#include "module.h"

typedef struct {
    module base;
    int my_data;
    void (*event)(void* ctx, void* arg);
} cpu;

void cpu_init(cpu* this, const char* name, int my_data);
void cpu_fini(cpu* this);

#endif // __CPU_H__