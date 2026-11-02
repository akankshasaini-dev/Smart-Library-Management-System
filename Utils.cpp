// Utils.cpp - safe input helpers. Every input is read as a whole line and
// validated, so typing letters where a number is expected never breaks cin.
#include "Utils.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>

using namespace std;

namespace {

string getRawLine() {
    string line;
    if (!getline(cin, line)) {
        cout << "\n[Input closed] Exiting program. (Data is saved after every change.)\n";
        exit(0);
    }
    return line;
}

} // namespace

string Utils::trim(const string& text) {
    size_t start = 0;
    while (start < text.size() && isspace(static_cast<unsigned char>(text[start]))) ++start;
    size_t end = text.size();
    while (end > start && isspace(static_cast<unsigned char>(text[end - 1]))) --end;
    return text.substr(start, end - start);
}

string Utils::toLower(string text) {
    transform(text.begin(), text.end(), text.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return text;
}

bool Utils::isAlphaNumeric(const string& text) {
    if (text.empty()) return false;
    for (unsigned char c : text) {
        if (!isalnum(c)) return false;
    }
    return true;
}

string Utils::readLine(const string& prompt, bool allowEmpty) {
    while (true) {
        cout << prompt;
        string line = trim(getRawLine());
        if (line.empty() && !allowEmpty) {
            cout << "  Error: input cannot be empty. Try again.\n";
            continue;
        }
        // '|' is the field separator in our data file, so it is not allowed in text.
        if (line.find('|') != string::npos) {
            cout << "  Error: the '|' character is not allowed. Try again.\n";
            continue;
        }
        return line;
    }
}

int Utils::readInt(const string& prompt, int minValue, int maxValue) {
    while (true) {
        cout << prompt;
        string line = trim(getRawLine());
        bool onlyDigits = !line.empty() && line.size() <= 9 &&
                          all_of(line.begin(), line.end(),
                                 [](unsigned char c) { return isdigit(c) != 0; });
        if (!onlyDigits) {
            cout << "  Error: please enter a whole number (digits only).\n";
            continue;
        }
        int value = stoi(line);
        if (value < minValue || value > maxValue) {
            cout << "  Error: number must be between " << minValue << " and " << maxValue << ".\n";
            continue;
        }
        return value;
    }
}

bool Utils::readYesNo(const string& prompt) {
    while (true) {
        cout << prompt;
        string line = toLower(trim(getRawLine()));
        if (line == "y" || line == "yes") return true;
        if (line == "n" || line == "no") return false;
        cout << "  Error: please type y or n.\n";
    }
}

string Utils::readStudentId(const string& prompt) {
    while (true) {
        cout << prompt;
        string line = trim(getRawLine());
        if (!isAlphaNumeric(line)) {
            cout << "  Error: Student ID must be letters/digits only (no spaces).\n";
            continue;
        }
        return line;
    }
}
