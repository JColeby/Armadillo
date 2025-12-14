#pragma once
#include <string>
#include <vector>

void removeQuotes(std::string& input) {
    if (input.empty()) return;
    size_t start = 0;
    size_t end = input.size() - 1;
    if (input[start] == '"') { ++start; }
    if (input[end] == '"') { --end; }
    input = input.substr(start, end - start + 1);
}

void removeQuotesFromVector(std::vector<std::string>& vec) {
    for (size_t i = 0; i < vec.size(); i++) {
        removeQuotes(vec[i]);
    }
}
