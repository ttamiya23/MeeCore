#include "mc_test.h"
#include <exception>

namespace
{
    // Throw exception if assert fails.
    extern "C" void throw_exception(const char *expr, const char *file, int line)
    {
        (void)expr;
        (void)file;
        (void)line;
        throw new std::exception();
    }
}

// Set up by overriding mc_assert behavior to throw exception instead of halt
// forever.
void MeeCoreTest::SetUp()
{
    mc_assert_set_override(throw_exception);
}