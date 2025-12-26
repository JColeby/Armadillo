#include <string>
#include <iostream>
#include <vector>

#include "../../../TerminalFormatting.h"
#include "../../../redCerr.h"

using std::string;
using std::cout;

using namespace VT;

int main(int argc, char* argv[]) {
    setCerrColorRed();
    std::vector<std::string> tokenizedInput(argv, argv + argc); // convert it to a proper array
    if (tokenizedInput.size() != 1) { std::cerr << "SYNTAX ERROR: no additional arguments were expected" << std::endl; return -1; }

    string newString;    // Did this so the background would always format correctly
    newString += R"ASCII(                                                                                 )ASCII""\n";
    newString += R"ASCII(       #######                                                                   )ASCII""\n";
    newString += R"ASCII(       #######         ___ _         _   _                 ____  _ _ _           )ASCII""\n";
    newString += R"ASCII(       #######        |_ _| |_ ___  | | | | ___ _ __ ___  / ___|(_) | |_   _     )ASCII""\n";
    newString += R"ASCII(       #######         | || __/ __| | |_| |/ _ \ '__/ _ \ \___ \| | | | | | |    )ASCII""\n";
    newString += R"ASCII(   "#############"     | || |_\__ \ |  _  |  __/ | |  __/  ___) | | | | |_| |    )ASCII""\n";
    newString += R"ASCII(     "#########"      |___|\__|___/ |_| |_|\___|_|  \___| |____/|_|_|_|\__, |    )ASCII""\n";
    newString += R"ASCII(       "#####"                                                         |___/     )ASCII""\n";
    newString += R"ASCII(         "#"                                                                     )ASCII""\n";
    newString += R"ASCII(                                                                                 )ASCII""\n";

    if (verify()) { cout << setBgColor(31,30,51) << YELLOW << newString << RESET_TEXT; }
    else { cout << newString; }
    return EXIT_SUCCESS;
}