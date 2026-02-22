#ifndef __AXI_SOCKET_H__
#define __AXI_SOCKET_H__

#include <stddef.h>
#include "cdes.h"

typedef enum {
    AXI_READ = 0,
    AXI_WRITE
} axi_payload_type;

typedef enum {
    AXI_READ_ADDR_START = 0,
    AXI_READ_ADDR_END,
    AXI_READ_DATA_START,
    AXI_READ_DATA_END,
    AXI_READ_DATA_LAST_START,
    AXI_READ_DATA_LAST_END
} axi_protocol_phase;

typedef struct {
    axi_payload_type type;
    size_t address;
    char* data;
    size_t data_length;
    size_t beat_index;
    axi_protocol_phase phase;
} axi_payload;

typedef struct {
    cdes_task* on_read_addr_end;
    cdes_task* on_read_data_start;
    cdes_task* on_read_data_last_start;
    cdes_event* read_addr_start;
    cdes_event read_addr_end;
    cdes_event read_data_start;
    cdes_event* read_data_end;
    cdes_event read_data_last_start;
    cdes_event* read_data_last_end;
    size_t data_width;
    cdes_simulation* sim;
    char connected;
} axi_initiator_socket;

typedef struct {
    cdes_task* on_read_addr_start;
    cdes_task* on_read_data_end;
    cdes_task* on_read_data_last_end;
    cdes_event read_addr_start;
    cdes_event* read_addr_end;
    cdes_event* read_data_start;
    cdes_event read_data_end;
    cdes_event* read_data_last_start;
    cdes_event read_data_last_end;
    size_t data_width;
    cdes_simulation* sim;
    char connected;
} axi_target_socket;

axi_payload* axi_payload_create(axi_payload* this);
void axi_payload_destroy(axi_payload* this);

axi_initiator_socket* axi_initiator_socket_create(axi_initiator_socket* this, cdes_simulation* sim, size_t data_width, cdes_task* on_read_addr_end, cdes_task* on_read_data_start, cdes_task* on_read_data_last_start);
void axi_initiator_socket_destroy(axi_initiator_socket* this);
void axi_initiator_socket_connect(axi_initiator_socket* this, axi_target_socket* target);
void axi_initiator_socket_send(axi_initiator_socket* this, axi_payload* payload, cdes_time delay);
void axi_initiator_socket_update(axi_initiator_socket* this, axi_payload* payload, cdes_time delay);

axi_target_socket* axi_target_socket_create(axi_target_socket* this, cdes_simulation* sim, size_t data_width, cdes_task* on_read_addr_start, cdes_task* on_read_data_end, cdes_task* on_read_data_last_end);
void axi_target_socket_destroy(axi_target_socket* this);
void axi_target_socket_connect(axi_target_socket* this, axi_initiator_socket* initiator);
void axi_target_socket_update(axi_target_socket* this, axi_payload* payload, cdes_time delay);

#endif  // __AXI_SOCKET_H__