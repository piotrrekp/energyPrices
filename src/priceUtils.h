#ifndef _PRICEUTILS_H_
#define _PRICEUTILS_H_

#include "energyPricesModel.h"
#include <optional>

namespace priceUtils {
	std::optional<double> selectPrice(const EnergyPrice &values);
};

#endif // _PRICEUTILS_H_
