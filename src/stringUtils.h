#ifndef _STRINGUTILS_H_
#define _STRINGUTILS_H_

#include <optional>
#include <string>

namespace stringUtils {
	std::string trimLeft(const std::string &text);
	std::string trimRight(const std::string &text);
	std::string trim(const std::string &text);
	std::optional<double> getDouble(const std::string &str);
}

#endif // _STRINGUTILS_H_
