#pragma once
#include <gtest/gtest.h>
extern "C"
{
#include "mc/utils.h"
}

// Base class for all MeeCore tests.
class MeeCoreTest : public testing::Test
{
protected:
    void SetUp() override;
};