#include <string>
#include <vector>
#include <iostream>
#include <regex>
#include <sstream>
#include <unistd.h>

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

struct Options {
  bool dontIncludeRegex = false;
  bool notCaseSensitive = false;
  string regularExpression;
  string input;
};


bool validateSyntaxAndSetFlags(std::vector<std::string> &tokenizedInput, Options& opt) {
    int nonFlagCount = 0;
    for (size_t i = 1; i < tokenizedInput.size(); i++) {
        const string& param = tokenizedInput[i];
        if (param[0] == '-') {
            for (int j = 1; j < param.size(); j++) {
                switch (param[j]) {
                    case 'v': opt.dontIncludeRegex = true; break;
                    case 'i': opt.notCaseSensitive = true; break;
                    default: cerr << "SYNTAX ERROR: Unexpected flag -" << param[j] << endl; return false;
                }
            }
        }
        else {
          switch (nonFlagCount) {
            case 0: opt.regularExpression = param; break;
            case 1: opt.input = param; break;
            default: cerr << "SYNTAX ERROR: Too many arguments provided" << endl; return false;
          }
          nonFlagCount++;
        }
    }
    if (nonFlagCount < 1) { cerr << "SYNTAX ERROR: Too few arguments." << endl; return false; }
    return true;
}


int main(int argc, char* argv[]) {
    Options opt;
    vector<string> tokenizedInput(argv, argv + argc);

    if (!validateSyntaxAndSetFlags(tokenizedInput, opt)) { EXIT_FAILURE; }

    try {
      std::regex regExpression;
      if (opt.notCaseSensitive) {
        regExpression = std::regex(opt.regularExpression, std::regex_constants::icase);
      } else {
        regExpression = std::regex(opt.regularExpression);
      }

      string line;
      std::smatch match;

      // if our input is piped
      if (!isatty(fileno(stdin))) {
        while (std::getline(std::cin, line)) {
          bool matchFound = std::regex_search(line, match, regExpression);
          if ((matchFound && !opt.dontIncludeRegex) || (!matchFound && opt.dontIncludeRegex)) {
            cout << line << "\n";
          }
        }
      }
      else if (!opt.input.empty()) {
        std::stringstream inputString(opt.input);
        while (std::getline(inputString, line)) {
          bool matchFound = std::regex_search(line, match, regExpression);
          if ((matchFound && !opt.dontIncludeRegex) || (!matchFound && opt.dontIncludeRegex)) {
            cout << line << "\n";
          }
        }
      }
      else {
        cerr << "No input provided\n";
        return EXIT_FAILURE;
      }

      return EXIT_SUCCESS;
    }
    catch (const std::regex_error& e) {
      cerr << "Invalid regular expression: " << e.what();
      return EXIT_FAILURE;
    }
}