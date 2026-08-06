#include "tgeParser.h"
#include "htmlTableExtractor.h"
#include "stringUtils.h"

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
	for (auto row = std::next(table.begin(), 2) ;row != table.end(); ++row) {
		if (stringUtils::trim(row->at(1)) != "60") {
			continue;
		}
		EnergyPrice ep;
		ep.time = row->at(0);
		ep.fixing1 = getDouble(stringUtils::trim(row->at(2)));
		ep.fixing2 = getDouble(stringUtils::trim(row->at(7)));
		ep.meanPrice = getDouble(stringUtils::trim(row->at(13)));

		prices.push_back(ep);
	}

	return prices;
}

std::optional<double> TgeParser::getDouble(const std::string &str) const {
	std::string value = str;
	std::replace(value.begin(), value.end(), ',', '.');

	try {
		std::size_t parsedCharacters = 0;
		const double result = std::stod(value, &parsedCharacters);

		if (parsedCharacters != value.size()) {
			return std::nullopt;
		}

		return result;
	} catch (const std::invalid_argument &) {
		return std::nullopt;
	} catch (const std::out_of_range &) {
		return std::nullopt;
	}
}
