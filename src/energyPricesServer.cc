#include "energyPricesServer.h"

#include <iomanip>

constexpr std::string_view IndexPage = "static/index.html";

energyPricesServer::energyPricesServer(crow::SimpleApp &_app, priceService &service) : app{_app}, pricesService{service} {
	setupRoutes();
}

void energyPricesServer::run() {
	app.port(18080).run();
}

void energyPricesServer::setupRoutes() {
	routeToIndex();
	routeToPricesTomorrow();
	routeToPricesToday();
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
		std::chrono::year_month_day date{
			std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())};
		std::chrono::year_month_day day{std::chrono::sys_days{date} + std::chrono::days{1}};

		auto prices = this->getPrices(day);
		if (!prices) {
			return crow::json::wvalue{nullptr};
		}
		return prepareResponse(*prices);
	});
}

void energyPricesServer::routeToPricesToday() {
	CROW_ROUTE(app, "/api/prices/today")([this](){
		std::chrono::year_month_day date{
			std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())};

		auto prices = this->getPrices(date);
		if (!prices) {
			return crow::json::wvalue{nullptr};
		}
		return prepareResponse(*prices);
	});
}

crow::json::wvalue energyPricesServer::prepareResponse(const dailyPrices &prices) {
		crow::json::wvalue response;
		std::size_t i = 0;
		response["date"] = stringUtils::getDate(prices.getDate());
		response["summary"]["min"] = prices.getSummary().min
			? *prices.getSummary().min : crow::json::wvalue(nullptr);
		response["summary"]["max"] = prices.getSummary().max
			? *prices.getSummary().max : crow::json::wvalue(nullptr);
		for (const auto &x : prices.getPrices()) {
			response["prices"][i]["time"] = x.time;
			if (x.price) {
				response["prices"][i++]["price"] =  *x.price;
			} else {
				response["prices"][i++]["price"] = nullptr;
			}
		}
		return response;
}

std::optional<dailyPrices> energyPricesServer::getPrices(const std::chrono::year_month_day deliveryDate) {
	return pricesService.getPriceTable(deliveryDate);
}
