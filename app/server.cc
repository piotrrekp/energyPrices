#include "energyPricesServer.h"
#include "tgePriceProvider.h"
#include "priceService.h"


int main() {
	tgePriceProvider provider;
	priceService service{provider};
	crow::SimpleApp app;
	energyPricesServer server(app, service);

	server.run();

	return 0;
}
