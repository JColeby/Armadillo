#include <string>
#include <vector>
#include <iostream>
#include <windows.h>
#include <filesystem>
#include <fstream>
#include "../../../commonFunctions/removeQuotes.h"
#include "../../../TerminalFormatting.h"
#include "edit.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

using namespace VT;

namespace fs = std::filesystem;


// did not want to refactor the edit class into a non-class, so I just copied it over
int main(int argc, char* argv[]) {
    std::vector<std::string> tokenizedInput(argv + 1, argv + argc);
    removeQuotesFromVector(tokenizedInput);

    if (!Edit::validateSyntax(tokenizedInput)) {
      cerr << "ERROR: invalid syntax. Expected only 1 argument" << endl;
      return -1;
    }

    Edit editor(tokenizedInput);
    editor.executeCommand();
    return 0;
}