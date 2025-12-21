#include <fstream>
#include <io.h>
#include <iostream>
#include <string>
#include <vector>



int main(int argc, char* argv[]) {
    std::vector<std::string> tokenizedInput(argv, argv + argc); // convert it to a proper array
    if (tokenizedInput.size() != 2) {
        std::cout << "SYNTAX ERROR: Expected only 1 parameters" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string& dirname = tokenizedInput[1];
    int result = mkdir(dirname.c_str());

    if (result != 0) {
        if (errno == EEXIST) {
            std::cerr << "ERROR: " << dirname << " already exists" << std::endl;
            return EXIT_FAILURE;
        }
        if (errno == ENOENT) {
            std::cerr << "ERROR: parent directory does not exist" << std::endl;
            return EXIT_FAILURE;
        }
        std::cerr << "ERROR: failed to create directory" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
