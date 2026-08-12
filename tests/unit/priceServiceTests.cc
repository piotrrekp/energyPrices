#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "priceService.h"

TEST(priceServiceTest, nullIfNoData) {
    priceService service;
    const energyPricesTable input{
	{" ", std::nullopt, std::nullopt, std::nullopt}
    };
    const displayPrices expected{{" ", std::nullopt}};
    EXPECT_EQ(service.getPriceTable(input), expected);
}

TEST(priceServiceTest, returnFixing1IfAvaiable) {
    priceService service;
    const double fixing1Price = 632.32;
    const energyPricesTable input{
	{" ", fixing1Price, std::nullopt, std::nullopt}
    };
    const displayPrices expected{{" ", fixing1Price}};
    EXPECT_EQ(service.getPriceTable(input), expected);
}

TEST(priceServiceTest, returnMeanIfAllPricesCompleted) {
    priceService service;
    const double meanPrice = 32.32;
    const energyPricesTable input{
	{" ", 43, 123.23, meanPrice}
    };
    const displayPrices expected{{" ", meanPrice}};
    EXPECT_EQ(service.getPriceTable(input), expected);
}
