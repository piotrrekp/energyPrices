#ifndef _DISPLAYMODEL_H_
#define _DISPLAYMODEL_H_

#include <optional>
#include <vector>
#include <string>


struct displayPrice {
	std::string time;
	std::optional<double> price;
	bool operator==(const displayPrice &) const = default;
};

using displayPrices = std::vector<displayPrice>;

#include <iostream>
#include <sstream>

inline std::ostream &operator<<(std::ostream &out, const displayPrice &price) {
    return out << "{"
               << price.time << ": " << (price.price ? std::to_string(*price.price) : "null")
               << "}";
}

inline std::ostream &operator<<(std::ostream &out, const displayPrices &prices) {
	std::stringstream tmp{"{\n"};
	for (const auto &price: prices) {
		tmp << "\t" << price << "\n";
	}
	tmp << "}";
	return out << tmp.str();
}

#endif // _DISPLAYMODEL_H_
