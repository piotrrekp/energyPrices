#include <string>
#include <fstream>
#include <sstream>

std::string loadFile(const std::string &path) {
	std::ifstream file(path);
	if (!file) {
		throw std::runtime_error("Cannot open file: " + path);
	}
	std::ostringstream buffer;
	buffer << file.rdbuf();

	return buffer.str();
}

