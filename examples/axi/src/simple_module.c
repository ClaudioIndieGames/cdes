#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "simple_module.h"

static void* on_sim_start(void* module, void* arg) {
    (void)arg;
    simple_module* this = module;

    axi_payload* payload = axi_payload_create(NULL);
    payload->type = AXI_READ;
    payload->address = 0x1000;
    payload->data_length = 64;
    payload->data = malloc(sizeof(char) * payload->data_length);
    cdes_time delay = 1e-3;
    axi_protocol_engine_update(this->out.pe, payload, delay);
    printf("[%s, %ld, out] Sending read request in %s\n",
        *cdes_time_to_string(cdes_simulation_get_time(this->sim), &(cdes_time_string){}), (unsigned long)pthread_self(), *cdes_time_to_string(delay, &(cdes_time_string){}));
    
    return NULL;
}

static void* on_read_addr_start(void* module, void* arg) {
    simple_module* this = module;
    axi_payload* payload = arg;

    printf("[%s, %ld, in] READ ADDR START\n",
        *cdes_time_to_string(cdes_simulation_get_time(this->sim), &(cdes_time_string){}), (unsigned long)pthread_self());

    axi_protocol_engine_update(this->in.pe, payload, 1e-3);

    payload->data[0] = 42;
    axi_protocol_engine_update(this->in.pe, payload, 10e-3);

    return NULL;
}

static void* on_read_addr_end(void* module, void* arg) {
    simple_module* this = module;
    axi_payload* payload = arg;
    (void)payload;

    printf("[%s, %ld, out] READ ADDR END\n",
        *cdes_time_to_string(cdes_simulation_get_time(this->sim), &(cdes_time_string){}), (unsigned long)pthread_self());

//    axi_protocol_engine_update(&this->out.pe, payload, 1e-3);

    return NULL;
}

static void* on_read_data_start(void* module, void* arg) {
    simple_module* this = module;
    axi_payload* payload = arg;

    printf("[%s, %ld, out] READ DATA START\n",
        *cdes_time_to_string(cdes_simulation_get_time(this->sim), &(cdes_time_string){}), (unsigned long)pthread_self());

    axi_protocol_engine_update(this->out.pe, payload, 1e-3);  
    
    return NULL;
}

static void* on_read_data_end(void* module, void* arg) {
    simple_module* this = module;
    axi_payload* payload = arg;

    printf("[%s, %ld, in] READ DATA END\n",
        *cdes_time_to_string(cdes_simulation_get_time(this->sim), &(cdes_time_string){}), (unsigned long)pthread_self());

    axi_protocol_engine_update(this->in.pe, payload, 1e-3);

    return NULL;
}

static void* on_read_data_last_start(void* module, void* arg) {
    simple_module* this = module;
    axi_payload* payload = arg;

    printf("[%s, %ld, out] READ DATA LAST START\n",
        *cdes_time_to_string(cdes_simulation_get_time(this->sim), &(cdes_time_string){}), (unsigned long)pthread_self());

    axi_protocol_engine_update(this->out.pe, payload, 1e-3);

    return NULL;  
}

static void* on_read_data_last_end(void* module, void* arg) {
    simple_module* this = module;
    axi_payload* payload = arg;

    printf("[%s, %ld, in] READ DATA LAST END\n",
        *cdes_time_to_string(cdes_simulation_get_time(this->sim), &(cdes_time_string){}), (unsigned long)pthread_self());

//    axi_protocol_engine_update(this->in.pe, payload, 1e-3); 

    free(payload->data);
    axi_payload_destroy(payload);

    return NULL;
}

simple_module* simple_module_create(simple_module* this, cdes_simulation* sim) {
    cdes_task_create(&this->on_sim_start, this, on_sim_start, 0);
    cdes_task_create(&this->on_read_addr_start, this, on_read_addr_start, 1);
    cdes_task_create(&this->on_read_addr_end, this, on_read_addr_end, 2);
    cdes_task_create(&this->on_read_data_start, this, on_read_data_start, 3);
    cdes_task_create(&this->on_read_data_end, this, on_read_data_end, 4);
    cdes_task_create(&this->on_read_data_last_start, this, on_read_data_last_start, 5);
    cdes_task_create(&this->on_read_data_last_end, this, on_read_data_last_end, 6);
    axi_target_socket_create(&this->in, sim, 32, &this->on_read_addr_start, &this->on_read_data_end, &this->on_read_data_last_end);
    axi_initiator_socket_create(&this->out, sim, 32, &this->on_read_addr_end, &this->on_read_data_start, &this->on_read_data_last_start);
    this->sim = sim;
    cdes_event_register_task(simulation_get_on_simulation_start_event(sim), &this->on_sim_start);
    return this;
}

void simple_module_destroy(simple_module* this) {
    cdes_task_destroy(&this->on_sim_start);
    cdes_task_destroy(&this->on_read_addr_start);
    cdes_task_destroy(&this->on_read_addr_end);
    cdes_task_destroy(&this->on_read_data_start);
    cdes_task_destroy(&this->on_read_data_end);
    cdes_task_destroy(&this->on_read_data_last_start);
    cdes_task_destroy(&this->on_read_data_last_end);
    axi_target_socket_destroy(&this->in);
    axi_initiator_socket_destroy(&this->out);
}