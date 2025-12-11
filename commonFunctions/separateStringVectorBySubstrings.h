#pragma once
#include <string>
#include <iostream>
#include <vector>

using std::string;
using std::vector;


inline vector<vector<string>> separateStringVectorBySubstrings(const vector<string>& tokenizedInput, const string separator)
{
    vector<vector<string>> individualCommands = {};
    vector<string> commandTokens;
    for (const auto& token : tokenizedInput) {
        if (token == separator) {
            individualCommands.push_back(commandTokens);
            commandTokens.clear();
        }
        else {
            commandTokens.push_back(token);
        }
    }
    individualCommands.push_back(commandTokens);
}