#include "fake_composite.h"

MC_DEFINE_COMPOSITE_DRIVER(fake_composite_sys_driver, fake_composite_ctx_t,
                           MC_NODE(sys1, &fake_sys_driver),
                           MC_NODE(sys2, &fake_sys_driver));