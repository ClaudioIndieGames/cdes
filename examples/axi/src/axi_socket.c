#include <stdlib.h>
#include <assert.h>
#include "axi_socket.h"

axi_payload* axi_payload_create(axi_payload* this) {
    if (this == NULL) {
        this = malloc(sizeof(axi_payload));
        assert(this && "Malloc failed");
    }
    return this;
}

void axi_payload_validate(axi_payload* this) {
    assert(this && "Passed NULL payload");
    assert(this->data_length && "Payload must have data length greater than zero");
}  

void axi_payload_destroy(axi_payload* this) {
    assert(this && "Passed NULL payload");
    free(this);
}

axi_initiator_socket* axi_initiator_socket_create(axi_initiator_socket* this, cdes_simulation* sim, size_t data_width, cdes_task* on_read_addr_end, cdes_task* on_read_data_start, cdes_task* on_read_data_last_start) {
    assert(this && "Passed NULL axi initiator socket");
    assert(sim && "Passed NULL simulation");
    assert(data_width && "Data width must be greater than zero");
    this->sim = sim;
    this->data_width = data_width;
    this->on_read_addr_end = on_read_addr_end;
    this->on_read_data_start = on_read_data_start;
    this->on_read_data_last_start = on_read_data_last_start;
    if (on_read_addr_end) {
        cdes_event_create(&this->read_addr_end);
        cdes_event_register_task(&this->read_addr_end, on_read_addr_end);
    }
    if (on_read_data_start) {
        cdes_event_create(&this->read_data_start);
        cdes_event_register_task(&this->read_data_start, on_read_data_start);
    }
    if (on_read_data_last_start) {
        cdes_event_create(&this->read_data_last_start);
        cdes_event_register_task(&this->read_data_last_start, on_read_data_last_start);
    }
    return this;
}

void axi_initiator_socket_destroy(axi_initiator_socket* this) {
    assert(this && "Passed NULL axi initiator socket");
    if (this->on_read_addr_end) {
        cdes_event_destroy(&this->read_addr_end);
    }
    if (this->on_read_data_start) {
        cdes_event_destroy(&this->read_data_start);
    }
    if (this->on_read_data_last_start) {
        cdes_event_destroy(&this->read_data_last_start);
    }
}

void axi_initiator_socket_connect(axi_initiator_socket* this, axi_target_socket* target) {
    assert(this && "Passed NULL axi initiator socket");
    assert(target && "Passed NULL axi target socket");
    assert(this->data_width == target->data_width && "axi initiator and target sockets have different data_width");
    assert(this->connected == 0 && "axi initiator socket is already connected");
    assert(target->connected == 0 && "axi target socket is already connected");
    if (target->on_read_addr_start) { this->read_addr_start = &target->read_addr_start; }
    if (this->on_read_addr_end) { target->read_addr_end = &this->read_addr_end; }
    if (this->on_read_data_start) { target->read_data_start = &this->read_data_start; }
    if (target->on_read_data_end) { this->read_data_end = &target->read_data_end; }
    if (this->on_read_data_last_start) { target->read_data_last_start = &this->read_data_last_start; }
    if (target->on_read_data_last_end) { this->read_data_last_end = &target->read_data_last_end; }
    this->connected = 1;
    target->connected = 1;
}

void axi_initiator_socket_send(axi_initiator_socket* this, axi_payload* payload, cdes_time delay) {
    assert(this && "Passed NULL axi initiator socket");
    axi_payload_validate(payload);
    payload->phase = AXI_READ_ADDR_START;
    if (this->read_addr_start) {
        cdes_simulation_event_notify(this->sim, this->read_addr_start, delay, payload);
    } else {
        assert(0 && "Read addr start not implemented");
    }
}

