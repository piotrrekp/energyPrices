#ifndef _HTMLTABLEEXTRACTOR_H_
#define _HTMLTABLEEXTRACTOR_H_

#include <libxml/HTMLparser.h>

#include "model.h"

class HtmlTableExtractor {
public:
	std::vector<RawTable> extractTables(const std::string_view html);
private:
	using HtmlDocument = std::unique_ptr<xmlDoc, decltype(&xmlFreeDoc)>;

	HtmlDocument readHtml(const std::string_view html);

	std::vector<xmlNodePtr> findTables(const HtmlDocument &html);

	RawTable getTable(xmlNodePtr table);
};

#endif // _HTMLTABLEEXTRACTOR_H_
