#include "tgePriceProvider.h"
#include "httpClient.h"
#include "tgeParser.h"
#include "tgeUrlBuilder.h"

energyPricesTable tgePriceProvider::getPrices(const std::chrono::year_month_day date) {
	tgeUrlBuilder builder;
	httpClient client(builder.getUrlForDate(date));
	TgeParser parser;
	auto result = parser.parseEnergyPricesTable(client.getPage());
	result.first = date;

	return result;
}
