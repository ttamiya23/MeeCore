#include "mc/utils.h"
#include "mc/debug.h"

void mc_assert_handler(const char *expr, const char *file, int line)
{
    MC_LOG_CRITICAL("CRASH: Assertion '%s' failed at %s:%d\n", expr, file,
                    line);
    while (1)
    {
        __asm("nop");
    }
}