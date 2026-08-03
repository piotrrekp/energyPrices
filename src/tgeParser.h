#ifndef _TGEPARSER_H_
#define _TGEPARSER_H_


#include "model.h"

class TgeParser {
public:
	RawTable parseEnergyPricesTable(const std::string_view html) const;
private:
	bool isProperTable(const RawTable &table) const;
};

#endif // _TGEPARSER_H_
