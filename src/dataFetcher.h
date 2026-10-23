#ifndef _DATAFETCHER_H_
#define _DATAFETCHER_H_

#include "priceProvider.h"
#include "daySaver.h"

class dataFetcher {
public:
	dataFetcher(priceProvider &provider, daySaver &writer);
	void store(const std::chrono::year_month_day &deliveryDay);

private:
	fetchedDay prepareDay(
			const energyPricesTable &data,
			const std::chrono::system_clock::time_point &timestmp,
			const std::chrono::year_month_day &day);

	priceProvider &provider;
	daySaver &writer;
};

#endif // _DATAFETCHER_H_
