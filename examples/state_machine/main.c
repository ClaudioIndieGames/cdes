
#include <stdio.h>
#include "cpu.h"

int main() {
    cpu c;
    cpu_init(&c, "test", 42);

    c.event(&c, NULL); // PRINT_INT
    c.event(&c, NULL); // PRINT_INT
    c.event(&c, NULL); // PRINT_NAME
    
    cpu_fini(&c);

    return 0;
}
