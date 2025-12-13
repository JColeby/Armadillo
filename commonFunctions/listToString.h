#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <string>


// takes in a list of strings and combines them into one, with each item separated by a newline
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

