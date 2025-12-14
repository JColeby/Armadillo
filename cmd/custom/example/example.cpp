#include <iostream>
#include <string>
#include <vector>

// I recommend using this to remove quotes around parameters
#include "../../../commonFunctions/removeQuotes.h"

struct Options {
  bool flagA = false;
  bool flagB = false;
  std::string nonFlag1;
  std::string nonFlag2;
  int count;
};


// I recommend setting flags and interpreting input like this.
bool validateSyntaxAndSetFlags(std::vector<std::string> &tokenizedInput, Options& opt) {

  if (tokenizedInput.size() < 2) { return false; }  // check that you don't have too few arguments

  int nonFlagCount = 0;
  for (size_t i = 1; i < tokenizedInput.size(); i++) { // looping through each parameter
    const std::string& param = tokenizedInput[i];

    // setting flags
    if (param[0] == '-') {
      for (int j = 1; j < param.size(); j++) {
        switch (param[j]) {
          // just set boolean values for the normal flags
        case 'a': opt.flagA = true; break;
        case 'b': opt.flagB = true; break;
          // if you have a flag that signifies that the user is passing in an optional parameter, do something like this
        case 'C': {
            try { opt.count = std::stoi(tokenizedInput[i + 1]); i++; break; }
            catch (...) { throw std::runtime_error("ERROR: expected a number as next parameter after flag -C"); }
          }
        default: break;
        }
      }
    }

    // setting normal Items
    else {
      switch (nonFlagCount) {
        case 0: opt.nonFlag1 = param; break;
        case 1: opt.nonFlag2 = param; break;
        default: return false;
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
    removeQuotesFromVector(tokenizedInput);
    if (!validateSyntaxAndSetFlags(tokenizedInput, opt)) { return -1; }

    // add functionality here
    std::cout << "Number of arguments: " << argc << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << "Argument " << i << ": " << argv[i] << std::endl;
    }
    return 0;
}
