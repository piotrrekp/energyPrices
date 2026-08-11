#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "stringUtils.h"

using namespace stringUtils;

TEST(stringUtils, parsesPricesWithThousandsSeparator) {
    EXPECT_DOUBLE_EQ(getDouble("1 203,99").value(), 1203.99);
    EXPECT_DOUBLE_EQ(getDouble("-1 203,99").value(), -1203.99);
    EXPECT_DOUBLE_EQ(getDouble("-12,34").value(), -12.34);
}
