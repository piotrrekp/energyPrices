#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <optional>


struct EnergyPrice {
	std::string time;
	std::optional<double> fixing1;
	std::optional<double> fixing2;
	std::optional<double> meanPrice;

	// EnergyPrice(): time("00:00_H00"), fixing1(std::nullopt), fixing2(std::nullopt), meanPrice(std::nullopt) {}
	// EnergyPrice(std::string t, double f1, double f2, double mean): time(t), fixing1(f1), fixing2(f2), meanPrice(mean) {}
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

using energyPricesTable = std::vector<EnergyPrice>;

inline std::ostream& operator<<(std::ostream &out, const energyPricesTable &prices) {
	for (const auto & row: prices) {
		out << row << std::endl;
	}

	return out;
}
