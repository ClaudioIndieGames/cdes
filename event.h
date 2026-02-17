#ifndef __EVENT_H__
#define __EVENT_H__

#include <assert.h>
#include <pthread.h>
#include "array/array.h"

typedef void* (*pthread_func_t)(void*);
typedef size_t simulation_time_t;

typedef struct {
    pthread_func_t func;
    void* module;
    size_t priority;
} task;

typedef struct {
    task* task;
    void* arg;
} thread_func;


typedef struct {
    array funcs;
    pthread_t id;
} thread;

typedef struct {
    array registered_tasks;
} event;

typedef struct {
    event* event;
    simulation_time_t scheduled_time;
    void* arg;
} scheduled_event;

typedef struct {
    array scheduled_events;
    array threads;
} event_scheduler;

typedef struct {
    event_scheduler scheduler;
    simulation_time_t time;
} simulation;

task* task_create(task* t, pthread_func_t func, void* module, size_t priority) {
    assert(t && "Passed NULL task");
    assert(func && "Passed NULL function pointer");
    assert(module && "Passed NULL module");
    t->func = func;
    t->module = module;
    t->priority = priority;
    return t;
}

void task_destroy(task* t) {
    assert(t && "Passed NULL task");
    // nop
}

thread_func* thread_func_create(thread_func* tf, task* t, void* arg) {
    assert(tf && "Passed NULL thread function");
    assert(t && "Passed NULL task");
    tf->task = t;
    tf->arg = arg;
    return tf;
}

void thread_func_destroy(thread_func* tf) {
    assert(tf && "Passed NULL thread function");
    // nop
}

thread* thread_create(thread* t) {
    assert(t && "Passed NULL thread");
    // allocating space for 2 func by default
    array_create_semi_dynamic(&t->funcs, sizeof(thread_func), 2);
    return t;
}

void thread_destroy(thread* t, bool single_threaded) {
    assert(t && "Passed NULL thread");
    if (!single_threaded) {
        pthread_join(t->id, NULL);
    }
    for (size_t i = 0; i < array_size(&t->funcs); ++i) {
        thread_func* tf = array_at(&t->funcs, i);
        thread_func_destroy(tf);
    }
    array_destroy(&t->funcs);
}

void* thread_main(void* arg) {
    array* funcs = arg;
    for (size_t i = 0; i < array_size(funcs); ++i) {
        thread_func tf = *(thread_func*)array_at(funcs, i);
        tf.task->func(tf.arg);
    }
    return NULL;
}

void thread_run(thread* t, bool single_threaded) {
    if (!single_threaded) {
        pthread_create(&t->id, NULL, thread_main, &t->funcs);
    } else {
        thread_main(&t->funcs);
    }
}

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

void event_notify(simulation* sim, event* e, simulation_time_t delay, void* arg) {
    assert(sim && "Passed NULL simulation");
    assert(e && "Passed NULL event");
    assert(delay && "Cannot notify with zero delay");
    simulation_time_t scheduled_time = delay + sim->time;
    for (size_t i = 0; i < array_size(&sim->scheduler.scheduled_events); ++i) {
        scheduled_event* se = array_at(&sim->scheduler.scheduled_events, i);
        if (se->scheduled_time > scheduled_time) {
            scheduled_event* new_se = array_insert_slot(&sim->scheduler.scheduled_events, i);
            scheduled_event_create(new_se, e, scheduled_time, arg);
            return;
        }
    }
    scheduled_event* new_se = array_append_slot(&sim->scheduler.scheduled_events);
    scheduled_event_create(new_se, e, scheduled_time, arg);
}

event_scheduler* event_scheduler_create(event_scheduler* s) {
    assert(s && "Passed NULL event scheduler");
    // allocating space for 10 events by default
    array_create_semi_dynamic(&s->scheduled_events, sizeof(scheduled_event), 10);
    // allocating space for 2 threads by default
    array_create_semi_dynamic(&s->threads, sizeof(thread), 2);
    return s;
}

