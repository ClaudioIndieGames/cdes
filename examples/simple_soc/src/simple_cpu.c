#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "simple_cpu.h"

static void* on_sim_start(void* module, void* args) {
    (void)args;
    simple_cpu* this = module;

    simple_payload* payload = simple_payload_create(20);
    cdes_time delay = 10e-3;
    payload->from = this;
    strcpy(payload->data, "Hi Memory\0");
    simple_initiator_socket_send_request(&this->out, payload, delay);

    printf("[%s, %ld, CPU] Sending '%s' in %s\n",
        *cdes_time_to_string(cdes_simulation_get_time(this->sim), &(cdes_time_string){}),
        (unsigned long)pthread_self(),
        payload->data,
        *cdes_time_to_string(delay, &(cdes_time_string){}));

    return NULL;
}

static void* on_response_received(void* module, void* args) {
    simple_cpu* this = module;
    simple_payload* payload = args;

    printf("[%s, %ld, CPU] Received '%s' from %p\n",
        *cdes_time_to_string(cdes_simulation_get_time(this->sim), &(cdes_time_string){}),
        (unsigned long)pthread_self(),
        payload->data,
        payload->from);
    
    simple_payload_destroy(payload);

    return NULL;
}

simple_cpu* simple_cpu_create(simple_cpu* this, cdes_simulation* sim) {
    cdes_task_create(&this->on_sim_start, this, on_sim_start, 0);
    cdes_task_create(&this->on_response_received, this, on_response_received, 1);
    cdes_event_register_task(simulation_get_on_simulation_start_event(sim), &this->on_sim_start);
    simple_initiator_socket_create(&this->out, sim, &this->on_response_received);
    this->sim = sim;
    return this;
}

void simple_cpu_destroy(simple_cpu* this) {
    cdes_task_destroy(&this->on_sim_start);
    cdes_task_destroy(&this->on_response_received);
    simple_initiator_socket_destroy(&this->out);
}