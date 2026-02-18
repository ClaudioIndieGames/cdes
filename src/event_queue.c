#include <assert.h>
#include "event_queue.h"

event_queue* event_queue_create(event_queue* eq, size_t initial_size) {
    assert(eq && "Passed NULL event ");
    array_create_semi_dynamic(&eq->scheduled_events, sizeof(scheduled_event), initial_size);
    return eq;
}

void event_queue_destroy(event_queue* eq) {
    assert(eq && "Passed NULL event queue");
    for (size_t i = 0; i < array_size(&eq->scheduled_events); ++i) {
        scheduled_event_destroy(array_at(&eq->scheduled_events, i));
    }
    array_destroy(&eq->scheduled_events);
}

void event_queue_push_copy(event_queue* eq, scheduled_event* se) {
    assert(eq && "Passed NULL event queue");
    assert(se && "Passed NULL scheduled event");
    for (size_t i = 0; i < array_size(&eq->scheduled_events); ++i) {
        scheduled_event* other_se = array_at(&eq->scheduled_events, i);
        if (scheduled_event_get_time(other_se) > scheduled_event_get_time(se)) {
            array_insert_copy(&eq->scheduled_events, se, i);
            return;
        }
    }
    array_append_copy(&eq->scheduled_events, se);
}

scheduled_event* event_queue_front(event_queue* eq) {
    assert(eq && "Passed NULL event queue");
    if (!array_empty(&eq->scheduled_events)) {
        return array_front(&eq->scheduled_events);
    }
    return NULL;
}

void event_queue_pop(event_queue* eq) {
    assert(eq && "Passed NULL event queue");
    if (!array_empty(&eq->scheduled_events)) {
        array_pop(&eq->scheduled_events);
    }
}

scheduled_event* scheduled_event_create(scheduled_event* se, event* e, simulation_time_t scheduled_time, void* arg) {
    assert(se && "Passed NULL scheduled event");
    assert(e && "Passed NULL event");
    se->event = e;
    se->scheduled_time = scheduled_time;
    se->arg = arg;
    return se;
}

void scheduled_event_destroy(scheduled_event* se) {
    assert(se && "Passed NULL scheduled event");
    // nop
}

simulation_time_t scheduled_event_get_time(scheduled_event* se) {
    assert(se && "Passed NULL scheduled event");
    return se->scheduled_time;
}