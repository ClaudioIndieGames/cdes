#include <stdlib.h>
#include <assert.h>
#include "simple_port.h"

simple_port* simple_port_create(simple_port* this, cdes_simulation* sim, cdes_task* on_received_message) {
    assert(this && "Passed NULL simple port");
    assert(sim && "Passed NULL simulation");
    assert(on_received_message && "Passed NULL task");
    cdes_event_create(&this->received_message);
    this->sim = sim;
    cdes_event_register_task(&this->received_message, on_received_message);
    return this;
}

void simple_port_destroy(simple_port* this) {
    assert(this && "Passed NULL simple port");
    cdes_event_destroy(&this->received_message);
}

void simple_port_connect(simple_port* this, simple_port* other) {
    assert(this && "Passed NULL simple port");
    assert(other && "Passed NULL simple port");
    this->sent_message = &other->received_message;
    other->sent_message = &this->received_message;
}

void simple_port_send(simple_port* this, simple_port_message* msg, cdes_time delay) {
    assert(this && "Passed NULL simple port");
    assert(msg && "Passed NULL message");
    cdes_simulation_event_notify(this->sim, this->sent_message, delay, msg);
}

simple_port_message* simple_port_message_create(size_t size) {
    simple_port_message* this = malloc(sizeof(simple_port_message));
    assert(this && "Malloc failed");
    this->data = malloc(sizeof(char) * size);
    assert(this->data && "Malloc failed");
    return this;
}

void simple_port_message_destroy(simple_port_message* this) {
    assert(this && "Passed NULL message");
    assert(this->data && "Passed NULL data");
    free(this->data);
    free(this);
}