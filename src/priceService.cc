#include "priceService.h"
#include <iostream>
#include <iomanip>

priceService::priceService(priceProvider &_provider) : provider(_provider) {}

displayPrices priceService::getPriceTable([[maybe_unused]]const energyPricesTable &table) {
	if (table.empty()) {
		return {};
	}
	displayPrices prices;
	for (const auto &row : table) {
		displayPrice price;
		price.time = formateTimePeriod(row.time);
		if (row.meanPrice) {
			price.price = row.meanPrice;
		} else if (row.fixing2) {
			price.price = row.fixing2;
		} else {
			price.price = row.fixing1;
		}

		prices.push_back(price);
	}
	return prices;
}

displayPrices priceService::getPriceTable(const std::chrono::year_month_day date) {
	return getPriceTable(provider.getPrices(date));
}

std::string priceService::formateTimePeriod(std::string_view time) {
	const auto found = time.find('H');
	if (found == time.npos) {
		return time.data();
	}
	const auto hour = std::stoi(std::string{time.substr(found + 1, time.npos).data()});
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(2) << hour - 1 << ":00 - " <<
		std::setfill('0') << std::setw(2) << hour << ":00";

	return ss.str();
}
