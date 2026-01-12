#include "unity.h"
#include "mc/utils.h"
#include "mc/time.h"
#include "fakes/fake_time.h"
#include "assert_helper.h"

// Globals
fake_time_ctx_t ctx;

void setUp()
{
    // Initialize the core with our fake driver before every test
    mc_time_init(&fake_time_driver, &ctx);
}

void test_init_calls_driver_init()
{
    // Artificially break the context to prove init fixes it
    ctx.is_initialized = 0;
    ctx.current_time_ms = 55;

    mc_time_init(&fake_time_driver, &ctx);

    TEST_ASSERT_EQUAL_HEX8(MC_INITIALIZED, ctx.is_initialized);
    TEST_ASSERT_EQUAL_UINT32(0, ctx.current_time_ms);
}

void test_get_ms_delegates_to_driver()
{
    ctx.current_time_ms = 4200;

    uint32_t now = mc_time_get_ms();

    TEST_ASSERT_EQUAL_UINT32(4200, now);
}

void test_delay_delegates_to_driver()
{
    ctx.current_time_ms = 1000;

    mc_time_delay(500);

    // The fake driver implementation of delay adds to the current time
    TEST_ASSERT_EQUAL_UINT32(1500, ctx.current_time_ms);
}

// -- Death Tests (Requires assertion_helper.h setup) --

void test_assert_death_if_init_driver_is_null()
{
    TEST_ASSERT_DEATH(mc_time_init(NULL, &ctx));
}

void test_assert_death_if_driver_missing_functions()
{
    mc_time_driver_t bad_driver = {
        .init = NULL,
        .get_ms = NULL,
        .delay = NULL};

    TEST_ASSERT_DEATH(mc_time_init(&bad_driver, &ctx));
}