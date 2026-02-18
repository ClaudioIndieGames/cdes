#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__

#include "event.h"
#include "simulation_time.h"

typedef struct {
    array scheduled_events;
} event_queue;

typedef struct {
    event* event;
    simulation_time_t scheduled_time;
    void* arg;
} scheduled_event;

event_queue* event_queue_create(event_queue* eq, size_t initial_size);
void event_queue_destroy(event_queue* eq);
void event_queue_push_copy(event_queue* eq, scheduled_event* se);
scheduled_event* event_queue_front(event_queue* eq);
void event_queue_pop(event_queue* eq);

scheduled_event* scheduled_event_create(scheduled_event* se, event* e, simulation_time_t scheduled_time, void* arg);
void scheduled_event_destroy(scheduled_event* se);
simulation_time_t scheduled_event_get_time(scheduled_event* se);

#endif  // __EVENT_QUEUE_H__