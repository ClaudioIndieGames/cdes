#include <stdio.h>
#include <pthread.h>
#define ARRAY_IMPLEMENTATION
#include "simulation.h"

// simulation
simulation sim;

// events
event e1;
event e2;

// some data
int data1 = 42;
int data2 = 69;

// tasks
task t1;
void* task1(void* arg) {
    printf("[%lds] Hi from task1, received %d at thread %ld\n", sim.time, *(int*)arg, (unsigned long)pthread_self());
    return NULL;
}
task t3;
void* task3(void* arg) {
    printf("[%lds] Hi from task3, received %d at thread %ld\n", sim.time, *(int*)arg, (unsigned long)pthread_self());
    return NULL;
}
task t2;
void* task2(void* arg) {
    printf("[%lds] Hi from task2, received %d at thread %ld\n", sim.time, *(int*)arg, (unsigned long)pthread_self());
    simulation_event_notify(&sim, &e2, 10, &data2);
    return NULL;
}

int main() {
    // constructor
    simulation_create(&sim);
    task_create(&t1, &t1, task1, 0);
    task_create(&t2, &t2, task2, 0);
    task_create(&t3, &t3, task3, 0);
    event_create(&e1);
    event_create(&e2);
    
    event_register_task(&e1, &t1);
    event_register_task(&e1, &t2);
    event_register_task(&e2, &t3);

    simulation_event_notify(&sim, &e1, 10, &data1);

    simulation_start(&sim, 1);

    // destructor
    event_destroy(&e1);
    event_destroy(&e2);
    task_destroy(&t1);
    task_destroy(&t2);
    task_destroy(&t3);
    simulation_destroy(&sim);

    return 0;
}