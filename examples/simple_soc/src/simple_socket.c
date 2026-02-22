#include <stdlib.h>
#include <assert.h>
#include "simple_socket.h"

simple_payload* simple_payload_create(size_t size) {
    simple_payload* this = malloc(sizeof(simple_payload));
    assert(this && "Malloc failed");
    this->data = malloc(sizeof(char) * size);
    assert(this->data && "Malloc failed");
    return this;
}

void simple_payload_destroy(simple_payload* this) {
    assert(this && "Passed NULL payload");
    assert(this->data && "Passed NULL payload data");
    free(this->data);
    free(this);
}

simple_initiator_socket* simple_initiator_socket_create(simple_initiator_socket* this, cdes_simulation* sim, cdes_task* on_response_received) {
    assert(this && "Passed NULL simple initiator socket");
    assert(sim && "Passed NULL simulation");
    assert(on_response_received && "Passed NULL task");
    cdes_event_create(&this->response_received);
    this->sim = sim;
    cdes_event_register_task(&this->response_received, on_response_received);
    return this;
}

void simple_initiator_socket_destroy(simple_initiator_socket* this) {
    assert(this && "Passed NULL simple initiator socket");
    cdes_event_destroy(&this->response_received);
}

void simple_initiator_socket_connect(simple_initiator_socket* this, simple_target_socket* target) {
    assert(this && "Passed NULL simple initiator socket");
    assert(target && "Passed NULL simple target socket");
    assert(this->request_sent == NULL && "Simple initiator socket is already connected");
    assert(target->response_sent == NULL && "Simple target socket is already connected");
    this->request_sent = &target->request_received;
    target->response_sent = &this->response_received;
}

void simple_initiator_socket_send_request(simple_initiator_socket* this, simple_payload* payload, cdes_time delay) {
    assert(this && "Passed NULL simple initiator socket");
    assert(payload && "Passed NULL payload");
    cdes_simulation_event_notify(this->sim, this->request_sent, delay, payload);
}

simple_target_socket* simple_target_socket_create(simple_target_socket* this, cdes_simulation* sim, cdes_task* on_request_received) {
    assert(this && "Passed NULL simple target socket");
    assert(sim && "Passed NULL simulation");
    assert(on_request_received && "Passed NULL task");
    cdes_event_create(&this->request_received);
    this->sim = sim;
    cdes_event_register_task(&this->request_received, on_request_received);
    return this;
}

void simple_target_socket_destroy(simple_target_socket* this) {
    assert(this && "Passed NULL simple target socket");
    cdes_event_destroy(&this->request_received);
}

void simple_target_socket_connect(simple_target_socket* this, simple_initiator_socket* initiator) {
    simple_initiator_socket_connect(initiator, this);
}

void simple_target_socket_send_response(simple_target_socket* this, simple_payload* payload, cdes_time delay) {
    assert(this && "Passed NULL simple target socket");
    assert(payload && "Passed NULL payload");
    cdes_simulation_event_notify(this->sim, this->response_sent, delay, payload);
}