#ifndef _TGEPARSER_H_
#define _TGEPARSER_H_


#include "model.h"
#include "energyPricesModel.h"

class TgeParser {
public:
	energyPricesTable parseEnergyPricesTable(const std::string_view html) const;
private:
	energyPricesTable extractEnergyPrices(const RawTable &table) const;
	bool isProperTable(const RawTable &table) const;
	std::optional<double> getValue(const RawRow &row, const std::size_t index) const;
	std::optional<double> getDouble(const std::string &str) const;
};

#endif // _TGEPARSER_H_
