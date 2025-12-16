#include "mc/utils.h"
#include "mc/debug.h"

/*
 * The __attribute__((weak)) is a GCC/Clang feature.
 * It tells the linker that this function can be overridden.
 */
__attribute__((weak)) void mc_assert_handler(const char *expr,
                                             const char *file, int line)
{
    MC_LOG_CRITICAL("CRASH: Assertion '%s' failed at %s:%d\n", expr, file,
                    line);
    while (1)
    {
        __asm("nop");
    }
}