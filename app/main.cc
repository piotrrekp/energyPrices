#include "httpClient.h"
#include "tgeParser.h"
#include "tgeUrlBuilder.h"

#include <iostream>


int main() {
	std::cout << "działam!" << std::endl;

	tgeUrlBuilder builder;
	httpClient client(builder.getUrlForTomorrow());
	auto page = client.getPage();
	TgeParser parser;
	auto table = parser.parseEnergyPricesTable(page);
	std::cout << "CENY NA JUTRO: " << std::endl;
	std::cout << table << std::endl;

	client = httpClient(builder.getUrlForToday());
	page = client.getPage();
	table = parser.parseEnergyPricesTable(page);

	std::cout << "CENY NA DZISIAJ: " << std::endl;
	std::cout << table << std::endl;




	std::cout << "już nie..." << std::endl;
	return 0;
}
