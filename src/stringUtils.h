#ifndef _STRINGUTILS_H_
#define _STRINGUTILS_H_

#include <string>

namespace stringUtils {
	std::string trimLeft(const std::string &text);
	std::string trimRight(const std::string &text);
	std::string trim(const std::string &text);
}

#endif // _STRINGUTILS_H_
