#include "energyPricesServer.h"

constexpr std::string_view IndexPage = "static/index.html";

energyPricesServer::energyPricesServer(crow::SimpleApp &_app) : app{_app} {
	setupRoutes();
}

void energyPricesServer::run() {
	app.port(18080).run();
}

void energyPricesServer::setupRoutes() {
	routeToIndex();
	routeToPricesTomorrow();
}

void energyPricesServer::routeToIndex() {
	CROW_ROUTE(app, "/")([](){
		crow::response response;
		response.set_static_file_info(IndexPage.data());
		return response;
	});
}

void energyPricesServer::routeToPricesTomorrow() {
	CROW_ROUTE(app, "/api/prices/tomorrow")([this](){
		crow::json::wvalue response;
		std::size_t i = 0;
		for (const auto &x : prices) {
			response["prices"][i]["time"] = x.time;
			response["prices"][i++]["price"] = x.price.value();

		}
		return response;
	});
}

void energyPricesServer::setPrices(const displayPrices &table) {
	prices = table;
}
