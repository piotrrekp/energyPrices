#include "priceService.h"
#include <iostream>
#include <iomanip>
#include <cmath>

priceService::priceService(priceProvider &_provider) : provider(_provider) {}

displayPrices priceService::getPriceTable(const energyPricesTable &table) {
	if (table.empty()) {
		return {};
	}
	displayPrices prices;
	for (const auto &row : table) {
		displayPrice price;
		price.time = formateTimePeriod(row.time);
		price.price = getPrice(row);
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

std::optional<double> priceService::getPrice(const EnergyPrice &row) {
	std::optional<double> value;
	if (row.meanPrice) {
		value = row.meanPrice;
	} else if (row.fixing2) {
		value = row.fixing2;
	} else {
		value = row.fixing1;
	}

	if (value) {
		return convertToKWh(*value);
	}
	return std::nullopt;
}

double priceService::convertToKWh(const double pricePerMWh) {
	return std::round (pricePerMWh / 10.) / 100.;

}

