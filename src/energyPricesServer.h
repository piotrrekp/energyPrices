#ifndef _ENERGYPRICESSERVER_H_
#define _ENERGYPRICESSERVER_H_

#include "crow.h"
#include "displayModel.h"
#include "priceService.h"

class energyPricesServer {
public:
	energyPricesServer(crow::SimpleApp &app, priceService &pricesService);
	void run();

private:
	void setupRoutes();
	void routeToIndex();
	void routeToPricesTomorrow();
	void routeToPricesToday();
	displayPrices getPrices(const std::chrono::year_month_day date);
        crow::json::wvalue extracted(crow::json::wvalue &response);
        crow::json::wvalue prepareResponse(const displayPrices &prices);

        crow::SimpleApp &app;
	priceService &pricesService;
};

#endif // _ENERGYPRICESSERVER_H_
