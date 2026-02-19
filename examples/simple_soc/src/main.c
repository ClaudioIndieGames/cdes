#include "cdes.h"
#include "simple_soc.h"

int main () {
    cdes_simulation* sim = cdes_simulation_create(&(cdes_simulation){0});
    simple_soc* soc1 = simple_soc_create(&(simple_soc){0}, sim);
    simple_soc* soc2 = simple_soc_create(&(simple_soc){0}, sim);

    cdes_simulation_start(sim, CDES_MULTI_THREADED);

    simple_soc_destroy(soc1);
    simple_soc_destroy(soc2);
    cdes_simulation_destroy(sim);

    return 0;
}