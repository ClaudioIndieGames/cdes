#ifndef __EVENT_H__
#define __EVENT_H__

#include "array.h"
#include "task.h"

typedef struct {
    array registered_tasks;
} event;

event* event_create(event* e);
void event_destroy(event* e);
void event_register_task(event* e, task* t);

#endif  // __EVENT_H__