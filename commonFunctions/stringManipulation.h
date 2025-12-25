#pragma once
#include <string>
#include <algorithm>

#include "bytesToReadableString.h"


namespace STR_MANIP {

    inline void toLower(std::string &s) {
        for (int i = 0; i < s.length(); i++) { s[i] = std::tolower(s[i]); }
    }

    inline std::string toLower(const std::string &s) {
        std::string str;
        for (int i = 0; i < s.length(); i++) { str.push_back(std::tolower(s[i])); }
        return str;
    }

    inline void toUpper(std::string &s) {
        for (int i = 0; i < s.length(); i++) { s[i] = std::toupper(s[i]); }
    }

    inline std::string toUpper(const std::string &s) {
        std::string str;
        for (int i = 0; i < s.length(); i++) { str.push_back(std::toupper(s[i])); }
        return str;
    }


    inline void strip(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isspace(c); }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), s.end());
    }


    inline bool endsWith(const std::string& str, const std::string& suffix) {
        if (suffix.size() > str.size()) return false;
        return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
    }

    inline bool startsWith(const std::string& str, const std::string& prefix) {
        if (prefix.size() > str.size()) return false;
        return std::equal(prefix.begin(), prefix.end(), str.begin());
    }

}
