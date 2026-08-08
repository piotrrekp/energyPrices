#include "httpClient.h"
#include "tgeParser.h"
#include <iostream>


int main() {
	std::cout << "działam!" << std::endl;

	httpClient client("https://tge.pl/energia-elektryczna-rdn");
	auto page = client.getPage();
	TgeParser parser;
	auto table = parser.parseEnergyPricesTable(page);
	std::cout << table << std::endl;




	std::cout << "już nie..." << std::endl;
	return 0;
}
