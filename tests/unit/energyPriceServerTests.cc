#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "energyPricesServer.h"

TEST(energyPricesServer, simpleJSON) {
    crow::SimpleApp app;
    energyPricesServer server(app);

    crow::request request;
    crow::response response;

    request.url = "/api/prices/tomorrow";

    app.validate();
    std::cerr << __PRETTY_FUNCTION__ << " -> " << &app << std::endl;
    app.handle_full(request, response);

    EXPECT_EQ(response.code, 200);
}
