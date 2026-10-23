#include "dataFetcher.h"
#include "priceUtils.h"

#include <algorithm>
#include <stdexcept>


dataFetcher::dataFetcher(priceProvider &dataProvider, daySaver &dataWriter)
	: provider(dataProvider), writer(dataWriter) {}

void dataFetcher::store(const std::chrono::year_month_day &deliveryDay) {
	auto prices = provider.getPrices(deliveryDay);
	if (prices.second.empty()) {
		return;
	}
	auto time = std::chrono::system_clock::now();
	auto dataToWrite = prepareDay(prices, time, deliveryDay);
	if (dataToWrite.prices.empty()) {
		return;
	}
	writer.store(dataToWrite);
}

fetchedDay dataFetcher::prepareDay(
		const energyPricesTable &data,
		const std::chrono::system_clock::time_point &timestmp,
		const std::chrono::year_month_day &day) {
	if (data.first != day) {
		throw std::invalid_argument{"Delivery date does not match requested date"};
	}
	fetchedDay dayPrices;
	dayPrices.date = day;
	dayPrices.timestamp = timestmp;
	dayPrices.prices.reserve(data.second.size());
	for (const auto &row : data.second) {
		if (auto price = priceUtils::selectPrice(row)) {
			dayPrices.prices.push_back({row.time, *price});
		}
	}
	// Ordinary 24-period days only; DST period rules remain to be defined.
	dayPrices.completed = data.second.size() == 24
		&& std::all_of(data.second.begin(), data.second.end(), [](const auto &row) {
			return row.meanPrice.has_value();
		});
	return dayPrices;
}
