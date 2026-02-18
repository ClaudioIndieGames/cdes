#ifndef __TASK_H__
#define __TASK_H__

#include <stddef.h>

typedef void* (*pthread_func_t)(void*);

typedef struct {
    void* module;
    pthread_func_t func;
    size_t priority;
} task;

task* task_create(task* t, void* module, pthread_func_t func, size_t priority);
void task_destroy(task* t);
pthread_func_t task_get_func(task* t);

#endif  // __TASK_H__