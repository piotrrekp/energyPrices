#include <gtest/gtest.h>
#include <string>

#include "../src/tgeParser.h"
#include "../utils/utils.h"


TEST(tgeParser, emptyHtmlThrowsException) {
	TgeParser parser;
	ASSERT_THROW(parser.parseEnergyPricesTable(""), std::runtime_error);
}

TEST(tgeParser, sanitizerTest) {
	TgeParser parser;
	std::string simplestHtml = "<html><body></<body></hmlt>";
	ASSERT_NO_THROW(parser.parseEnergyPricesTable(simplestHtml));
}
