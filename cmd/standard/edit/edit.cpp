#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "edit.h"
#include "../../../redCerr.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;


// did not want to refactor the edit class into a non-class, so I just copied it over
int main(int argc, char* argv[]) {
    setCerrColorRed();
    std::vector<std::string> tokenizedInput(argv + 1, argv + argc);

    if (!Edit::validateSyntax(tokenizedInput)) {
      cerr << "ERROR: invalid syntax. Expected only 1 argument" << endl;
      return EXIT_FAILURE;
    }

    Edit editor(tokenizedInput);
    editor.executeCommand();
    return EXIT_SUCCESS;
}