void axi_initiator_socket_update(axi_initiator_socket* this, axi_payload* payload, cdes_time delay) {
    assert(this && "Passed NULL axi initiator socket");
    axi_payload_validate(payload);
    switch(payload->phase) {
        case AXI_READ_ADDR_END:
            // nop
            break;
        case AXI_READ_DATA_START:
            payload->phase = AXI_READ_DATA_END;
            if (this->read_data_end) {
                cdes_simulation_event_notify(this->sim, this->read_data_end, delay, payload);
            }
            break;
        case AXI_READ_DATA_LAST_START:
            payload->phase = AXI_READ_DATA_LAST_END;
            if (this->read_data_last_end) {
                cdes_simulation_event_notify(this->sim, this->read_data_last_end, delay, payload);
            }
            break;
        default:
            assert(0 && "Unknown protocol phase");
    }
}

axi_target_socket* axi_target_socket_create(axi_target_socket* this, cdes_simulation* sim, size_t data_width, cdes_task* on_read_addr_start, cdes_task* on_read_data_end, cdes_task* on_read_data_last_end) {
    assert(this && "Passed NULL axi target socket");
    assert(sim && "Passed NULL simulation");
    assert(data_width && "Data width must be greater than zero");
    this->sim = sim;
    this->data_width = data_width;
    this->on_read_addr_start = on_read_addr_start;
    this->on_read_data_end = on_read_data_end;
    this->on_read_data_last_end = on_read_data_last_end;
    if (on_read_addr_start) {
        cdes_event_create(&this->read_addr_start);
        cdes_event_register_task(&this->read_addr_start, on_read_addr_start);
    }
    if (on_read_data_end) {
        cdes_event_create(&this->read_data_end);
        cdes_event_register_task(&this->read_data_end, on_read_data_end);
    }
    if (on_read_data_last_end) {
        cdes_event_create(&this->read_data_last_end);
        cdes_event_register_task(&this->read_data_last_end, on_read_data_last_end);
    }
    return this;
}

void axi_target_socket_destroy(axi_target_socket* this) {
    assert(this && "Passed NULL axi target socket");
    if (this->on_read_addr_start) {
        cdes_event_destroy(&this->read_addr_start);
    }
    if (this->on_read_data_end) {
        cdes_event_destroy(&this->read_data_end);
    }
    if (this->on_read_data_last_end) {
        cdes_event_destroy(&this->read_data_last_end);
    }
}

void axi_target_socket_connect(axi_target_socket* this, axi_initiator_socket* initiator) {
    axi_initiator_socket_connect(initiator, this);
}

void axi_target_socket_update(axi_target_socket* this, axi_payload* payload, cdes_time delay) {
    assert(this && "Passed NULL axi target socket");
    axi_payload_validate(payload);
    switch(payload->phase) {
        case AXI_READ_ADDR_START:
            payload->phase = AXI_READ_ADDR_END;
            if (this->read_addr_end) {
                cdes_simulation_event_notify(this->sim, this->read_addr_end, delay, payload);
            }
            break;
        case AXI_READ_ADDR_END:
            if (payload->data_length > this->data_width) {
                payload->phase = AXI_READ_DATA_START;
                if (this->read_data_start) {
                    cdes_simulation_event_notify(this->sim, this->read_data_start, delay, payload);
                }
            } else {
                payload->phase = AXI_READ_DATA_LAST_START;
                if (this->read_data_last_start) {
                    cdes_simulation_event_notify(this->sim, this->read_data_last_start, delay, payload);
                }
            }
            break;
        case AXI_READ_DATA_END:
            payload->beat_index += 1;
            if (payload->data_length > payload->beat_index * this->data_width) {
                payload->phase = AXI_READ_DATA_START;
                if (this->read_data_start) {
                    cdes_simulation_event_notify(this->sim, this->read_data_start, delay, payload);
                }
            } else {
                payload->phase = AXI_READ_DATA_LAST_START;
                if (this->read_data_last_start) {
                    cdes_simulation_event_notify(this->sim, this->read_data_last_start, delay, payload);
                }
            }
            break;
        case AXI_READ_DATA_LAST_END:
            // nop
            break;
        default:
            assert(0 && "Unknown protocol phase");
    }
}