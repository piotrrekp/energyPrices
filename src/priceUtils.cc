#include "priceUtils.h"

std::optional<double> priceUtils::selectPrice(const EnergyPrice &values) {
	if (values.meanPrice) return values.meanPrice;
	else if (values.fixing2) return values.fixing2;
	else return values.fixing1;
}

