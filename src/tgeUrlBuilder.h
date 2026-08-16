#ifndef _TGEDATEBUILDER_H_
#define _TGEDATEBUILDER_H_

#include <chrono>
#include <string>

class tgeUrlBuilder {
public:
	std::string getUrlForDate(const std::chrono::year_month_day forDate);
};

#endif // _TGEDATEBUILDER_H_
