#include <string>
#include <vector>
#include <iostream>
#include <windows.h>
#include <filesystem>
#include <fstream>
#include "../../../commonFunctions/removeQuotes.h"
#include "../../../TerminalFormatting.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

using namespace VT;

namespace fs = std::filesystem;

struct Options {
  bool showAll = false;
  string directory;
};


bool validateSyntaxAndSetFlags(std::vector<std::string> &tokenizedInput, Options& opt) {
    for (int i = 1; i < tokenizedInput.size(); i++) {
        string param = tokenizedInput[i];
        if (!param.empty() and param[0] == '-') {
            for (int j = 1; j < param.size(); j++) {
                switch (param[j]) {
                    case 'a': opt.showAll = true; break;
                    default: break;
                }
            }
        }
        else if (opt.directory.empty()) { opt.directory = param; }
        else {
            cerr << "ERROR: Invalid syntax" << endl;
            return false;
        }
    }
    if (opt.directory.empty()) { opt.directory = "."; }
    return true;
}



bool endsWith(const std::string& s, const std::string end) {
    if (s.length() < end.length()) return false;
    return s.compare(s.length() - end.length(), end.length(), end) == 0;
}



int main(int argc, char* argv[]) {
    Options opt;
    std::vector<std::string> tokenizedInput(argv, argv + argc);

    removeQuotesFromVector(tokenizedInput);
    if (!validateSyntaxAndSetFlags(tokenizedInput, opt)) { return -1; }

    std::stringstream output;

    try {
      for (const auto &entry : fs::directory_iterator(opt.directory)) {
        std::string name = entry.path().filename().string();

        // checking if the file is hidden by windows or has a '.' at the front of the file
        DWORD attributes = GetFileAttributesA(entry.path().string().c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES) { continue; }
        if (!opt.showAll and (attributes & FILE_ATTRIBUTE_HIDDEN) != 0) { continue; }
        if (!opt.showAll and !name.empty() and name[0] == '.') { continue; }

        if (entry.is_directory()) { output << CYAN; }
        else if (endsWith(name, ".exe")) { output << YELLOW; }
        else if (endsWith(name, ".ardo")) { output << GREEN; }
        output << " " << name << RESET_TEXT << "\n";
      }
    }
    catch (const fs::filesystem_error &e) {
      cerr << "ERROR: Cannot open directory: " << opt.directory << endl;
      return -1;
    }

    cout << output.str();
}