#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "energyPricesServer.h"
#include "mockPriceProvider.h"

TEST(energyPricesServer, returnsPricesAsJson) {
    testing::NiceMock<mockPriceProvider> provider;

    const energyPricesTable input{
        {"00:00 - 01:00", 123.45, std::nullopt, std::nullopt},
        {"01:00 - 02:00", 234.56, std::nullopt, std::nullopt},
    };

    ON_CALL(provider, getPrices)
        .WillByDefault(testing::Return(input));

    priceService service{provider};

    crow::SimpleApp app;
    energyPricesServer server{app, service};

    crow::request request;
    crow::response response;

    request.url = "/api/prices/tomorrow";

    app.validate();
    app.handle_full(request, response);

    ASSERT_EQ(response.code, 200);

    const auto json = crow::json::load(response.body);

    ASSERT_TRUE(json);
    ASSERT_TRUE(json.has("prices"));
    ASSERT_EQ(json["prices"].size(), 2);

    EXPECT_EQ(json["prices"][0]["time"].s(), "00:00 - 01:00");
    EXPECT_DOUBLE_EQ(json["prices"][0]["price"].d(), 123.45);

    EXPECT_EQ(json["prices"][1]["time"].s(), "01:00 - 02:00");
    EXPECT_DOUBLE_EQ(json["prices"][1]["price"].d(), 234.56);
}
