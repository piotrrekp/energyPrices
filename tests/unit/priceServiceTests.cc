#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "priceService.h"
#include "mockPriceProvider.h"

class priceServiceTest : public ::testing::Test {
protected:
    testing::NiceMock<mockPriceProvider> provider;
    priceService service{provider};
    std::chrono::year_month_day unimportantDate{};
};

TEST_F(priceServiceTest, nullIfNoData) {
    const energyPricesTable input{{" ", std::nullopt, std::nullopt, std::nullopt}};
    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));
    const displayPrices expected{{" ", std::nullopt}};
    EXPECT_EQ(service.getPriceTable(unimportantDate), expected);
}

TEST_F(priceServiceTest, returnFixing1IfAvailable) {
    const double fixing1Price = 632.32;
    const energyPricesTable input{{" ", fixing1Price, std::nullopt, std::nullopt}};
    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));
    const displayPrices expected{{" ", fixing1Price}};

    EXPECT_EQ(service.getPriceTable(unimportantDate), expected);
}
TEST_F(priceServiceTest, returnFixing2IfMeanIsNotAvailable) {
    const double fixing2Price = 123.23;
    const energyPricesTable input{{" ", 43.0, fixing2Price, std::nullopt}};

    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));

    const displayPrices expected{{" ", fixing2Price}};

    EXPECT_EQ(service.getPriceTable(unimportantDate), expected);
}

TEST_F(priceServiceTest, returnMeanIfAvailable) {
    const double meanPrice = 32.32;
    const energyPricesTable input{{" ", 43, 123.23, meanPrice}};
    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));
    const displayPrices expected{{" ", meanPrice}};

    EXPECT_EQ(service.getPriceTable(unimportantDate), expected);
}

TEST_F(priceServiceTest, timeFormatFirstHour) {
    const double _ = .0;
    const std::string time = "2026-08-14_H01";
    const energyPricesTable input{{time , _, _, _}};
    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));

    const std::string expectedTime = "00:00 - 01:00";
    auto restult = service.getPriceTable(unimportantDate);
    ASSERT_EQ(restult.size(), 1);
    EXPECT_EQ(restult.at(0).time, expectedTime);
}

TEST_F(priceServiceTest, timeFormatLastHour) {
    const double _ = .0;
    const std::string time = "2026-08-14_H24";
    const energyPricesTable input{{time , _, _, _}};
    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));

    const std::string expectedTime = "23:00 - 24:00";
    auto restult = service.getPriceTable(unimportantDate);
    ASSERT_EQ(restult.size(), 1);
    EXPECT_EQ(restult.at(0).time, expectedTime);
}
