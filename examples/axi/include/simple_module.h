#ifndef __SIMPLE_MODULE_H__
#define __SIMPLE_MODULE_H__

#include "cdes.h"
#include "axi_socket.h"

typedef struct {
    axi_target_socket in;
    axi_initiator_socket out;
    cdes_task on_sim_start;
    cdes_task on_read_addr_start;
    cdes_task on_read_addr_end;
    cdes_task on_read_data_start;
    cdes_task on_read_data_end;
    cdes_task on_read_data_last_start;
    cdes_task on_read_data_last_end;
    cdes_simulation* sim;
} simple_module;

simple_module* simple_module_create(simple_module* this, cdes_simulation* sim);
void simple_module_destroy(simple_module* this);

#endif  // __SIMPLE_MODULE_H__