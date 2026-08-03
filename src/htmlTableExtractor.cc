#include "htmlTableExtractor.h"
#include "model.h"

#include <iostream>
#include <queue>

std::string_view asStringView(const xmlChar* text) {
	if (text == nullptr) {
		return {};
	}
	return reinterpret_cast<const char*>(text);
}

std::string getNodeContent(xmlNodePtr node) {
	if (!node) {
		return "";
	}
	auto *rawContent = xmlNodeGetContent(node);
	auto res =  std::string(reinterpret_cast<const char *>(rawContent));
	xmlFree(rawContent);
	return res;
}

void printNodes(xmlNodePtr node, int depth) {
	for (auto current = node; current != nullptr; current = current->next) {
		if (current->type == XML_ELEMENT_NODE) {
			std::cout << std::string(depth * 2, ' ') << current->name << std::endl;
		}
		printNodes(current->children, depth + 1);
	}
}

HtmlTableExtractor::HtmlDocument HtmlTableExtractor::readHtml(const std::string_view html) {
	return {htmlReadMemory(
			html.data(),
			html.size(),
			nullptr,
			nullptr,
			HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING),
	       &xmlFreeDoc};
}


std::vector<xmlNodePtr> HtmlTableExtractor::findTables(const HtmlDocument &html) {
	constexpr std::string_view TABLE = "table";
	auto *root = xmlDocGetRootElement(html.get());
	if (!root) {
		return {};
	}

	std::queue<xmlNodePtr> nodes;
	nodes.push(root);

	std::vector<xmlNodePtr> found{};

	while(!nodes.empty()) {
		auto *node = nodes.front();
		nodes.pop();
		if (node->type == XML_ELEMENT_NODE && asStringView(node->name) == TABLE) {
			found.push_back(node);
		}

		auto *child = node->children;
		while (child) {
			nodes.push(child);
			child = child->next;
		}
	}
	return found;
}

std::vector<RawTable> HtmlTableExtractor::extractTables(const std::string_view html) {
	auto doc = readHtml(html);
	auto found = findTables(doc);
	std::vector<RawTable> res;

	for (auto* table : found) {
		res.push_back(getTable(table));
	}

	return res;
}


RawTable HtmlTableExtractor::getTable(xmlNodePtr table) {
	if (!table) {
		return {};
	}

	auto isElement = [](xmlNodePtr node, std::string_view name) {
		return node->type == XML_ELEMENT_NODE && asStringView(node->name) == name;
	};


	std::queue<xmlNodePtr> nodes;
	nodes.push(table);

	RawTable res;

	while(!nodes.empty()) {
		auto *node = nodes.front();
		nodes.pop();

		if (isElement(node, "tr")) {
			RawRow row;
			auto *cell = node->children;
			while (cell) {
				if (isElement(cell, "th") || isElement(cell, "td")) {
					row.push_back(getNodeContent(cell));
				}
				cell = cell->next;
			}
			if (!row.empty()) {
				res.push_back(std::move(row));
			}
		}

		auto *child = node->children;
		while (child) {
			nodes.push(child);
			child = child->next;
		}
	}

	return res;
}
