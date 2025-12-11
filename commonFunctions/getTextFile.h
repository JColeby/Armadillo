#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

inline std::string getTextFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}
