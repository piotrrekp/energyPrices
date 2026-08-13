#include "energyPricesServer.h"
#include "tgeParser.h"
#include "httpClient.h"
#include "tgeUrlBuilder.h"
#include "priceService.h"


int main() {
	TgeParser tge;
	tgeUrlBuilder builder;
	std::string url = builder.getUrlForTomorrow();
	httpClient client(url);
	auto html = client.getPage();
	auto prices = tge.parseEnergyPricesTable(html);
	crow::SimpleApp app;
	energyPricesServer server(app);
	priceService service;
	server.setPrices(service.getPriceTable(prices));
	server.run();

	return 0;
}
