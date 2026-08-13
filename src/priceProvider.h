#ifndef _PRICEPROVIDER_H_
#define _PRICEPROVIDER_H_

#include "energyPricesModel.h"
#include <chrono>

class priceProvider {
public:
	virtual ~priceProvider() = default;
	virtual energyPricesTable getPrices(const std::chrono::year_month_day date) = 0;
};

#endif // _PRICEPROVIDER_H_
