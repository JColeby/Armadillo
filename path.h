#pragma once
#include <string>

// points to the central folder that holds all the commands, text files, and other resources.
inline std::string ARDO_PATH =
    std::string(std::getenv("LOCALAPPDATA")) + "\\Armadillo"; // Make sure this points to the Armadillo folder



// inline std::string ARDO_PATH = "C:\\Users\\jcbos\\Documents\\Armadillo\\Armadillo";