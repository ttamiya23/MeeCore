#include "mc/utils.h"
#include "mc/debug.h"

static mc_assert_callback_func_t callback_ = NULL;
static mc_assert_callback_func_t override_ = NULL;

void mc_assert_set_callback(mc_assert_callback_func_t callback)
{
    callback_ = callback;
}

void mc_assert_set_override(mc_assert_callback_func_t override)
{
    override_ = override;
}

void mc_assert_handler(const char *expr, const char *file, int line)
{
    if (override_ != NULL)
    {
        return override_(expr, file, line);
    }

    MC_LOG_CRITICAL("CRASH: Assertion '%s' failed at %s:%d\n", expr, file,
                    line);
    if (callback_ != NULL)
    {
        callback_(expr, file, line);
    }
    while (1)
    {
        __asm("nop");
    }
}