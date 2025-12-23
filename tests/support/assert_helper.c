#include "assert_helper.h"
#include "mc/utils.h"

void mc_assert_handler(const char *expr, const char *file, int line)
{
    // Silence unused parameter warnings
    (void)expr;
    (void)file;
    (void)line;

    // Jump safely out of the crashing function
    Throw(ASSERTION_ERROR_CODE);
}