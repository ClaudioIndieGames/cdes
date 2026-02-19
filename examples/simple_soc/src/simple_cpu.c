#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "simple_cpu.h"

static void* on_sim_start(void* module, void* args) {
    (void)args;
    simple_cpu* this = module;
    printf("[%ld, %ld, CPU] Sending 'Hi Memory' in 10\n", cdes_simulation_get_time(this->sim), (unsigned long)pthread_self());
    simple_port_message* msg = simple_port_message_create(20);
    msg->from = this;
    strcpy(msg->data, "Hi Memory\0");
    simple_port_send(&this->p, msg, 10);
    return NULL;
}

static void* on_receive_msg(void* module, void* args) {
    simple_cpu* this = module;
    simple_port_message* msg = args;
    printf("[%ld, %ld, CPU] Received '%s' from %p\n", cdes_simulation_get_time(this->sim), (unsigned long)pthread_self(), msg->data, msg->from);
    simple_port_message_destroy(msg);
    return NULL;
}

simple_cpu* simple_cpu_create(simple_cpu* this, cdes_simulation* sim) {
    cdes_task_create(&this->on_sim_start, this, on_sim_start, 1);
    cdes_task_create(&this->on_received_message, this, on_receive_msg, 0);
    cdes_event_register_task(simulation_get_on_simulation_start_event(sim), &this->on_sim_start);
    simple_port_create(&this->p, sim, &this->on_received_message);
    this->sim = sim;
    return this;
}

void simple_cpu_destroy(simple_cpu* this) {
    cdes_task_destroy(&this->on_sim_start);
    cdes_task_destroy(&this->on_received_message);
    simple_port_destroy(&this->p);
}