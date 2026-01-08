#include "system/modules/analog_vector3_system.h"
#include "system/modules/analog_system.h"
#include "system/composite.h"

#define ANALOG_VECTOR3_SYS_X_VALUE_ALIAS "x"
#define ANALOG_VECTOR3_SYS_X_TARGET_VALUE_ALIAS "targetX"
#define ANALOG_VECTOR3_SYS_Y_VALUE_ALIAS "y"
#define ANALOG_VECTOR3_SYS_Y_TARGET_VALUE_ALIAS "targetY"
#define ANALOG_VECTOR3_SYS_Z_VALUE_ALIAS "z"
#define ANALOG_VECTOR3_SYS_Z_TARGET_VALUE_ALIAS "targetZ"

MC_DEFINE_COMPOSITE_DRIVER(mc_analog_vector3_sys_driver,
                           mc_analog_vector3_system_ctx_t,
                           MC_NODE(x, &mc_analog_sys_driver),
                           MC_NODE(y, &mc_analog_sys_driver),
                           MC_NODE(z, &mc_analog_sys_driver));

const mc_analog_vector3_system_config_t mc_analog_vector3_sys_config = {
    .x = {.value_name = ANALOG_VECTOR3_SYS_X_VALUE_ALIAS, .target_value_name = ANALOG_VECTOR3_SYS_X_TARGET_VALUE_ALIAS},
    .y = {.value_name = ANALOG_VECTOR3_SYS_Y_VALUE_ALIAS, .target_value_name = ANALOG_VECTOR3_SYS_Y_TARGET_VALUE_ALIAS},
    .z = {.value_name = ANALOG_VECTOR3_SYS_Z_VALUE_ALIAS, .target_value_name = ANALOG_VECTOR3_SYS_Z_TARGET_VALUE_ALIAS},

};