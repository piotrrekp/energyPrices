#pragma once

#include "stringUtils.h"
#include <chrono>
#include <vector>
#include <string>
#include <ostream>
#include <optional>


struct EnergyPrice {
	std::string time;
	std::optional<double> fixing1;
	std::optional<double> fixing2;
	std::optional<double> meanPrice;

	bool operator==(const EnergyPrice &) const = default;

	std::ostream &printOptional(std::ostream &out, const std::optional<double> &value) {
		if (value) {
			out << *value;
		} else {
			out << " - ";
		}
		return out;
	}

	friend std::ostream& operator<<(std::ostream &out, const EnergyPrice &price) {
		return out << "{" << price.time << " : "
			<< (price.fixing1.has_value() ? std::to_string(price.fixing1.value()) : std::string(" - ")) << ", "
			<< (price.fixing2.has_value() ? std::to_string(price.fixing2.value()) : std::string(" - ")) << ", "
			<< (price.meanPrice.has_value() ? std::to_string(price.meanPrice.value()) : std::string(" - ")) << ", "
			<< "}";
	}
};

using energyPricesTable = std::pair<std::chrono::year_month_day, std::vector<EnergyPrice>>;

inline std::ostream& operator<<(std::ostream &out, const energyPricesTable &prices) {
	out << "Prices for " << stringUtils::getDate(prices.first) << std::endl;
	for (const auto & row: prices.second) {
		out << row << std::endl;
	}

	return out;
}
