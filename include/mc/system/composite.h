#ifndef MC_SYSTEM_COMPOSITE_H
#define MC_SYSTEM_COMPOSITE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/system/core.h"
#include <stddef.h>

    // Struct to combine multiple system drivers into one bigger driver.
    typedef struct mc_composite_driver_t
    {
        uint8_t count;
        const mc_system_driver_t *const *systems;
        const size_t *ctx_offsets;
    } mc_composite_driver_t;

    // Trampoline methods for composite systems.
    void mc_composite_init(void *ctx, const mc_composite_driver_t *driver);

    mc_sys_status_t mc_composite_invoke(
        void *ctx, const mc_composite_driver_t *driver, uint8_t func_id,
        int32_t *args, uint8_t arg_count);

    mc_sys_status_t mc_composite_write_input(
        void *ctx, const mc_composite_driver_t *driver, uint8_t x_id,
        int32_t val);

    mc_sys_status_t mc_composite_write_input(
        void *ctx, const mc_composite_driver_t *driver, uint8_t x_id,
        int32_t val);

    mc_sys_status_t mc_composite_read_input(
        void *ctx, const mc_composite_driver_t *driver, uint8_t x_id,
        int32_t *val);

    mc_sys_status_t mc_composite_read_output(
        void *ctx, const mc_composite_driver_t *driver, uint8_t y_id,
        int32_t *val);

    uint8_t mc_composite_get_function_count(
        void *ctx, const mc_composite_driver_t *driver);

    uint8_t mc_composite_get_input_count(
        void *ctx, const mc_composite_driver_t *driver);

    uint8_t mc_composite_get_output_count(
        void *ctx, const mc_composite_driver_t *driver);

