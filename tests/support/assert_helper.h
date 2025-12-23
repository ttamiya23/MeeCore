#ifndef ASSERT_HELPER_H_
#define ASSERT_HELPER_H_

#include "CException.h"

// Define a consistent error code for our throws
#define ASSERTION_ERROR_CODE 1

/**
 * @brief Macro to verify that a function call triggers an assertion.
 * Usage: TEST_ASSERT_DEATH( mc_list_append(NULL, &node) );
 */
#define TEST_ASSERT_DEATH(code_under_test)                              \
    do                                                                  \
    {                                                                   \
        CEXCEPTION_T e;                                                 \
        Try                                                             \
        {                                                               \
            code_under_test;                                            \
            /* If we get here, the assert didn't fire */                \
            TEST_FAIL_MESSAGE("Expected Assertion was NOT triggered."); \
        }                                                               \
        Catch(e)                                                        \
        {                                                               \
            /* Verify we caught the specific Throw(1) from our stub */  \
            TEST_ASSERT_EQUAL(ASSERTION_ERROR_CODE, e);                 \
        }                                                               \
    } while (0)

#endif // ASSERT_HELPER_H_