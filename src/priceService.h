#ifndef _PRICESERVICE_H_
#define _PRICESERVICE_H_

#include "displayModel.h"
#include "energyPricesModel.h"
#include "priceProvider.h"

class priceService {
public:
	priceService(priceProvider &priceProvider);
	std::optional<dailyPrices> getPriceTable(const std::chrono::year_month_day date);
	priceSummary getPriceSummary(const std::chrono::year_month_day date);
private:
	bool checkDate(std::string_view dateFromTable, std::string_view requestedDate);
	bool anyPricesExists(const dailyPrices &prices);
	std::optional<double> getPrice(const EnergyPrice &row);
	dailyPrices getPriceTable(const energyPricesTable &table);
	std::string formateTimePeriod(std::string_view time);
	double convertToKWh(const double pricePerMWh);
	priceProvider &provider;
};

#endif // _PRICESERVICE_H_
