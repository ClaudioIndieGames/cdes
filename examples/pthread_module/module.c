#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <semaphore.h>

pthread_t module_handle;
sem_t module_method_completed;
sem_t module_call_method;

// NOTE: this variables are for excelusive use of the module and the scheduler, no mutex required
enum module_method_name {
    CREATE,
    PRINT_INT_ONCE,
    PRINT_INT_TWICE,
    DESTROY
};
enum module_method_name module_method_name = CREATE;
void* module_method_arg;

void* module(void* arg) {
    (void)arg;

    while(1) {
        switch(module_method_name) {
            case CREATE:
                // constructor
                int str_size = *(int*)module_method_arg;
                char* str = malloc(sizeof(char) * str_size);
                str[0] = 'H'; str[1] = 'i'; str[2] = '\0';
                printf("Module constructed\n");
                break;
            case PRINT_INT_ONCE: {
                printf("%s: %d\n", str, *(int*)module_method_arg);
            } break;
            case PRINT_INT_TWICE: {
                printf("%s: %d %d\n", str, *(int*)module_method_arg, *(int*)module_method_arg);
            } break;
            case DESTROY: {
                // destructor
                free(str);
                printf("Module destroyed\n");
                return NULL;
            } break;
        }
        sem_post(&module_method_completed);
        sem_wait(&module_call_method);
    }

    return NULL;
}

int main() {
    int var = 10;

    sem_init(&module_method_completed, 0, 0);
    sem_init(&module_call_method, 0, 0);
    
    // construct module 
    module_method_name = CREATE;
    module_method_arg = &var;
    pthread_create(&module_handle, NULL, module, NULL);
    sem_wait(&module_method_completed);

    // call module method
    module_method_name = PRINT_INT_ONCE;
    module_method_arg = &var;
    sem_post(&module_call_method);
    sem_wait(&module_method_completed);

    // call module destructor
    module_method_name = DESTROY;
    sem_post(&module_call_method);
    //sem_wait(&module_method_completed);

    // join
    pthread_join(module_handle, NULL);

    return 0;
}
