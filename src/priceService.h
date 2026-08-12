#ifndef _PRICESERVICE_H_
#define _PRICESERVICE_H_

#include "displayModel.h"
#include "energyPricesModel.h"

class priceService {
public:
	displayPrices getPriceTable(const energyPricesTable &table);
};

#endif // _PRICESERVICE_H_
