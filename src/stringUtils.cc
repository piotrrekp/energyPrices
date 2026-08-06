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
}
