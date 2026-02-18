#include <assert.h>
#include "thread.h"

thread* thread_create(thread* t) {
    assert(t && "Passed NULL thread");
    // allocating space for 2 func by default
    array_create_semi_dynamic(&t->funcs, sizeof(thread_func), 2);
    return t;
}

void thread_destroy(thread* t, char is_single_threaded) {
    assert(t && "Passed NULL thread");
    if (!is_single_threaded) {
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
        pthread_func_t func = task_get_func(tf.task);
        func(tf.arg);
    }
    return NULL;
}

void thread_run(thread* t, char is_single_threaded) {
    if (!is_single_threaded) {
        pthread_create(&t->id, NULL, thread_main, &t->funcs);
    } else {
        thread_main(&t->funcs);
    }
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