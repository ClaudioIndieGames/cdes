#ifndef __SIMPLE_PORT_H__
#define __SIMPLE_PORT_H__

#include <stddef.h>
#include "cdes.h"

typedef struct {
    cdes_event received_message;
    cdes_event* sent_message;
    cdes_simulation* sim;
} simple_port;

typedef struct {
    void* from;
    char* data;
} simple_port_message;

simple_port* simple_port_create(simple_port* this, cdes_simulation* sim, cdes_task* on_received_message);
void simple_port_destroy(simple_port* this);
void simple_port_connect(simple_port* this, simple_port* other);
void simple_port_send(simple_port* this, simple_port_message* msg, cdes_time delay);

simple_port_message* simple_port_message_create(size_t size);
void simple_port_message_destroy(simple_port_message* this);

#endif  // __SIMPLE_PORT_H__