#include "httpClient.h"
#include "tgeParser.h"
#include "tgeUrlBuilder.h"

#include <iostream>


int main() {
	std::chrono::ydsdear_month_day date{
		std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())};
	std::chrono::year_month_day day{std::chrono::sys_days{date} + std::chrono::days{1}};
	tgeUrlBuilder builder;
	httpClient client(builder.getUrlForDate(day));
	auto page = client.getPage();
	TgeParser parser;
	auto table = parser.parseEnergyPricesTable(page);
	std::cout << "CENY NA JUTRO: " << std::endl;
	std::cout << table << std::endl;

	client = httpClient(builder.getUrlForDate(date));
	page = client.getPage();
	table = parser.parseEnergyPricesTable(page);

	std::cout << "CENY NA DZISIAJ: " << std::endl;
	std::cout << table << std::endl;
	return 0;
}
