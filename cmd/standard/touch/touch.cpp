#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "../../../redCerr.h"

int main(int argc, char* argv[]) {
    setCerrColorRed();
    std::vector<std::string> tokenizedInput(argv, argv + argc); // convert it to a proper array
    if (tokenizedInput.size() != 2) {
        std::cout << "SYNTAX ERROR: Expected only 1 parameter" << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream outputFile(tokenizedInput[1]);
    if (outputFile.is_open()) { outputFile << ""; }
    else { std::cerr << "ERROR: failed to create file"; return EXIT_FAILURE; }
    return EXIT_SUCCESS;
}