// --- [Internal] Argument Mapping Helpers ---
#define MC_GET_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define MC_COUNT_ARGS(...) \
    MC_GET_NTH_ARG(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define MC_INDIRECT_EXPAND(MACRO, ARGS) MACRO ARGS
#define MC_MAP(FUNC, ...) \
    MC_INDIRECT_EXPAND(MC_MAP_IMPL, (MC_COUNT_ARGS(__VA_ARGS__), FUNC, __VA_ARGS__))
#define MC_MAP_IMPL(N, FUNC, ...) MC_MAP_IMPL_##N(FUNC, __VA_ARGS__)
#define MC_MAP_IMPL_1(FUNC, x) FUNC(x)
#define MC_MAP_IMPL_2(FUNC, x, ...) FUNC(x), MC_MAP_IMPL_1(FUNC, __VA_ARGS__)
#define MC_MAP_IMPL_3(FUNC, x, ...) FUNC(x), MC_MAP_IMPL_2(FUNC, __VA_ARGS__)
#define MC_MAP_IMPL_4(FUNC, x, ...) FUNC(x), MC_MAP_IMPL_3(FUNC, __VA_ARGS__)
#define MC_MAP_IMPL_5(FUNC, x, ...) FUNC(x), MC_MAP_IMPL_4(FUNC, __VA_ARGS__)
#define MC_MAP_IMPL_6(FUNC, x, ...) FUNC(x), MC_MAP_IMPL_5(FUNC, __VA_ARGS__)
#define MC_MAP_IMPL_7(FUNC, x, ...) FUNC(x), MC_MAP_IMPL_6(FUNC, __VA_ARGS__)
#define MC_MAP_IMPL_8(FUNC, x, ...) FUNC(x), MC_MAP_IMPL_7(FUNC, __VA_ARGS__)
#define MC_MAP_IMPL_9(FUNC, x, ...) FUNC(x), MC_MAP_IMPL_8(FUNC, __VA_ARGS__)
#define MC_MAP_IMPL_10(FUNC, x, ...) FUNC(x), MC_MAP_IMPL_9(FUNC, __VA_ARGS__)

// --- Extraction Helpers ---
#define MC_NODE(field, driver) (field, driver)
#define MC_EXTRACT_DRIVER(node) MC_GET_DRIVER node
#define MC_EXTRACT_OFFSET(node) MC_GET_OFFSET node

// Internal helpers to unpack (field, driver)
#define MC_GET_DRIVER(field, driver) driver
#define MC_GET_OFFSET(field, driver) offsetof(CTX_TYPE, field)

// --- THE MAIN MACRO ---
// Usage: MC_DEFINE_COMPOSITE(blinker_driver, blinker_ctx_t, MC_NODE(pwm, &pwm_drv), ...)
#define MC_DEFINE_COMPOSITE_DRIVER(NAME, CTX_TYPE_ARG, ...)                               \
    /* 1. Define Internal Context Type for Offset Calculation */                          \
    typedef CTX_TYPE_ARG NAME##_CTX_TYPE;                                                 \
    typedef CTX_TYPE_ARG CTX_TYPE; /* Shadow for macros */                                \
                                                                                          \
    /* 2. Generate Systems Array (Flash) */                                               \
    static const mc_system_driver_t *const NAME##_systems[] = {                           \
        MC_MAP(MC_EXTRACT_DRIVER, __VA_ARGS__)};                                          \
                                                                                          \
    /* 3. Generate Offsets Array (Flash) */                                               \
    static const size_t NAME##_offsets[] = {                                              \
        MC_MAP(MC_EXTRACT_OFFSET, __VA_ARGS__)};                                          \
                                                                                          \
    /* 4. Generate Driver (Flash) */                                                      \
    static const mc_composite_driver_t NAME##_driver = {                                  \
        .count = sizeof(NAME##_systems) / sizeof(NAME##_systems[0]),                      \
        .systems = NAME##_systems,                                                        \
        .ctx_offsets = NAME##_offsets};                                                   \
                                                                                          \
    /* 5. Generate Trampoline Functions (Flash) */                                        \
    /* These inject the driver pointer into the generic impl functions */                 \
    static void NAME##_init(void *ctx)                                                    \
    {                                                                                     \
        mc_composite_init(ctx, &NAME##_driver);                                           \
    }                                                                                     \
    static mc_sys_status_t NAME##_write_input(void *ctx, uint8_t id, int32_t val)         \
    {                                                                                     \
        return mc_composite_write_input(ctx, &NAME##_driver, id, val);                    \
    }                                                                                     \
    static mc_sys_status_t NAME##_read_input(void *ctx, uint8_t id, int32_t *val)         \
    {                                                                                     \
        return mc_composite_read_input(ctx, &NAME##_driver, id, val);                     \
    }                                                                                     \
    static mc_sys_status_t NAME##_read_output(void *ctx, uint8_t id, int32_t *val)        \
    {                                                                                     \
        return mc_composite_read_output(ctx, &NAME##_driver, id, val);                    \
    }                                                                                     \
    static mc_sys_status_t NAME##_invoke(void *ctx, uint8_t id, int32_t *args, uint8_t c) \
    {                                                                                     \
        return mc_composite_invoke(ctx, &NAME##_driver, id, args, c);                     \
    }                                                                                     \
    static uint8_t NAME##_get_input_count(void *ctx)                                      \
    {                                                                                     \
        return mc_composite_get_input_count(ctx, &NAME##_driver);                         \
    }                                                                                     \
    static uint8_t NAME##_get_output_count(void *ctx)                                     \
    {                                                                                     \
        return mc_composite_get_output_count(ctx, &NAME##_driver);                        \
    }                                                                                     \
    static uint8_t NAME##_get_function_count(void *ctx)                                   \
    {                                                                                     \
        return mc_composite_get_function_count(ctx, &NAME##_driver);                      \
    }                                                                                     \
                                                                                          \
    /* 6. Define The Driver (Flash) */                                                    \
    const mc_system_driver_t NAME = {                                                     \
        .init = NAME##_init,                                                              \
        .write_input = NAME##_write_input,                                                \
        .read_input = NAME##_read_input,                                                  \
        .read_output = NAME##_read_output,                                                \
        .invoke = NAME##_invoke,                                                          \
        .get_input_count = NAME##_get_input_count,                                        \
        .get_output_count = NAME##_get_output_count,                                      \
        .get_function_count = NAME##_get_function_count};

#ifdef __cplusplus
}
#endif

#endif /* MC_SYSTEM_COMPOSITE_H */