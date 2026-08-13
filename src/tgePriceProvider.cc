#include "tgePriceProvider.h"
#include "httpClient.h"
#include "tgeParser.h"
#include "tgeUrlBuilder.h"

energyPricesTable tgePriceProvider::getPrices(const std::chrono::year_month_day date) {
	tgeUrlBuilder builder;
	httpClient client(builder.getUrlForDate(date));
	TgeParser parser;

	return parser.parseEnergyPricesTable(client.getPage());
}
