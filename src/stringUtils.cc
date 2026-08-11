#include "stringUtils.h"

#include <algorithm>
#include <ranges>


namespace stringUtils {
	std::string trimLeft(const std::string &text) {
		auto trimmed = text;
		const auto first = std::ranges::find_if_not(trimmed, [](unsigned char c) {
				return std::isspace(c);});
		trimmed.erase(trimmed.begin(), first);
		return trimmed;
	}

	std::string trimRight(const std::string &text) {
		auto trimmed = text;
		const auto last = std::ranges::find_if_not(trimmed | std::views::reverse, [](unsigned char c) {
				return std::isspace(c);});
		trimmed.erase(last.base(), trimmed.end());
		return trimmed;
	}

	std::string trim(const std::string &text) {
		auto trimmed = text;
		trimmed = trimRight(trimmed);
		trimmed = trimLeft(trimmed);
		return trimmed;
	}

	std::optional<double> getDouble(const std::string &str) {
		std::string value = trim(str);
		std::ranges::replace(value, ',', '.');
		std::erase(value, ' ');

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
}
