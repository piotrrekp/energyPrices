#include "tgeParser.h"

#include "htmlTableExtractor.h"



RawTable TgeParser::parseEnergyPricesTable(const std::string_view html) const {
	HtmlTableExtractor extractor;
	auto tables = extractor.extractTables(html);
	for (auto &table : tables) {
		if (isProperTable(table)) {
			return table;
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

