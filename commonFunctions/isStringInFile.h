#pragma once
#include <filesystem>
#include <fstream>


// Loops through each line in a provided file and returns true if the start of a line matches the word you passed in
inline string isStringInFile(const std::string& startOfLine, const std::string& filename)
{
    std::ifstream file(filename);
    if (!file) { throw std::runtime_error("Could not find/open " + filename); }
    std::string line;

    while (std::getline(file, line)) {
        if (line.rfind(startOfLine, 0) == 0) { // exact match
            file.close();
            return line;
        }
    }
    file.close();
    return "";
}