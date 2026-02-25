#include "cdes.h"
#include "simple_module.h"

int main () {
    cdes_simulation* sim = cdes_simulation_create(&(cdes_simulation){0});
    simple_module* module = simple_module_create(&(simple_module){0}, sim);

    axi_socket_connect(&module->out, &module->in);

    cdes_simulation_start(sim, CDES_MULTI_THREADED);

    simple_module_destroy(module);
    cdes_simulation_destroy(sim);

    return 0;
}