void event_scheduler_destroy(event_scheduler* s) {
    assert(s && "Passed NULL event scheduler");
    for (size_t i = 0; i < array_size(&s->scheduled_events); ++i) {
        scheduled_event_destroy(array_at(&s->scheduled_events, i));
    }
    array_destroy(&s->scheduled_events);
    if (!array_empty(&s->threads)) {
        assert(0 && "Threads array should be empty");
    }
    array_destroy(&s->threads);
}

scheduled_event* event_scheduler_front(event_scheduler* s) {
    assert(s && "Passed NULL event scheduler");
    if (!array_empty(&s->scheduled_events)) {
        return array_front(&s->scheduled_events);
    }
    return NULL;
}

scheduled_event* event_scheduler_next(event_scheduler* s) {
    assert(s && "Passed NULL event scheduler");
    if (!array_empty(&s->scheduled_events)) {
        array_pop(&s->scheduled_events);
        return event_scheduler_front(s);
    }
    return NULL;
}

simulation* simulation_create(simulation* sim) {
    assert(sim && "Passed NULL simulation");
    event_scheduler_create(&sim->scheduler);
    sim->time = 0;
    return sim;
}

void simulation_destroy(simulation* sim) {
    assert(sim && "Passed NULL simulation");
    event_scheduler_destroy(&sim->scheduler);
}

void simulation_start(simulation* sim, bool single_threaded) {
    assert(sim && "Passed NULL simulation");

    // initialize simulation time with the scheduled time of the first event
    scheduled_event* se = event_scheduler_front(&sim->scheduler);
    if (se != NULL) { sim->time = se->scheduled_time; }

    // iterate the scheduled events
    while (se != NULL) {
        
        // iterate the tasks registered to the current event
        array* rt = &se->event->registered_tasks;
        for (size_t i = 0; i < array_size(rt); ++i) {
            task* t = *(task**)array_at(rt, i);

            // check if a thread for the active module already exist, and if not create it
            thread* selected_thread = NULL;
            for (size_t j = 0; j < array_size(&sim->scheduler.threads); ++j) {
                thread* th = array_at(&sim->scheduler.threads, j);
                if (!array_empty(&th->funcs)) {
                    thread_func* tf = array_at(&th->funcs, 0);
                    if (tf->task->module == t->module) {
                        selected_thread = th;
                        break;
                    }
                }
            }
            if (selected_thread == NULL) {
                selected_thread = array_append_slot(&sim->scheduler.threads);
                thread_create(selected_thread);
            }

            // insert the task sorted by priority in the selected thread
            size_t pos = array_size(&selected_thread->funcs);
            for (size_t j = 0; j < array_size(&selected_thread->funcs); ++j) {
                thread_func* tf = array_at(&selected_thread->funcs, j);
                if (tf->task->priority < t->priority) {
                    pos = j;
                    break;
                }
            }
            thread_func* new_tf = array_insert_slot(&selected_thread->funcs, pos);
            thread_func_create(new_tf, t, se->arg);

        }

        // go to next event
        se = event_scheduler_next(&sim->scheduler);

        // create threads until the simulation time changes or no more events are scheduled
        if (se == NULL || sim->time != se->scheduled_time) {

            // run all threads
            for (size_t i = 0; i < array_size(&sim->scheduler.threads); ++i) {
                thread* t = array_at(&sim->scheduler.threads, i);
                thread_run(t, single_threaded);
            }

            // destroy all threads
            for (size_t i = 0; i < array_size(&sim->scheduler.threads); ++i) { 
                thread* t = array_at(&sim->scheduler.threads, i);
                thread_destroy(t, single_threaded);
            }
            array_clear(&sim->scheduler.threads);

            // after the run some new events might have been triggered
            se = event_scheduler_front(&sim->scheduler);
            
            // update simulation time
            if (se != NULL) { sim->time = se->scheduled_time; }
        }
    }

}

#endif  // __EVENT_H__