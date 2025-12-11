#include "mc/utils.h"
#include "mc/Debug.h"

/*
 * The __attribute__((weak)) is a GCC/Clang feature.
 * It tells the linker that this function can be overridden.
 */
__attribute__((weak)) void mc_assert_handler(const char *file, int line)
{
    dbg_LogCritical("CRASH: Assertion '%s' failed at %s:%d\n", expr, file, line);
    while (1)
    {
        __asm("nop");
    }
}