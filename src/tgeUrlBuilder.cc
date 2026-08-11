#include "tgeUrlBuilder.h"

#include <iostream>
#include <sstream>
#include <iomanip>

const std::string baseUrl = "https://tge.pl/energia-elektryczna-rdn?dateShow=";

std::string tgeUrlBuilder::getUrlForDate(const std::chrono::year_month_day forDate) {
	std::chrono::year_month_day day{std::chrono::sys_days{forDate} - std::chrono::days{1}};
	std::stringstream ss;
	ss << baseUrl
		<< std::setw(2) << std::setfill('0') << static_cast<unsigned>(day.day()) << "-"
		<< std::setw(2) << std::setfill('0') << static_cast<unsigned>(day.month()) << "-"
		<< static_cast<int>(day.year());
	return ss.str();
}

std::string tgeUrlBuilder::getUrlForTomorrow() {
        std::chrono::year_month_day date{
		std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())};
	std::chrono::year_month_day day{std::chrono::sys_days{date} + std::chrono::days{1}};

	return getUrlForDate(day);
}

std::string tgeUrlBuilder::getUrlForToday() {
        std::chrono::year_month_day date{
		std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())};
	return getUrlForDate(date);
}
