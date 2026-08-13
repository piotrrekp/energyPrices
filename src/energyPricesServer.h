#ifndef _ENERGYPRICESSERVER_H_
#define _ENERGYPRICESSERVER_H_

#include "crow.h"
#include "displayModel.h"

class energyPricesServer {
public:
	energyPricesServer(crow::SimpleApp &app);
	void setPrices(const displayPrices &table);
	void run();

private:
	void setupRoutes();
	void routeToIndex();
	void routeToPricesTomorrow();

	crow::SimpleApp &app;
	displayPrices prices;
};

#endif // _ENERGYPRICESSERVER_H_
