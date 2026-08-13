#ifndef _MOCKPRICEPROVIDER_H_
#define _MOCKPRICEPROVIDER_H_


#include <gmock/gmock.h>
#include "priceProvider.h"

class mockPriceProvider : public priceProvider {
public:
    MOCK_METHOD(energyPricesTable, getPrices, (const std::chrono::year_month_day), (override));
};


#endif // _MOCKPRICEPROVIDER_H_
