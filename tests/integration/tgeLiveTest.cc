#include <gtest/gtest.h>

#include <chrono>
#include <cmath>
#include <set>
#include <string>

#include "tgeParser.h"
#include "tgeUrlBuilder.h"
#include "httpClient.h"
#include "stringUtils.h"

TEST(TgeParserIntegration, getTableFromSite) {
	// Use UTC dates, matching the application's current date convention.
	const auto today = std::chrono::floor<std::chrono::days>(
		std::chrono::system_clock::now());
	const std::chrono::year_month_day yesterday{today - std::chrono::days{1}};
	tgeUrlBuilder builder;
	const auto url = builder.getUrlForDate(yesterday);
	SCOPED_TRACE(url);
	httpClient client(url);
	const auto page = client.getPage();

	TgeParser parser;
	const auto parsed = parser.parseEnergyPricesTable(page);
	const auto &table = parsed.second;
	ASSERT_FALSE(table.empty()) << "No hourly prices for yesterday";
	EXPECT_EQ(parsed.first, yesterday);
	const auto periodPrefix = stringUtils::getDate(yesterday) + "_H";
	std::set<std::string> periods;
	for (const auto &row : table) {
		SCOPED_TRACE(row.time);
		EXPECT_TRUE(row.time.starts_with(periodPrefix));
		EXPECT_TRUE(periods.insert(row.time).second) << "Duplicate period";
		EXPECT_TRUE(row.meanPrice || row.fixing2 || row.fixing1)
			<< "No available price";
		for (const auto &price : {row.meanPrice, row.fixing2, row.fixing1}) {
			if (price) {
				EXPECT_TRUE(std::isfinite(*price));
			}
		}
	}
}
