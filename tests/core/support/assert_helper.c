#include "assert_helper.h"
#include "mc/utils.h"

void throw_error(const char *expr, const char *file, int line)
{
    // Silence unused parameter warnings
    (void)expr;
    (void)file;
    (void)line;

    // Jump safely out of the crashing function
    Throw(ASSERTION_ERROR_CODE);
}

void test_assert_init()
{
    mc_assert_set_override(throw_error);
}