#include <iostream>
#include <sstream>
#include <string>
#include <vector>


struct Options {
  bool flagA = false;
  bool flagB = false;
  std::string nonFlag1;
  std::string nonFlag2;
  int count = 10;
};


// I recommend setting flags and interpreting input like this.
bool validateSyntaxAndSetFlags(std::vector<std::string> &tokenizedInput, Options& opt) {
  if (tokenizedInput.empty()) { std::cerr << "SYNTAX ERROR: too few parameters" << std::endl; return false; }  // check that you don't have too few arguments
  int nonFlagCount = 0;
  for (size_t i = 1; i < tokenizedInput.size(); i++) { // looping through each parameter
    const std::string& param = tokenizedInput[i];

    // setting flags
    if (param[0] == '-') {
      for (int j = 1; j < param.size(); j++) {
        try {
          switch (param[j]) {
            // just set boolean values for the normal flags
            case 'a': opt.flagA = true; break;
            case 'b': opt.flagB = true; break;
            // if you have a flag that signifies that the user is passing in an optional parameter, do something like this
            case 'C': opt.count = std::stoi(tokenizedInput[i + 1]); i++; break;
            default: std::cerr << "SYNTAX ERROR: Unexpected flag: -" << param[j] << std::endl; return false;
          }
        }
        catch (const std::out_of_range&) {
          std::cerr << "SYNTAX ERROR: expected an additional parameter after flag -" << param[j] << std::endl; return false;
        }
        catch (const std::invalid_argument&) {
          std::cerr << "SYNTAX ERROR: number expected after flag -" <<  param[j] << ". " << tokenizedInput[i+1] << "is not a number" << param[j] << std::endl; return false;
        }
      }
    }

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
  return true;
}



int main(int argc, char* argv[]) {
    // standard setup
    Options opt;
    std::vector<std::string> tokenizedInput(argv, argv + argc); // convert it to a proper array
    if (!validateSyntaxAndSetFlags(tokenizedInput, opt)) { return -1; }

    // add functionality here
    std::cout << "Number of arguments: " << argc << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << "Argument " << i << ": " << argv[i] << std::endl;
    }
    return 0;
}
