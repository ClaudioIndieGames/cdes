#include <assert.h>
#include "simulation.h"
#include "thread.h"

simulation* simulation_create(simulation* sim) {
    assert(sim && "Passed NULL simulation");
    // allocating space for 10 events by default
    event_queue_create(&sim->eq, 10);
    sim->time = 0;
    // allocating space for 2 threads by default
    array_create_semi_dynamic(&sim->threads, sizeof(thread), 2);
    return sim;
}

void simulation_destroy(simulation* sim) {
    assert(sim && "Passed NULL simulation");
    event_queue_destroy(&sim->eq);
    if (!array_empty(&sim->threads)) {
        assert(0 && "There should not be any thread at this point");
    }
    array_destroy(&sim->threads);
}

void simulation_start(simulation* sim, char is_single_threaded) {
    assert(sim && "Passed NULL simulation");

    // initialize simulation time with the scheduled time of the first event
    scheduled_event* se = event_queue_front(&sim->eq);
    if (se != NULL) { sim->time = se->scheduled_time; }

    // iterate the scheduled events
    while (se != NULL) {
        
        // iterate the tasks registered to the current event
        array* rt = &se->event->registered_tasks;
        for (size_t i = 0; i < array_size(rt); ++i) {
            task* t = *(task**)array_at(rt, i);

            // check if a thread for the active module already exist, and if not create it
            thread* selected_thread = NULL;
            for (size_t j = 0; j < array_size(&sim->threads); ++j) {
                thread* th = array_at(&sim->threads, j);
                if (!array_empty(&th->funcs)) {
                    thread_func* tf = array_at(&th->funcs, 0);
                    if (tf->task->module == t->module) {
                        selected_thread = th;
                        break;
                    }
                }
            }
            if (selected_thread == NULL) {
                selected_thread = array_append_slot(&sim->threads);
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
        event_queue_pop(&sim->eq);
        se = event_queue_front(&sim->eq);

        // create threads until the simulation time changes or no more events are scheduled
        if (se == NULL || sim->time != se->scheduled_time) {

            // run all threads
            for (size_t i = 0; i < array_size(&sim->threads); ++i) {
                thread* t = array_at(&sim->threads, i);
                thread_run(t, is_single_threaded);
            }

            // destroy all threads
            for (size_t i = 0; i < array_size(&sim->threads); ++i) { 
                thread* t = array_at(&sim->threads, i);
                thread_destroy(t, is_single_threaded);
            }
            array_clear(&sim->threads);

            // after the run some new events might have been triggered
            se = event_queue_front(&sim->eq);
            
            // update simulation time
            if (se != NULL) { sim->time = se->scheduled_time; }
        }
    }

}

simulation_time_t simulation_get_time(simulation* sim) {
    assert(sim && "Passed NULL simulation");
    return sim->time;
}

void simulation_event_notify(simulation* sim, event* e, simulation_time_t delay, void* arg) {
    assert(sim && "Passed NULL simulation");
    assert(e && "Passed NULL event");
    assert(delay && "Cannot notify with zero delay");
    scheduled_event se;
    scheduled_event_create(&se, e, simulation_get_time(sim) + delay, arg);
    event_queue_push_copy(&sim->eq, &se);
}