#ifndef __CDES_THREAD_H__
#define __CDES_THREAD_H__

#include <pthread.h>
#include "array.h"
#include "cdes_task.h"
#include "cdes_simulation.h"

typedef struct {
    array funcs;
    pthread_t id;
} cdes_thread;

typedef struct {
    cdes_task* task;
    void* args;
} cdes_thread_func;

cdes_thread_func* cdes_thread_func_create(cdes_thread_func* tf, cdes_task* t, void* args);
void cdes_thread_func_destroy(cdes_thread_func* tf);

cdes_thread* cdes_thread_create(cdes_thread* t);
void cdes_thread_destroy(cdes_thread* t, cdes_simulation_mode mode);
void* cdes_thread_main(void* args);
void cdes_thread_run(cdes_thread* t, cdes_simulation_mode mode);

#endif  // __CDES_THREAD_H__