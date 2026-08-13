#ifndef _PRICESERVICE_H_
#define _PRICESERVICE_H_

#include "displayModel.h"
#include "energyPricesModel.h"
#include "priceProvider.h"

class priceService {
public:
	priceService(priceProvider &priceProvider);
	displayPrices getPriceTable(const std::chrono::year_month_day date);
private:
	displayPrices getPriceTable(const energyPricesTable &table);
	std::string formateTimePeriod(std::string_view time);
	priceProvider &provider;
};

#endif // _PRICESERVICE_H_
