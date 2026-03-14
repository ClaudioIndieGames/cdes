#ifndef __MODULE_H__
#define __MODULE_H__

#define YIELD(x) do { \
    (x).state = 256 + __LINE__; return; \
    case 256 + __LINE__:; \
} while (0)

typedef struct {
    const char* name;
    size_t state;
//    cdes_arena context_arena;
} module;

void module_init(module* this, const char* name);
void module_fini(module* this);

#endif // __MODULE_H__