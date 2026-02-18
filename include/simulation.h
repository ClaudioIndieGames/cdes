#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#include "event_queue.h"

typedef struct {
    event_queue eq;
    simulation_time_t time;
    array threads;
} simulation;

simulation* simulation_create(simulation* sim);
void simulation_destroy(simulation* sim);
void simulation_start(simulation* sim, char is_single_threaded);
simulation_time_t simulation_get_time(simulation* sim);
void simulation_event_notify(simulation* sim, event* e, simulation_time_t delay, void* arg);

#endif  // __SIMULATION_H__