#include "tgeParser.h"
#include "htmlTableExtractor.h"
#include "stringUtils.h"

#include <ranges>
#include <charconv>
#include <stdexcept>

namespace {
std::chrono::year_month_day deliveryDate(std::string_view period) {
	if (period.size() < 11 || period[4] != '-' || period[7] != '-' || period[10] != '_') {
		throw std::runtime_error{"Invalid TGE delivery date"};
	}
	auto number = [](std::string_view part) {
		int value = 0;
		const auto [end, error] = std::from_chars(part.data(), part.data() + part.size(), value);
		if (error != std::errc{} || end != part.data() + part.size()) {
			throw std::runtime_error{"Invalid TGE delivery date"};
		}
		return value;
	};
	const auto date = std::chrono::year{number(period.substr(0, 4))}
		/ number(period.substr(5, 2)) / number(period.substr(8, 2));
	if (!date.ok()) {
		throw std::runtime_error{"Invalid TGE delivery date"};
	}
	return date;
}
}

energyPricesTable TgeParser::parseEnergyPricesTable(const std::string_view html) const {
	HtmlTableExtractor extractor;
	auto tables = extractor.extractTables(html);
	for (auto &table : tables) {
		if (isProperTable(table)) {
			return extractEnergyPrices(table);
		}
	}
	return {{}, {}};
}

bool TgeParser::isProperTable(const RawTable &table) const {
	for (const auto &row : table) {
		for (const auto &cell : row) {
			if (cell == "Data dostawy") {
				return true;
			}
		}
	}
	return false;
}

energyPricesTable TgeParser::extractEnergyPrices(const RawTable &table) const {
	energyPricesTable prices{};
	for (const auto &row : table | std::views::drop(2)) {
		if (row.size() < 2 || stringUtils::trim(row.at(1)) != "60") {
			continue;
		}
		EnergyPrice ep;
		ep.time = stringUtils::trim(row.at(0));
		const auto date = deliveryDate(ep.time);
		if (!prices.second.empty() && prices.first != date) {
			throw std::runtime_error{"Mixed TGE delivery dates"};
		}
		prices.first = date;
		ep.fixing1 = getValue(row, 2);
		ep.fixing2 = getValue(row, 7);
		ep.meanPrice = getValue(row, 13);

		prices.second.push_back(ep);
	}

	return prices;
}

std::optional<double> TgeParser::getValue(const RawRow &row, const std::size_t index) const {
	if (row.size() <= index) {
		return std::nullopt;
	}

	return stringUtils::getDouble(row.at(index));
}

