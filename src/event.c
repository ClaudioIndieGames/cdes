#include <assert.h>
#include "event.h"

event* event_create(event* e) {
    assert(e && "Passed NULL event");
    // allocating space for 2 tasks by default
    array_create_semi_dynamic(&e->registered_tasks, sizeof(task*), 2);
    return e;
}

void event_destroy(event* e) {
    assert(e && "Passed NULL event");
    array_destroy(&e->registered_tasks);
}

void event_register_task(event* e, task* t) {
    assert(e && "Passed NULL event");
    assert(t && "Passed NULL task");
    array_append_copy(&e->registered_tasks, &t);
}