#include "tgeParser.h"

#include <libxml/HTMLparser.h>

namespace  {
	using HtmlDocument = std::unique_ptr<xmlDoc, decltype(&xmlFreeDoc)>;
	HtmlDocument readHtml(const std::string_view html) {
		return {htmlReadMemory(
				html.data(),
				html.size(),
				nullptr,
				nullptr,
				HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING),
		       &xmlFreeDoc};
	}
}

RawTable TgeParser::parseEnergyPricesTable(const std::string_view html) const {
	auto document = readHtml(html);
	if (document == nullptr) {
		throw std::runtime_error("Filed to parse HTML document");
	}
	return {};
}

