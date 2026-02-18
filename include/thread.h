#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include "array.h"
#include "task.h"

typedef struct {
    array funcs;
    pthread_t id;
} thread;

typedef struct {
    task* task;
    void* arg;
} thread_func;

thread_func* thread_func_create(thread_func* tf, task* t, void* arg);
void thread_func_destroy(thread_func* tf);

thread* thread_create(thread* t);
void thread_destroy(thread* t, char is_single_threaded);
void* thread_main(void* arg);
void thread_run(thread* t, char is_single_threaded);

#endif  // __THREAD_H__