#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "../../../redCerr.h"

// structure to store all the parameters and flags provided by the user
struct Options {
    int count = 10;
    bool flagA = false;
    bool flagB = false;
    std::string nonFlag1;
    std::string nonFlag2;
};


// For complicated commands, I recommend setting flags and interpreting input like this.
bool validateSyntaxAndSetFlags(std::vector<std::string> &tokenizedInput, Options& opt) {
    int nonFlagCount = 0;
    for (size_t i = 1; i < tokenizedInput.size(); i++) { // looping through each parameter except the command name
        const std::string& param = tokenizedInput[i];

        // setting flag values
        if (param.size() > 1 && param[0] == '-' && !std::isdigit(param[1])) {
            std::string currentFlag; // holds a string of the flag we are evaluating so the user knows which flag caused an error
            try {
                // multi-character flags
                if (param[1] == '-') {
                    currentFlag = param;
                    if (param == "--count") { opt.count = std::stoi(tokenizedInput[i + 1]); i++; }
                }
                // single letter flags
                else {
                    currentFlag = "-?";
                    for (int j = 1; j < param.size(); j++) {
                        currentFlag[1] = param[j];
                        switch (param[j]) {
                            // set boolean values for normal flags
                            case 'a': opt.flagA = true; break;
                            case 'b': opt.flagB = true; break;
                            // if you have a flag that signifies that the user is passing in an optional parameter, do something like this. errors will be caught below.
                            case 'C': opt.count = std::stoi(tokenizedInput[i + 1]); i++; break;
                            default: std::cerr << "SYNTAX ERROR: Unexpected flag: -" << currentFlag << std::endl; return false;
                        }
                    }
                }
            }
            // catch the errors thrown from the statement above.
            catch (const std::out_of_range&) {
                std::cerr << "SYNTAX ERROR: expected additional parameter after flag " << currentFlag << std::endl;
                return false;
            }
            catch (const std::invalid_argument&) {
                std::cerr << "SYNTAX ERROR: number expected after flag " <<  currentFlag << ". " << tokenizedInput[i+1] << " is not a number" << std::endl;
                return false;
            }
            catch (...) {
                std::cerr << "SYNTAX ERROR: unknown error occurred while interpreting flag " << currentFlag << std::endl;
                return false;
            }
        }
        else if (param.size() == 1 && param[0] == '-') { std::cerr << "SYNTAX ERROR: expected flag identifier after '-'" << std::endl; return false;}

        // setting normal Items
        else {
            switch (nonFlagCount) {
                case 0: opt.nonFlag1 = param; break;
                case 1: opt.nonFlag2 = param; break;
                default: std::cerr << "SYNTAX ERROR: Too many arguments provided" << std::endl; return false;
            }
            nonFlagCount++;
        }
    }

    // verify the user provided necessary parameters
    if (opt.nonFlag1.empty()) { std::cerr << "SYNTAX ERROR: expected additional parameter 'nonFlag1'" << std::endl; return false; }
    if (opt.nonFlag2.empty()) { std::cerr << "SYNTAX ERROR: expected additional parameter 'nonFlag2'" << std::endl; return false; }

    return true;
}



int main(int argc, char* argv[]) {
    // standard setup
    setCerrColorRed(); // this will redirect cerr to a stream buffer that sets the color to red and prints to the location of cerr
    Options opt;
    std::vector<std::string> tokenizedInput(argv, argv + argc); // convert it to a proper array
    if (!validateSyntaxAndSetFlags(tokenizedInput, opt)) { return EXIT_FAILURE; }

    // add functionality here
    std::cout << "Number of arguments: " << argc << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << "Argument " << i << ": " << argv[i] << std::endl;
    }
    return EXIT_SUCCESS;
}
