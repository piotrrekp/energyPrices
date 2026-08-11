#include "tgeParser.h"
#include "htmlTableExtractor.h"
#include "stringUtils.h"

#include <ranges>

energyPricesTable TgeParser::parseEnergyPricesTable(const std::string_view html) const {
	HtmlTableExtractor extractor;
	auto tables = extractor.extractTables(html);
	for (auto &table : tables) {
		if (isProperTable(table)) {
			return extractEnergyPrices(table);
		}
	}
	return {{}};
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

energyPricesTable TgeParser::extractEnergyPrices([[maybe_unused]] const RawTable &table) const {
	energyPricesTable prices{};
	for (const auto &row : table | std::views::drop(2)) {
		if (stringUtils::trim(row.at(1)) != "60") {
			continue;
		}
		EnergyPrice ep;
		ep.time = row.at(0);
		ep.fixing1 = getValue(row, 2);
		ep.fixing2 = getValue(row, 7);
		ep.meanPrice = getValue(row, 13);

		prices.push_back(ep);
	}

	return prices;
}

std::optional<double> TgeParser::getValue(const RawRow &row, const std::size_t index) const {
	if (row.size() <= index) {
		return std::nullopt;
	}

	return stringUtils::getDouble(row.at(index));
}

