#include <assert.h>
#define ARRAY_IMPLEMENTATION
#include "cdes_simulation.h"
#include "cdes_thread.h"

cdes_simulation* cdes_simulation_create(cdes_simulation* sim) {
    assert(sim && "Passed NULL simulation");
    // allocating space for 10 events by default
    cdes_event_queue_create(&sim->eq, 10);
    sim->time = 0;
    cdes_event_create(&sim->on_simulation_start_event);
    // allocating space for 2 threads by default
    array_create_semi_dynamic(&sim->threads, sizeof(cdes_thread), 2);
    return sim;
}

void cdes_simulation_destroy(cdes_simulation* sim) {
    assert(sim && "Passed NULL simulation");
    cdes_event_queue_destroy(&sim->eq);
    cdes_event_destroy(&sim->on_simulation_start_event);
    if (!array_empty(&sim->threads)) {
        assert(0 && "There should not be any thread at this point");
    }
    array_destroy(&sim->threads);
}

void cdes_simulation_start(cdes_simulation* sim, cdes_simulation_mode mode) {
    assert(sim && "Passed NULL simulation");

    // schedule the on simulation start event first
    cdes_scheduled_event* se = &(cdes_scheduled_event){0};
    cdes_scheduled_event_create(se, &sim->on_simulation_start_event, 0, NULL);
    cdes_event_queue_push_copy(&sim->eq, se);

    // iterate the scheduled events
    while (se != NULL) {
        
        // iterate the tasks registered to the current event
        array* rt = &se->event->registered_tasks;
        for (size_t i = 0; i < array_size(rt); ++i) {
            cdes_task* t = *(cdes_task**)array_at(rt, i);

            // check if a thread for the active module already exist, and if not create it
            cdes_thread* selected_thread = NULL;
            for (size_t j = 0; j < array_size(&sim->threads); ++j) {
                cdes_thread* th = array_at(&sim->threads, j);
                if (!array_empty(&th->funcs)) {
                    cdes_thread_func* tf = array_at(&th->funcs, 0);
                    if (tf->task->module == t->module) {
                        selected_thread = th;
                        break;
                    }
                }
            }
            if (selected_thread == NULL) {
                selected_thread = array_append_slot(&sim->threads);
                cdes_thread_create(selected_thread);
            }

            // insert the task sorted by priority in the selected thread
            size_t pos = array_size(&selected_thread->funcs);
            for (size_t j = 0; j < array_size(&selected_thread->funcs); ++j) {
                cdes_thread_func* tf = array_at(&selected_thread->funcs, j);
                if (tf->task->priority < t->priority) {
                    pos = j;
                    break;
                }
            }
            cdes_thread_func* new_tf = array_insert_slot(&selected_thread->funcs, pos);
            cdes_thread_func_create(new_tf, t, se->args);

        }

        // go to next event
        cdes_event_queue_pop(&sim->eq);
        se = cdes_event_queue_front(&sim->eq);

        // create threads until the simulation time changes or no more events are scheduled
        if (se == NULL || sim->time != se->scheduled_time) {

            // run all threads
            for (size_t i = 0; i < array_size(&sim->threads); ++i) {
                cdes_thread* t = array_at(&sim->threads, i);
                cdes_thread_run(t, mode);
            }

            // destroy all threads
            for (size_t i = 0; i < array_size(&sim->threads); ++i) { 
                cdes_thread* t = array_at(&sim->threads, i);
                cdes_thread_destroy(t, mode);
            }
            array_clear(&sim->threads);

            // after the run some new events might have been triggered
            se = cdes_event_queue_front(&sim->eq);
            
            // update simulation time
            if (se != NULL) { sim->time = se->scheduled_time; }
        }
    }

}

cdes_time cdes_simulation_get_time(cdes_simulation* sim) {
    assert(sim && "Passed NULL simulation");
    return sim->time;
}

void cdes_simulation_event_notify(cdes_simulation* sim, cdes_event* e, cdes_time delay, void* args) {
    assert(sim && "Passed NULL simulation");
    assert(e && "Passed NULL event");
    assert(delay && "Cannot notify with zero delay");
    cdes_scheduled_event se;
    cdes_scheduled_event_create(&se, e, cdes_simulation_get_time(sim) + delay, args);
    cdes_event_queue_push_copy(&sim->eq, &se);
}

cdes_event* simulation_get_on_simulation_start_event(cdes_simulation* sim) {
    assert(sim && "Passed NULL simulation");
    return &sim->on_simulation_start_event;
}