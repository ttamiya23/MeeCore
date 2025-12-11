#include "assert_helper.h"

void stub_assert_throw_exception(const char *expr, const char *file, int line, int num_calls)
{
    // Silence unused parameter warnings
    (void)expr;
    (void)file;
    (void)line;
    (void)num_calls;

    // Jump safely out of the crashing function
    Throw(ASSERTION_ERROR_CODE);
}