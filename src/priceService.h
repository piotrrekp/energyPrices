#ifndef _PRICESERVICE_H_
#define _PRICESERVICE_H_

#include "displayModel.h"
#include "energyPricesModel.h"
#include "priceProvider.h"

class priceService {
public:
	priceService(priceProvider &priceProvider);
	dailyPrices getPriceTable(const std::chrono::year_month_day date);
	priceSummary getPriceSummary(const std::chrono::year_month_day date);
private:
	std::optional<double> getPrice(const EnergyPrice &row);
	dailyPrices getPriceTable(const energyPricesTable &table);
	std::string formateTimePeriod(std::string_view time);
	double convertToKWh(const double pricePerMWh);
	priceProvider &provider;
};

#endif // _PRICESERVICE_H_
