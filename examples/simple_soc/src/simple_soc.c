#include "simple_soc.h"

simple_soc* simple_soc_create(simple_soc* this, cdes_simulation* sim) {
    simple_cpu_create(&this->cpu, sim);
    simple_memory_create(&this->mem, sim);
    this->sim = sim;

    simple_initiator_socket_connect(&this->cpu.out, &this->mem.in);

    return this;
}

void simple_soc_destroy(simple_soc* this) {
    simple_cpu_destroy(&this->cpu);
    simple_memory_destroy(&this->mem);
}