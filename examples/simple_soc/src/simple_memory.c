#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "simple_memory.h"

static void* on_request_received(void* module, void* args) {
    simple_memory* this = module;
    simple_payload* payload = args;

    printf("[%s, %ld, Memory] Received '%s' from %p\n",
        *cdes_time_to_string(cdes_simulation_get_time(this->sim), &(cdes_time_string){}),
        (unsigned long)pthread_self(),
        payload->data,
        payload->from);
    
    cdes_time delay = 10e-3;
    payload->from = this;
    strcpy(payload->data, "Hi CPU\0");
    simple_target_socket_send_response(&this->in, payload, delay);

    printf("[%s, %ld, Memory] Sending '%s' in %s\n",
        *cdes_time_to_string(cdes_simulation_get_time(this->sim), &(cdes_time_string){}),
        (unsigned long)pthread_self(),
        payload->data,
        *cdes_time_to_string(delay, &(cdes_time_string){}));

    return NULL;
}

simple_memory* simple_memory_create(simple_memory* this, cdes_simulation* sim) {
    cdes_task_create(&this->on_request_received, this, on_request_received, 0);
    simple_target_socket_create(&this->in, sim, &this->on_request_received);
    this->sim = sim;
    return this;
}

void simple_memory_destroy(simple_memory* this) {
    cdes_task_destroy(&this->on_request_received);
    simple_target_socket_destroy(&this->in);
}