#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "httpClient.h"

TEST(simple, tryToGetPage) {
    httpClient client("https://curl.se/");
    auto page = client.getPage();
    ASSERT_FALSE(page.empty());
}
