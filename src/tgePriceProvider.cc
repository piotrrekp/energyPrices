#include "tgePriceProvider.h"
#include "httpClient.h"
#include "tgeParser.h"
#include "tgeUrlBuilder.h"
#include <stdexcept>

energyPricesTable tgePriceProvider::getPrices(const std::chrono::year_month_day date) {
	tgeUrlBuilder builder;
	httpClient client(builder.getUrlForDate(date));
	TgeParser parser;
	auto result = parser.parseEnergyPricesTable(client.getPage());
	if (!result.second.empty() && result.first != date) {
		throw std::runtime_error{"TGE delivery date does not match requested date"};
	}

	return result;
}
