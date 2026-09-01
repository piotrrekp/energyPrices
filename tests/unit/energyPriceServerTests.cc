#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "energyPricesServer.h"
#include "mockPriceProvider.h"

const auto today{
	std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())
};

const auto tomorrow{today + std::chrono::days{1}};

TEST(energyPricesServer, returnsPricesAsJson) {
	testing::NiceMock<mockPriceProvider> provider;

	const energyPricesTable input{
		tomorrow,
		{{stringUtils::getDate(tomorrow) + "_H01", 123.45, std::nullopt, std::nullopt},
		{stringUtils::getDate(tomorrow) + "_H02", 234.56, std::nullopt, std::nullopt}}
	};

	EXPECT_CALL(provider, getPrices(testing::Truly(
		[](const std::chrono::year_month_day &day) {
			return day == tomorrow;
		}))).WillOnce(testing::Return(input));

	priceService service{provider};

	crow::SimpleApp app;
	energyPricesServer server{app, service};

	crow::request request;
	crow::response response;

	request.url = "/api/prices/tomorrow";

	app.validate();
	app.handle_full(request, response);

	ASSERT_EQ(response.code, 200);

	std::cerr << response.body << std::endl;
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
		today,
		{{stringUtils::getDate(today) + "_H01", 345.67, std::nullopt, std::nullopt},
		{stringUtils::getDate(today) + "_H02", 456.78, std::nullopt, std::nullopt}},
	};

	EXPECT_CALL(provider, getPrices(testing::Truly(
		[](const std::chrono::year_month_day &day) {
			return day == today;})))
		.WillOnce(testing::Return(input));

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

TEST(energyPricesServer, responseContainsDateAndSummary) {
	testing::NiceMock<mockPriceProvider> provider;

	const energyPricesTable input{
		today,
		{{stringUtils::getDate(today) + "_H01", 345.67, std::nullopt, std::nullopt},
		{stringUtils::getDate(today) + "_H02", 456.78, std::nullopt, std::nullopt}},
	};

	EXPECT_CALL(provider, getPrices(testing::Truly(
			[](const std::chrono::year_month_day &day) {
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

	ASSERT_TRUE(json.has("date"));
	EXPECT_EQ(json["date"].s(), stringUtils::getDate(today));

	ASSERT_TRUE(json.has("summary"));
	EXPECT_DOUBLE_EQ(json["summary"]["min"].d(), 0.35);
	EXPECT_DOUBLE_EQ(json["summary"]["max"].d(), 0.46);
}

TEST(energyPricesServer, tomorrowReturnsNoDataWhenProviderReturnsToday) {
	testing::NiceMock<mockPriceProvider> provider;
	const std::chrono::year_month_day today{
		std::chrono::floor<std::chrono::days>(
			std::chrono::system_clock::now())
	};

	const auto tomorrow = std::chrono::year_month_day{
		std::chrono::sys_days{today} + std::chrono::days{1}
	};

	const energyPricesTable input{
		today,
		{{"00:00 - 01:00", 345.67, {}, {}}}
	};

	EXPECT_CALL(provider, getPrices(tomorrow)).WillOnce(testing::Return(input));

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
	EXPECT_EQ(json.t(), crow::json::type::Null);
}

TEST(energyPricesServer, noDataIfDateIsValidButStillNoPrices) {
	testing::NiceMock<mockPriceProvider> provider;

	const energyPricesTable input{
		today,
		{
			{stringUtils::getDate(today) + "_H01", std::nullopt, std::nullopt, std::nullopt},
			{stringUtils::getDate(today) + "_H02", std::nullopt, std::nullopt, std::nullopt}
		}
	};

	EXPECT_CALL(provider, getPrices(testing::_))
		.WillOnce(testing::Return(input));

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
	EXPECT_EQ(json.t(), crow::json::type::Null);
}

