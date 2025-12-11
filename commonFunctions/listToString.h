#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

inline std::string listToString(const std::vector<std::string>& items) {
    std::stringstream finalString;
    for (size_t i = 0; i < items.size(); ++i) {
        finalString << items[i];
        if (i != items.size() - 1) {
            finalString << std::endl;
        }
    }
    return finalString.str();
}

