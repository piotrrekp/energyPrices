#include <optional>
#include <vector>
#include <string>


struct displayPrice {
	std::string time;
	std::optional<double> price;
	bool operator==(const displayPrice &) const = default;
};

using displayPrices = std::vector<displayPrice>;
