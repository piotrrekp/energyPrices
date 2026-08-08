#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>

#include "tgeParser.h"

std::ostream& operator<<(std::ostream &out, const RawTable &table) {
	std::string toPrint;
	for (const auto &row : table) {
		for (const auto &cell : row) {
			toPrint += cell;
			toPrint += ", ";
		}
		toPrint += "\n";
	}

	return out << toPrint;
}


TEST(tgeParser, getDataFromTable) {
	constexpr std::string_view html = R"HTML(
	<!DOCTYPE html>
	<html lang="pl">
	<body>
	<table class="table table-hover table-rdb" id="rdn">
	    <thead>
		<tr>
		    <th>&nbsp;</th>
		    <th>&nbsp;</th>
		    <th colspan="2">Fixing I</th>
		    <th colspan="2">Notowania ciągłe</th>
		    <th colspan="5">Fixing II</th>
		    <th colspan="6">Łącznie notowania</th>
		</tr>
		<tr>
		    <th>Data dostawy</th>
		    <th>Typ instrumentu</th>

		    <th>Kurs [PLN/MWh]</th>
		    <th>Wolumen [MW]</th>

		    <th>Kurs (średnioważony) [PLN/MWh]</th>
		    <th>Wolumen [MW]</th>

		    <th>Kurs jednolity [EUR/MWh]</th>
		    <th>Kurs jednolity [PLN/MWh]</th>
		    <th>Wolumen [MW]</th>
		    <th>Wolumen kupna [MW]</th>
		    <th>Wolumen sprzedaży [MW]</th>

		    <th>Kurs min. [PLN/MWh]</th>
		    <th>Kurs max. [PLN/MWh]</th>
		    <th>Kurs (średnioważony) [PLN/MWh]</th>
		    <th>Wolumen [MWh]</th>
		    <th>Wolumen kupna [MWh]</th>
		    <th>Wolumen sprzedaży [MWh]</th>
		</tr>
	    </thead>

	    <tbody>
		<tr>
		    <td>2026-07-18_H01</td>
		    <td>60</td>

		    <td>669,04</td>
		    <td>-</td>

		    <td>655,71</td>
		    <td>30,00</td>

		    <td>156,18</td>
		    <td>679,53</td>
		    <td>-</td>
		    <td>-</td>
		    <td>-</td>

		    <td>522,01</td>
		    <td>963,83</td>
		    <td>671,62</td>
		    <td>5 947,625</td>
		    <td>5 947,625</td>
		    <td>4 967,275</td>
		</tr>

		<tr>
		    <td>2026-07-18_Q00:15</td>
		    <td>15</td>

		    <td>522,01</td>
		    <td>4 522,60</td>

		    <td>-</td>
		    <td>-</td>

		    <td>168,41</td>
		    <td>732,75</td>
		    <td>1 836,90</td>
		    <td>1 836,90</td>
		    <td>710,00</td>

		    <td>522,01</td>
		    <td>732,75</td>
		    <td>583,22</td>
		    <td>1 597,375</td>
		    <td>1 597,375</td>
		    <td>1 315,650</td>
		</tr>

		<tr>
		    <td>2026-07-18_H02</td>
		    <td>60</td>

		    <td>615,00</td>
		    <td>-</td>

		    <td>-</td>
		    <td>-</td>

		    <td>145,23</td>
		    <td>631,88</td>
		    <td>-</td>
		    <td>-</td>
		    <td>-</td>

		    <td>547,97</td>
		    <td>706,54</td>
		    <td>621,20</td>
		    <td>5 290,450</td>
		    <td>5 290,450</td>
		    <td>4 830,750</td>
		</tr>
	    </tbody>
	</table>
	</body>
	</html>
	)HTML";

	TgeParser parser;
	std::vector<EnergyPrice> result = parser.parseEnergyPricesTable(html);
	const std::vector<EnergyPrice> expected = {
		{"2026-07-18_H01", 669.04, 679.53, 671.62},
		{"2026-07-18_H02", 615.00, 631.88, 621.20}};

	EXPECT_EQ(result, expected);
}
