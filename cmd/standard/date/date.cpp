#include <chrono>
#include <ctime>
#include <string>
#include <iostream>
#include <vector>

#include "../../../TerminalFormatting.h"

using std::string;
using std::cout;

using namespace VT;

int main(int argc, char* argv[]) {
    std::vector<std::string> tokenizedInput(argv, argv + argc); // convert it to a proper array
    if (tokenizedInput.size() != 1) { std::cerr << "SYNTAX ERROR: no additional arguments were expected" << std::endl; return -1; }

    std::chrono::time_point<std::chrono::system_clock> systemTime = std::chrono::system_clock::now();
    std::time_t dateAndTime = std::chrono::system_clock::to_time_t(systemTime);
    cout << std::ctime(&dateAndTime) << std::endl;

    return 0;
}