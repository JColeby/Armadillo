#include <fstream>
#include <iostream>
#include <string>
#include <vector>



int main(int argc, char* argv[]) {
    std::vector<std::string> tokenizedInput(argv, argv + argc); // convert it to a proper array
    if (tokenizedInput.size() != 3) {
        std::cout << "SYNTAX ERROR: Expected only 2 parameters" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string& src = tokenizedInput[1];
    const std::string& dst = tokenizedInput[2];

    std::ifstream in(src, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "ERROR: could not open source file: " << src << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream out(dst, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "ERROR: could not create destination file: " << src << std::endl;
        return EXIT_FAILURE;
    }

    out << in.rdbuf();

    if (!out.good()) {
        std::cerr << "ERROR: something went wrong when writing to destination file: " << dst << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
