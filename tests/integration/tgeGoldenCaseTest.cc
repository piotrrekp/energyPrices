#include <gtest/gtest.h>
#include <string>

#include "tgeParser.h"
#include "httpClient.h"
#include "../utils/utils.h"

TEST(TgeParserIntegration, parsesHourlyPricesFromGoldenCase) {
	const auto html =
		loadFile(std::string(TEST_DATA_DIR) + "/tgeGoldenCase.html");

	const energyPricesTable expected{
		{"2026-07-18_H01", 669.04, 679.53, 671.62},
		{"2026-07-18_H02", 615.00, 631.88, 621.20},
		{"2026-07-18_H03", 590.00, 617.83, 597.74},
		{"2026-07-18_H04", 560.04, 588.68, 568.26},
		{"2026-07-18_H05", 550.00, 583.19, 558.67},
		{"2026-07-18_H06", 529.47, 577.30, 541.77},
		{"2026-07-18_H07", 528.79, 552.04, 534.15},
		{"2026-07-18_H08", 520.00, 528.64, 522.40},
		{"2026-07-18_H09", 524.99, 531.63, 526.55},
		{"2026-07-18_H10", 400.00, 467.99, 415.96},
		{"2026-07-18_H11", 161.36, 265.53, 184.27},
		{"2026-07-18_H12", 94.93, 68.05, 89.01},
		{"2026-07-18_H13", 24.97, 5.33, 21.13},
		{"2026-07-18_H14", 10.00, 5.30, 9.11},
		{"2026-07-18_H15", 9.09, 15.51, 10.40},
		{"2026-07-18_H16", 20.00, 32.44, 22.98},
		{"2026-07-18_H17", 150.95, 141.95, 147.04},
		{"2026-07-18_H18", 440.00, 479.53, 448.18},
		{"2026-07-18_H19", 580.98, 575.44, 579.27},
		{"2026-07-18_H20", 642.49, 684.37, 660.72},
		{"2026-07-18_H21", 774.63, 850.93, 801.36},
		{"2026-07-18_H22", 760.43, 759.91, 761.09},
		{"2026-07-18_H23", 680.20, 690.97, 684.09},
		{"2026-07-18_H24", 627.85, 644.87, 634.39},
	};

	const TgeParser parser;
	const auto result = parser.parseEnergyPricesTable(html);

	EXPECT_EQ(result, expected);
}


TEST(TgeParserIntegration, skipsRowWithMissingColumns) {
	auto html =
		loadFile(std::string(TEST_DATA_DIR) + "/tgeMissionPrices.html");

	const TgeParser parser;
	const auto prices = parser.parseEnergyPricesTable(html);

	ASSERT_EQ(prices.size(), 3);
	EXPECT_EQ(prices[0].time, "2026-07-18_H01");
	EXPECT_EQ(prices[2].time, "2026-07-18_H03");

	const auto &price = prices[1];
	EXPECT_EQ(price.time, "2026-07-18_H02");
	EXPECT_EQ(price.fixing1, std::nullopt);
	EXPECT_EQ(price.fixing2, std::nullopt);
	EXPECT_EQ(price.meanPrice, std::nullopt);
}

TEST(TgeParserIntegration, getTableFromSite) {
	httpClient client("https://tge.pl/energia-elektryczna-rdn?dateShow=07-08-2026");
	auto page = client.getPage();

	TgeParser parser;
	auto table = parser.parseEnergyPricesTable(page);
	const std::vector<EnergyPrice> expected{
		{"2026-08-08_H01", 699.86, 671.39, 693.18},
		{"2026-08-08_H02", 668.01, 643.19, 662.94},
		{"2026-08-08_H03", 642.52, 627.81, 639.45},
		{"2026-08-08_H04", 640.01, 633.04, 638.67},
		{"2026-08-08_H05", 634.04, 624.18, 632.08},
		{"2026-08-08_H06", 632.49, 630.37, 632.54},
		{"2026-08-08_H07", 624.66, 630.69, 626.00},
		{"2026-08-08_H08", 576.70, 593.35, 580.25},
		{"2026-08-08_H09", 500.47, 495.68, 503.34},
		{"2026-08-08_H10", 326.89, 272.27, 313.72},
		{"2026-08-08_H11", 46.63, 26.36, 42.15},
		{"2026-08-08_H12", 20.00, 15.31, 18.84},
		{"2026-08-08_H13", 20.00, 13.82, 18.57},
		{"2026-08-08_H14", 17.20, 20.80, 18.14},
		{"2026-08-08_H15", 20.01, 37.66, 24.30},
		{"2026-08-08_H16", 92.10, 135.31, 103.44},
		{"2026-08-08_H17", 300.74, 408.60, 328.08},
		{"2026-08-08_H18", 505.38, 539.07, 520.55},
		{"2026-08-08_H19", 632.35, 657.21, 645.25},
		{"2026-08-08_H20", 789.65, 757.32, 781.88},
		{"2026-08-08_H21", 837.16, 842.65, 838.92},
		{"2026-08-08_H22", 820.00, 786.55, 808.79},
		{"2026-08-08_H23", 735.23, 736.20, 736.90},
		{"2026-08-08_H24", 682.33, 665.40, 678.26},
	};
	EXPECT_EQ(table, expected);
}
