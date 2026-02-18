#include <assert.h>
#include "task.h"

task* task_create(task* t, void* module, pthread_func_t func, size_t priority) {
    assert(t && "Passed NULL task");
    assert(module && "Passed NULL module");
    assert(func && "Passed NULL function");
    t->module = module;
    t->func = func;
    t->priority = priority;
    return t;
}

void task_destroy(task* t) {
    assert(t && "Passed NULL task");
    // nop
}

pthread_func_t task_get_func(task* t) {
    assert(t && "Passed NULL task");
    return t->func;
}