#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "priceService.h"
#include "mockPriceProvider.h"

class priceServiceTest : public ::testing::Test {
protected:
    testing::NiceMock<mockPriceProvider> provider;
    priceService service{provider};
    std::chrono::year_month_day unimportantDate{};
    std::string unimportantHour = " ";
};

TEST_F(priceServiceTest, nullIfNoData) {
    const energyPricesTable input{unimportantDate, {{unimportantHour, std::nullopt, std::nullopt, std::nullopt}}};
    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));
    const dailyPrices expected(unimportantDate, {{unimportantHour, std::nullopt}});
    const auto result = service.getPriceTable(unimportantDate);
    const auto prices = result.getPrices();

    EXPECT_EQ(result, expected);
}

TEST_F(priceServiceTest, returnFixing1IfAvailable) {
    const double fixing1Price = 632.32;
    const energyPricesTable input{unimportantDate, {{unimportantHour, fixing1Price, std::nullopt, std::nullopt}}};
    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));

    const double expectedValue = 0.63;
    const dailyPrices expected{unimportantDate, {{unimportantHour, expectedValue}}};

    EXPECT_EQ(service.getPriceTable(unimportantDate), expected);
}
TEST_F(priceServiceTest, returnFixing2IfMeanIsNotAvailable) {
    const double fixing2Price = 123.23;
    const energyPricesTable input{unimportantDate, {{unimportantHour, 43.0, fixing2Price, std::nullopt}}};

    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));

    const double expectedValue = 0.12;
    const dailyPrices expected{unimportantDate, {{unimportantHour, expectedValue}}};
    EXPECT_EQ(service.getPriceTable(unimportantDate), expected);
}

TEST_F(priceServiceTest, returnMeanIfAvailable) {
    const double meanPrice = 32.32;
    const energyPricesTable input{unimportantDate, {{unimportantHour, 43, 123.23, meanPrice}}};
    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));

    const  double expectedValue = 0.03;
    const dailyPrices expected{unimportantDate, {{unimportantHour, expectedValue}}};
    EXPECT_EQ(service.getPriceTable(unimportantDate), expected);
}

TEST_F(priceServiceTest, timeFormatFirstHour) {
    const double _ = .0;
    const std::string time = "2026-08-14_H01";
    const energyPricesTable input{unimportantDate, {{time , _, _, _}}};
    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));

    const std::string expectedTime = "00:00 - 01:00";
    auto restult = service.getPriceTable(unimportantDate).getPrices();
    ASSERT_EQ(restult.size(), 1);
    EXPECT_EQ(restult.at(0).time, expectedTime);
}

TEST_F(priceServiceTest, timeFormatLastHour) {
    const double _ = .0;
    const std::string time = "2026-08-14_H24";
    const energyPricesTable input{unimportantDate, {{time , _, _, _}}};
    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));

    const std::string expectedTime = "23:00 - 24:00";
    auto restult = service.getPriceTable(unimportantDate).getPrices();
    ASSERT_EQ(restult.size(), 1);
    EXPECT_EQ(restult.at(0).time, expectedTime);
}

TEST_F(priceServiceTest, price_for_kWh_positiveValue) {
    const double positiveValue = 126.5;
    const std::string time = "H24";
    const energyPricesTable input{unimportantDate, {{time , positiveValue, positiveValue, positiveValue}}};
    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));


    const double expectedValue = 0.13;
    auto restult = service.getPriceTable(unimportantDate).getPrices();
    ASSERT_EQ(restult.size(), 1);
    EXPECT_EQ(restult.at(0).price, expectedValue);
}

TEST_F(priceServiceTest, price_for_kWh_positiveValue_2) {
    const double positiveValue = 1923.5;
    const std::string time = "H24";
    const energyPricesTable input{unimportantDate, {{time , positiveValue, positiveValue, positiveValue}}};
    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));


    const double expectedValue = 1.92;
    auto restult = service.getPriceTable(unimportantDate).getPrices();
    ASSERT_EQ(restult.size(), 1);
    EXPECT_EQ(restult.at(0).price, expectedValue);
}

TEST_F(priceServiceTest, price_for_kWh_negativeValue) {
    const double value = -32.52;
    const std::string time = "H24";
    const energyPricesTable input{unimportantDate, {{time , value, value, value}}};
    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));


    const double expectedValue = -.03;
    auto restult = service.getPriceTable(unimportantDate).getPrices();
    ASSERT_EQ(restult.size(), 1);
    EXPECT_EQ(restult.at(0).price, expectedValue);
}

TEST_F(priceServiceTest, price_for_kWh_null) {
    const double value = .0;
    const std::string time = "H24";
    const energyPricesTable input{unimportantDate, {{time , value, value, value}}};
    ON_CALL(provider, getPrices).WillByDefault(::testing::Return(input));


    const double expectedValue = 0.;
    auto restult = service.getPriceTable(unimportantDate).getPrices();
    ASSERT_EQ(restult.size(), 1);
    EXPECT_EQ(restult.at(0).price, expectedValue);
}

TEST(dailyPricesTest, calculatesMinMaxPrice) {
    std::chrono::year_month_day unimportantDate{};
    const std::vector<hourlyPrice> prices{
        {"00:00 - 01:00", 0.25},
        {"01:00 - 02:00", 0.80},
        {"02:00 - 03:00", 0.40}
    };

    dailyPrices result{unimportantDate, prices};

    EXPECT_EQ(result.getSummary().min, 0.25);
    EXPECT_EQ(result.getSummary().max, 0.80);
}

TEST(dailyPricesTest, calculateMinMaxPrice_ingnoresMissingPrice) {
    std::chrono::year_month_day unimportantDate{};
    const std::vector<hourlyPrice> prices{
        {"00:00 - 01:00", 0.25},
        {"01:00 - 02:00", std::nullopt},
        {"02:00 - 03:00", 0.40}
    };

    dailyPrices result{unimportantDate, prices};

    EXPECT_EQ(result.getSummary().min, 0.25);
    EXPECT_EQ(result.getSummary().max, 0.40);
}

TEST(dailyPricesTest, returnsEmptySummaryWhenNoPrices) {
    std::chrono::year_month_day unimportantDate{};
    const std::vector<hourlyPrice> prices{
        {"00:00 - 01:00", std::nullopt},
        {"01:00 - 02:00", std::nullopt},
    };

    dailyPrices result{unimportantDate, prices};

    EXPECT_EQ(result.getSummary().min, std::nullopt);
    EXPECT_EQ(result.getSummary().max, std::nullopt);
}
