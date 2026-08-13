#ifndef _TGEPRICEPROVIDER_H_
#define _TGEPRICEPROVIDER_H_

#include "priceProvider.h"

class tgePriceProvider : public priceProvider {
public:
	energyPricesTable getPrices(const std::chrono::year_month_day date) override;
};

#endif // _TGEPRICEPROVIDER_H_
