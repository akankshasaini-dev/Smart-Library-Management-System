// Utils.h - small helpers for safe console input and string handling.
#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {

std::string trim(const std::string& text);
std::string toLower(std::string text);
bool isAlphaNumeric(const std::string& text);

// Each reader keeps asking until the input is valid.
// If the input stream ends (Ctrl+Z / end of a script) the program exits cleanly.
std::string readLine(const std::string& prompt, bool allowEmpty = false);
int readInt(const std::string& prompt, int minValue, int maxValue);
bool readYesNo(const std::string& prompt);
std::string readStudentId(const std::string& prompt);

} // namespace Utils

#endif
