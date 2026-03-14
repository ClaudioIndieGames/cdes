
#include <stdio.h>
#include "module.h"

void module_init(module* this, const char* name) {
    this->name = name;
    this->state = 0;
}

void module_fini(module* this) {
    (void)this; // Unused parameter
    // TODO: cleanup resources if needed
}