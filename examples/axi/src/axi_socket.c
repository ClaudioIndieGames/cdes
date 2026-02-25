#include <stdlib.h>
#include <assert.h>
#include "axi_socket.h"

axi_payload* axi_payload_create(axi_payload* this) {
    if (this == NULL) {
        this = malloc(sizeof(axi_payload));
        assert(this && "Malloc failed");
    }
    this->address = 0x0;
    this->type = AXI_READ;
    this->beat_index = 0;
    this->state = AXI_READ_REQ_INIT;
    return this;
}

void axi_payload_destroy(axi_payload* this) {
    assert(this && "Passed NULL payload");
    free(this);
}

void axi_payload_validate(axi_payload* this) {
    assert(this && "Passed NULL payload");
    assert(this->data_length && "Payload must have data length greater than zero");
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
    if (this->pe) {
        axi_protocol_engine_destroy(this->pe);
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

void axi_socket_connect(axi_initiator_socket* initiator, axi_target_socket* target) {
    assert(initiator && "Passed NULL axi initiator socket");
    assert(target && "Passed NULL axi target socket");
    assert(initiator->data_width == target->data_width && "axi initiator and target sockets have different data_width");
    assert(initiator->pe == NULL && "axi initiator socket is already connected");
    assert(target->pe == NULL && "axi target socket is already connected");
    initiator->pe = axi_protocol_engine_create(initiator, target);
    target->pe = initiator->pe;
}

static void* axi_protocol_engine_fsm(void* module, void* arg) {
    axi_protocol_engine* this = module;
    axi_payload* payload = arg;

    cdes_simulation* sim = this->initiator->sim;
    cdes_time delay = 1e-3;
    switch(payload->state) {
        case AXI_READ_REQ_INIT:
            payload->state = AXI_READ_ADDR_START;
            if (this->target->on_read_addr_start) {
                cdes_simulation_event_notify(sim, &this->target->read_addr_start, delay, payload);
            } else {
                assert(0 && "on_read_addr_start must be implemented!");
            }
            break;
        case AXI_READ_ADDR_START:
            payload->state = AXI_READ_ADDR_END;
            if (this->initiator->on_read_addr_end) {
                cdes_simulation_event_notify(sim, &this->initiator->read_addr_end, delay, payload);
            }
            break;
        case AXI_READ_ADDR_END:
            if (payload->data_length > this->initiator->data_width) {
                payload->state = AXI_READ_DATA_START;
                if (this->initiator->on_read_data_start) {
                    cdes_simulation_event_notify(sim, &this->initiator->read_data_start, delay, payload);
                } else {
                    cdes_simulation_event_notify(sim, &this->next_state, delay, payload);
                }
            } else {
                payload->state = AXI_READ_DATA_LAST_START;
                if (this->initiator->on_read_data_last_start) {
                    cdes_simulation_event_notify(sim, &this->initiator->read_data_last_start, delay, payload);
                } else {
                    cdes_simulation_event_notify(sim, &this->next_state, delay, payload);
                }
            }
            break;
        case AXI_READ_DATA_START:
            payload->state = AXI_READ_DATA_END;
            if (this->target->on_read_data_end) {
                cdes_simulation_event_notify(sim,  &this->target->read_data_end, delay, payload);
            } else {
                cdes_simulation_event_notify(sim, &this->next_state, delay, payload);
            }
            break;
        case AXI_READ_DATA_END:
            payload->beat_index += 1;
            if (payload->data_length > (payload->beat_index + 1) * this->initiator->data_width) {
                payload->state = AXI_READ_DATA_START;
                if (this->initiator->on_read_data_start) {
                    cdes_simulation_event_notify(sim, &this->initiator->read_data_start, delay, payload);
                } else {
                    cdes_simulation_event_notify(sim, &this->next_state, delay, payload);
                }
            } else {
                payload->state = AXI_READ_DATA_LAST_START;
                if (this->initiator->on_read_data_last_start) {
                    cdes_simulation_event_notify(sim, &this->initiator->read_data_last_start, delay, payload);
                } else {
                    cdes_simulation_event_notify(sim, &this->next_state, delay, payload);
                }
            }
            break;
        case AXI_READ_DATA_LAST_START:
            payload->state = AXI_READ_DATA_LAST_END;
            if (this->target->on_read_data_last_end) {
                cdes_simulation_event_notify(sim, &this->target->read_data_last_end, delay, payload);
            } else {
                cdes_simulation_event_notify(sim, &this->next_state, delay, payload);
            }
            break;
        case AXI_READ_DATA_LAST_END:
            // nop
            break;
        default:
            assert(0 && "Unknown protocol state");
    }

    return NULL;
}

axi_protocol_engine* axi_protocol_engine_create(axi_initiator_socket* initiator, axi_target_socket* target) {
    assert(initiator && "Passed NULL axi initiator socket");
    assert(target && "Passed NULL axi target socket");
    axi_protocol_engine* this = malloc(sizeof(axi_protocol_engine));
    assert(this && "Malloc failed");

    cdes_task_create(&this->axi_protocol_engine_fsm, this, axi_protocol_engine_fsm, 0);
    cdes_event_create(&this->next_state);
    cdes_event_register_task(&this->next_state, &this->axi_protocol_engine_fsm);
    this->initiator = initiator;
    this->target = target;
    return this;
}

void axi_protocol_engine_destroy(axi_protocol_engine* this) {
    assert(this && "Passed NULL axi protocol engine");
    cdes_task_destroy(&this->axi_protocol_engine_fsm);
    cdes_event_destroy(&this->next_state);
    free(this);
}

void axi_protocol_engine_update(axi_protocol_engine* this, axi_payload* payload, cdes_time delay) {
    assert(this && "Passed NULL axi protocol engine");
    axi_payload_validate(payload);
    cdes_simulation_event_notify(this->initiator->sim, &this->next_state, delay, payload);
}