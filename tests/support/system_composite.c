#include "system_composite.h"

MC_DEFINE_COMPOSITE_DRIVER(composite_sys_driver, composite_sys_ctx_t,
                           MC_NODE(test_1, &test_sys_driver),
                           MC_NODE(test_2, &test_sys_driver));