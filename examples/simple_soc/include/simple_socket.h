#ifndef __SIMPLE_SOCKET_H__
#define __SIMPLE_SOCKET_H__

#include <stddef.h>
#include "cdes.h"

typedef struct {
    void* from;
    char* data;
} simple_payload;

typedef struct {
    cdes_event* request_sent;
    cdes_event response_received;
    cdes_simulation* sim;
} simple_initiator_socket;

typedef struct {
    cdes_event* response_sent;
    cdes_event request_received;
    cdes_simulation* sim;
} simple_target_socket;

simple_payload* simple_payload_create(size_t size);
void simple_payload_destroy(simple_payload* this);

simple_initiator_socket* simple_initiator_socket_create(simple_initiator_socket* this, cdes_simulation* sim, cdes_task* on_response_received);
void simple_initiator_socket_destroy(simple_initiator_socket* this);
void simple_initiator_socket_connect(simple_initiator_socket* this, simple_target_socket* target);
void simple_initiator_socket_send_request(simple_initiator_socket* this, simple_payload* payload, cdes_time delay);

simple_target_socket* simple_target_socket_create(simple_target_socket* this, cdes_simulation* sim, cdes_task* on_request_received);
void simple_target_socket_destroy(simple_target_socket* this);
void simple_target_socket_connect(simple_target_socket* this, simple_initiator_socket* initiator);
void simple_target_socket_send_response(simple_target_socket* this, simple_payload* payload, cdes_time delay);

#endif  // __SIMPLE_SOCKET_H__