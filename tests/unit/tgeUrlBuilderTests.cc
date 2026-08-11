#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "tgeUrlBuilder.h"

TEST(tgeDateBuilder, check) {
    tgeUrlBuilder builder;
    EXPECT_EQ(
        builder.getUrlForDate(std::chrono::year{2026}/8/8),
        "https://tge.pl/energia-elektryczna-rdn?dateShow=07-08-2026");
}

TEST(tgeDateBuilder, check_1) {
    tgeUrlBuilder builder;
    EXPECT_EQ(
        builder.getUrlForDate(std::chrono::year{2000}/1/1),
        "https://tge.pl/energia-elektryczna-rdn?dateShow=31-12-1999");
}
