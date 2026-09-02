#ifndef _DISPLAYMODEL_H_
#define _DISPLAYMODEL_H_

#include <algorithm>
#include <chrono>
#include <iostream>
#include <optional>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

struct hourlyPrice {
	std::string time;
	std::optional<double> price;
	bool operator==(const hourlyPrice &) const = default;
	bool operator<=>(const hourlyPrice &) const = default;
};

inline std::ostream &operator<<(std::ostream &out, const hourlyPrice &price) {
    return out << "{"
               << price.time << ": " << (price.price ? std::to_string(*price.price) : "null")
               << "}";
}


struct priceSummary {
	std::optional<double> min;
	std::optional<double> max;

	bool operator==(const priceSummary &) const = default;
	bool operator<=>(const priceSummary &) const = default;
};

class dailyPrices {
public:
    dailyPrices(std::chrono::year_month_day _date, std::vector<hourlyPrice> _prices)
        : date{_date}, prices{std::move(_prices)}, summary{calculateSummary(prices)} {}

    const auto& getDate() const {
        return date;
    }

    const auto& getPrices() const {
        return prices;
    }

    const auto& getSummary() const {
        return summary;
    }

    bool operator==(const dailyPrices &) const = default;

private:
    static priceSummary calculateSummary(const std::vector<hourlyPrice>& prices) {
	auto validPrices = prices |
	    std::views::filter([](const hourlyPrice &price) {
		    return price.price.has_value();
	});
	if (validPrices.empty()) {
	    return {std::nullopt, std::nullopt};
	}
	auto [min, max] = std::ranges::minmax(validPrices, {}, &hourlyPrice::price);
	return {min.price, max.price};
    }

    std::chrono::year_month_day date;
    std::vector<hourlyPrice> prices;
    priceSummary summary;
};

inline std::ostream &operator<<(std::ostream &out, const dailyPrices &prices) {
	std::stringstream tmp{"{\n"};
	for (const auto &price: prices.getPrices()) {
		tmp << "\t" << price << "\n";
	}
	tmp << "}";
	return out << tmp.str();
}

#endif // _DISPLAYMODEL_H_
