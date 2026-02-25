#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <semaphore.h>

#define NUM_THREADS 3

pthread_t threads[NUM_THREADS];
size_t thread_index[NUM_THREADS];
sem_t sem[NUM_THREADS];

void* task(void* arg)
{
    size_t tid = *(size_t*)arg;

    printf("Started task by %ld\n", tid);

    sem_wait(&sem[tid]);

    printf("Ended task by %ld\n", tid);

    return NULL;
}

int main()
{
    // init semaphores
    for (size_t i = 0; i < NUM_THREADS; i++)
        sem_init(&sem[i], 0, 0);

    // create threads
    for (size_t i = 0; i < NUM_THREADS; i++)
    {
        thread_index[i] = i;
        pthread_create(&threads[i], NULL, task, &thread_index[i]);
    }

    // signal threads
    for (size_t i = 0; i < NUM_THREADS; i++)
    {
        sleep(2);
        sem_post(&sem[i]);
    }

    // join
    for (size_t i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    return 0;
}
