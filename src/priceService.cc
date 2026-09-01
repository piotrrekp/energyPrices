#include "priceService.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cmath>

priceService::priceService(priceProvider &_provider) : provider(_provider) {}

dailyPrices priceService::getPriceTable(const energyPricesTable &table) {
	if (table.second.empty()) {
		return dailyPrices(table.first, {});
	}

	if (!checkDate(table.second.at(0).time, stringUtils::getDate(table.first))) {
		return dailyPrices{{}, {}};
	}

	std::vector<hourlyPrice> allPrices;
	for (const auto &row : table.second) {
		hourlyPrice price;
		price.time = formateTimePeriod(row.time);
		price.price = getPrice(row);
		allPrices.push_back(price);
	}

	return dailyPrices(table.first, allPrices);
}

std::optional<dailyPrices> priceService::getPriceTable(const std::chrono::year_month_day date) {
	auto prices = getPriceTable(provider.getPrices(date));
	if (prices.getDate() == date && anyPricesExists(prices)) {
		return prices;
	} else {
		return std::nullopt;
	}
}

bool priceService::anyPricesExists(const dailyPrices &prices) {
	return std::ranges::any_of(prices.getPrices(), [](const auto &hourly){
		return hourly.price.has_value();
	});
}

bool priceService::checkDate(std::string_view dateFromTable, std::string_view requestedDate) {
	const auto found = dateFromTable.find('_');
	if (found == dateFromTable.npos) {
		return false;
	}

	return dateFromTable.substr(0, found) == requestedDate;
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
