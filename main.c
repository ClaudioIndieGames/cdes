#include <stdio.h>
#include "event.h"

// scheduler
event_scheduler s;

// events
event e1;
event e2;

// tasks
void* task1(void* arg) {
    printf("[%lds] Hi from task1 at thread %ld\n", s.sim_time, (unsigned long)pthread_self());
    return NULL;
}

void* task3(void* arg) {
    printf("[%lds] Hi from task3 at thread %ld\n", s.sim_time, (unsigned long)pthread_self());
    return NULL;
}

void* task2(void* arg) {
    printf("[%lds] Hi from task2 at thread %ld\n", s.sim_time, (unsigned long)pthread_self());
    event_register_task(&e2, task3);
    event_scheduler_schedule(&s, &e2, 10);
    return NULL;
}

int main() {
    // constructor
    event_scheduler_create(&s);
    event_create(&e1);
    event_create(&e2);

    event_register_task(&e1, task1);
    event_register_task(&e1, task2);
    event_scheduler_schedule(&s, &e1, 10);

    event_scheduler_run_multi(&s);

    // destructor
    event_destroy(&e1);
    event_destroy(&e2);
    event_scheduler_destroy(&s);

    return 0;
}