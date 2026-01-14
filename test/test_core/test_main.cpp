#include <gtest/gtest.h>

// This is the ONE main function that rules them all.
// It automatically finds and runs all tests defined in other files.
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
