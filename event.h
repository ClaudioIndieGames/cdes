#ifndef __EVENT_H__
#define __EVENT_H__

#include <assert.h>
#include <pthread.h>
#include "array/array.h"

typedef void* (*task_t)(void*);

typedef struct {
    array tasks;
    unsigned long scheduled_time;
} event;

event* event_create(event* e) {
    assert(e && "Passed NULL event");
    // allocating space for 2 tasks by default
    array_create_semi_dynamic(&e->tasks, sizeof(task_t), 2);
    return e;
}

array* event_tasks(event* e) {
    assert(e && "Passed NULL event");
    return &e->tasks;
}

void event_register_task(event* e, task_t c) {
    assert(e && "Passed NULL event");
    array_append_copy(&e->tasks, &c);
}

void event_destroy(event* e) {
    assert(e && "Passed NULL event");
    array_destroy(&e->tasks);
}

typedef struct {
    array event_queue;
    array threads;
    unsigned long sim_time;
} event_scheduler;

event_scheduler* event_scheduler_create(event_scheduler* s) {
    assert(s && "Passed NULL event scheduler");
    // allocating space for 10 events by default
    array_create_semi_dynamic(&s->event_queue, sizeof(event), 10);
    // allocating space for 2 threads by default
    array_create_semi_dynamic(&s->threads, sizeof(pthread_t), 2);
    s->sim_time = 0;
    return s;
}

void event_scheduler_destroy(event_scheduler* s) {
    assert(s && "Passed NULL event scheduler");
    array_destroy(&s->event_queue);
}

void event_scheduler_schedule(event_scheduler* s, event* e, unsigned long delay) {
    assert(s && "Passed NULL event scheduler");
    assert(e && "Passed NULL event");
    e->scheduled_time = delay + s->sim_time;
    for (size_t i = 0; i < array_size(&s->event_queue); ++i) {
        event* e_scheduled = array_at(&s->event_queue, i);
        if (e_scheduled->scheduled_time > e->scheduled_time) {
            array_insert_copy(&s->event_queue, e, i);
            return;
        }
    }
    array_append_copy(&s->event_queue, e);
}

event* event_scheduler_front(event_scheduler* s) {
    assert(s && "Passed NULL event scheduler");
    if (!array_empty(&s->event_queue)) {
        return array_front(&s->event_queue);
    }
    return NULL;
}

event* event_scheduler_next(event_scheduler* s) {
    assert(s && "Passed NULL event scheduler");
    if (!array_empty(&s->event_queue)) {
        array_pop(&s->event_queue);
        return event_scheduler_front(s);
    }
    return NULL;
}

void event_scheduler_run_single(event_scheduler* s) {
    assert(s && "Passed NULL event scheduler");
    event* e_scheduled = event_scheduler_front(s);
    while (e_scheduled != NULL) {
        s->sim_time = e_scheduled->scheduled_time;
        array* tasks = event_tasks(e_scheduled);
        for (size_t i = 0; i < array_size(tasks); ++i) {
            task_t task = *(task_t*)array_at(tasks, i);
            task(NULL);
        }
        e_scheduled = event_scheduler_next(s);
    }
}

void event_scheduler_run_multi(event_scheduler* s) {
    assert(s && "Passed NULL event scheduler");
    event* e_scheduled = event_scheduler_front(s);
    while (e_scheduled != NULL) {
        s->sim_time = e_scheduled->scheduled_time;
        array* tasks = event_tasks(e_scheduled);
        for (size_t i = 0; i < array_size(tasks); ++i) {
            task_t task = *(task_t*)array_at(tasks, i);
            pthread_t* thread = array_append_slot(&s->threads);
            pthread_create(thread, NULL, task, NULL);
        }
        e_scheduled = event_scheduler_next(s);
        if (e_scheduled == NULL || e_scheduled->scheduled_time > s->sim_time) {
            for (size_t i = 0; i < array_size(&s->threads); ++i) {
                pthread_join(*(pthread_t*)array_at(&s->threads, i), NULL);
            }
            array_clear(&s->threads);
            e_scheduled = event_scheduler_front(s);
        }
    }
}

#endif  // __EVENT_H__