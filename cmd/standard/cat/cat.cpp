#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "../../../commonFunctions/getTextFile.h"
#include "../../../TerminalFormatting.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

using namespace VT;

int main(int argc, char* argv[]) {
  vector<string> tokenizedInput(argv, argv + argc);
  if (tokenizedInput.size() != 2) {
    cerr << "SYNTAX ERROR: Incorrect number of arguments. Command syntax: cat <filepath>\n";
    return EXIT_FAILURE;
  }

  try {
    string output = getTextFile(tokenizedInput[1]);
    if (output.empty() || output.back() != '\n') {
      output.push_back('\n');
    }
    cout << output << endl;
    return EXIT_SUCCESS;
  }
  catch (const std::exception& e) {
    cerr << "ERROR: " << e.what() << endl;
    return EXIT_FAILURE;
  }
}