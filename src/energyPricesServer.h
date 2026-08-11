#ifndef _ENERGYPRICESSERVER_H_
#define _ENERGYPRICESSERVER_H_

#include "crow.h"

class energyPricesServer {
public:
	energyPricesServer();
	void run();

private:
	void setupRoutes();
	crow::SimpleApp app;
};

#endif // _ENERGYPRICESSERVER_H_
