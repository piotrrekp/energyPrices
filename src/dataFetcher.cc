#include "dataFetcher.h"


dataFetcher::dataFetcher(priceProvider &dataProvider, daySaver &dataWriter)
	: provider(dataProvider), writer(dataWriter) {}

void dataFetcher::store(const std::chrono::year_month_day &deliveryDay) {
	auto prices = provider.getPrices(deliveryDay);
	auto time = std::chrono::system_clock::now();
	auto dataToWrite = prepareDay(prices, time, deliveryDay);
	writer.store(dataToWrite);
}

fetchedDay dataFetcher::prepareDay(
		[[maybe_unused]] const energyPricesTable &data,
		[[maybe_unused]] const std::chrono::system_clock::time_point &timestmp,
		[[maybe_unused]] const std::chrono::year_month_day &day) {
	fetchedDay dayPrices;
	dayPrices.date = day;
	// dayPrices.prices
	return dayPrices;
}
