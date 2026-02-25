#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#define NUM_THREADS 3

pthread_mutex_t mutex[NUM_THREADS];
pthread_cond_t cond[NUM_THREADS];
pthread_t threads[NUM_THREADS];
size_t thread_index[NUM_THREADS];

void* task(void* arg) {
    size_t tid = *(size_t*)arg;

    // mutex lock
    if (pthread_mutex_lock(&mutex[tid]) != 0) {
        assert(0 && "pthread_mutex_lock() error");
    }

    printf("Started task by %ld\n", tid);

    // condition wait
    if (pthread_cond_wait(&cond[tid], &mutex[tid]) != 0) {
        assert(0 && "pthread_cond_wait() error");
    }

    printf("Ended task by %ld\n", tid);

    return NULL;
}

int main() {
    
    // create mutex
    for (size_t i = 0; i < NUM_THREADS; ++i) {
        if (pthread_mutex_init(&mutex[i], NULL) == -1) {
            assert(0 && "pthread_mutex_init() error");
        }
    }

    // create condition
    for (size_t i = 0; i < NUM_THREADS; ++i) {
        if (pthread_cond_init(&cond[i], NULL) != 0) {
            assert(0 && "pthread_cond_init() error");
        }
    }

    // create threads
    for (size_t i = 0; i < NUM_THREADS; ++i) {
        thread_index[i] = i;
        if (pthread_create(&threads[i], NULL, task, &thread_index[i]) != 0) {
            assert(0 && "pthread_create() error");
        }
    }

    // signal condition
    for (size_t i = 0; i < NUM_THREADS; ++i) {
        sleep(2);
        if (pthread_cond_signal(&cond[i]) != 0) {
            assert(0 && "pthread_cond_signal() error");
        }
    }

    // join threads
    for (size_t i = 0; i < NUM_THREADS; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            assert(0 && "pthread_join() error");
        }
    }

    return 0;
}
