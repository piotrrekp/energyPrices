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

    EXPECT_CALL(provider, getPrices(testing::Truly(
        [](const std::chrono::year_month_day &day) {
            const std::chrono::year_month_day today{
                std::chrono::floor<std::chrono::days>(
                    std::chrono::system_clock::now())
            };

            const std::chrono::year_month_day tomorrow{
                std::chrono::sys_days{today} + std::chrono::days{1}
            };

            return day == tomorrow;
        })))
        .WillOnce(testing::Return(input));

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
    EXPECT_DOUBLE_EQ(json["prices"][0]["price"].d(), 0.12);

    EXPECT_EQ(json["prices"][1]["time"].s(), "01:00 - 02:00");
    EXPECT_DOUBLE_EQ(json["prices"][1]["price"].d(), 0.23);
}

TEST(energyPricesServer, endpointForTodayExist) {
    testing::NiceMock<mockPriceProvider> provider;
    priceService service{provider};

    crow::SimpleApp app;
    energyPricesServer server{app, service};

    crow::request request;
    crow::response response;

    request.url = "/api/prices/today";

    app.validate();
    app.handle_full(request, response);

    ASSERT_EQ(response.code, 200);
}

TEST(energyPricesServer, returnsTodayPricesAsJson) {
    testing::NiceMock<mockPriceProvider> provider;

    const energyPricesTable input{
        {"00:00 - 01:00", 345.67, std::nullopt, std::nullopt},
        {"01:00 - 02:00", 456.78, std::nullopt, std::nullopt},
    };

	EXPECT_CALL(provider, getPrices(testing::Truly(
		[](const std::chrono::year_month_day &day) {
			const std::chrono::year_month_day today{
				std::chrono::floor<std::chrono::days>(
					std::chrono::system_clock::now())
			};

			return day == today;
		}))).WillOnce(testing::Return(input));

    priceService service{provider};

    crow::SimpleApp app;
    energyPricesServer server{app, service};

    crow::request request;
    crow::response response;

    request.url = "/api/prices/today";

    app.validate();
    app.handle_full(request, response);

    ASSERT_EQ(response.code, 200);

    const auto json = crow::json::load(response.body);

    ASSERT_TRUE(json);
    ASSERT_TRUE(json.has("prices"));
    ASSERT_EQ(json["prices"].size(), 2);

    EXPECT_EQ(json["prices"][0]["time"].s(), "00:00 - 01:00");
    EXPECT_DOUBLE_EQ(json["prices"][0]["price"].d(), 0.35);

    EXPECT_EQ(json["prices"][1]["time"].s(), "01:00 - 02:00");
    EXPECT_DOUBLE_EQ(json["prices"][1]["price"].d(), 0.46);
}
