
#include <stdio.h>
#include "cpu.h"

enum state {
    PRINT_INT = 0,
    PRINT_NAME
};

static void next_state(void* ctx, void* arg) {
    (void)arg; // Unused parameter
    cpu* this = (cpu*)ctx;

    switch (this->base.state) {
        case PRINT_INT:
            printf("my_data: %d\n", this->my_data);
            YIELD(this->base);
            this->my_data += 1; // Increment my_data for demonstration
            printf("my_data: %d\n", this->my_data);
            this->base.state = PRINT_NAME;
            break;
        case PRINT_NAME:
            printf("name: '%s'\n", this->base.name);
            break;
        default:
            printf("Unknown state\n");
            break;
    }

}

void cpu_init(cpu* this, const char* name, int my_data) {
    module_init(&this->base, name);
    this->my_data = my_data;
    this->event = next_state;
}

void cpu_fini(cpu* this) {
    module_fini(&this->base);
}