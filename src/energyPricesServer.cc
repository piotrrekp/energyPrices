#include "energyPricesServer.h"

energyPricesServer::energyPricesServer() {
	setupRoutes();
}

void energyPricesServer::run() {
	app.port(18080).run();
}

void energyPricesServer::setupRoutes() {
	CROW_ROUTE(app, "/")([](){ return "HELLO WORLD!"; });
}
