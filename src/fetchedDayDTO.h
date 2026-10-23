#ifndef _FETCHEDDAYDTO_H_
#define _FETCHEDDAYDTO_H_

#include <chrono>
#include <string>
#include <vector>

struct fetchedPrice {
	std::string period;
	double pricePerMWh;

	bool operator<=>(const fetchedPrice &) const = default;
};

struct fetchedDay {
	std::chrono::year_month_day date;
	std::chrono::system_clock::time_point timestamp;
	std::vector<fetchedPrice> prices;
	bool completed = false;
};

#endif // _FETCHEDDAYDTO_H_
