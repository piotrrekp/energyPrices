#include "energyPricesServer.h"

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
		crow::json::wvalue response;
		std::size_t i = 0;

		std::chrono::year_month_day date{
			std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())};
		std::chrono::year_month_day day{std::chrono::sys_days{date} + std::chrono::days{1}};

		auto prices = this->getPrices(day);
		for (const auto &x : prices) {
			response["prices"][i]["time"] = x.time;
			response["prices"][i++]["price"] = x.price.value();

		}
		return response;
	});
}

void energyPricesServer::routeToPricesToday() {
	CROW_ROUTE(app, "/api/prices/today")([this](){
		crow::json::wvalue response;
		std::size_t i = 0;

		std::chrono::year_month_day date{
			std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())};

		auto prices = this->getPrices(date);
		for (const auto &x : prices) {
			response["prices"][i]["time"] = x.time;
			response["prices"][i++]["price"] = x.price.value();

		}
		return response;
	});
}

#include <iomanip>
displayPrices energyPricesServer::getPrices(const std::chrono::year_month_day date) {
	std::cout << __PRETTY_FUNCTION__ << " -> for date " <<
		static_cast<int>(date.year()) << '-'
		    << std::setw(2) << std::setfill('0')
		    << static_cast<unsigned>(date.month()) << '-'
		    << std::setw(2)
		    << static_cast<unsigned>(date.day()) << std::endl;
	return pricesService.getPriceTable(date);
}
