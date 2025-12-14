#include <string>
#include <vector>
#include <iostream>
#include <regex>
#include <sstream>
#include "../../../commonFunctions/removeQuotes.h"
#include "../../../TerminalFormatting.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

using namespace VT;


struct Options {
  bool dontIncludeRegex = false;
  bool notCaseSensitive = false;
  string regularExpression;
  string input;
};


bool validateSyntaxAndSetFlags(std::vector<std::string> &tokenizedInput, Options& opt) {
    if (tokenizedInput.size() < 2) { return false; }
    int nonFlagCount = 0;
    for (size_t i = 1; i < tokenizedInput.size(); i++) {
        const string& param = tokenizedInput[i];
        if (param[0] == '-') {
            for (int j = 1; j < param.size(); j++) {
                switch (param[j]) {
                case 'v': opt.dontIncludeRegex = true; break;
                case 'i': opt.notCaseSensitive = true; break;
                default: break;
                }
            }
        }
        else {
          if (nonFlagCount == 0) { opt.regularExpression = param; }
          else if (nonFlagCount == 1) { opt.input = param; }
          else { return false; }
          nonFlagCount++;
        }
    }
    return true;
}


int main(int argc, char* argv[]) {
    Options opt;
    vector<string> tokenizedInput(argv, argv + argc);

    removeQuotesFromVector(tokenizedInput);
    if (!validateSyntaxAndSetFlags(tokenizedInput, opt)) { return -1; }

    try {
      std::regex regExpression;
      if (opt.notCaseSensitive) {
        regExpression = std::regex(opt.regularExpression, std::regex_constants::icase);
      } else {
        regExpression = std::regex(opt.regularExpression);
      }

      string line;
      std::smatch match;

      // if a string was passed in
      if (!opt.input.empty()) {
        std::stringstream inputString(opt.input);
        while (std::getline(inputString, line)) { // Read line by line until end of stream
          bool matchFound = std::regex_search(line, match, regExpression);
          if ((matchFound and !opt.dontIncludeRegex) or (!matchFound and opt.dontIncludeRegex)) {
            cout << line << "\n";
          }
        }
      }

      // if no string was passed in, we read from cin instead
      else {
        while (std::getline(std::cin, line)) { // Read line by line until end of stream
          bool matchFound = std::regex_search(line, match, regExpression);
          if ((matchFound and !opt.dontIncludeRegex) or (!matchFound and opt.dontIncludeRegex)) {
            cout << line << "\n";
          }
        }
      }

      return 0;
    }
    catch (const std::regex_error& e) {
      cerr << "Invalid regular expression: " << e.what();
      return -1;
    }
}