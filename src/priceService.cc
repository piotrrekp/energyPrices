#include "priceService.h"

displayPrices priceService::getPriceTable([[maybe_unused]]const energyPricesTable &table) {
	if (table.empty()) {
		return {};
	}
	displayPrices prices;
	for (const auto &row : table) {
		displayPrice price;
		price.time = row.time;
		if (row.meanPrice) {
			price.price = row.meanPrice;
		} else if (row.fixing2) {
			price.price = row.fixing2;
		} else {
			price.price = row.fixing1;
		}

		prices.push_back(price);
	}
	return prices;
}
